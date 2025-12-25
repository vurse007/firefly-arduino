const int PWM_PIN = 3; //output pin for voltage

// ems parameters
// ~
const int FREQUENCY = 35; //35hz -> 35 pulses per second
const unsigned long PULSE_PERIOD = 1000000/FREQUENCY; //in microseconds
const int PULSE_WIDTH = 200; //each pulse lasts 200 microseconds
const int INTENSITY = 128; //pwm power level (0-255) -> 128 = 50% power

const int BURST_ON_TIME = 5000; //rapid pulsations characterized by parameters above for 5000 ms
const int BURST_OFF_TIME = 2000; //rest time of 2000 ms -> all impulses are off


//timekeepers + statekeepers
unsigned long lastPulseTime = 0; //unsigned long -> 32 bits only positive -> can store between 0 & ~4bn -> MICROSECONDS
unsigned long burstStartTime = 0; //MILISECONDS -> in 5 or 2 second intervals, less resolution needed
bool burstActive = true; //starts true so stimulation starts at program start
bool pulseState = false; //tracks individual pulse state to prevent going over PULSE_WIDTH length


void setup() {
  // put your setup code here, to run once:
  pinMode(PWM_PIN, OUTPUT); //configure p3 as signal output
  Serial.begin(9600); //start communication channel with computer @ 9600 baud (speed)
  
  Serial.println("ems waveform generator started."); //serial prints
  Serial.println("printing parameters:");
  Serial.println("frequency (hz): "); Serial.print(FREQUENCY);
  Serial.println("pulse width (microseconds): "); Serial.print(PULSE_WIDTH);
  Serial.println("intensity (0-255): "); Serial.print(INTENSITY);
  Serial.println("burst run time (ms): "); Serial.print(BURST_ON_TIME);
  Serial.println("burst rest time (ms): "); Serial.print(BURST_OFF_TIME);

  burstStartTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMicros = micros();
  unsigned long currentMillis = millis();

  // top layer burst period power on and shut off logic:
  if (burstActive == true && ((currentMillis - burstStartTime) >= BURST_ON_TIME)) { //if the burst has been running and its time to turn it off then:
    burstActive = false; //update statekeeper
    burstStartTime = currentMillis; //resets timekeeper to use for 2s rest period
    analogWrite(PWM_PIN, 0); //completely shut off
    Serial.println("burst off");
  }
  else if (burstActive == false && ((currentMillis - burstStartTime) >= BURST_OFF_TIME)) { //if the burst is off and rest time is over then:
    burstActive = true; //update statekeeper
    burstStartTime = currentMillis; //restart timekeeper for 5 second on period
    Serial.println("burst on");
  }

  // bottom layer individual pulse generator during burst on period:
  if (burstActive == true) {
    if ((currentMicros - lastPulseTime) >= PULSE_PERIOD) { //has enough time passed between pulses?
      lastPulseTime = currentMicros;
      pulseState = true;
      analogWrite(PWM_PIN, INTENSITY);
    }

    if (pulseState == true && ((currentMicros - lastPulseTime) >= PULSE_WIDTH)) { //has enough time passed for us to turn off signal and end pulse?
      pulseState = false;
      analogWrite(PWM_PIN, 0);
    }
  }
}
