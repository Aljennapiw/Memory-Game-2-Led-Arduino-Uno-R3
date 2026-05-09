#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int greenLed = 3;
const int redLed = 4;
const int greenButton = 2;
const int redButton = 5;
const int buzzer = 9;

int sequence[100]; 
int level = 0;
bool gameStarted = false;

// VARIABLES FOR DYNAMIC SPEED
int initialDelay = 800; // Speed at Level 1 (in milliseconds)
int currentDelay;

void setup() {
  lcd.init();
  lcd.backlight();
  
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(redButton, INPUT_PULLUP);
  
  randomSeed(analogRead(A0)); 
  
  lcd.setCursor(0, 0);
  lcd.print("  MEMORY GAME");
  lcd.setCursor(0, 1);
  lcd.print("PRESS TO START!");
}

void loop() {
  if (!gameStarted) {
    if (digitalRead(greenButton) == LOW || digitalRead(redButton) == LOW) {
      gameStarted = true;
      level = 0;
      currentDelay = initialDelay; // Reset speed
      lcd.clear();
      lcd.print("READY... GO!");
      tone(buzzer, 1000, 200);
      delay(1000);
    }
    return;
  }

  sequence[level] = random(3, 5); 
  level++;

  // SPEED CALCULATION: Reduce delay as level increases
  // We subtract 50ms every level, but keep it at minimum 150ms
  currentDelay = initialDelay - (level * 50);
  if (currentDelay < 150) currentDelay = 150; 

  lcd.clear();
  lcd.print("Level: "); lcd.print(level);
  lcd.setCursor(0, 1);
  lcd.print("Speed: "); lcd.print(currentDelay); lcd.print("ms");
  delay(1000);

  // SHOW SEQUENCE WITH DYNAMIC SPEED
  for (int i = 0; i < level; i++) {
    digitalWrite(sequence[i], HIGH);
    tone(buzzer, sequence[i] == 3 ? 400 : 800, currentDelay / 2); 
    delay(currentDelay); // This is where the magic happens!
    digitalWrite(sequence[i], LOW);
    delay(currentDelay / 2);
  }

  lcd.setCursor(0, 1);
  lcd.print("YOUR TURN!      ");

  for (int i = 0; i < level; i++) {
    int buttonPressed = 0;
    unsigned long startTime = millis(); 

    while (buttonPressed == 0) {
      if (millis() - startTime > 5000) {
        gameOver("TIME'S UP!");
        return;
      }
      if (digitalRead(greenButton) == LOW) buttonPressed = 3;
      if (digitalRead(redButton) == LOW) buttonPressed = 4;
    }
    
    digitalWrite(buttonPressed, HIGH);
    tone(buzzer, buttonPressed == 3 ? 400 : 800, 100);
    while (digitalRead(greenButton) == LOW || digitalRead(redButton) == LOW);
    delay(100);
    digitalWrite(buttonPressed, LOW);

    if (buttonPressed != sequence[i]) {
      gameOver("WRONG BUTTON!");
      return; 
    }
  }
  delay(1000);
}

void gameOver(String reason) {
  lcd.clear();
  lcd.print("GAME OVER!");
  lcd.setCursor(0, 1);
  lcd.print(reason);
  
  tone(buzzer, 150, 800); 
  for(int j=0; j<5; j++) {
    digitalWrite(redLed, HIGH);
    delay(100);
    digitalWrite(redLed, LOW);
    delay(100);
  }
  gameStarted = false;
  delay(2000);
  lcd.clear();
  lcd.print("PRESS TO");
  lcd.setCursor(0, 1);
  lcd.print("PLAY AGAIN...");
}