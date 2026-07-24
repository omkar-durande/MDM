#include <Arduino_LSM9DS1.h>

#define LED_RED   LEDR
#define LED_GREEN LEDG
#define LED_BLUE  LEDB

const float TILT_THRESHOLD = 0.3;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // LEDs OFF
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.println("IMU Ready");
}

void loop() {

  float x, y, z;

  if (IMU.accelerationAvailable()) {

    IMU.readAcceleration(x, y, z);

    // Send JSON to Python
    Serial.print("{\"x\":");
    Serial.print(x);
    Serial.print(",\"y\":");
    Serial.print(y);
    Serial.print(",\"z\":");
    Serial.print(z);
    Serial.println("}");

    // Tilt control
    if (y > TILT_THRESHOLD) {

      // Tilt LEFT -> Green
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_BLUE, HIGH);

    } 
    else if (y < -TILT_THRESHOLD) {

      // Tilt RIGHT -> Red
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_BLUE, HIGH);

    } 
    else {

      // Flat -> Blue
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, HIGH);
    }
  }

  delay(100);
}