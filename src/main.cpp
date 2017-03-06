#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESPAsyncTCP.h>

#define LEDPin D4


const char *ssid = "TNCAP993B7D";
const char *password = "DFC3B98A7A";


const char *ap_ssid = "pino";
const char *ap_password = "password";

const char *hostName = "pino";

const int port = 3000;
const char maxClients = 7;



AsyncClient *clients[maxClients] = {NULL};

AsyncServer server(port);


IPAddress ip(192, 168, 2, 100);
IPAddress gateway(192, 168, 2, 254);
IPAddress subnet(255, 255, 255, 0);


uint8_t toInt(unsigned char numero) {
    return (numero - 48);
}

unsigned char firstEmpty(AsyncClient **pointer) {
    unsigned char i = 0;
    while (pointer[i] != NULL) {
        Serial.printf("ptr: %p\n", pointer[i]);
        i++;
    }
    return i;
}

void setup() {
    // put your setup code here, to run once:
    randomSeed(analogRead(0));
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    digitalWrite(LEDPin,LOW);

    WiFi.mode(WIFI_AP_STA);

    WiFi.begin(ssid, password);

    WiFi.softAP(ap_ssid, ap_password);

    WiFi.softAPConfig(ip, gateway, subnet);


    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    ArduinoOTA.begin();
    MDNS.begin(hostName);
    MDNS.addService("my-service", "tcp", port);
    server.onClient(
            [](void *server, AsyncClient *client) {
                uint8_t *number = (uint8_t *) malloc(sizeof(uint8_t));
                *number = firstEmpty(clients);
                clients[*number] = client;
                Serial.printf("TCP: Client %i connected\n", *number);

                client->onData(
                        [](void *number, AsyncClient *client, void *data, size_t len) {
                            if (((char *) data)[0] == ':') {
                                if (clients[toInt(((char *) data)[1])] == NULL) {
                                    return client->write("error: idiot\n");
                                }
                                clients[toInt(((char *) data)[1])]->write(((char *) data));
                            }
                            Serial.printf("Data from %i: ", *(char *) number);
                            for (unsigned char i = 0; i < len; ++i) {

                                Serial.print(((char *) data)[i]);
                            }

                            Serial.println();
                        }, number
                );
                client->onDisconnect([](void *number, AsyncClient *client) {
                    client->~AsyncClient();
                    clients[*(char *) number] = NULL;
                }, number);
            }, NULL);

    server.begin();
}


void loop() {
    // put your main code here, to run repeatedly:
    //ArduinoOTA.handle();


    int randomclient,prevrandomclient;

    do {
        randomclient = random(firstEmpty(clients)+1);
    }while(randomclient==prevrandomclient);
    Serial.println(randomclient);

    if(clients[randomclient]!=NULL)
    {
        if(randomclient == firstEmpty(clients) + 1)
        {
            digitalWrite(LEDPin,HIGH);
            delay(5000);
            digitalWrite(LEDPin,LOW);
        }
        clients[randomclient]->write("2");
        Serial.println("Dati inviato");
    }
    prevrandomclient=randomclient;
    Serial.println("2");
    delay(5000);


}