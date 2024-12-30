#include <EEPROM.h>
#include <DMXSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PushButtonTaps.h>
#include <KY040.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  //LCD Address 0x27
// Rotary encoder
#define CLK_PIN 2  // aka. A
#define DT_PIN 3   // aka. B
KY040 g_rotaryEncoder(CLK_PIN, DT_PIN);

const int BUTTON_PIN = 4;
PushButtonTaps pushBtn;

byte dmxvalues[512];

bool state = false;
int selectedvalue = 1;
int encodervalue = 1;
int encodervalueold = 1;

long backlightsettime = 20000;
long backlightlasttime = 0;

// ISR to handle the interrupts for CLK and DT
void ISR_rotaryEncoder() {
  // Process pin states for CLK and DT
  switch (g_rotaryEncoder.getRotation()) {
    case KY040::CLOCKWISE:
      //OnButtonRight();
      encodervalue++;
      break;
    case KY040::COUNTERCLOCKWISE:
      // OnButtonLeft();
      encodervalue--;
      break;
  }
}

void setup() {

  rotarysetup();
  DMXSerial.init(DMXController, 5);
  pushBtn.setButtonPin(BUTTON_PIN);
  lcdsetup();
  eepromload();
  updatedisplay();
  resetBacklightTimer();
  DMXUpdate();
}


void loop() {
  CheckButton();
  checkBacklight();

  if (encodervalue != encodervalueold) {
    updatedisplay();
    DMXUpdate();
    encodervalueold = encodervalue;
  }
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
  DMXUpdate();
}

void DoubleTap() {
  //Double Tap hat keine weitere Funktion
  updatedisplay();
  DMXUpdate();
}

void LongPress() {
  eepromUpdate();
  updatedisplay();
  DMXUpdate();
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
  // Set interrupts for CLK and DT
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), ISR_rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT_PIN), ISR_rotaryEncoder, CHANGE);
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