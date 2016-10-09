#include <PulsePosition.h>


PulsePositionInput rcIn;

const int LED = 13;

const int PPM_INPUT = 20; // PPM input from RC receiver

//motor A connected between A01 and A02
//motor B connected between B01 and B02

const int STBY = 10; //standby

//Motor A
const int PWMA = 3; //Speed control 
const int AIN1 = 9; //Direction
const int AIN2 = 8; //Direction

//Motor B
const int PWMB = 5; //Speed control
const int BIN1 = 11; //Direction
const int BIN2 = 12; //Direction


class TB6612FNG_channel
{

 public:
  TB6612FNG_channel(int pwm, int fwd, int rev) : _pwm_pin(pwm), _fwd_pin(fwd), _rev_pin(rev){}
  void init()
  {
    pinMode(_pwm_pin, OUTPUT);
    pinMode(_fwd_pin, OUTPUT);
    pinMode(_rev_pin, OUTPUT);
  }

  void move(int vel)
  {
    digitalWrite(_fwd_pin, vel > 0 ? HIGH : LOW);
    digitalWrite(_rev_pin, vel > 0 ? LOW : HIGH);
    analogWrite(_pwm_pin, min(abs(vel), 255));
  }
  
 private:
  int _pwm_pin;
  int _fwd_pin;
  int _rev_pin;
  
};

TB6612FNG_channel leftMotor(PWMA,AIN1, AIN2);
TB6612FNG_channel rightMotor(PWMB, BIN1, BIN2);


void setup(){
  Serial.begin(115200);

  rcIn.begin(PPM_INPUT);

  
  pinMode(LED, OUTPUT);
  pinMode(STBY, OUTPUT);
  leftMotor.init();
  rightMotor.init();
  
  digitalWrite(STBY, LOW);
}

void loop()
{
  int yaw, surge;
  bool enable;
  
  if(getRCInput(yaw, surge, enable))
  {

    int rightVel = surge + (yaw/2);
    int leftVel = surge - (yaw/2);
    
    enableMotors(enable);

    Serial.print(rightVel);
    Serial.print("  ");
    Serial.print(leftVel);
    
    rightMotor.move(rightVel);
    leftMotor.move(leftVel);
    Serial.println();
  }
}

bool getRCInput(int& yaw, int& surge, bool& enable)
{
  if(rcIn.available() == 6)
  {
    yaw =  (rcIn.read(1)-1500) / 2;
    surge = (rcIn.read(2)-1500) / 2;
    enable = rcIn.read(5) > 1700;
    return true;
  }

  return false;
  
}

void enableMotors(bool en)
{
    digitalWrite(STBY, en ? HIGH : LOW);  // this enables motors
    digitalWrite(LED, en ? HIGH : LOW);   // this turns on the LED
}
