
/*
 * Wiper firmware control
 * menymp
 * Dec 20 2024 
 * 
 * allows the control of a wiper motor for an old wagoneer with
 * a dc 12 electric motor.
 */

/* user inputs */
const int START_MODE_INPUT = 2;
const int INTERVAL_POT_ANALOG_INPUT = A0;

/* sensor inputs */
const int ZERO_POSITION_INPUT = 3;

/* outputs */
const int MOTOR_SPEED_OUT = 4;

/* constants */
const int BASE_MINIMUN_INTERVAL = 1000;
const int HOMING_DUTY_CYCLE = 140;
const int OPERATIONAL_DUTY_CYCLE = 250;
const int STOP_DUTY_CYCLE = 0;

const int START_ENABLED = 1;
const int IN_ZERO_POSITION = 1;

int previous_state = 0;
int rest_count = 0;

/* FSM STATES */
typedef enum {
  STOP,
  RUNNING,
  REST,
  BRAKIN
}SYSTEM_STATE;
SYSTEM_STATE state = STOP;

#define STOP_STATE_MSG    "STOP_STATE"
#define RUNNING_STATE_MSG "RUNNING_STATE"
#define REST_STATE_MSG    "REST_STATE"
#define BRAKING_STATE_MSG "BRAKIN_STATE"
#define PANIC_STATE_MSG   "PANIC_STATE"

void setup() {
  // put your setup code here, to run once:
  pinMode(START_MODE_INPUT, INPUT_PULLUP);
  pinMode(ZERO_POSITION_INPUT, INPUT_PULLUP);
  // pinMode(MOTOR_SPEED_OUT, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int knob_position = analogRead(INTERVAL_POT_ANALOG_INPUT);
  int start_state = digitalRead(START_MODE_INPUT);
  int zero_position = digitalRead(ZERO_POSITION_INPUT);
  int user_select_interval = map(knob_position, 0, 1023, 0, 6000);
  /* log current values */
  Serial.print(knob_position);
  Serial.print(",");
  Serial.print(zero_position);
  Serial.print(",");
  Serial.print(start_state);
  Serial.print(",");
  Serial.print(zero_position);
  Serial.print(",");
  Serial.print(rest_count);
  Serial.print(",");
  
  switch(state)
  {
    case STOP:
    if (start_state == START_ENABLED)
    {
      state = RUNNING;
    }
    analogWrite(MOTOR_SPEED_OUT, STOP_DUTY_CYCLE);
    Serial.println(STOP_STATE_MSG);
    break;
    
    case RUNNING:
    if (start_state != START_ENABLED && zero_position != IN_ZERO_POSITION)
    {
      state = BRAKING;
    }
    if (zero_position == IN_ZERO_POSITION)
    {
      state = REST;
    }
    analogWrite(MOTOR_SPEED_OUT, OPERATIONAL_DUTY_CYCLE);
    Serial.println(RUNNING_STATE_MSG);
    break;
    
    case REST:
    if (start_state != START_ENABLED)
    {
      state = STOP;
    }
    if (rest_count > (1000 + user_select_interval))
    {
      rest_count = 0;
    }
    rest_count += 50;

    analogWrite(MOTOR_SPEED_OUT, STOP_DUTY_CYCLE);
    Serial.println(REST_STATE_MSG);
    break;
    
    case BRAKING:
    if (start_state == START_ENABLED)
    {
      state = RUNNING;
    }
    if (zero_position == IN_ZERO_POSITION)
    {
      state = STOP;
    }
    analogWrite(MOTOR_SPEED_OUT, HOMING_DUTY_CYCLE);
    Serial.println(BRAKING_STATE_MSG);
    break;
    
    default:
    // something really bad happened
    analogWrite(MOTOR_SPEED_OUT, STOP_DUTY_CYCLE);
    Serial.println(PANIC_STATE_MSG);
    break;
  }
  delay(50);
}
