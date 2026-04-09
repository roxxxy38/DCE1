#include <WiFi.h>
#include <OOCSI.h>

OOCSI oocsi;

const char* ssid     = "iotroam";
const char* password = "DIAdVNXGWE";

const int pinGreenLed = 2;
const int pinRedLed   = 38;
const int pinButton1  = 37;
const int pinButton2  = 4;
const int pinButton3  = 36;
const int pinButton4  = 5;
const int pinButton5  = 35;
const int pinButton6  = 6;
const int pinReset    = 12;
const int pinEnter    = 10;

String enteredCode = "";
const String correctCode = "6251";
bool waitingForSignal = true;

void processMessage() {
  bool start = oocsi.getBool("start", false);
  if (start) {
    waitingForSignal = false;
    Serial.println("Signal received! Calculator puzzle starting.");
  }
}

void checkButton(int pin, String digit) {
  if (digitalRead(pin) == LOW) {
    enteredCode += digit;
    Serial.print(digit);
    while (digitalRead(pin) == LOW);
    delay(50);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  pinMode(pinGreenLed, OUTPUT);
  pinMode(pinRedLed,   OUTPUT);
  digitalWrite(pinGreenLed, LOW);
  digitalWrite(pinRedLed,   LOW);

  pinMode(pinButton1, INPUT_PULLUP);
  pinMode(pinButton2, INPUT_PULLUP);
  pinMode(pinButton3, INPUT_PULLUP);
  pinMode(pinButton4, INPUT_PULLUP);
  pinMode(pinButton5, INPUT_PULLUP);
  pinMode(pinButton6, INPUT_PULLUP);
  pinMode(pinReset,   INPUT_PULLUP);
  pinMode(pinEnter,   INPUT_PULLUP);

  oocsi.connect("TEAM12_CALCULATOR", "oocsi.id.tue.nl", processMessage);
  Serial.println("Waiting for OOCSI signal...");
}

void loop() {
  oocsi.check();
  if (waitingForSignal) return;

  checkButton(pinButton1, "1");
  checkButton(pinButton2, "2");
  checkButton(pinButton3, "3");
  checkButton(pinButton4, "4");
  checkButton(pinButton5, "5");
  checkButton(pinButton6, "6");

  if (digitalRead(pinReset) == LOW) {
    enteredCode = "";
    digitalWrite(pinGreenLed, LOW);
    digitalWrite(pinRedLed,   LOW);
    Serial.println("\nReset! Memory cleared.");
    while (digitalRead(pinReset) == LOW);
    delay(50);
  }

  if (digitalRead(pinEnter) == LOW) {
    Serial.print("\nFinal Code: ");
    Serial.println(enteredCode);

    if (enteredCode == correctCode) {
      Serial.println("Correct!");
      digitalWrite(pinGreenLed, HIGH);
      digitalWrite(pinRedLed,   LOW);
      // Send solved signal to central module
      oocsi.newMessage("TEAM12_CENTRAL_MODULE");
      oocsi.addBool("calc_solved", true);
      oocsi.sendMessage();
    } else {
      Serial.println("Wrong!");
      digitalWrite(pinRedLed,   HIGH);
      digitalWrite(pinGreenLed, LOW);
      enteredCode = "";
    }

    while (digitalRead(pinEnter) == LOW);
    delay(50);
  }
}