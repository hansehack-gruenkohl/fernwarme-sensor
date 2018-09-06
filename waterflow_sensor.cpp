#include "waterflow_sensor.h"
#include <Arduino.h>
#include <avr/interrupt.h>

// http://www.hobbytronics.co.uk/download/YF-S201.ino

volatile int flow_frequency;  // Measures flow meter pulses

void flowSensorPulse() {
    flow_frequency++;
}

void waterflow_init(int port) {
    pinMode(port, INPUT);
    attachInterrupt(port, flowSensorPulse, RISING);

    interrupts();
}

int waterflow_measure() {
    flow_frequency = 0;
    delayMicroseconds(1000*1000);

    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min. (Results in +/- 3% range)
    int litresPerHour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour 
    flow_frequency = 0;                              // Reset Counter
    Serial.print(litresPerHour, DEC);                // Print litres/hour
    Serial.println(" L/hour"); 
    return litresPerHour;
}
