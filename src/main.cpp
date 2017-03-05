#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncTCPbuffer.h>

#define LEDPin D4

const char* ssid= "pino";
const char* password= "password";

AsyncClient client1;

IPAddress ip(192,168,2,100);



void setup() {
    // put your setup code here, to run once:

    Serial.begin(115200);
    Serial.setDebugOutput(true);

    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid, password);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("STA: Failed to connect!\nSTA: will retry in 1s...\n");
        WiFi.disconnect();
        delay(1000);
        WiFi.begin(ssid, password);
    }

    client1.connect(ip,3000);
    client1.onConnect(
            [](void* client, AsyncClient* cli){
                cli->write("1");
                Serial.println("1");

                cli->onData(
                        [](void *number, AsyncClient *client, void *data, size_t len) {
                            if (((char *) data)[0] == '2') {
                                digitalWrite(LEDPin,HIGH);
                                Serial.println("Led acceso");

                            }

                            for (unsigned char i = 0; i < len; ++i) {

                                Serial.print(((char *) data)[i]);
                            }

                            Serial.println();
                        },NULL
                );
            },NULL);




}

void loop() {
    // put your main code here, to run repeatedly:

}