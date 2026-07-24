Case Study Title

Implementation of Watchdog Timer (WDT) Using LED Heartbeat System on Arduino Nano 33 BLE Sense

Problem Statement

Embedded systems may stop responding due to software bugs or unexpected conditions. In critical applications, manual intervention is not always possible. This project demonstrates how the Watchdog Timer (WDT) can automatically recover the Arduino Nano 33 BLE Sense from a software hang by resetting the microcontroller.

Project Objective

To demonstrate the working of the Watchdog Timer by blinking the onboard LED at regular intervals and intentionally creating an infinite loop. If the watchdog is not refreshed within the timeout period, it automatically resets the board and resumes normal operation.

Working:
The Arduino Nano 33 BLE Sense starts normally.
The onboard LED blinks once every second to indicate that the system is running.
During normal operation, the program continuously refreshes the Watchdog Timer.
After 10 seconds, the program intentionally enters an infinite loop (while(1);).
Since the Watchdog Timer is no longer refreshed, it times out.
The WDT automatically resets the microcontroller.
After the reset, the LED starts blinking again, proving that the Watchdog Timer successfully recovered the system.

Expected Output:
LED blinks every second.
After about 10 seconds, the LED stops because the program hangs.
Within the watchdog timeout (e.g., 4 seconds), the board automatically resets.
The LED starts blinking again from the beginning.
This cycle repeats continuously.

Applications:
Industrial automation
Medical devices
Automotive electronics
IoT devices
Smart sensors
Remote monitoring systems

Components Required:
Arduino Nano 33 BLE Sense
USB Cable
Arduino IDE

(No external LED is required since you can use the built-in LED.)

Code:
#include <Arduino.h>
#include "nrfx_wdt.h"

// Define the onboard LED pin
const int LED_PIN = LED_BUILTIN; 

// Watchdog Timer instance
nrfx_wdt_channel_id m_wdt_channel_id;

// Watchdog event handler (executed right before reset, if needed)
void wdt_event_handler(void) {
    // NOTE: Keep this minimal. The system is about to reset.
}

void initWatchdog() {
    // 1. Configure the Watchdog Timer
    nrfx_wdt_config_t config = NRFX_WDT_DEFAULTS;
    
    // Set timeout period to 4000 milliseconds (4 seconds)
    config.reload_value = 4000; 

    // 2. Initialize the WDT
    uint32_t err_code = nrfx_wdt_init(&config, wdt_event_handler);
    if (err_code != NRF_SUCCESS) {
        // Initialization failed
        while (1); 
    }

    // 3. Allocate a channel for the WDT
    err_code = nrfx_wdt_channel_alloc(&m_wdt_channel_id);
    if (err_code != NRF_SUCCESS) {
        // Channel allocation failed
        while (1);
    }

    // 4. Enable the Watchdog Timer
    nrfx_wdt_enable();
}

void setup() {
    // Initialize the onboard LED pin as an output
    pinMode(LED_PIN, OUTPUT);
    
    // Initialize serial communication for debugging
    Serial.begin(9600);
    // Wait up to 2 seconds for serial port to open (optional)
    unsigned long startTime = millis();
    while (!Serial && (millis() - startTime < 2000)); 

    Serial.println("\n--- System Booted / Reset Success ---");
    Serial.println("Initializing Watchdog Timer (4s timeout)...");
    
    // Initialize the WDT
    initWatchdog();
    
    Serial.println("Watchdog active. Starting 10-second normal operation loop.");
}

void loop() {
    // --- Phase 1: Normal Operation (First 10 Seconds) ---
    for (int i = 1; i <= 10; i++) {
        // Blink the onboard LED (Heartbeat)
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(800);

        // Feed/Refresh the Watchdog Timer to prevent reset
        nrfx_wdt_channel_feed(m_wdt_channel_id);

        // Print status to Serial Monitor
        Serial.print("Heartbeat ");
        Serial.print(i);
        Serial.println("s: WDT Fed.");
    }

    // --- Phase 2: Intentional Software Hang ---
    Serial.println("\n!!! Simulating System Hang (Entering Infinite Loop) !!!");
    Serial.println("LED will stop blinking. Watch for automatic reset in ~4 seconds...");
    Serial.flush(); // Ensure everything is printed before the freeze

    // Turn LED solid HIGH right before freezing so you have a visual cue
    digitalWrite(LED_PIN, HIGH); 

    // Infinite loop: WDT is NO LONGER FED here
    while (1) {
        // The microcontroller traps itself here.
        // After 4 seconds without nrfx_wdt_channel_feed(), the WDT triggers a hard reset.
    }
}

Expected Output:
LED blinks every second.
After about 10 seconds, the LED stops because the program hangs.
Within the watchdog timeout (e.g., 4 seconds), the board automatically resets.
The LED starts blinking again from the beginning.
This cycle repeats continuously.

----x------------------x------------------Output-----------------x-----------------x---

--- System Booted / Reset Success ---
Initializing Watchdog Timer (4s timeout)...
Watchdog active. Starting 10-second normal operation loop.
Heartbeat 1s: WDT Fed.
Heartbeat 2s: WDT Fed.
Heartbeat 3s: WDT Fed.
Heartbeat 4s: WDT Fed.
Heartbeat 5s: WDT Fed.
Heartbeat 6s: WDT Fed.
Heartbeat 7s: WDT Fed.
Heartbeat 8s: WDT Fed.
Heartbeat 9s: WDT Fed.
Heartbeat 10s: WDT Fed.

!!! Simulating System Hang (Entering Infinite Loop) !!!
LED will stop blinking. Watch for automatic reset in ~4 seconds...

--- System Booted / Reset Success ---
Initializing Watchdog Timer (4s timeout)...
Watchdog active. Starting 10-second normal operation loop.
Heartbeat 1s: WDT Fed.
...