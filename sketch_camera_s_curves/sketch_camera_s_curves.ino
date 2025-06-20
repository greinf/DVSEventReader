const int BACKLIGHT_PIN = 3;
const int PULSE_PIN = 4;
const int PH_DIODE_PIN = A0;
unsigned long phase_start_time {};
unsigned long phase_end_time {};
unsigned long delta_phase_time {};


void setup() {
  Serial.begin(9600);
  pinMode(BACKLIGHT_PIN, OUTPUT);
  pinMode(PULSE_PIN, OUTPUT);
  pinMode(PH_DIODE_PIN, INPUT);
  digitalWrite(BACKLIGHT_PIN, LOW);
  digitalWrite(PULSE_PIN, LOW);
}



void loop() {
  if (Serial.available() > 0) {
    char received = Serial.read();
    if (received == 'P') {
      int diode_Val_B; //Backround Voltage Value 
      int diode_Val_P; //Pulse Voltage Value
      digitalWrite(BACKLIGHT_PIN, HIGH);   // Backround lighting on
      delay(500); //stabilize
      diode_Val_B = analogRead(PH_DIODE_PIN); //get radiation from Photodiode
      phase_start_time = micros();  //time critical steps
      digitalWrite(PULSE_PIN, HIGH);  // Puls on
      diode_Val_P = analogRead(PH_DIODE_PIN);     //measure pulse + backround radiation 
      digitalWrite(PULSE_PIN, LOW);  //shut of both pulse and backlight
      digitalWrite(BACKLIGHT_PIN, LOW);  
      phase_end_time = micros(); //get time 
      delay(1000); 
      int critical_time_delta = phase_end_time - phase_start_time;
      Serial.print(critical_time_delta);
      Serial.print(",");
      Serial.print(diode_Val_B);
      Serial.print(",");
      Serial.println(diode_Val_P); //needed to for '\n' at the end of a run.
      delay(500);
    }
  }
}

