#include <Adafruit_MPU6050.h>
#include <Mouse.h>

// Finger position readers (potentiometers)
int thumb_pin = A0; 
int pointer_pin = A1; 
int middle_pin = A2; 

int thumb = 0;
int pointer = 0;
int middle = 0;

// MPU6050 sensor
Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_accel, *mpu_gyro;

// Mouse control parameters
int range = 12;               // output range of X or Y movement
int responseDelay = 2;        // response delay of the mouse, in ms
int threshold = range / 4;    // resting threshold
int center = range / 2;       // resting position value
int minima[] = {-12, -12};    // actual reading minima for {x, y}
int maxima[] = {12, 12};      // actual reading maxima for {x, y}
int mouseReading[2];          // final mouse readings for {x, y}

// Click detection
bool wasClicked = false;

void setup(void) {
  Serial.begin(74880);
  while (!Serial)
    delay(10);
  
  Serial.println("Adafruit MPU6050 Mouse Controller");

  // Initialize MPU6050
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

  // Initialize Mouse
  Mouse.begin();
}

void update_fingers() {
  thumb = analogRead(thumb_pin);
  pointer = analogRead(pointer_pin);
  middle = analogRead(middle_pin);
}

int readAxis(float value, int axisNumber) {
  int distance = 0;
  
  // Update min/max for this axis
  if (value < minima[axisNumber]) {
    minima[axisNumber] = value;
  }
  if (value > maxima[axisNumber]) {
    maxima[axisNumber] = value;
  }

  // Map the reading to output range
  int reading = map(value * 10, minima[axisNumber] * 10, maxima[axisNumber] * 10, 0, range);

  // If the output reading is outside the rest position threshold, use it
  if (abs(reading - center) > threshold) {
    distance = (reading - center);
  }

  // Invert Y axis
  if (axisNumber == 1) {
    distance = -distance;
  }

  return distance;
}

void loop() {
  update_fingers();
  
  sensors_event_t accel;
  sensors_event_t gyro;

  // Get sensor events
  mpu_accel->getEvent(&accel);
  mpu_gyro->getEvent(&gyro);

  // Read and scale the two axes for mouse movement
  int xReading = readAxis(accel.acceleration.x, 0);
  int yReading = readAxis(accel.acceleration.y, 1);

  // Move the mouse
  Mouse.move(xReading, yReading, 0);

  // Check for click condition: both A0 and A1 at 1023
  if (thumb >= 1020 && pointer >= 1020) {
    if (!wasClicked) {
      Mouse.press(MOUSE_LEFT);
      Serial.println("CLICK!");
      wasClicked = true;
    }
  } else {
    if (wasClicked) {
      Mouse.release(MOUSE_LEFT);
      wasClicked = false;
    }
  }

  // Display Accelerometer readings
  Serial.print("\t\tACCEL X: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" \tY: ");
  Serial.print(accel.acceleration.y);
  Serial.print(" \tZ: ");
  Serial.print(accel.acceleration.z);
  Serial.println(" m/s^2 ");

  // Display Gyro readings
  Serial.print("\t\tGYRO X: ");
  Serial.print(gyro.gyro.x);
  Serial.print(" \tY: ");
  Serial.print(gyro.gyro.y);
  Serial.print(" \tZ: ");
  Serial.print(gyro.gyro.z);
  Serial.println(" radians/s ");

  // Display finger sensor readings
  Serial.print(" \t\tFINGERS: ");
  Serial.print("Thumb: "); Serial.print(thumb);
  Serial.print("\tPointer: "); Serial.print(pointer);
  Serial.print("\tMiddle: "); Serial.print(middle);

  // Serial plotter compatible values
  Serial.println();
  Serial.print("\t\t"); 
  Serial.print("Thumb:"); Serial.print(thumb);
  Serial.print(", "); Serial.print("Pointer:"); Serial.print(pointer);
  Serial.print(", "); Serial.print("Middle:"); Serial.print(middle);
  Serial.print(", "); Serial.print("A:"); Serial.print(accel.acceleration.x);
  Serial.print(", "); Serial.print("B:"); Serial.print(accel.acceleration.y);
  Serial.print(", "); Serial.print("C:"); Serial.print(accel.acceleration.z);
  Serial.print(", "); Serial.print("D:"); Serial.print(gyro.gyro.x);
  Serial.print(", "); Serial.print("E:"); Serial.print(gyro.gyro.y);
  Serial.print(", "); Serial.print("F:"); Serial.print(gyro.gyro.z);
  Serial.println();
  Serial.println();
  Serial.println();
  
  delay(responseDelay);
}