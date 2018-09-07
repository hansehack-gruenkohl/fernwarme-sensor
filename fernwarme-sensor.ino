#include <stdlib.h>
#include <lmic.h>
#include "utils.h"
#include "lora.h"
#include "waterflow_sensor.h"

#define LED_1 5
#define LED_2 6
#define LED_3 11
#define LED_4 12
#define POTI_LED LED_1
#define TRANSMITTING_LED LED_3

#define WATER_FLOW_SENSOR_PORT 13

#define SEND_INTERVAL_IN A0

static osjob_t sendjob;
unsigned transmit_interval = 30;
int send_interval_in = 0;

void onEvent(ev_t ev) {
    switch (ev) {
    // data has been sent out
    case EV_TXCOMPLETE:
        Serial.println(" ");
        Serial.println("TX COMPLETE");
        digitalWrite(TRANSMITTING_LED, LOW);
        Serial.print("Transmit-Interval = ");
        Serial.print(transmit_interval, DEC);
        Serial.println("sec");
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(transmit_interval), do_send);
        break;
    default:
        break;
    }
}

void do_send(osjob_t*) {
    if (lora_isSending()) {
        return;
    }

    digitalWrite(TRANSMITTING_LED, HIGH);

    payload_type types[LORA_FIELD_COUNT];
    lora_payload fields[LORA_FIELD_COUNT];
    lora_prepareTypes(types);

    types[0] = PAYLOAD_TYPE_INT;
    fields[0].i = waterflow_measure();

    lora_send(types, fields);
}



void setup() {
    Serial.println("SCS Edu-Kit init");
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(TRANSMITTING_LED, OUTPUT);
    pinMode(POTI_LED, OUTPUT);

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(TRANSMITTING_LED, LOW);
    digitalWrite(POTI_LED, LOW);

    waterflow_init(WATER_FLOW_SENSOR_PORT);

    os_init();

    Serial.begin(9600);
    Serial.println("SETUP!");

    lora_init();
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
    waterflow_measure();

    send_interval_in = analogRead(SEND_INTERVAL_IN);
    transmit_interval = (send_interval_in < 512) ? 10 : 1;
}
