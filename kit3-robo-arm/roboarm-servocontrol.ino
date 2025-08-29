// Arduino: continuous-serial-recv.ino
#include <Servo.h>

Servo myServo;
int angle = 90;

char inBuf[10];
uint8_t inIdx = 0;

void setup() {
  Serial.begin(9600);
  myServo.attach(9);
  myServo.write(angle);
  Serial.print("Ready. Angle:");
  Serial.println(angle);
}

void loop() {
  // non-blocking serial read into buffer until newline
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inIdx > 0) {
        inBuf[inIdx] = '\0';
        int val = atoi(inBuf);    // parse integer
        inIdx = 0;

        // Validate range 0..180
        if (val >= 0 && val <= 180) {
          // Always echo what we received (useful for logging on the web page)
          Serial.print("Set: ");
          Serial.println(val);

          // Move only if different (avoid jitter & unnecessary writes)
          if (val != angle) {
            angle = val;
            myServo.write(angle);
            Serial.print("Moved to: ");
            Serial.println(angle);
          }
        } else {
          Serial.print("BadVal: ");
          Serial.println(val);
        }
      }
    } else {
      // collect digits (ignore other chars)
      if (inIdx < (sizeof(inBuf) - 1)) {
        if ((c >= '0' && c <= '9') || c == '-') inBuf[inIdx++] = c;
        // else ignore
      }
    }
  }

  // other non-blocking tasks could go here
}
