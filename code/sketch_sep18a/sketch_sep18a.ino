#include <Servo.h>
#define ROTATION_THRESHOLD 5

int previousStreerAngle = 90;
// consts
const int minimumDistance = 20;
const int minimumFrontDistance = 30;
int maxSteerDegree = 30;

// Servo
const int servoPin = 10;
Servo Servo;

// sonar pins
#define COMMON_TRIGGER_PIN 12
#define LEFT_ECHO_PIN 2
#define RIGHT_ECHO_PIN 3
#define FRONTLEFT_ECHO_PIN 4
#define FRONCENTER_ECHO_PIN 5
#define FRONTRIGHT_ECHO_PIN 6
// left sonar
const int COMMON_TRIGGER_PIN = 12;
float leftDistance = 0;

// right sonar
const int echoPinRight = 4;
float rightDistance = 0;

// motor driver
const int enable = 9;
const int motorIn1 = 8;
const int motorIn2 = 7;

unsigned long now_time = 0;
int degreeToSteer = 0;

int get_appropriate_echo_pin(String sonar)
{
  switch (sonar)
  {
  case "left":
    return LEFT_ECHO_PIN;
  case "right":
    return RIGHT_ECHO_PIN;
  case "frontleft":
    return FRONTLEFT_ECHO_PIN;
  case "frontcenter":
    return FRONCENTER_ECHO_PIN;
  case "frontright":
    return FRONTRIGHT_ECHO_PIN;
  }
}

float get_distance_cm(String sonar)
{
  digitalWrite(COMMON_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(COMMON_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(COMMON_TRIGGER_PIN, LOW);
  float duration = pulseIn(get_appropriate_echo_pin(sonar), HIGH);
  return duration * 0.034 / 2;
}

void steering(String direction, float currentDistance)
{
  int changeInRotation = map(currentDistance, 0, minimumDistance, maxSteerDegree, 0);
  if (direction == "left")
  {
    // mapping the steering
    degreeToSteer = 90 + changeInRotation;
  }
  else if (direction == "right")
  {
    degreeToSteer = 90 - changeInRotation;
  }
  else if (direction == "hardleft")
  {
    degreeToSteer = 135;
  }
  else if (direction == "hardright")
  {
    degreeToSteer = 45;
  }
  else
  {
    degreeToSteer = 90;
    Servo.write(degreeToSteer);
  }

  Serial.println(direction);
  Serial.println(currentDistance);
  Serial.print(" ,");
  Serial.println(degreeToSteer);

  // need to think of delay

  // Will ignore the rotation if the angle of rotation in below the threshold
  if (abs(degreeToSteer - previousStreerAngle) > ROTATION_THRESHOLD)
  {
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

  // Sonars Configuration
  pinMode(COMMON_TRIGGER_PIN, OUTPUT);
  pinMode(LEFT_ECHO_PIN, INPUT);
  pinMode(RIGHT_ECHO_PIN, INPUT);
  pinMode(FRONTLEFT_ECHO_PIN, INPUT);
  pinMode(FRONCENTER_ECHO_PIN, INPUT);
  pinMode(FRONTRIGHT_ECHO_PIN, INPUT);

  // motor driver
  pinMode(enable, OUTPUT);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);

  go_forward(255);

  Serial.begin(9600);
}

void loop()
{
  int front_distance = min(get_distance_cm("frontleft"), get_distance_cm("frontcenter"), get_distance_cm("frontright"));
  if (front_distance < minimumFrontDistance)
  {
    // Got obstacle in front
    // Checking the left side
    int left_distance = get_distance_cm("left");
    if (left_distance < minimumDistance)
    {
      steering("hardright", left_distance);
    }
    else
    {
      int right_distance = get_distance_cm("right");
      steering("hardleft", right_distance);
    }
  }
  else
  {
    int left_distance = get_distance_cm("left");
    if (left_distance < minimumDistance)
    {
      steering("right", left_distance);
    }
    else
    {
      int right_distance = get_distance_cm("right");
      steering("left", right_distance);
    }
  }
}
