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
Adafruit_Sensor *mpu_accel;

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
bool swapXY = false;          // Set to true to swap X and Y axes

// Sensitivity multiplier (1.0 = normal, 2.0 = twice as fast, 0.5 = half speed)
float sensitivity = 1.0;

// Click detection
bool wasClicked = false;

void setup(void) {
  Serial.begin(74880);
  while (!Serial)
    delay(10);
  
  Serial.println("Mouse initiating");

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("Taking over");
  mpu_accel = mpu.getAccelerometerSensor();
  mpu_accel->printSensorDetails();

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

  // Get sensor events
  mpu_accel->getEvent(&accel);

  // MPU orientation: VCC forward, components on LEFT side of RIGHT hand
  // This means: MPU Y-axis = cursor X (left/right), MPU X-axis = cursor Y (forward/back)
  // With components on left, Y is inverted for natural right-hand movement
  float rawX = -accel.acceleration.y;  // Move hand left/right
  float rawY = accel.acceleration.x;   // Move hand forward/back
  
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
      wasClicked = true;
    }
  } else {
    if (wasClicked) {
      Mouse.release(MOUSE_LEFT);
      wasClicked = false;
    }
  }
  
  delay(responseDelay);
}