#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// encoder
#define encoderPinA 5
#define encoderPinB 6
#define encoderPinC 7

volatile long pos = 0;
unsigned long t1;
int stato = 0;
bool FIRST = true;
int eA = LOW;
int eB = LOW;
int UP, DN, CLIC;
int bt, pvbt;

void setup() {
  Serial.begin(115200);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(encoderPinC, INPUT_PULLUP);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();
}

void loop() {
  doEncoder();
  bt = !digitalRead(encoderPinC);
  if ((millis() - t1) > 200) {
    if (pos > 0) UP = 1;
    else if (pos < 0) DN = 1;
    pos = 0;
    t1 = millis();
  }
  
  if (bt && !pvbt) {
    CLIC = 1;
  }

  switch(stato) {
    case 0:
      menu0();
      break;
    case 1:
      menu1();
      break;  
    case 2:
      menu2();
      break;  
    case 3:
      menu3();
      break;  
  }

  pvbt = bt;
  CLIC = 0;
  UP = 0;
  DN = 0;
}

void doEncoder() {
  bool aggiorna = false;
  eB = digitalRead(encoderPinB);
  if (digitalRead(encoderPinA) != eA) {
    eA = !eA;
    // look for a low-to-high on channel A
    if (eA == HIGH) {
      // check channel B to see which way encoder is turning
      if (eB == LOW) {
        //pos = pos + 1;         // CW
        pos = 1;
      } else {
        //pos = pos - 1;         // CCW
        pos = -1;
      }
    } else { // look for a high-to-low on channel A
      // check channel B to see which way encoder is turning
      if (eB == HIGH) {
        //pos = pos + 1;          // CW
        pos = 1;
      } else {
        //pos = pos - 1;          // CCW
        pos = -1;
      }
    }  
  }
}

void go(int st) {
  FIRST = true; 
  stato = st;
}

void oledPrint(char *str) {
  display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.cp437(true);                 // Use full 256 char 'Code Page 437' font
  display.write(str);
  display.display();
}

void menu0() {
  if (FIRST) {
    Serial.println("menu 0");
    oledPrint("menu 0");
    FIRST = false; 
  }
  if (UP) go(1);
  //if (DN) go(1);
  if (CLIC) Serial.println("Selected 0");
}

void menu1() {
  if (FIRST) {
    Serial.println("menu 1");
    oledPrint("menu 1");
    FIRST = false; 
  }
  if (UP) go(2);
  if (DN) go(0);
  if (CLIC) Serial.println("Selected 1");
}

void menu2() {
  if (FIRST) {
    Serial.println("menu 2");
    oledPrint("menu 2");
    FIRST = false; 
  }
  if (UP) go(3);
  if (DN) go(1);
  if (CLIC) Serial.println("Selected 2");
}

void menu3() {
  if (FIRST) {
    Serial.println("menu 3");
    oledPrint("menu 3");
    FIRST = false; 
  }
  //if (UP) go(0);
  if (DN) go(2);
  if (CLIC) Serial.println("Selected 3");
}
