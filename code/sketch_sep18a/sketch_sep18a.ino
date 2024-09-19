#include <Servo.h>
#define ROTATION_THRESHOLD 5

int previousStreerAngle = 90;
// consts
const int minimumDistance = 20;
int maxSteerDegree = 30;

// Servo
const int servoPin = 10;
Servo Servo;

// left sonar
const int pingPinLeft = 12;
const int echoPinLeft = 11;
float leftDistance = 0;

// right sonar
const int echoPinRight = 4;
const int pingPinRight = 5;
float rightDistance = 0;

// motor driver
const int enable = 9;
const int motorIn1 = 8;
const int motorIn2 = 7;

unsigned long now_time = 0;
int degreeToSteer = 0;

float get_distance_cm(String sonar)
{
  digitalWrite(sonar == "left" ? pingPinLeft : pingPinRight, LOW);
  delayMicroseconds(2);
  digitalWrite(sonar == "left" ? pingPinLeft : pingPinRight, HIGH);
  delayMicroseconds(10);
  digitalWrite(sonar == "left" ? pingPinLeft : pingPinRight, LOW);
  float duration = pulseIn(sonar == "left" ? echoPinLeft : echoPinRight, HIGH);
  return duration * 0.034 / 2;
}


void steering(String direction, float currentDistance) {
  int changeInRotation = map(currentDistance,0,minimumDistance,maxSteerDegree,0);
  if (direction == "left") {
    // mapping the steering
    degreeToSteer = 90 + changeInRotation;
  } else if(direction == "right") {
    degreeToSteer = 90 - changeInRotation;
  }else{
    degreeToSteer = 90;
    Servo.write(degreeToSteer);
  }

  Serial.println(direction);
  Serial.println(currentDistance);
  Serial.print(" ,");
  Serial.println(degreeToSteer);

  // need to think of delay

  // Will ignore the rotation if the angle of rotation in below the threshold
  if(abs(degreeToSteer - previousStreerAngle) > ROTATION_THRESHOLD) {
    Servo.write(degreeToSteer);
  }
}

void go_forward(int speed)
{
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  analogWrite(enable, speed);
}

void stop()
{
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  analogWrite(enable, 0);
}

void setup()
{

  // servo configuration
  Servo.attach(servoPin);
  Servo.write(90);
  delay(1000);

  // sonar left
  pinMode(pingPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);

  // sonar right
  pinMode(pingPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);

  // motor driver
  pinMode(enable, OUTPUT);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);

  go_forward(255);

  Serial.begin(9600);
}

void loop()
{
  leftDistance = get_distance_cm("left");
  rightDistance = get_distance_cm("right");

  if (leftDistance < minimumDistance)
  {
    steering("right", leftDistance);
  }
  else if (rightDistance < minimumDistance)
  {
    steering("left", rightDistance);
  }else{
    steering("straight", 22);
  }
}
