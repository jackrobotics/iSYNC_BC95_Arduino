#include <Arduino.h>
#include <AltSoftSerial.h>
#include <BC95Udp.h>
#include <CoAP.h>

AltSoftSerial bc95serial;

BC95UDP udp;
Coap coap(udp);

#define iSYNC_KEY "5c888df67f56637c67cac702"

void responseHandler(CoapPacket *packet, IPAddress remoteIP, int remotePort) {
    char buff[6];
    Serial.println("#iSYNC -> NBIoT");
    Serial.print("#[");
    sprintf(buff, "%d.%02d", packet->code >> 5, packet->code & 0b00011111);
    Serial.print(buff);
    Serial.print("]-> DATA : ");
    for (int i=0; i< packet->payloadlen; i++)Serial.print((char) (packet->payload[i]));
    Serial.println();
    Serial.println("#----------------------------");
}

void setup() {
    bc95serial.begin(9600);
    BC95.begin(bc95serial);
    BC95.reset();

    Serial.begin(9600);
    Serial.println(F("Starting..."));

    while (!BC95.attachNetwork()) {
        Serial.println("...");
        delay(1000);
    }
    Serial.println(F("NB-IOT attached.."));
    Serial.print("IP: ");
    Serial.println(BC95.getIPAddress());

    coap.response(responseHandler);
    coap.start();
}

long lasttime = millis();

void loop() {
    coap.loop();
    if(millis()-lasttime > 5000){
        lasttime = millis();
        String Temperature=String(random(0,100));
        String Humidity=String(random(0,100));
        String payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";

        Serial.println("#iSYNC <- NBIoT");
        Serial.print("#DATA : ");
        Serial.println(payload);
        coap.iSYNC_POST(iSYNC_KEY,payload);
        Serial.println("#----------------------------");
    }
}