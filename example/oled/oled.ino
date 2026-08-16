/*
   ESP32 Frequency Counter example
   The max frequency is 40MHz at 80MHz APB clock.
   Stable accurate pulse counting by hardware gating.
      GPIO4  counter input rising edge
      GPIO5  Self gate signal port
      GPIO16 32MHz test signal output
   Copyright (c) 2026, Siliconvalley4066
   Licenced under the GNU GPL Version 3.0
*/
#include <FreqCountESPgate.h>
#include <Tiny4kOLED.h>
#include "customfont.h"

#define GPIO_PIN 16  // should not assign GPIO #36 throough #39

byte selfont = 0;
uint16_t gatetime = 1000;

void setup() {
  Serial.begin(115200);
  delay(2000);
  while (!Serial)
    ;

  oled.begin(128, 64, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
  oled.setContrast(128);
  oled.setFont(FONT6X8);
  oled.clear();
  oled.setCursor(0, 0);
  oled.print("Frequency Counter");
  oled.on();

  FreqCount.pulse_test(GPIO_PIN, 32000000); // self test signal
  FreqCount.begin(gatetime);
  Serial.println("Frequency Counter Initialized");
}

void loop() {
  // uint32_t count_ovf;
  // uint16_t pulsecount;
  if (FreqCount.available()) {
    uint32_t count = FreqCount.read();
    if (gatetime < 500) count = avefilter(count);
    displayfreq(count);
    // pulsecount = FreqCount.pulseCountx;
    // count_ovf = FreqCount.count_ovfx;
    Serial.println(count);
    // Serial.print(pulsecount); Serial.print(" ");
    // Serial.println(count_ovf);
  }
}

void displayfreq(unsigned long freq) {
  static boolean blink = true;
  char s[16];
  int st, sp, dsp;
  sprintf(s, "%9d", freq);
  if (selfont == 0) {
    oled.setFont(FONT14X24DIGITS);
  } else if (selfont == 1) {
    oled.setFont(FONT14X24SEG);
  } else if (selfont == 2) {
    oled.setFont(FONT14X24NUMB);
  } else {
    oled.setFont(FONT14X32DIGITS);
  }
  int x = 0;
  if (s[0] != ' ') {
    st = 0; sp = 13; dsp = 5;
  } else {
    st = 1; sp = 14; dsp = 8;
  }
  for (int j = st; j < 9; ++j) {
    oled.setCursor(x, 0);
    oled.print(s[j]);
    x += sp;
    if (j == 2 || j == 5) {  // small thousand separator
      if (s[j] != ' ')
        oled.print(",");
      else
        oled.print(" ");
      x += dsp;
    }
  }
  oled.clearToEOL();
  oled.setFont(FONT6X8);
  oled.setCursor(0, 3);
  if (selfont < 3) {
    oled.print("Int 80MHz ");
    oled.print(gatetime > 500 ? "1sec" : "0.1s");
  }
  if (blink = !blink) oled.invertOutput(true);
  oled.setCursor(116, 3);
  oled.print("Hz");
  oled.invertOutput(false);
  digitalWrite(LED_BUILTIN, blink ? HIGH : LOW);
}

#define AVESIZE 10
unsigned long avefilter(unsigned long  frq) {
  static bool first = true;
  static unsigned long long sum = 0;
  static int pos = 0;
  static unsigned long ff[AVESIZE];
  unsigned long average;
  if (first) {
    sum = 0;
    for (int i = 0; i < AVESIZE; ++i) {
      ff[i] = frq;
      sum += frq;
    }
    average = frq;
    first = false;
  } else {
    sum = sum - ff[pos] + frq;
    ff[pos] = frq;
    if (++pos >= AVESIZE) pos = 0;
    average = sum / AVESIZE;
  }
  return (average);
}
