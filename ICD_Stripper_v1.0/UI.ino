#define CONTINUOUS_MODE 0
#define INTERLOOP_MODE 1
/*SETTING*/
#define SETTING_MODE 0
#define SETTING_RPM 1
#define SETTING_TIMER 2
#define SAVE_PRESET 3
#define SETTING_BACK 4
byte recent_mode , recent_rpm , recent_interloop_active_duration , recent_interloop_idle_duration;
unsigned int recent_timer;
time_t start_time, timer, active_duration , idle_duration , interloop_period ;

void timeout_display() {
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Time out!!");
  lcd.setCursor(4, 3);
  lcd.print("[*]Stop alarm");

}
void timeout_handle() {
  timeout_display();
  boolean keyPress = false;
  char pressed_key;
  byte beep_state = 0;
  unsigned long beep_duration = 750;
  unsigned long space_duration = 250;
  unsigned long beep_timer = millis();
  digitalWrite(BUZZER_PIN, LOW);
  while (true) {
    if (beep_state == 0 && millis() >= beep_timer + beep_duration) {
      beep_state = 1;
      beep_timer = millis();
      digitalWrite(BUZZER_PIN, HIGH);
    }
    else if (beep_state == 1 && millis() >= beep_timer + space_duration) {
      beep_state = 0;
      beep_timer = millis();
      digitalWrite(BUZZER_PIN, LOW);
    }
    if (!keyPress) {
      unsigned int key = keypad_read();
      char pressed_key_num = 0;
      for (char i = 0; i < 16 ; i++) {
        pressed_key_num += (key & (1 << i)) >> i;
        if ((key & (1 << i)) >> i == 1)pressed_key = i;
      }
      if (pressed_key_num == 1) {
        keyPress = true;
      }
    }
    else {
      unsigned int key = keypad_read();
      char pressed_key_num = 0;

      for (char i = 0; i < 16 ; i++) {
        pressed_key_num += (key & (1 << i)) >> i;
      }
      if (pressed_key_num == 0) {
        bip();

        keyPress = false;
        if (keypad_pin_map[pressed_key] == '*') {
          UI_state = MAINMENU_STATE;
          break;
        }
      }
    }
  }

}

void start_page_disp() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Initiating...");
  lcd.setCursor(4, 2);
  lcd.print("Please wait!");
  load_recent_value(0);
}

void running_disp() {
  time_t _now = now();
  time_t ellapsed_time = _now - start_time;
  time_t time_left = (start_time + timer) - _now;

  lcd.clear();
  lcd.setCursor(0, 0);
  if (recent_mode == CONTINUOUS_MODE) {
    lcd.print("Running:Continuous");
  }
  else if (recent_mode == INTERLOOP_MODE) {
    lcd.print("Running:Interloop");
    lcd.setCursor(0, 1);
    time_t interloop_phase = ellapsed_time % interloop_period;
    Serial.println(interloop_phase);
    if (interloop_phase < active_duration) {
      lcd.print("Active ");
      lcd.print(interloop_phase / 60);
      lcd.print(":");
      if (interloop_phase % 60 < 10)lcd.print("0");
      lcd.print(interloop_phase % 60);
      lcd.print(" /");
      lcd.print(recent_interloop_active_duration);
      lcd.print(" min");


    }
    else {
      lcd.print("Idle   ");
      lcd.print((interloop_phase - active_duration) / 60);
      lcd.print(":");
      if ((interloop_phase - active_duration)  % 60 < 10)lcd.print("0");
      lcd.print((interloop_phase - active_duration)  % 60);
      lcd.print(" / ");
      lcd.print(recent_interloop_active_duration);
      lcd.print(" min");
    }



  }
  lcd.setCursor(0, 2);

  if (recent_timer == 0)lcd.print("No timer");
  else {
    byte Hour = (time_left / 3600);
    byte Minute = (time_left / 60) % 60;
    byte Second = time_left % 60;
    lcd.print("Time left  ");
    if (Hour < 10)lcd.print("0");
    lcd.print(Hour);
    lcd.print(":");
    if (Minute < 10)lcd.print("0");
    lcd.print(Minute);
    lcd.print(":");
    if (Second < 10)lcd.print("0");
    lcd.print(Second);
  }
  lcd.setCursor(0, 3);
  lcd.print("[*]Pause");
}
void running_init() {
  start_time = now();
  timer = (unsigned long)recent_timer * 60;
  Serial.print("timer = ");
  Serial.println(timer);

  active_duration = recent_interloop_active_duration * 60;
  idle_duration = recent_interloop_idle_duration * 60;
  interloop_period = active_duration + idle_duration ;
}
void running_handle() {



  running_disp();
  time_t recent_time = now();

  if (recent_mode == CONTINUOUS_MODE) {
    /*run motor*/
    set_RPM(recent_rpm);
  }
  boolean keyPress = false;
  char pressed_key;
  boolean motor_running = false;
  while (true) {
    if (recent_time != now()) {
      recent_time = now();
      time_t _now = now();
      time_t ellapsed_time = _now - start_time;
      time_t time_left = (start_time + timer) - _now;
      if (_now >= start_time + timer && recent_timer != 0) {
        set_home();

        UI_state = TIMEOUT_STATE;
        break;
      }
      if (recent_mode == INTERLOOP_MODE) {
        time_t interloop_phase = ellapsed_time % interloop_period;
        if (interloop_phase < active_duration) {//active run motor
          if(!motor_running)set_RPM(recent_rpm);
          motor_running = true;
        }
        else {//idle stop motor
           if(motor_running)set_home();
           motor_running = false;
        }
      }
      running_disp();
    }
    if (!keyPress) {
      unsigned int key = keypad_read();
      char pressed_key_num = 0;
      for (char i = 0; i < 16 ; i++) {
        pressed_key_num += (key & (1 << i)) >> i;
        if ((key & (1 << i)) >> i == 1)pressed_key = i;
      }
      if (pressed_key_num == 1) {
        keyPress = true;
      }
    }
    else {
      unsigned int key = keypad_read();
      char pressed_key_num = 0;

      for (char i = 0; i < 16 ; i++) {
        pressed_key_num += (key & (1 << i)) >> i;
      }
      if (pressed_key_num == 0) {
        bip();

        keyPress = false;
        if (keypad_pin_map[pressed_key] == '*') {
          UI_state = PAUSE_STATE;
          break;
        }
      }
    }
  }

}
void pause_display() {
  time_t _now = now();
  time_t ellapsed_time = _now - start_time;
  time_t time_left = (start_time + timer) - _now;

  lcd.clear();
  lcd.setCursor(0, 0);
  if (recent_mode == CONTINUOUS_MODE) {
    lcd.print("Pause:Continuous");
  }
  else if (recent_mode == INTERLOOP_MODE) {
    lcd.print("Pause:Interloop");
    lcd.setCursor(0, 1);
    time_t interloop_phase = ellapsed_time % interloop_period;
    Serial.println(interloop_phase);
    if (interloop_phase < active_duration) {
      lcd.print("Active ");
      lcd.print(interloop_phase / 60);
      lcd.print(":");
      if (interloop_phase % 60 < 10)lcd.print("0");
      lcd.print(interloop_phase % 60);
      lcd.print(" /");
      lcd.print(recent_interloop_active_duration);
      lcd.print(" min");


    }
    else {
      lcd.print("Idle   ");
      lcd.print((interloop_phase - active_duration) / 60);
      lcd.print(":");
      if ((interloop_phase - active_duration)  % 60 < 10)lcd.print("0");
      lcd.print((interloop_phase - active_duration)  % 60);
      lcd.print(" / ");
      lcd.print(recent_interloop_active_duration);
      lcd.print(" min");
    }



  }
  lcd.setCursor(0, 2);

  if (recent_timer == 0)lcd.print("No timer");
  else {
    byte Hour = (time_left / 3600);
    byte Minute = (time_left / 60) % 60;
    byte Second = time_left % 60;
    lcd.print("Time left  ");
    if (Hour < 10)lcd.print("0");
    lcd.print(Hour);
    lcd.print(":");
    if (Minute < 10)lcd.print("0");
    lcd.print(Minute);
    lcd.print(":");
    if (Second < 10)lcd.print("0");
    lcd.print(Second);
  }
  lcd.setCursor(0, 3);
  lcd.print("[*]Exit    [#]Resume");
}

void pause_handle() {
  /*Stop motor*/
  set_RPM(0);
  time_t pause_time = now();
  pause_display();
  while (true) {
    char key = get_key();
    if (key == '*') {
      set_home();
      UI_state = MAINMENU_STATE;
      break;
    }
    else if (key == '#') {
      UI_state = RUNNING_STATE;
      start_time += now() - pause_time;
      break;
    }
  }

}
boolean running_confirmation() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hold [#] to comfirm");
  lcd.setCursor(0, 3);
  lcd.print("[*]Cancel");

  boolean holding = false;
  unsigned long holding_time = millis();
  byte progress_bar = 0;
  unsigned int  require_hoding_time = 1000;
  while (true) {

    unsigned int key = keypad_read();
    char pressed_key_num = 0;
    char pressed_key;

    for (char i = 0; i < 16 ; i++) {
      pressed_key_num += (key & (1 << i)) >> i;
      if ((key & (1 << i)) >> i == 1)pressed_key = i;
    }
    if (pressed_key_num == 1 && keypad_pin_map[pressed_key] == '#') {
      if (!holding) {
        holding = true;
        holding_time = millis();
      }
      else if (millis() - holding_time < require_hoding_time) {
        if (millis() - holding_time > progress_bar * require_hoding_time / 20) {
          lcd.setCursor(progress_bar, 1);
          lcd.print("#");
          progress_bar++;
        }
      }
      else break;
    }
    else {
      if (holding) {
        holding = false;
        lcd.setCursor(0, 1);
        lcd.print("                    ");
      }
      progress_bar = 0;
      if (keypad_pin_map[pressed_key]  == '*') {
        while (1) {//loop-until release all key
          unsigned int key = keypad_read();
          char pressed_key_num = 0;

          for (char i = 0; i < 16 ; i++) {
            pressed_key_num += (key & (1 << i)) >> i;
          }
          if (pressed_key_num == 0) {
            bip();

            return false;
          }
        }

      }
    }
  }
  running_start_beep();
  return true;
}
void running_start_beep() {
  digitalWrite(BUZZER_PIN, LOW);
  delay(75);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(25);
  digitalWrite(BUZZER_PIN, LOW);
  delay(75);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(25);
  digitalWrite(BUZZER_PIN, LOW);
  delay(75);
  digitalWrite(BUZZER_PIN, HIGH);

}
void main_menu_handle() {
  while (true) {
    main_menu_disp();
    char key = get_key();
    if (key == '*') {
      UI_state = SETTING_STATE;
      break;
    }
    else if (key == '#') {

      /*Place initial setup for running here*/

      if (running_confirmation()) {
        UI_state = RUNNING_STATE;
        running_init();
        break;
      }

    }
    else if (key >= 'A' && key <= 'D') { //Load preset
      load_preset_handle(key);
    }
  }
}
void main_menu_disp() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (recent_mode == CONTINUOUS_MODE) {
    lcd.print("Mode:Continuous");
  }
  else if (recent_mode == INTERLOOP_MODE) {
    lcd.print("Mode:Interloop");
    lcd.setCursor(0, 1);
    lcd.print("act/idl : ");
    lcd.print(recent_interloop_active_duration);

    lcd.print("/");
    lcd.print(recent_interloop_idle_duration);
  }
  lcd.setCursor(0, 2);
  lcd.print("RPM : ");
  lcd.print(recent_rpm);
  lcd.setCursor(9, 2);
  lcd.print("Timer ");
  if (recent_timer == 0)lcd.print("-");
  else {
    lcd.print(recent_timer / 60);
    lcd.print(":");
    lcd.print(recent_timer % 60);
  }
  lcd.setCursor(0, 3);
  lcd.print("[*]Setting  [#]Start");
}
void setting_handle() {
  byte page = 0;
  byte setting_stage = 0;
  while (true) {
    setting_disp(page);
    char key = get_key();
    if (key == '*') {
      page++;
      if (page > 4)page = 0;
      setting_stage = 0;
    }
    else if (key == '#') {
      switch (page) {
        case SETTING_MODE:
          setting_mode();
          break;
        case SETTING_RPM:
          setting_rpm();
          break;
        case SETTING_TIMER:
          setting_timer();
          break;
        case SAVE_PRESET:
          save_preset();
          break;
        case SETTING_BACK:
          UI_state = MAINMENU_STATE;
          return;
      }
    }
  }
}
void setting_disp(byte page) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setting");
  lcd.setCursor(0, 3);
  lcd.print("[*]Next [#]Enter");
  switch (page) {
    case SETTING_MODE:
      lcd.setCursor(5, 1);
      lcd.print("<Set mode>");
      break;
    case SETTING_RPM:
      lcd.setCursor(5, 1);
      lcd.print("<Set Speed>");
      break;
    case SETTING_TIMER:
      lcd.setCursor(5, 1);
      lcd.print("<Set Timer>");
      break;
    case SAVE_PRESET:
      lcd.setCursor(2, 1);
      lcd.print("<Save to Preset>");
      break;
    case SETTING_BACK:
      lcd.setCursor(7, 1);
      lcd.print("<Back>");
      break;
  }
}
void setting_mode() {
  while (true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Select mode");
    lcd.setCursor(4, 1);
    lcd.print("[1] Continuous");
    lcd.setCursor(4, 2);
    lcd.print("[2] Interloop");
    lcd.setCursor(0, 3);
    lcd.print("[*] Back");
    char key = get_key();
    if (key == '*')break;
    else if (key == '1') {
      recent_mode = CONTINUOUS_MODE;
      return;
    }
    else if (key == '2') {
      byte interloop_active_duration_set;
      lcd.noCursor();
      lcd.noBlink();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Active Duration");
      lcd.setCursor(0, 1);
      lcd.print("Enter 0-240 Minute");
      lcd.setCursor(0, 3);
      lcd.print("[*]Cancel [#]Confirm");
      lcd.setCursor(0, 2);
      lcd.print(">");
      lcd.cursor();
      lcd.blink();
      String value = "";
      boolean exit_setting = false;
      while (!exit_setting) {
        key = get_key();
        if (key >= '0' && key <= '9' && value.length() < 3) {
          value += key;
          lcd.print(key);
        }
        else if (key == '*') {
          if (value.length() > 0) {
            value = "";
            lcd.setCursor(0, 2);
            lcd.print(">   ");
            lcd.setCursor(1, 2);
          }
          else {
            lcd.noCursor();
            lcd.noBlink();
            exit_setting = true;
          }
        }
        else if (key == '#') {
          int num = value.toInt();
          if (num > 240) {
            value = "";
            lcd.setCursor(0, 2);
            lcd.print(">   ");
            lcd.setCursor(1, 2);
          } else {
            interloop_active_duration_set = num;
            break;
          }
        }
      }
      if (!exit_setting) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Idle Duration");
        lcd.setCursor(0, 1);
        lcd.print("Enter 0-240 Minute");
        lcd.setCursor(0, 3);
        lcd.print("[*]Cancel [#]Confirm");
        lcd.setCursor(0, 2);
        lcd.print(">");
        lcd.cursor();
        lcd.blink();
        value = "";
      }
      while (!exit_setting) {
        key = get_key();
        if (key >= '0' && key <= '9' && value.length() < 3) {
          value += key;
          lcd.print(key);
        }
        else if (key == '*') {
          if (value.length() > 0) {
            value = "";
            lcd.setCursor(0, 2);
            lcd.print(">   ");
            lcd.setCursor(1, 2);
          }
          else {
            lcd.noCursor();
            lcd.noBlink();
            exit_setting = true;
          }
        }
        else if (key == '#') {
          int num = value.toInt();
          if (num > 240) {
            value = "";
            lcd.setCursor(0, 2);
            lcd.print(">   ");
            lcd.setCursor(1, 2);
          } else {
            recent_interloop_active_duration = interloop_active_duration_set;
            recent_interloop_idle_duration = num;
            recent_mode = INTERLOOP_MODE;
            lcd.noCursor();
            lcd.noBlink();
            update_recent_value_to_EEPROM(0);
            return;
          }
        }
      }
    }
  }
}
void setting_rpm() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setting Speed");
  lcd.setCursor(0, 1);
  lcd.print("Enter 0-100 RPM");
  lcd.setCursor(0, 3);
  lcd.print("[*]Cancel [#]Confirm");
  lcd.setCursor(0, 2);
  lcd.print(">");
  lcd.cursor();
  lcd.blink();
  String value = "";
  while (true) {
    char key = get_key();
    if (key >= '0' && key <= '9' && value.length() < 3) {
      value += key;
      lcd.print(key);
    }
    else if (key == '*') {
      if (value.length() > 0) {
        value = "";
        lcd.setCursor(0, 2);
        lcd.print(">   ");
        lcd.setCursor(1, 2);
      }
      else {
        lcd.noCursor();
        lcd.noBlink();
        break;
      }
    }
    else if (key == '#') {
      int num = value.toInt();
      if (num > 100) {
        value = "";
        lcd.setCursor(0, 2);
        lcd.print(">   ");
        lcd.setCursor(1, 2);
      } else {
        recent_rpm = num;
        update_recent_value_to_EEPROM(0);
        lcd.noCursor();
        lcd.noBlink();
        break;
      }
    }
  }
}
void setting_timer() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setting Timer");
  lcd.setCursor(0, 1);
  lcd.print("Enter HH:MM Minute");
  lcd.setCursor(0, 3);
  lcd.print("[*]Cancel [#]Confirm");
  lcd.setCursor(8, 2);
  lcd.print("(0=no timer)");
  lcd.setCursor(0, 2);
  lcd.print(">  :  ");
  lcd.setCursor(1, 2);
  lcd.cursor();
  lcd.blink();
  String value = "";
  while (true) {
    char key = get_key();
    if (key >= '0' && key <= '9' && value.length() < 4) {
      value += key;
      lcd.print(key);
      if (value.length() == 2)lcd.print(":");
    }
    else if (key == '*') {
      if (value.length() > 0) {
        value = "";
        lcd.setCursor(0, 2);
        lcd.print(">  :  ");
        lcd.setCursor(1, 2);
      }
      else {
        break;
      }
    }
    else if (key == '#' && value.length() == 4) {
      byte Hour = value.substring(0, 2).toInt();
      byte Minute = value.substring(2, 4).toInt();
      if (Minute > 59)Minute = 59;
      recent_timer = Hour * 60 + Minute;

      Serial.println("timer = " + String(recent_timer));
      update_recent_value_to_EEPROM(0);
      break;
    }
  }
  lcd.noCursor();
  lcd.noBlink();
}
void save_preset() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Preset");
  lcd.setCursor(0, 1);
  lcd.print("Enter A,B,C or D");
  lcd.setCursor(0, 3);
  lcd.print("[*]Cancel [#]Confirm");
  lcd.setCursor(0, 2);
  lcd.print(">");
  lcd.cursor();
  lcd.blink();
  byte selected_preset = 5;
  while (true) {
    char key = get_key();
    if (key >= 'A' && key <= 'D') {
      lcd.setCursor(1, 2);
      lcd.print(key);
      selected_preset = key - 'A' + 1;
    }
    else if (key == '*') {
      break;
    }
    else if (key == '#' && selected_preset < 5) {
      update_recent_value_to_EEPROM(selected_preset);
      break;
    }
  }
  lcd.noCursor();
  lcd.noBlink();
}
void load_recent_value(byte block) {
  /*Load value from EEPROM*/
  recent_mode = EEPROM.read(RECENT_MODE_ADDR + 6 * block);
  recent_rpm = EEPROM.read(RECENT_RPM_ADDR + 6 * block);
  recent_interloop_active_duration = EEPROM.read(RECENT_INTERLOOP_ACTIVE_DURATION_ADDR + 6 * block);
  recent_interloop_idle_duration = EEPROM.read(RECENT_INTERLOOP_IDLE_DURATION_ADDR + 6 * block);
  EEPROM.get(RECENT_TIMER_ADDR + 6 * block, recent_timer);
  Serial.print("recent_timer = ");
  Serial.println(recent_timer);
  /*Handle unusual value*/
  if (recent_mode > 1) {
    EEPROM.write(RECENT_MODE_ADDR + 6 * block, 0);
    recent_mode = EEPROM.read(RECENT_MODE_ADDR + 6 * block);
  }
  if (recent_rpm > 100 || recent_rpm < 1 ) {
    EEPROM.write(RECENT_RPM_ADDR + 6 * block, 1);
    recent_rpm = EEPROM.read(RECENT_RPM_ADDR + 6 * block);
  }
  if (recent_interloop_active_duration > 240 || recent_interloop_active_duration < 1) {
    EEPROM.write(RECENT_INTERLOOP_ACTIVE_DURATION_ADDR + 6 * block, 1);
    recent_interloop_active_duration = EEPROM.read(RECENT_INTERLOOP_ACTIVE_DURATION_ADDR + 6 * block);
  }
  if (recent_interloop_idle_duration > 240 || recent_interloop_idle_duration < 1) {
    EEPROM.write(RECENT_INTERLOOP_IDLE_DURATION_ADDR + 6 * block, 1);
    recent_interloop_idle_duration = EEPROM.read(RECENT_INTERLOOP_IDLE_DURATION_ADDR + 6 * block);
  }
  if (recent_timer > 6039 ) {
    EEPROM.put(RECENT_TIMER_ADDR + 6 * block, 0);
    EEPROM.get(RECENT_TIMER_ADDR + 6 * block, recent_timer);
  }
}
void update_recent_value_to_EEPROM(byte block) {
  Serial.println("update_recent_value_to_EEPROM");
  Serial.println("block = "+String(block));
  EEPROM.update(RECENT_MODE_ADDR + 6 * block, recent_mode);
  EEPROM.update(RECENT_RPM_ADDR + 6 * block, recent_rpm);
  EEPROM.update(RECENT_INTERLOOP_ACTIVE_DURATION_ADDR + 6 * block, recent_interloop_active_duration);
  EEPROM.update(RECENT_INTERLOOP_IDLE_DURATION_ADDR + 6 * block, recent_interloop_idle_duration);
  int ee_recent_timer;
  EEPROM.get(RECENT_TIMER_ADDR + 6 * block, ee_recent_timer);
  if (ee_recent_timer != recent_timer)EEPROM.put(RECENT_TIMER_ADDR + 6 * block, recent_timer);
}
void load_preset_handle(char preset) {
  if (!(preset >= 'A' && preset <= 'D'))return;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Load preset ");
  lcd.print(preset);
  lcd.print(" ?");
  lcd.setCursor(0, 3);
  lcd.print("[*]Cancel [#]Confirm");
  while (true) {
    char key = get_key();
    if (key == '*')break;
    else if (key == '#') {
      load_recent_value(preset - 'A' + 1);
      update_recent_value_to_EEPROM(0);
      break;
    }
  }
}
void bip() {
  digitalWrite(BUZZER_PIN, LOW);
  delay(10);
  digitalWrite(BUZZER_PIN, HIGH);
}
void beep() {
  digitalWrite(BUZZER_PIN, LOW);
  delay(100);
  digitalWrite(BUZZER_PIN, HIGH);
}
