#include <FreqCountESPgate.h>

#define GPIO_PIN 16  // should not assign GPIO #36 throough #39

void setup() {
  Serial.begin(115200);
  delay(2000);
  FreqCount.pulse_test(GPIO_PIN, 32000000); // self test signal 32MHz
  FreqCount.begin(1000);
  Serial.println("Frequency Counter Initialized");
}

void loop() {
  if (FreqCount.available()) {
    Serial.println(FreqCount.read());
  }
}
