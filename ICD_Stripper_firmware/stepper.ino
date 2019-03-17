long stepper_pos = 0;
byte stepper_dir = 0;//0 for CW , 1 for CCW
boolean stepper_pos_mode  = false;
void set_RPM(int RPM) {
  float pulse_per_minute = float(RPM) * PULSE_PER_REV;
  float time_micros = ((1000000.0 * 60.0) / pulse_per_minute) / 2.0;
  if (RPM > 0 && RPM <= 100)Timer1.initialize(time_micros);
  else if(RPM == 0)Timer1.stop();
  //Serial.println(pulse_per_minute);
  //Serial.println(time_micros);
}
void stepper_init() {
  pinMode(PUL_POSITIVE_PIN , OUTPUT);
  pinMode(PUL_NEGATIVE_PIN , OUTPUT);
  pinMode(DIR_POSITIVE_PIN , OUTPUT);
  pinMode(DIR_NEGATIVE_PIN , OUTPUT);
  pinMode(ENA_POSITIVE_PIN , OUTPUT);
  pinMode(ENA_NEGATIVE_PIN , OUTPUT);
  pinMode(END_STOP_PIN, INPUT_PULLUP);
  stepper_disable();
  Timer1.stop();
  Timer1.attachInterrupt(run_stepper);
}
void stepper_enable() {
  digitalWrite(ENA_POSITIVE_PIN, LOW);
}
void stepper_disable() {
  digitalWrite(ENA_POSITIVE_PIN, HIGH);
}
void run_stepper() {
  static byte pul_status = 0;
  digitalWrite(PUL_POSITIVE_PIN, pul_status);
  pul_status = !pul_status;
  if (pul_status == 0) {
    if (stepper_dir == 0)stepper_pos++;
    else stepper_pos--;
  }
}
void CW_mode() {
  digitalWrite(DIR_POSITIVE_PIN, LOW);
  stepper_dir = 0;
}
void CCW_mode() {
  digitalWrite(DIR_POSITIVE_PIN, HIGH);
  stepper_dir = 1;
}
void set_home() {
  stepper_stop();
  unsigned long _delay = RPM_to_micros(10);
  CCW_mode();

  //_delay = RPM_to_micros(5);
  while (digitalRead(END_STOP_PIN) == LOW) {
    digitalWrite(PUL_POSITIVE_PIN, HIGH);
    delayMicroseconds(_delay);
    digitalWrite(PUL_POSITIVE_PIN, LOW);
    delayMicroseconds(_delay);
  }
  delay(100);
  //CCW_mode();
  while (digitalRead(END_STOP_PIN) == HIGH) {
    digitalWrite(PUL_POSITIVE_PIN, HIGH);
    delayMicroseconds(_delay);
    digitalWrite(PUL_POSITIVE_PIN, LOW);
    delayMicroseconds(_delay);
  }
  stepper_stop();
  stepper_pos = 0;
  delay(100);
  while (digitalRead(END_STOP_PIN) == LOW) {
    digitalWrite(PUL_POSITIVE_PIN, HIGH);
    delayMicroseconds(_delay);
    digitalWrite(PUL_POSITIVE_PIN, LOW);
    delayMicroseconds(_delay);
    stepper_pos--;
  }
  stepper_stop();
  delay(100);
  stepper_goto(stepper_pos / 2, 10);
  delay(100);
  stepper_pos = 0;
}
void stepper_goto(long pos, int RPM) {
  unsigned long _delay = RPM_to_micros(RPM);
  if (pos - stepper_pos > 0) {
    CW_mode();
    for (int i = stepper_pos; i < pos; i++) {
      digitalWrite(PUL_POSITIVE_PIN, HIGH);
      delayMicroseconds(_delay);
      digitalWrite(PUL_POSITIVE_PIN, LOW);
      delayMicroseconds(_delay);
    }
  }
  else {
    CCW_mode();
    for (int i = stepper_pos; i > pos; i--) {
      digitalWrite(PUL_POSITIVE_PIN, HIGH);
      delayMicroseconds(_delay);
      digitalWrite(PUL_POSITIVE_PIN, LOW);
      delayMicroseconds(_delay);
    }
  }
}
void stepper_stop() {
  Timer1.stop();
}
unsigned long RPM_to_micros(int RPM) {
  float pulse_per_minute = float(RPM) * PULSE_PER_REV;
  return ((1000000.0 * 60.0) / pulse_per_minute) / 2.0;
}



