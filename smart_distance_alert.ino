#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define trigPin 9
#define echoPin 10

#define redLED 7
#define yellowLED 6
#define greenLED 5
#define buzzer 8

LiquidCrystal_I2C lcd(0x27, 16, 2);

// timing
unsigned long previousMillis = 0;
unsigned long detectStart = 0;

const long interval = 300;
bool blinkState = false;
bool objectDetected = false;

int distance;

// 🔹 Distance averaging
int getDistance() {
  long sum = 0;

  for (int i = 0; i < 5; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    int d = duration * 0.034 / 2;

    sum += d;
    delay(5);
  }

  return sum / 5;
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("System Init...");
  delay(1000);
  lcd.clear();

  Serial.begin(9600);
}

void loop() {

  distance = getDistance();

  // LCD Line 1 (clean UI)
  lcd.setCursor(0, 0);
  lcd.print("D:");
  lcd.print(distance);
  lcd.print("cm        ");

  unsigned long currentMillis = millis();

  // Detect object within 30 cm
  if (distance < 30) {

    if (!objectDetected) {
      detectStart = currentMillis;
      objectDetected = true;
    }

    // Only trigger after 2 seconds
    if (currentMillis - detectStart >= 2000) {

      // 🔴 DANGER
      if (distance < 10) {
        lcd.setCursor(0,1);
        lcd.print("Status: DANGER ");

        digitalWrite(redLED, HIGH);
        digitalWrite(yellowLED, LOW);
        digitalWrite(greenLED, LOW);

        tone(buzzer, 1200); // strong alert
      }

      // 🟡 WARNING
      else {
        lcd.setCursor(0,1);
        lcd.print("Status: WARN   ");

        digitalWrite(redLED, LOW);
        digitalWrite(greenLED, LOW);

        if (currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          blinkState = !blinkState;

          digitalWrite(yellowLED, blinkState);

          if (blinkState) {
            tone(buzzer, 800);
          } else {
            noTone(buzzer);
          }
        }
      }
    }

  } else {
    // 🟢 SAFE
    objectDetected = false;

    lcd.setCursor(0,1);
    lcd.print("Status: SAFE   ");

    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(greenLED, HIGH);

    noTone(buzzer);
  }

  // Serial logging (professional touch)
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

}