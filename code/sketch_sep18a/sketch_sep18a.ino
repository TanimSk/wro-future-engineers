#include <Servo.h>

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


unsigned long now_time = 0;
int degreeToSteer = 0;


float get_distance_cm(String sonar) {
  digitalWrite(sonar == "left" ? pingPinLeft : pingPinRight, LOW);
  delayMicroseconds(2);
  digitalWrite(sonar == "left" ? pingPinLeft : pingPinRight, HIGH);
  delayMicroseconds(10);
  digitalWrite(sonar == "left" ? pingPinLeft : pingPinRight, LOW);
  float duration = pulseIn(sonar == "left" ? echoPinLeft : echoPinRight, HIGH);
  return duration * 0.034 / 2;
}


void steering(String direction, float threshold) {
  if (direction == "left") {
    // mapping the steering
    degreeToSteer = 90 + ((maxSteerDegree / minimumDistance) * (minimumDistance - threshold));
  } else {
    degreeToSteer = 90 - ((maxSteerDegree / minimumDistance) * (minimumDistance - threshold));
  }

  Serial.println(direction);
  Serial.println(threshold);
  Serial.print(" ,");
  Serial.println(degreeToSteer);


  // need to think of delay
  Servo.write(degreeToSteer);
}


void setup() {

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

  Serial.begin(9600);
}


void loop() {
  leftDistance = get_distance_cm("left");
  rightDistance = get_distance_cm("right");

  if (leftDistance < minimumDistance) {
    steering("right", leftDistance);
  } else if (rightDistance < minimumDistance) {
    steering("left", rightDistance);
  }
}
