#include <Servo.h>

#define ROTATION_THRESHOLD 5
#define MAX_SONARS 5

int previousSteerAngle = 90;
int degreeToSteer = 0;
unsigned long start_time = 0;

// Constants
const int minimumDistance = 20;
const int minimumFrontDistance = 30;
const int maxSteerDegree = 30;
const int hardCornerDegree = 30;

// Servo
const int servoPin = 10;
Servo servo;

// Motor driver
const int enable = 9;
const int motorIn1 = 8;
const int motorIn2 = 7;

// Sonar pins
const int sonarPins[MAX_SONARS][2] = {
    {A2, 2},  // left
    {A4, 3},  // front-left
    {A3, A5}, // front-center
    {A1, 4},  // front-right
    {A0, 6}   // right
};

enum Sonar
{
  LEFT,
  FRONT_LEFT,
  FRONT_CENTER,
  FRONT_RIGHT,
  RIGHT
};

// Function to get sonar distance in cm
float getDistanceCM(Sonar sonar)
{
  int triggerPin = sonarPins[sonar][0];
  int echoPin = sonarPins[sonar][1];

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

// Function to steer the vehicle
void steering(Sonar direction, float currentDistance)
{
  int changeInRotation = map(currentDistance, 0, minimumDistance, maxSteerDegree, 0);

  switch (direction)
  {
  case LEFT:
    degreeToSteer = 90 + changeInRotation;
    break;
  case RIGHT:
    degreeToSteer = 90 - changeInRotation;
    break;
  default:
    degreeToSteer = 90;
    break;
  }

  if (abs(degreeToSteer - previousSteerAngle) > ROTATION_THRESHOLD)
  {
    servo.write(degreeToSteer);
    previousSteerAngle = degreeToSteer;
  }
}

// Motor control
void goForward(int speed)
{
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  analogWrite(enable, speed);
}

void stopMotor()
{
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  analogWrite(enable, 0);
}

void setup()
{
  // Servo configuration
  servo.attach(servoPin);
  servo.write(90);
  delay(1000);

  // Sonar pins configuration
  for (int i = 0; i < MAX_SONARS; ++i)
  {
    pinMode(sonarPins[i][1], INPUT);
    pinMode(sonarPins[i][0], OUTPUT);
  }

  // Motor driver configuration
  pinMode(enable, OUTPUT);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);

  goForward(255);
  Serial.begin(9600);
}

void loop()
{
  delay(100);

  // Read sonar distances
  int frontCenterDistance = getDistanceCM(FRONT_CENTER);
  int frontLeftDistance = getDistanceCM(FRONT_LEFT);
  int frontRightDistance = getDistanceCM(FRONT_RIGHT);
  int leftDistance = getDistanceCM(LEFT);
  int rightDistance = getDistanceCM(RIGHT);

  // Print distances for debugging
  Serial.println("-------------------------------------");
  Serial.print("Front Center: ");
  Serial.println(frontCenterDistance);
  Serial.print("Front Left: ");
  Serial.println(frontLeftDistance);
  Serial.print("Front Right: ");
  Serial.println(frontRightDistance);
  Serial.print("Left: ");
  Serial.println(leftDistance);
  Serial.print("Right: ");
  Serial.println(rightDistance);

  // Check for obstacles and steer accordingly
  if (frontCenterDistance < minimumFrontDistance || frontLeftDistance < minimumFrontDistance || frontRightDistance < minimumFrontDistance)
  {
    if (leftDistance < minimumDistance)
    {
      steering(RIGHT, leftDistance); // Steer hard right if left side is blocked
    }
    else
    {
      steering(LEFT, rightDistance); // Steer hard left otherwise
    }
  }
  else
  {
    if (leftDistance < minimumDistance)
    {
      steering(RIGHT, leftDistance);
    }
    else
    {
      steering(LEFT, rightDistance);
    }
  }
}
