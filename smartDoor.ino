#include <WiFi.h> // Optional, for Serial Monitor debug only
#include <Adafruit_Fingerprint.h>
#include <Keypad.h>
#include <HardwareSerial.h>

// Define pins
#define BUZZER 15      // Buzzer positive pin connected to GPIO15 (D15)
#define RELAY 4        // Relay control pin for door lock

// Fingerprint setup
HardwareSerial mySerial(1);  // Use Serial1 for fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Keypad configuration
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'9', '6', '3', 'C'},
  {'8', '5', '2', 'B'},
  {'7', '4', '1', 'A'},
  {'*', '0', '#', 'D'}  // D will act as "Enter" key
};

byte rowPins[ROWS] = {33, 25, 26, 27};    // Connect to R1, R2, R3, R4
byte colPins[COLS] = {14, 12, 13, 32};    // Connect to C1, C2, C3, C4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Keypad password
String password = "1234";
String inputPassword = "";

unsigned long lastFingerprintCheck = 0;
const unsigned long fingerprintInterval = 2000; // 2 seconds

void setup() {
  Serial.begin(115200);
  mySerial.begin(57600, SERIAL_8N1, 16, 17);  // Fingerprint: RX=16, TX=17

  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(RELAY, LOW);   // Keep door locked initially
  digitalWrite(BUZZER, LOW);  // Buzzer off initially

  Serial.println("System started.");

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor detected.");
  } else {
    Serial.println("Fingerprint sensor not found. Halting.");
    while (1);
  }
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);

    if (key == 'D') {  // Enter key
      if (inputPassword == password) {
        Serial.println("Correct password! Unlocking door.");
        unlockDoor();
      } else {
        Serial.println("Wrong password!");
        buzz(2);  // Buzz twice for wrong password
      }
      inputPassword = "";  // Clear after attempt
    } else if (key == '*') {
      inputPassword = "";
      Serial.println("? Password input cleared.");
    } else {
      inputPassword += key;
    }
  }

  // Check fingerprint every 2 seconds
  if (millis() - lastFingerprintCheck >= fingerprintInterval) {
    lastFingerprintCheck = millis();
    checkFingerprint();
  }
}

void checkFingerprint() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return;

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprint matched!");

    if (finger.fingerID == 1) {
      Serial.println("Student: Nowrin");
    } else if (finger.fingerID == 2) {
      Serial.println("Student: Jacky");
    }else if (finger.fingerID == 3) {
      Serial.println("Student: Mohua");
    }else if (finger.fingerID == 4) {
      Serial.println("Student: Faiza");
    }

    unlockDoor();
  } else {
    Serial.println("Fingerprint not recognized.");
    buzz(2);
  }
}

void unlockDoor() {
  digitalWrite(RELAY, HIGH);   // Unlock door
  digitalWrite(BUZZER, HIGH);  // Buzzer on
  delay(3000);                 // Wait 3 seconds
  digitalWrite(RELAY, LOW);   // Lock door again
  digitalWrite(BUZZER, LOW);  // Buzzer off
}

void buzz(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }
}
