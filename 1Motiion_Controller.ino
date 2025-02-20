#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Mouse.h>
#include <Keyboard.h>
 
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
int16_t accx, accy, accz;
int vx, vy;
int TRIGGER_BUTTON = 5;
int WEAPON_TOGGLE = 9;
bool weaponModeOn = false;
float angle;
 
//code for smoothing input 
int readIndex = 0;
const int numReadings = 20;
int angleReadings[numReadings];
int total = 0;
float averageAngle = 0.0;
 
 
int oldZ = 0;
int newZ = 0;
 
void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    while (1);
  }
  pinMode(TRIGGER_BUTTON, INPUT_PULLUP);
  pinMode(WEAPON_TOGGLE, INPUT_PULLUP);
 
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    angleReadings[thisReading] = 0;
  }
}
 
void loop() {
 
  int triggerPushed = digitalRead(TRIGGER_BUTTON);
  int weaponToggle = digitalRead(WEAPON_TOGGLE);
 
  total = total - angleReadings[readIndex];
  angleReadings[readIndex] = angle;
  total = total + angleReadings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
 
 
  if (triggerPushed == HIGH) {
    //digitalWrite(13, LOW);
    if (Mouse.isPressed()) {
      Mouse.release();
    }
  } else {
    // Serial.println("button pushed");
    Mouse.press();
  }
 
  if (weaponToggle == HIGH) {
    weaponModeOn = false;
  } else {
    if (!weaponModeOn) {
      Keyboard.write('2');
      delay(400);
    }
    weaponModeOn = true;
  }
 
  //accx, accy, accz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  mpu.getAcceleration(&accx, &accy, &accz);
 
  //Serial.println(gy);
 
  // Serial.println(angle);
 
  oldZ = newZ;
  vx = (gx + 300) / 150; // "+300" because the x axis of gyroscope give values about -350 while it's not moving. Change this value if you get something different using the TEST code, chacking if there are values far from zero.
  vy = -(gz - 100) / 150; // same here about "-100"
 
  /*
    Serial.print("gx = ");
    Serial.print(gx);
    Serial.print(" | gz = ");
    Serial.print(gz);
    Serial.print(" | gy = ");
    Serial.println(gy);
 
  */
 
  if (gx > 32000) {
    Serial.println("Flick Right");
    for (int i; i < 12; i++) {
      Mouse.move(50, 0);
    }
    delay(250);
  } else if (gx < -32000) {
    Serial.println("Flick Left");
    for (int i; i < 12; i++) {
      Mouse.move(-50, 0);
    }
    delay(250);
  }
 
  /*
    Serial.print("accx = ");
    Serial.print(accx);
    Serial.print(" | accy = ");
    Serial.print(accy);
    Serial.print(" | accz = ");
    Serial.println(accz);
  */
 
 
  averageAngle = total / numReadings;
  if (!weaponModeOn && accy > 16000)
  {
 
 
    if (averageAngle < 10) {
      Serial.println("Build floor");
      Keyboard.write(KEY_F2);
    } else if (averageAngle > 10 & averageAngle < 45) {
      Serial.println("Build ramp");
      Keyboard.write(KEY_F3);
    } else {
      Serial.println("Build wall");
      Keyboard.write(KEY_F1);
    }
 
    // Serial.println(averageAngle);
 
    //Serial.println("shake");
    //Keyboard.write(KEY_F1);
    delay(250);
    Mouse.click();
  } else if (weaponModeOn && accy > 16000)
  {
    if (averageAngle < 10) {
      Keyboard.write('r');
    }
  }
 
  if (weaponModeOn) {
    //Serial.println("Weapon mode on");
    Mouse.move(vx, vy);
  }
 
  //working
  //angle = atan2((float) (ay - 16384), (float) (ax - 16384)) * (180.0 / PI) * -1;
  angle = atan2((float) ay, (float) ~ax) * (180.0 / PI);
  //float angle = atan2((float) ay, (float) -ax) * (180.0 / PI);
 
  //Serial.println(averageAngle);
 
  delay(20);
}
