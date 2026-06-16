#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Servo.h>

// RFID
#define SS_PIN 10
#define RST_PIN A3
MFRC522 rfid(SS_PIN, RST_PIN);

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// RTC
RTC_DS3231 rtc;

// Servo
Servo lockServo;

#define SERVO_PIN 8
#define BUZZER A0

// LEDs
#define RED_LED 2
#define ORANGE_R 5
#define ORANGE_G 6
#define PURPLE_R 7
#define PURPLE_B 9
#define GREEN_LED 3
#define BLUE_LED 4

// RFID UID (01 02 03 04)
byte allowedUID[] = {0x01, 0x02, 0x03, 0x04};

bool medicineActive = false;
bool medicineTaken = false;
unsigned long reminderStart = 0;
unsigned long medicineStart = 0;
int lastReminderHour = -1;

void setup() {
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // run once then comment

  lcd.init();
  lcd.backlight();

  lockServo.attach(SERVO_PIN);
  lockServo.write(0);

  pinMode(BUZZER, OUTPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(ORANGE_R, OUTPUT);
  pinMode(ORANGE_G, OUTPUT);
  pinMode(PURPLE_R, OUTPUT);
  pinMode(PURPLE_B, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  allLEDsOff();

  lcd.setCursor(0,0);
  lcd.print("Smart Medicine");
  lcd.setCursor(0,1);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();

// ONLY show time when no medicine alert is active
if (!medicineActive && !medicineTaken) {
  displayTime(now);
}

checkSchedule(now);

  if (medicineActive) {

  if (!medicineTaken) {
    tone(BUZZER, 1000);
  }

  checkRFID();

    if (!medicineTaken && millis() - reminderStart > 10000) {
      missedDose();
    }
  }

  if (medicineTaken && millis() - medicineStart > 5000) {
    finishCycle();
  }

  delay(300);
}

// ---------------- DISPLAY TIME ----------------
void displayTime(DateTime now) {
  lcd.setCursor(0,0);
  lcd.print("Time ");

  if (now.hour() < 10) lcd.print("0");
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
}

// ---------------- SCHEDULE ----------------
void checkSchedule(DateTime now) {
  if (medicineActive) return;

  static unsigned long lastChange = 0;
  static int ledStep = 0;

  if (millis() - lastChange >= 30000) {
    lastChange = millis();

    activate(ledStep);

    ledStep++;

    if (ledStep > 4) {
      ledStep = 0;
    }
  }
}

// ---------------- ACTIVATE ----------------
void activate(int type) {
  medicineActive = true;
  medicineTaken = false;
  reminderStart = millis();

  allLEDsOff();

  switch(type) {
    case 0:
      digitalWrite(RED_LED, HIGH);
      break;

    case 1:
       digitalWrite(GREEN_LED, HIGH);
      break;

    case 2:
      digitalWrite(BLUE_LED, HIGH);
      break;

    case 3:
      digitalWrite(ORANGE_R, HIGH);
      digitalWrite(ORANGE_G, HIGH);
      break;

    case 4:
      digitalWrite(PURPLE_R, HIGH);
      digitalWrite(PURPLE_B, HIGH);
      break;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MEDICINE TIME!");
}

// ---------------- RFID CHECK ----------------
void checkRFID() {

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  // 🔍 DEBUG: show UID in Serial Monitor
  Serial.print("UID:");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(" ");
    Serial.print(rfid.uid.uidByte[i]);
  }
  Serial.println();

  if (isAuthorized()) {
    noTone(BUZZER);

    lockServo.write(90);

    lcd.clear();
    lcd.print("Access Granted");

    delay(500);

    lcd.clear();
    lcd.print("Welcome Kurt!");

    delay(500);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("time for your");
    lcd.setCursor(0,1);
    lcd.print("Medicine!");


    medicineTaken = true;
    medicineStart = millis();
  }
  else {
    lcd.clear();
    lcd.print("Access Denied!");
    delay(1500);
  }

  rfid.PICC_HaltA();
}

// ---------------- UID CHECK ----------------
bool isAuthorized() {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != allowedUID[i]) {
      return false;
    }
  }
  return true;
}

// ---------------- MISSED DOSE ----------------
void missedDose() {
  noTone(BUZZER);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("You missed");
  lcd.setCursor(0,1);
  lcd.print("your Medication!");
  delay(3000);
  resetSystem();
}

// ---------------- FINISH ----------------
void finishCycle() {
  lockServo.write(0);
  noTone(BUZZER);
  resetSystem();
}

// ---------------- RESET ----------------
void resetSystem() {
  medicineActive = false;
  medicineTaken = false;
  allLEDsOff();
  lcd.clear();
}

// ---------------- LED OFF ----------------
void allLEDsOff() {
  digitalWrite(RED_LED, LOW);
  digitalWrite(ORANGE_R, LOW);
  digitalWrite(ORANGE_G, LOW);
  digitalWrite(PURPLE_R, LOW);
  digitalWrite(PURPLE_B, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
}
