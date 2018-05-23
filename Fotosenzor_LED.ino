// VARS
int ON_OFF_BTN = 3;
int CAL_BTN = 4;
int LED_LIGHT = 2;
int LED_CHECK = 13;
int FOTO_SENSOR = A0;
int ON_OFF_STATE;
int CAL_STATE;
int SENSOR_READ;
int DEVIDOR = 1;
float VOLTAGE;
bool LED_STATE = false;
bool SENS_CAL = false;

// POBRISE SERIAL SCREEN
void CLEAR_SCR() {
  for (int i=0; i<100; i++) {
   Serial.print("\n");
  }
}

// LED FUNKCIJA - VKLOPI LED
void LED_STATUS_FUNC() {
  digitalWrite(LED_LIGHT, LOW);
  digitalWrite(LED_CHECK, HIGH);
  LED_STATE = true;
  CLEAR_SCR();
  Serial.println(">> LANDING OSVETLJEN <<");
  Serial.print("NAPETOST: ");
  Serial.println(VOLTAGE);
  delay(1000);
}

// DOKLER JE LED VKLOPLJENA PREVERJA SAMO GUMB ZA IZKLOP LED LUCI, CE GUMB NI PRITISNJEN CAKA V LOOPU
void ON_OFF_STATUS_FUNC() {
  while(LED_STATE) {
    ON_OFF_STATE = digitalRead(ON_OFF_BTN);
    if(ON_OFF_STATE == LOW) {
      CLEAR_SCR();
      digitalWrite(LED_LIGHT, HIGH);
      digitalWrite(LED_CHECK, LOW);
      LED_STATE = false;
      Serial.println(">> LED OFF <<");
      delay(1000);
    } else {
      delay(100);
    }
  }
}

// KALIBRACIJA FOTOUPORA - spreminja DEVIDOR dokler vrednost ni med 1.3 in 1.7 ali pa nastavi DEVIDOR na 1023 ce senzor daje napetost 0
void AUTO_CAL_FUNC() {
  for(int i = 0; i < 5; i++) {
    digitalWrite(LED_CHECK, HIGH);
    delay(100);
    digitalWrite(LED_CHECK, LOW);
    delay(250);
  }
  digitalWrite(LED_CHECK, HIGH);
  delay(250);
  digitalWrite(LED_CHECK, LOW);
  DEVIDOR = 1;
  CLEAR_SCR();
  Serial.println(">> KALIBRACIJA SENZORJA ... <<");
  Serial.print("ZACETNA NAPETOST: ");
  Serial.println(VOLTAGE);
  Serial.print("ZACETNA NAPETOST SENZOR: ");
  Serial.println(SENSOR_READ);
  Serial.print("ZACETNI DELITELJ: ");
  Serial.println(DEVIDOR);
  delay(5000);
  while(!SENS_CAL) {
    SENSOR_READ = analogRead(FOTO_SENSOR);
    VOLTAGE = SENSOR_READ * (DEVIDOR / 1023.0);
    CLEAR_SCR();
    Serial.println(">> KALIBRACIJA <<");
    Serial.println(SENSOR_READ);
    while(VOLTAGE > 1.7) {
      if(SENSOR_READ == 0) {
        DEVIDOR = 1023;
        break;
      }
      DEVIDOR -= 1;
      SENSOR_READ = analogRead(FOTO_SENSOR);
      VOLTAGE = SENSOR_READ * (DEVIDOR / 1023.0);
      delay(10);
    }
    while(VOLTAGE < 1.3) {
      if(SENSOR_READ == 0) {
        DEVIDOR = 1023;
        break;
      }
      DEVIDOR += 1;
      SENSOR_READ = analogRead(FOTO_SENSOR);
      VOLTAGE = SENSOR_READ * (DEVIDOR / 1023.0);
      delay(10);
    }
    if((VOLTAGE < 1.7 && VOLTAGE > 1.3) || SENSOR_READ == 0) {
      for(int i = 0; i < 10; i++) {
        Serial.print(". ");
        delay(250);
      }
      digitalWrite(LED_CHECK, LOW);
      SENS_CAL = true;
      CLEAR_SCR();
      Serial.println(">> SENZOR KALIBRIRAN <<");
      Serial.print("NAPETOST: ");
      Serial.println(VOLTAGE);
      Serial.print("NAPETOST SENZOR: ");
      Serial.println(SENSOR_READ);
      Serial.print("DELITELJ: ");
      Serial.println(DEVIDOR);
      for(int i=0; i < 3; i++) {
        digitalWrite(LED_CHECK, HIGH);
        delay(1000);
        digitalWrite(LED_CHECK, LOW);
        delay(250);
      }
      CLEAR_SCR();
      break;
    }
    delay(100);
  }
}

// SETUP zacetnih var in kalibracija fotoupora
void setup() {
  Serial.begin(9600);
  pinMode(ON_OFF_BTN, INPUT_PULLUP);
  pinMode(CAL_BTN, INPUT_PULLUP);
  pinMode(LED_LIGHT, OUTPUT);
  pinMode(LED_CHECK, OUTPUT);
  digitalWrite(LED_LIGHT, HIGH);
  digitalWrite(LED_CHECK, LOW);
  ON_OFF_STATE = digitalRead(ON_OFF_BTN);
  SENSOR_READ = analogRead(FOTO_SENSOR);
  VOLTAGE = SENSOR_READ * (DEVIDOR / 1023.0);
  AUTO_CAL_FUNC();
}

// glavni loop
void loop() {
  CAL_STATE = digitalRead(CAL_BTN);
  ON_OFF_STATE = digitalRead(ON_OFF_BTN);
  SENSOR_READ = analogRead(FOTO_SENSOR);
  VOLTAGE = SENSOR_READ * (DEVIDOR / 1023.0);
  // preveri ce je gumb za kalibracijo pritisnjen
  delay(50);
  if(CAL_STATE == LOW) {
    SENS_CAL = false;
    AUTO_CAL_FUNC();
    delay(10);
  }
  // preveri ce je fotoupor osvetljen in gre v funkcijo za vklop LED luci
  if(VOLTAGE > 2) {
    LED_STATUS_FUNC();
    delay(10);
  }
  // preveri ce je LED prizgana in gre v loop
  if(LED_STATE) {
    ON_OFF_STATUS_FUNC();
    delay(10);
  }
  // preveri ce je gumb za izklop pritisnjen gre v funkcijo gumba za izklop
  if(ON_OFF_STATE == LOW) {
    ON_OFF_STATUS_FUNC();
    delay(1000);
  }
  delay(10);
}
