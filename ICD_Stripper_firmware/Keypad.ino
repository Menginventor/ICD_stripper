byte keypad_pin [] = KEYPAD_PIN;
char keypad_pin_map[] = {'D', '#', '0', '*', 'C', '9', '8', '7', 'B', '6', '5', '4', 'A', '3', '2', '1'};

char get_key() {
  char pressed_key;
  while (1) {
    unsigned int key = keypad_read();
    char pressed_key_num = 0;

    for (char i = 0; i < 16 ; i++) {
      pressed_key_num += (key & (1 << i)) >> i;
      if ((key & (1 << i)) >> i == 1)pressed_key = i;
    }
    if (pressed_key_num == 1) {
      break;
    }
  }
  while (1) {//loop-until release all key
    unsigned int key = keypad_read();
    char pressed_key_num = 0;

    for (char i = 0; i < 16 ; i++) {
      pressed_key_num += (key & (1 << i)) >> i;
    }
    if (pressed_key_num == 0) {
      bip();
      return keypad_pin_map[pressed_key];
    }
  }
}
void print_key_pressed(unsigned int key) {
  for (char i = 0; i < 16; i++) {
    if ((key & (1 << i)) >> i == 1) {
      Serial.print(keypad_pin_map[i]);
      if (i < 15)Serial.print(",");
    }
  }
  Serial.println();
}
void keypad_init() {
  for (char i = 0; i < 4; i++) {
    pinMode(keypad_pin[i], INPUT_PULLUP);
  }
  for (char i = 4; i < 8; i++) {
    pinMode(keypad_pin[i], INPUT);
    digitalWrite(keypad_pin[i], LOW);
  }
}

unsigned int keypad_read() {
  unsigned int result = 0;
  for (char i = 0; i < 4; i++) {
    result |= (read_row(i)) << i * 4;
  }
  return result;
}

unsigned int read_row(byte row) {
  unsigned int result = 0;
  for (char i = 4; i < 8; i++) {
    if (row + 4 != i)pinMode(keypad_pin[i], INPUT);
    else pinMode(keypad_pin[i], OUTPUT);
  }

  for (char i = 0; i < 4; i++) {
    result |= (!digitalRead(keypad_pin[i])) << i;
  }
  return result;
}

void print_bin(unsigned int data) {
  for (char i = 15; i >= 0; i--) {
    Serial.print((data & (1 << i)) >> i);
    if (i > 0)Serial.print(",");
    else Serial.println();
  }
}

