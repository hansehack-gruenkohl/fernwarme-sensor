#include <stdlib.h>
#include <lmic.h>
#include "utils.h"
#include "lora.h"
#include "waterflow_sensor.h"

#define LED_1 5
#define LED_2 6
#define LED_3 11
#define LED_4 12
#define TRANSMITTING_LED LED_3

#define WATER_FLOW_SENSOR_PORT 13

static osjob_t sendjob;
const unsigned TX_INTERVAL = 30;

void onEvent(ev_t ev) {
    switch (ev) {
    // data has been sent out
    case EV_TXCOMPLETE:
        Serial.println(" ");
        Serial.println("TX COMPLETE");
        digitalWrite(TRANSMITTING_LED, LOW);
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
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

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(TRANSMITTING_LED, LOW);

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
}
