//moving control input
const byte L_PULSE_PIN = 5;
const byte R_PULSE_PIN = 6;
const byte L_DIR_PIN = 7;
const byte R_DIR_PIN = 9;
const byte TEST_PIN = 13;

const unsigned long DEFAULT_WAIT_TIME = 1000;

volatile unsigned long last_accept_time = 0;
volatile byte car_stop = 1;
volatile byte raw_speed = 255;
volatile byte speed_level = 0;
volatile byte pulse_level = 0;
volatile byte move_status = 0;

volatile unsigned int pulse_count = 0;

unsigned int delay_time[4] = {11817, 5461, 3788, 3297};
//unsigned int pulse_left[16] =  {41, 40, 43, 36, 40, 36, 36, 35, 34, 33, 35, 30, 34, 30, 29, 35};
//unsigned int pulse_right[16] = {29, 30, 34, 30, 35, 33, 34, 35, 36, 36, 40, 36, 43, 40, 41, 35}; //last one is for error case
unsigned int pulse_left[16] =  {48, 45, 42, 40, 35, 35, 33, 32, 30, 30, 30, 29, 26, 25, 24, 32};
unsigned int pulse_right[16] = {24, 25, 26, 29, 30, 30, 30, 32, 33, 35, 35, 40, 42, 45, 48, 32}; //last one is for error case

void setup() {
  //pin modes
  pinMode(L_PULSE_PIN, OUTPUT);
  pinMode(R_PULSE_PIN, OUTPUT);
  pinMode(L_DIR_PIN, OUTPUT);
  pinMode(R_DIR_PIN, OUTPUT);
  pinMode(TEST_PIN, OUTPUT);
  Serial.begin(9600);
  last_accept_time = millis();
}

void loop() {
  if(Serial.available() > 0){
    //read serial value
    raw_speed = Serial.read();
    last_accept_time = millis();
    digitalWrite(TEST_PIN, HIGH);
    car_stop = 0;
    //status and pulse get
    move_status = (raw_speed >> 4) & 0xf;
    pulse_level = raw_speed & 0xf;
    pulse_count = 0;
    //set speed level and direction
    switch(move_status){
    case 0:
      digitalWrite(L_DIR_PIN, HIGH);
      digitalWrite(R_DIR_PIN, HIGH);
      car_stop = 1;
      break;
    case 1: case 2: case 3: case 4: //forward
      speed_level = move_status - 1;
      digitalWrite(L_DIR_PIN, HIGH);
      digitalWrite(R_DIR_PIN, LOW);
      break;
    case 5: case 6: case 7: case 8: //backward
      speed_level = move_status - 5;
      digitalWrite(L_DIR_PIN, LOW);
      digitalWrite(R_DIR_PIN, HIGH);
      break;
    case 9: case 10: case 11: //clockwise
      speed_level = move_status - 9;
      digitalWrite(L_DIR_PIN, HIGH);
      digitalWrite(R_DIR_PIN, HIGH);
      break;
    case 12: case 13: case 14: //anti-clockwise
      speed_level = move_status - 12;
      digitalWrite(L_DIR_PIN, LOW);
      digitalWrite(R_DIR_PIN, LOW);
      break;
    default:
      digitalWrite(L_DIR_PIN, HIGH);
      digitalWrite(R_DIR_PIN, HIGH);
      car_stop = 1;
      break;
    }
  }else{
    if(millis() - last_accept_time > DEFAULT_WAIT_TIME){
      //over time
      digitalWrite(TEST_PIN, LOW);
      digitalWrite(L_PULSE_PIN, HIGH);
      digitalWrite(R_PULSE_PIN, HIGH);
      digitalWrite(L_DIR_PIN, HIGH);
      digitalWrite(R_DIR_PIN, HIGH);
      car_stop = 1;
    }
  }

  if(car_stop == 1){
    return;
  }
  //emit pulse if car is not in stop status
  if(pulse_count % pulse_left[pulse_level] == 0) digitalWrite(L_PULSE_PIN, LOW);
  else if(pulse_count % (pulse_left[pulse_level] / 2) == 0)digitalWrite(L_PULSE_PIN, HIGH);
  
  if(pulse_count % pulse_right[pulse_level] == 0) digitalWrite(R_PULSE_PIN, LOW);
  else if(pulse_count % (pulse_right[pulse_level] / 2) == 0)digitalWrite(R_PULSE_PIN, HIGH);
  pulse_count++;
  delayMicroseconds(delay_time[speed_level] / pulse_left[pulse_level] / pulse_right[pulse_level]);
}
