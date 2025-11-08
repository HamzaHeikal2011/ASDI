#include <Adafruit_MPU6050.h>
// Added potentiometer control for pointer ---
int thumb_pin = A0; int thumb = 0;
int pointer_pin = A1; int pointer = 0;
int middle_pin = A2; int middle = 0;
int ring_pin = A3; int ring = 0;
int pinkie_pin = A4; int pinkie = 0;

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_accel, *mpu_gyro;

void setup(void) {
  Serial.begin(74880);
  while (!Serial)
    delay(10);
  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 Found!");
  mpu_accel = mpu.getAccelerometerSensor();
  mpu_accel->printSensorDetails();

  mpu_gyro = mpu.getGyroSensor();
  mpu_gyro->printSensorDetails();
}


void update_fingers() {
  thumb = analogRead(thumb_pin);
  pointer = analogRead(pointer_pin);
  middle = analogRead(middle_pin);
  ring = analogRead(ring_pin);
  pinkie = analogRead(pinkie_pin);
}

void loop() {
    update_fingers();
  //Get a new normalized sensor event
    sensors_event_t accel;
    sensors_event_t gyro;
    mpu_accel->getEvent(&accel);
    mpu_gyro->getEvent(&gyro);

  //Display Accellerometer readings
    Serial.print("\t\tACCEL X: ");
    Serial.print(accel.acceleration.x);
    Serial.print(" \tY: ");
    Serial.print(accel.acceleration.y);
    Serial.print(" \tZ: ");
    Serial.print(accel.acceleration.z);
    Serial.println(" m/s^2 ");

  //Display Gyro readings
    Serial.print("\t\tGYRO X: ");
    Serial.print(gyro.gyro.x);
    Serial.print(" \tY: ");
    Serial.print(gyro.gyro.y);
    Serial.print(" \tZ: ");
    Serial.print(gyro.gyro.z);
    Serial.println(" radians/s ");

  //Display finger sensor readings
    Serial.print(" \t\tFINGERS: ");
    Serial.print("Thumb: "); Serial.print(thumb);
    Serial.print("\tPointer: "); Serial.print(pointer);
    Serial.print("\tMiddle: "); Serial.print(middle);
    Serial.print("\tRing: "); Serial.print(ring);
    Serial.print("\tPinkie: "); Serial.print(pinkie);

  //Serial plotter values
    Serial.println();
    Serial.print("\t\t"); 
    // Serial.print("Thumb:"); Serial.print(thumb);
    // Serial.print(", "); Serial.print("Pointer:"); Serial.print(pointer);
    // Serial.print(", "); Serial.print("Middle:"); Serial.print(middle);
    // Serial.print(", "); Serial.print("Ring:"); Serial.print(ring);
    // Serial.print(", "); Serial.print("Pinkie:"); Serial.print(pinkie);
    // Serial.print(", "); 
    Serial.print("A:"); Serial.print(accel.acceleration.x);
    Serial.print(", "); Serial.print("B:"); Serial.print(accel.acceleration.y);
    Serial.print(", "); Serial.print("C:"); Serial.print(accel.acceleration.z);
    Serial.print(", "); Serial.print("D:"); Serial.print(gyro.gyro.x);
    Serial.print(", "); Serial.print("E:"); Serial.print(gyro.gyro.y);
    Serial.print(", "); Serial.print("F:"); Serial.print(gyro.gyro.z);
    Serial.println();
    Serial.println();
    Serial.println();
    delay(10);    
}

