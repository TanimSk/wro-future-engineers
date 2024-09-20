#include <Servo.h>
#define ROTATION_THRESHOLD 5

int previousStreerAngle = 90;

// consts
const int minimumDistance = 20;
const int minimumFrontDistance = 30;
int maxSteerDegree = 30;
int hardCornerDegree = 30;

// Servo
const int servoPin = 10;
Servo Servo;

// sonar pins
#define COMMON_TRIGGER_PIN 13
#define LEFT_ECHO_PIN 2
#define FRONTLEFT_ECHO_PIN 3
#define FRONCENTER_ECHO_PIN A5 // A5
#define FRONTRIGHT_ECHO_PIN 4
#define RIGHT_ECHO_PIN 5

// left sonar
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
  if (sonar == "left")
  {
    return LEFT_ECHO_PIN;
  }
  else if (sonar == "right")
  {
    return RIGHT_ECHO_PIN;
  }
  else if (sonar == "frontleft")
  {
    return FRONTLEFT_ECHO_PIN;
  }
  else if (sonar == "frontcenter")
  {
    return FRONCENTER_ECHO_PIN;
  }
  else if (sonar == "frontright")
  {
    return FRONTRIGHT_ECHO_PIN;
  }
  else
  {
    return -1; // Return an invalid pin if the sonar type is unknown
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
    degreeToSteer = 90 + hardCornerDegree;
  }
  else if (direction == "hardright")
  {
    degreeToSteer = 90 - hardCornerDegree;
  }
  else
  {
    degreeToSteer = 90;
    Servo.write(degreeToSteer);
  }
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
  int front_center_distance = get_distance_cm("frontcenter");
  int front_left_distance = get_distance_cm("frontleft");
  int front_right_distance = get_distance_cm("frontright");
  int left_distance = get_distance_cm("left");
  int right_distance = get_distance_cm("right");

  Serial.println("-------------------------------------");
  Serial.print("Front Center: ");
  Serial.println(front_center_distance);
  Serial.print("Front Left: ");
  Serial.println(front_left_distance);
  Serial.print("Front Right: ");
  Serial.println(front_right_distance);
  Serial.print("Left: ");
  Serial.println(left_distance);
  Serial.print("Right: ");
  Serial.println(right_distance);

  if (front_center_distance < minimumFrontDistance || front_left_distance < minimumFrontDistance || front_right_distance < minimumFrontDistance)
  {
    // Got obstacle in front
    // Checking the left side

    if (left_distance < minimumDistance)
    {
      steering("hardright", left_distance);
    }
    else
    {

      steering("hardleft", right_distance);
    }
  }
  else
  {
    if (left_distance < minimumDistance)
    {
      steering("right", left_distance);
    }
    else
    {
      steering("left", right_distance);
    }
  }
}
