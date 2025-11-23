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

// Orientation configuration - CHANGE THESE to fix directions
bool invertX = false;         // Set to true to invert X axis
bool invertY = false;         // Set to true to invert Y axis
bool swapXY = true;          // Set to true to swap X and Y axes

// Sensitivity multiplier (1.0 = normal, 2.0 = twice as fast, 0.5 = half speed)
float sensitivity = 1.0;  // Adjust this if too fast or slow

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
  // For gyroscope: directly scale the value
  // Gyro values are typically -5 to +5 rad/s for normal hand movements
  int distance = 0;
  
  // Apply a deadzone threshold (ignore small movements)
  float deadzone = 0.1;
  
  if (abs(value) > deadzone) {
    // Scale gyro value directly to mouse movement
    distance = (int)(value * 10);  // Multiply by 10 for reasonable speed
    
    // Clamp to prevent too-fast movement
    if (distance > range) distance = range;
    if (distance < -range) distance = -range;
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

  // MPU orientation: VCC forward, components on LEFT side of RIGHT hand
  // Using gyroscope for tilt control
  float rawX = -gyro.gyro.y;  // Tilt hand left/right
  float rawY = gyro.gyro.x;   // Tilt hand forward/back
  
  // Apply orientation transformations
  float processedX = swapXY ? rawY : rawX;
  float processedY = swapXY ? rawX : rawY;
  
  if (invertX) processedX = -processedX;
  if (invertY) processedY = -processedY;

  // Read and scale the two axes for mouse movement
  int xReading = readAxis(processedX, 0) * sensitivity;
  int yReading = readAxis(processedY, 1) * sensitivity;

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
      Serial.println("RELEASE!");
      wasClicked = false;
    }
  }

  // Print sensor data every 200ms
  if (millis() % 200 < 10) {
    Serial.print("GYRO X:"); Serial.print(gyro.gyro.x, 3);
    Serial.print(" Y:"); Serial.print(gyro.gyro.y, 3);
    Serial.print(" | MOUSE X:"); Serial.print(xReading);
    Serial.print(" Y:"); Serial.print(yReading);
    Serial.print(" | T:"); Serial.print(thumb);
    Serial.print(" P:"); Serial.println(pointer);
  }
  
  delay(responseDelay);
}
