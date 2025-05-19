#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

char ssid[] = "";
char pass[] = "";
char auth[] = BLYNK_AUTH_TOKEN;

#define V_DOOR     V0
#define V_RAIN     V1
#define V_GARAGE   V2

Servo doorServo;
Servo rainServo;
Servo garageServo;

#define DOOR_SERVO_PIN    25
#define RAIN_SERVO_PIN    26
#define GARAGE_SERVO_PIN  27

#define IR_GARAGE_ENTRY_PIN    32
#define IR_GARAGE_PARK_PIN     33
#define IR_DOOR_OBSTACLE_PIN   35

#define RAIN_SENSOR_PIN   34

int doorOnPos = 120;
int rainOnPos = 110;
int garageOnPos = 180;

int servoOffPos = 0;

bool entryDetected = false;
bool parkedDetected = false;
bool obstacleDetected = false;

BlynkTimer timer;

BLYNK_WRITE(V_DOOR) {
  int value = param.asInt();
  if (value == 1) {
    doorServo.write(doorOnPos);
    Serial.println("Door opened.");
  } else {
    doorServo.write(servoOffPos);
    Serial.println("Door closed.");
  }
}

BLYNK_WRITE(V_RAIN) {
  int value = param.asInt();
  if (value == 1) {
    rainServo.write(rainOnPos);
    Serial.println("Rain cover extended.");
  } else {
    rainServo.write(servoOffPos);
    Serial.println("Rain cover retracted.");
  }
}

BLYNK_WRITE(V_GARAGE) {
  int value = param.asInt();
  if (value == 1) {
    garageServo.write(garageOnPos);
    Serial.println("Garage opened.");
  } else {
    garageServo.write(servoOffPos);
    Serial.println("Garage closed.");
  }
}

void setup() {
  Serial.begin(115200);
  doorServo.attach(DOOR_SERVO_PIN);
  rainServo.attach(RAIN_SERVO_PIN);
  garageServo.attach(GARAGE_SERVO_PIN);
  doorServo.write(servoOffPos);
  rainServo.write(servoOffPos);
  garageServo.write(servoOffPos);
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(IR_GARAGE_ENTRY_PIN, INPUT);
  pinMode(IR_GARAGE_PARK_PIN, INPUT);
  pinMode(IR_DOOR_OBSTACLE_PIN, INPUT);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  timer.run();

  int rainValue = digitalRead(RAIN_SENSOR_PIN);
  if (rainValue == LOW) {
    Blynk.logEvent("rain_alert", "Rain detected! Please check the shelter.");
    Serial.println("Rain detected!");
    delay(1000);
  }

  int entrySensor = digitalRead(IR_GARAGE_ENTRY_PIN);
  if (entrySensor == LOW && !entryDetected) {
    Blynk.logEvent("car_entry", "Car detected at garage entrance.");
    Serial.println("Car at garage entry.");
    entryDetected = true;
  } else if (entrySensor == HIGH) {
    entryDetected = false;
  }

  int parkedSensor = digitalRead(IR_GARAGE_PARK_PIN);
  if (parkedSensor == LOW && !parkedDetected) {
    Blynk.logEvent("car_parked", "Car parked in the garage.");
    Serial.println("Car parked.");
    parkedDetected = true;
  } else if (parkedSensor == HIGH) {
    parkedDetected = false;
  }

  int doorSensor = digitalRead(IR_DOOR_OBSTACLE_PIN);
  if (doorSensor == LOW && !obstacleDetected) {
    Blynk.logEvent("door_obstacle", "Object detected near the door!");
    Serial.println("Obstacle near door.");
    obstacleDetected = true;
  } else if (doorSensor == HIGH) {
    obstacleDetected = false;
  }

  delay(200);
}
