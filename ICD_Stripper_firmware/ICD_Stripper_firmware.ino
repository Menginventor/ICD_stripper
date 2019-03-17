#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <TimeLib.h>

/*Stepper setup*/
#define PUL_POSITIVE_PIN 23
#define PUL_NEGATIVE_PIN 25

#define DIR_POSITIVE_PIN 27
#define DIR_NEGATIVE_PIN 29

#define ENA_POSITIVE_PIN 31
#define ENA_NEGATIVE_PIN 33

#define PULSE_PER_REV 6400//Please check driver configuration.

#define END_STOP_PIN 35
/*-------------*/
/*Key pads setup*/
#define KEYPAD_PIN {51,49,47,45,43,41,39,37} // c1,c2,c3,c4,r1,r2,r3,r4
/*--------------*/
#define BUZZER_PIN 53

/*EEPROM Map*/
/*each block*/
#define RECENT_MODE_ADDR 0
#define RECENT_RPM_ADDR 1
#define RECENT_INTERLOOP_ACTIVE_DURATION_ADDR 2
#define RECENT_INTERLOOP_IDLE_DURATION_ADDR 3
#define RECENT_TIMER_ADDR 4
#define RECENT_TIMER_ADDR_H 4
#define RECENT_TIMER_ADDR_L 5
/*
    [recent used]    INDEX 0 - 5
      [preset A]     INDEX 6 - 11
      [preset B]     INDEX 12 - 17
      [preset C]     INDEX 18 - 23
      [preset D]     INDEX 24 - 29
*/
/*-------------*/


/*UI State*/
#define MAINMENU_STATE 0
#define SETTING_STATE 1
#define RUNNING_STATE 2
#define PAUSE_STATE 3
#define TIMEOUT_STATE 4
byte UI_state = MAINMENU_STATE;
//byte UI_state = TIMEOUT_STATE;
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(9600);
  lcd.begin();
  digitalWrite(BUZZER_PIN, HIGH);
  pinMode(BUZZER_PIN, OUTPUT);

  keypad_init();
  start_page_disp();
  stepper_init();
  stepper_enable();
  set_home();
  CCW_mode();

  main_menu_disp();
  beep();
}

void loop() {
  switch (UI_state) {
    case MAINMENU_STATE:
      Serial.println("Main menu");
      main_menu_handle();
      CCW_mode();
      break;
    case SETTING_STATE:
      Serial.println("Setting");
      setting_handle();
      break;
    case RUNNING_STATE:
      Serial.println("Running");
      running_handle();
      break;
    case PAUSE_STATE:
      Serial.println("Pause");
      pause_handle();
      break;
    case TIMEOUT_STATE:
      Serial.println("Time out");
      timeout_handle();
      break;
  }
}


