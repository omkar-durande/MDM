#include <Arduino_APDS9960.h>

// Define RGB pins for Nano 33 BLE Sense
// Note: LOW = ON, HIGH = OFF for these built-in LEDs
#define RED_LED   LEDR
#define GREEN_LED LEDG
#define BLUE_LED  LEDB

// Variables for the counter mechanism
int counter = 0;
bool handWasClose = false; 
const int proximityThreshold = 100; // Adjust if needed (0 to 255)
const int targetCount = 10;
bool targetReached = false;

void setup() {
  Serial.begin(9600);
  while (!Serial); 

  // Initialize the sensor
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor.");
    while (1);
  }

  // Set LED pins as outputs
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  // Initial State: Turn Red ON, Green and Blue OFF
  digitalWrite(RED_LED, LOW);   // LOW turns it ON
  digitalWrite(GREEN_LED, HIGH); // HIGH turns it OFF
  digitalWrite(BLUE_LED, HIGH);  // HIGH turns it OFF

  Serial.println("Sensor ready. Red LED is ON. Bring hand close to count.");
  Serial.println("-------------------------------------------------");
}

void loop() {
  if (APDS.proximityAvailable()) {
    int proximity = APDS.readProximity();

    // Check if hand is closer than the threshold
    if (proximity >= proximityThreshold) {
      
      if (!handWasClose) {
        handWasClose = true; // Lock state

        // If we were waiting at the finished state (Green LED), reset everything now
        if (targetReached) {
          counter = 1; // Start counting from 1 for the new round
          targetReached = false;
          
          // Switch LEDs back: Red ON, Green OFF
          digitalWrite(RED_LED, LOW);
          digitalWrite(GREEN_LED, HIGH);
          
          Serial.println("\n--- New Cycle Started! ---");
        } else {
          // Normal increment during the counting phase
          counter++;
        }
        
        Serial.print("Hand Detected! Current Count: ");
        Serial.println(counter);
        
        // Check if we hit 10
        if (counter >= targetCount && !targetReached) {
          Serial.println("\n*** 10 Repetitions Finished! ***");
          Serial.println("Blue LED is now ON. Hover hand again to restart.\n");
          
          targetReached = true;
          
          // Switch LEDs: Red OFF, Green ON
          digitalWrite(RED_LED, HIGH);
          digitalWrite(GREEN_LED, LOW);
        }
      }
    } else {
      // Hand has been moved away, unlock for the next wave
      handWasClose = false;
    }
  }

  delay(30); // Stability delay
}