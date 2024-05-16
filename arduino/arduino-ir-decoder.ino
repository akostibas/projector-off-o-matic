#include <Keyboard.h>
#include <Adafruit_DotStar.h>

// Setup for IR system
const int IR_INPUT_PIN = 2;
const int KEY_POWER_OFF = 238;
const bool DEBUG = false;

// Setup our fancy RGB DotStar
#define NUMPIXELS 1
#define DATAPIN   7
#define CLOCKPIN  8
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

// Global state
int previousState = 0;
long count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(IR_INPUT_PIN, INPUT);
  
  // Turn off DotStar
  strip.begin();
  strip.show();
}

// the loop routine runs over and over again forever:
void loop() {
  if (detectPowerOff()) {
    powerOff();
  }

  delay(1);
}

void powerOff() {
  debug("Power off the projector!");
  
  Keyboard.begin();
  Keyboard.press(KEY_POWER_OFF);
  delay(100);
  Keyboard.releaseAll();

  blinkDotStar(0xFF0000, 1000);
}

void debug(String message) {
  if (DEBUG) {
    Serial.println(message);
  }
}

// Blinks the RGB dotstar for a set amount of time. Blocking operation!
void blinkDotStar(uint32_t color, int milliseconds) {
  strip.setPixelColor(0, color);
  strip.show();
  
  delay(milliseconds);
  
  strip.setPixelColor(0, 0x000000);
  strip.show();
}