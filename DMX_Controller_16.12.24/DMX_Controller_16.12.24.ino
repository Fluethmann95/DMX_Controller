#include <EEPROM.h>
#include <DMXSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PushButtonTaps.h>
#include "KY040rotary.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
KY040 Rotary(4, 5, 2);               //ck, dt, sw

const int BUTTON_PIN = 2;
PushButtonTaps pushBtn;

byte dmxvalues[512];

bool state = false;
int selectedvalue = 1;
int encodervalue = 1;

long backlightsettime = 10000;
long backlightlasttime = 0;

void setup() {
  DMXSerial.init(DMXController);
  pushBtn.setButtonPin(BUTTON_PIN);
  lcdsetup();
  rotarysetup();
  eepromload();
  updatedisplay();

  resetBacklightTimer();
}


void loop() {
  CheckButton();
  Rotary.Process(millis());
  DMXUpdate();
  checkBacklight();
}

void homeview() {
  lcd.clear();
  if (encodervalue > 512) {
    encodervalue = 1;
  } else if (encodervalue < 1) {
    encodervalue = 512;
  }

  lcd.setCursor(0, 0);
  lcd.print("DMX Channel: ");
  lcd.setCursor(13, 0);
  lcd.print(encodervalue);
  lcd.setCursor(0, 1);
  lcd.print("Value: ");
  lcd.setCursor(13, 1);
  lcd.print(dmxvalues[encodervalue]);

  lcd.setCursor(0, 2);
  lcd.print("Long Press  = SAVE");
  lcd.setCursor(0, 3);
  lcd.print("Short Press = MENU");
}

void menuview() {

  if (encodervalue > 255) {
    encodervalue = 0;
  } else if (encodervalue < 0) {
    encodervalue = 255;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DMX Channel: ");
  lcd.setCursor(13, 0);
  lcd.print(selectedvalue);
  lcd.setCursor(0, 1);
  lcd.print("Set Value: ");
  lcd.setCursor(13, 1);
  lcd.print(encodervalue);
  dmxvalues[selectedvalue] = encodervalue;
  DMXUpdate();
  lcd.setCursor(0, 2);
  lcd.print("Long Press  = SAVE");
  lcd.setCursor(0, 3);
  lcd.print("Short Press = MENU");
}

void resetBacklightTimer() {
  lcd.backlight();
  backlightlasttime = millis();
}

void checkBacklight() {
  if ((backlightlasttime + backlightsettime) < millis()) {
    lcd.noBacklight();
  }
}
void DMXUpdate() {
  for (int i = 1; i < 513; i++) {
    DMXSerial.write(i, dmxvalues[i]);
  }
}
void SingleTap() {
  if (!state) {
    state = !state;
    selectedvalue = encodervalue;

    encodervalue = dmxvalues[selectedvalue];
  } else {
    state = !state;
    encodervalue = selectedvalue;
  }

  updatedisplay();
}

void DoubleTap() {
  //Double Tap hat keine weitere Funktion
  updatedisplay();
}

void LongPress() {
  eepromUpdate();
  updatedisplay();
}

void eepromUpdate() {
  for (int i = 0; i < 512; i++) {
    EEPROM.update(i, dmxvalues[i]);
  }
  lcd.noBacklight();
  delay(200);
  lcd.backlight();
}
void updatedisplay() {
  if (!state) {
    homeview();

  } else {
    menuview();
  }
  resetBacklightTimer();
}

void rotarysetup() {
  Rotary.Begin();
  Rotary.OnButtonLeft(OnButtonLeft);
  Rotary.OnButtonRight(OnButtonRight);
}

void OnButtonLeft(void) {
  encodervalue--;
  updatedisplay();
}
void OnButtonRight(void) {
  encodervalue++;
  updatedisplay();
}

void CheckButton() {
  byte tap = pushBtn.checkButtonStatus();
  if (tap != 0) {
    switch (tap) {
      case 1:  //Single Tap
        SingleTap();
        break;
      case 2:  //Double Tap
        DoubleTap();
        break;
      case 3:  //Long Press
        LongPress();
        break;
      default:
        //Error, kommt nicht vor...
        break;
    }
  }
}

void eepromload() {
  for (int i = 0; i < 512; i++) {
    EEPROM.get(i, dmxvalues[i]);
  }
}
void lcdsetup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("DMX Controller");
  lcd.setCursor(1, 1);
  lcd.print("Initialisierung...");
  lcd.setCursor(4, 2);
  lcd.print(__DATE__);
  lcd.setCursor(2, 3);
  lcd.print("Fabian Fluethmann");
  delay(3000);
  lcd.clear();
}