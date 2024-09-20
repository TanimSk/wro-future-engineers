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
#define LEFT_ECHO_PIN 2
#define LEFT_TRIGGER_PIN A2

#define FRONTLEFT_ECHO_PIN 3
#define FRONTLEFT_TRIGGER_PIN A4

#define FRONCENTER_ECHO_PIN A5
#define FRONCENTER_TRIGGER_PIN A3

#define FRONTRIGHT_ECHO_PIN 4
#define FRONTRIGHT_TRIGGER_PIN A1

#define RIGHT_ECHO_PIN 6
#define RIGHT_TRIGGER_PIN A0

// left sonar
float leftDistance = 0;

// right sonar
const int echoPinRight = 4;
float rightDistance = 0;

// motor driver
const int enable = 9;
const int motorIn1 = 8;
const int motorIn2 = 7;

unsigned long start_time = 0;
int degreeToSteer = 0;

// Sonar distances
int front_center_distance = 300;
int front_left_distance = 300;
int front_right_distance = 300;
int left_distance = 300;
int right_distance = 300;

int *get_appropriate_sonar_pin(String sonar) {
  int *array = new int[2];

  if (sonar == "left") {
    array[0] = LEFT_TRIGGER_PIN;
    array[1] = LEFT_ECHO_PIN;
  } else if (sonar == "right") {
    array[0] = RIGHT_TRIGGER_PIN;
    array[1] = RIGHT_ECHO_PIN;
  } else if (sonar == "frontleft") {
    array[0] = FRONTLEFT_TRIGGER_PIN;
    array[1] = FRONTLEFT_ECHO_PIN;
  } else if (sonar == "frontcenter") {
    array[0] = FRONCENTER_TRIGGER_PIN;
    array[1] = FRONCENTER_ECHO_PIN;
  } else if (sonar == "frontright") {
    array[0] = FRONTRIGHT_TRIGGER_PIN;
    array[1] = FRONTRIGHT_ECHO_PIN;
  } else {
    array[0] = -1;
    array[1] = -1;
  }

  return array;
}

float get_distance_cm(String sonar) {
  int *sonar_pins = get_appropriate_sonar_pin(sonar);
  digitalWrite(sonar_pins[0], LOW);
  delayMicroseconds(2);
  digitalWrite(sonar_pins[0], HIGH);
  delayMicroseconds(10);
  digitalWrite(sonar_pins[0], LOW);
  float duration = pulseIn(sonar_pins[1], HIGH);

  Serial.println(sonar + " ");
  Serial.println(sonar_pins[0]);
  Serial.println(sonar_pins[1]);
  Serial.println();
  return duration * 0.034 / 2;
}

void steering(String direction, float currentDistance) {
  int changeInRotation = map(currentDistance, 0, minimumDistance, maxSteerDegree, 0);
  if (direction == "left") {
    // mapping the steering
    degreeToSteer = 90 + changeInRotation;
  } else if (direction == "right") {
    degreeToSteer = 90 - changeInRotation;
  } else if (direction == "hardleft") {
    degreeToSteer = 90 + hardCornerDegree;
  } else if (direction == "hardright") {
    degreeToSteer = 90 - hardCornerDegree;
  } else {
    degreeToSteer = 90;
    // Servo.write(degreeToSteer);
  }

  // Will ignore the rotation if the angle of rotation in below the threshold
  if (abs(degreeToSteer - previousStreerAngle) > ROTATION_THRESHOLD) {
    // Servo.write(degreeToSteer);
  }
}

void go_forward(int speed) {
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  analogWrite(enable, speed);
}

void stop() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  analogWrite(enable, 0);
}

void setup() {

  // servo configuration
  Servo.attach(servoPin);
  Servo.write(90);
  delay(1000);

  // Sonars Configuration
  pinMode(LEFT_ECHO_PIN, INPUT);
  pinMode(LEFT_TRIGGER_PIN, OUTPUT);
  pinMode(RIGHT_ECHO_PIN, INPUT);
  pinMode(RIGHT_TRIGGER_PIN, OUTPUT);
  pinMode(FRONTLEFT_ECHO_PIN, INPUT);
  pinMode(FRONTLEFT_TRIGGER_PIN, OUTPUT);
  pinMode(FRONCENTER_ECHO_PIN, INPUT);
  pinMode(FRONCENTER_TRIGGER_PIN, OUTPUT);
  pinMode(FRONTRIGHT_ECHO_PIN, INPUT);
  pinMode(FRONTRIGHT_TRIGGER_PIN, OUTPUT);

  // motor driver
  pinMode(enable, OUTPUT);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);

  go_forward(255);

  Serial.begin(9600);
}

void loop() {
  delay(100);

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

  if (front_center_distance < minimumFrontDistance || front_left_distance < minimumFrontDistance || front_right_distance < minimumFrontDistance) {
    // Got obstacle in front
    // Checking the left side

    if (left_distance < minimumDistance) {
      steering("hardright", left_distance);
    } else {

      steering("hardleft", right_distance);
    }
  } else {
    if (left_distance < minimumDistance) {
      steering("right", left_distance);
    } else {
      steering("left", right_distance);
    }
  }
}
