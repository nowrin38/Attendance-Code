#include <Adafruit_Fingerprint.h>

HardwareSerial mySerial(2);  // Use UART2
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup() {
  Serial.begin(115200);
  delay(100);
  mySerial.begin(57600, SERIAL_8N1, 16, 17); // RX=16, TX=17

  Serial.println("\n\nFingerprint Enrollment");

  if (finger.verifyPassword()) {
    Serial.println(" Found fingerprint sensor!");
  } else {
    Serial.println(" Did not find fingerprint sensor :(");
    while (1);
  }
}

uint8_t readNumber() {
  while (!Serial.available());
  return Serial.parseInt();
}

void loop() {
  Serial.println("Enter ID # (1-127) to enroll: ");
  while (!Serial.available());
  id = readNumber();
  if (id == 0) return; // ID 0 not allowed

  Serial.print("Enrolling ID #"); Serial.println(id);

  while (!enrollFingerprint(id)) {
    Serial.println("Retrying enrollment...");
  }
  Serial.println("Enrollment complete!");
}

uint8_t enrollFingerprint(uint8_t id) {
  int p = -1;
  Serial.print("Place finger on sensor to enroll ID ");
  Serial.println(id);

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) Serial.print(".");
    else if (p != FINGERPRINT_OK) Serial.println("Error capturing image");
    delay(100);
  }
  Serial.println("\nImage taken");

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return 0;
  }
  Serial.println("Remove finger");
  delay(2000);

  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) Serial.print(".");
    else if (p != FINGERPRINT_OK) Serial.println("Error capturing image");
    delay(100);
  }
  Serial.println("\nImage taken");

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return 0;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("Fingerprints did not match");
    return 0;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.print("Stored fingerprint with ID ");
    Serial.println(id);
    return 1;
  } else {
    Serial.println("Error storing fingerprint");
    return 0;
  }
}
