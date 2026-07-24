#include <PDM.h>

const int LED = LED_BUILTIN;

static const char channels = 1;
static const int frequency = 16000;

// Defining our thresholds
const int MEDIUM_THRESHOLD = 400;  // Anything above this is Medium
const int HIGH_THRESHOLD = 1500;   // Anything above this is High

short sampleBuffer[512];
volatile int samplesRead;

// Track the maximum value observed during the print interval window
int currentMaxVolume = 0;

// Timing variables for non-blocking LED blink and text output
unsigned long ledTurnOffTime = 0;
bool isLedOn = false;

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 1000; // Evaluates sound every 300ms

void onPDMdata();

void setup() {
  Serial.begin(9600);

  pinMode(LED, OUTPUT);

  PDM.onReceive(onPDMdata);

  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}

void loop() {
  // Read from the mic buffer as fast as data becomes available
  if (samplesRead) {
    for (int i = 0; i < samplesRead; i++) {
      int value = abs(sampleBuffer[i]);
      // Track the absolute highest peak during this interval
      if (value > currentMaxVolume) {
        currentMaxVolume = value;
      }
    }
    samplesRead = 0; // Clear immediately for the next microphone batch
  }

  // Evaluate and print the sound level category every 300ms
  if (millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis();

    Serial.print("Volume: ");
    Serial.print(currentMaxVolume);
    Serial.print(" -> ");

    // Categorize the sound level
    if (currentMaxVolume >= HIGH_THRESHOLD) {
      Serial.println("[ AAE Band Kar Band Kar! ] 🚨");
      
      // Turn on LED for a longer duration for high noise
      digitalWrite(LED, HIGH);
      isLedOn = true;
      ledTurnOffTime = millis() + 400; 
      
    } else if (currentMaxVolume >= MEDIUM_THRESHOLD && currentMaxVolume < HIGH_THRESHOLD) {
      Serial.println("[ MEDIUM ] 🗣️");
      
      // Quick blink for medium noise
      digitalWrite(LED, HIGH);
      isLedOn = true;
      ledTurnOffTime = millis() + 150;
      
    } else {
      Serial.println("[ Once More Once More ] 👌😁");
    }

    // Reset the tracker for the next 300ms window
    currentMaxVolume = 0;
  }

  // Non-blocking LED Turn Off mechanism
  if (isLedOn && (millis() >= ledTurnOffTime)) {
    digitalWrite(LED, LOW);
    isLedOn = false;
  }
}

void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2;
}