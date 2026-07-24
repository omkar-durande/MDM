#include <Arduino.h>
#include <mbed.h>

// Using Mbed's DigitalOut instead of Arduino's pinMode/digitalWrite
mbed::DigitalOut myLed(digitalPinToPinName(LED_BUILTIN));

// Create an Mbed RTOS thread
rtos::Thread myThread;

// The function our thread will run
void blink_thread() {
  while (true) {
    myLed = !myLed; // Toggle the LED
    rtos::ThisThread::sleep_for(500); // Mbed's delay function
  }
}

void setup() {
  Serial.begin(9600);
 
  // Start the Mbed thread
  myThread.start(blink_thread);
}

void loop() {
  // Standard Arduino code can run here simultaneously
  Serial.println("Main loop is still running!");
  delay(2000);
}