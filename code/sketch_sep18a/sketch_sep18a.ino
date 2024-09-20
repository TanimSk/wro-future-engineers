#include <Servo.h>

#define ROTATION_THRESHOLD 3
#define MAX_SONARS 5

int previousSteerAngle = 90;
int degreeToSteer = 0;

// Constants
const int minimumDistance = 20;
const int minimumFrontDistance = 30;

const int maxSteerDegreeRight = 22;
const int maxSteerDegreeLeft = 26;
// const int hardCornerDegree = 30;

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
void steering(String direction, float currentDistance)
{
  int changeInRotation = 0;

  if (direction == "LEFT")
  {
    changeInRotation = map(currentDistance, 0, minimumDistance, maxSteerDegreeLeft, 0);
    degreeToSteer = 90 + changeInRotation;
    Serial.println("Steering left" + String(degreeToSteer));
  }
  else if (direction == "RIGHT")
  {
    changeInRotation = map(currentDistance, 0, minimumDistance, maxSteerDegreeRight, 0);
    degreeToSteer = 90 - changeInRotation;
    Serial.println("Steering right" + String(degreeToSteer));
  }
  else
  {
    degreeToSteer = 90;
  }

  if (abs(degreeToSteer - previousSteerAngle) > ROTATION_THRESHOLD)
  {
    Serial.println("Steering to " + String(degreeToSteer));
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

void goBackward(int speed)
{
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
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
  goForward(255);
  delay(80);

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

  // convert leftDistance to string
  //

  String distances = String(leftDistance) + ", " + String(rightDistance) + ", " + String(frontCenterDistance) + ", " + String(frontLeftDistance) + ", " + String(frontRightDistance) + "\n";
  Serial.print(distances);

  // String distances = String(leftDistance) + ", " + String(frontCenterDistance) + ", " + String(rightDistance) + "\n";
  // Serial.print(distances);

  // Check for obstacles and steer accordingly
  if ((frontCenterDistance < minimumFrontDistance) || (frontLeftDistance < minimumFrontDistance) || (frontRightDistance < minimumFrontDistance))
  {
    if ((leftDistance < minimumDistance) && (rightDistance < minimumDistance))
    {
      Serial.println("Going back");
      goBackward(255);
      delay(2000);
    }
    else if (leftDistance < 30)
    {
      // Left e obstacle
      steering("RIGHT", 0); // Steer hard right if left side is blocked
      delay(1800);
    }
    else if (rightDistance < 30)
    {
      // Right e obstacle
      steering("LEFT", 0); // Steer hard left otherwise
      delay(1800);
      // reset angle
      previousSteerAngle = 90;
    }
    else
    {
      steering("LEFT", 0);
      delay(1800);
      // reset angle
      previousSteerAngle = 90;
    }
  }
  else
  {
    if (leftDistance < minimumDistance)
    {
      steering("RIGHT", leftDistance);
    }
    else if (rightDistance < minimumDistance)
    {
      steering("LEFT", rightDistance);
    }
    else
    {
      servo.write(90);
      // reset the angle
      previousSteerAngle = 90;
    }
  }
}
