#include <Arduino.h>
#include <ArduinoBLE.h>
#include <HardwareBLESerial.h>

HardwareBLESerial &bleSerial = HardwareBLESerial::getInstance();

// Game Grid Settings
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 5;

// Player and Target Positions
int playerX = 2;
int playerY = 2;
int targetX = 7;
int targetY = 3;
int score = 0;

// RGB LED Pins (Adjust pins if your specific board uses different ones)
const int RED_LED   = 22;
const int GREEN_LED = 23;
const int BLUE_LED  = 24;

void generateTarget() {
  targetX = random(0, GRID_WIDTH);
  targetY = random(0, GRID_HEIGHT);
}

void clearLEDs() {
  // Sets all LEDs to HIGH (OFF for active-low configuration)
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
}

void flashLED(int pin) {
  digitalWrite(pin, LOW);  // Turn ON
  delay(50);               // Short blink duration
  digitalWrite(pin, HIGH); // Turn OFF
}

void drawGame() {
  Serial.print("\033[2J");
  Serial.print("\033[H");
  
  Serial.print("=== BLUEFRUIT GAMEPAD === SCORE: ");
  Serial.println(score);

  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      if (x == playerX && y == playerY) {
        Serial.print("O ");
      } else if (x == targetX && y == targetY) {
        Serial.print("X ");
      } else {
        Serial.print(". ");
      }
    }
    Serial.println();
  }
  Serial.println("[1] Reset Game   [2] Move Target");
  Serial.println("===============================");
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize LED Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  clearLEDs();

  randomSeed(analogRead(0));

  if (!bleSerial.beginAndSetupBLE("BLUEFRUIT GAME")) {
    while (1);
  }

  drawGame();
}

void loop() {
  bleSerial.poll();

  if (bleSerial.available() >= 4) {
    char startChar = bleSerial.read();
    char typeChar  = bleSerial.read();
    
    if (startChar == '!' && typeChar == 'B') {
      char buttonId = bleSerial.read(); 
      char pressed  = bleSerial.read(); 

      if (pressed == '1') {
        bool moved = false;
        
        // --- ARROW BUTTONS (Movement) ---
        if (buttonId == '5' && playerY > 0) {       
          playerY--; moved = true;
        } 
        else if (buttonId == '6' && playerY < GRID_HEIGHT - 1) { 
          playerY++; moved = true;
        } 
        else if (buttonId == '7' && playerX > 0) {  
          playerX--; moved = true;
        } 
        else if (buttonId == '8' && playerX < GRID_WIDTH - 1) { 
          playerX++; moved = true;
        }
        
        // --- NUMBER BUTTONS (Special Actions) ---
        else if (buttonId == '1') {  // Button 1: Reset the game
          score = 0;
          playerX = 2;
          playerY = 2;
          generateTarget();
          bleSerial.println("Game Reset!");
          flashLED(RED_LED); // Red flash for reset
        }
        else if (buttonId == '2') {  // Button 2: Teleport target manually
          generateTarget();
          bleSerial.println("Target Shifted!");
          flashLED(BLUE_LED); // Blue flash for manual movement actions
        }

        // If a movement arrow was pressed, blink Blue
        if (moved) {
          flashLED(BLUE_LED);
        }

        // Check if player caught the target
        if (playerX == targetX && playerY == targetY) {
          score++;
          bleSerial.print("SCORE ACCUMULATED: ");
          bleSerial.println((int64_t)score);
          generateTarget();
          flashLED(GREEN_LED); // Green flash for score increase
        }

        drawGame();
      }
    }
  }
}