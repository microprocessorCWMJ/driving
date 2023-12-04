#define S2 31
#define S3 33      
#define sensorOut1 35 
#define S22 37
#define S32 39
#define sensorOut2 41
#define S23 43
#define S33 45      
#define sensorOut3 47 
#define S24 49
#define S34 51
#define sensorOut4 53

#define LeftFrontpinTrig 22
#define LeftFrontpinEcho 24
#define LeftBehindpinTrig 26
#define LeftBehindpinEcho 28
#define RightFrontpinTrig 30
#define RightFrontpinEcho 32
#define RightBehindpinTrig 34
#define RightBehindpinEcho 36

#include <Wire.h>

const int MPU_ADDR = 0x68;  // I2C통신을 위한 MPU6050의 주소
const double RADIAN_TO_DEGREE = 180 / 3.14159;

// length of kickboard (It needs to be updated)
const double kickboard_length_cm = 3.5;

//----------------------------------------------------For Driving Mode--------------------------------------------------//
// If users change the button to park, driving mode must be false.
bool driving_mode = true;
// If white color is repetitively detected, on_the_crosswalk flag is set.
// It names "on_the_crosswalk", but it is also setted when user is changing lane repetitivley.
bool on_the_crosswalk = true;
// If ultrasound is detected objects repetitively, many_objects_around flag is set.
bool many_objects_around = false;
// If parking is completed, parking completion flag is set.
bool parking_complete = false;
//----------------------------------------------------------------------------------------------------------------------//

int16_t AcX, AcY, AcZ;   // Acceleration & Gyro 
double angleAcY;

// variables that need to count the number of reckless driving on crosswalk or lane.
float tilted_reference_time = 0.0;
float tilted_previous_time = 0.0;
float tilted_current_time = 0.0;
uint8_t tilted_count = 0;
uint8_t tilted_warning_count = 0;
//recklessness: Dangerous driving even if there are detected so many objects(person, vehicle, dog, whatever ... )
uint8_t tilted_recklessness_count = 0;

float current_time_for_object_detection = 0.0;
float previous_time_for_object_detection = 0.0;
float current_time_for_object_detection_end = 0.0;
float previous_time_for_object_detection_end = 0.0;

double getAngleXY() {
  getData();  
  // Pitch angle
  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;
}

// Initiate I2C communication between imu sensor and arduino.
void initSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);   // Address for I2C communication
  Wire.write(0x6B);    // Write on 0x6B for communication with MPU6050
  Wire.write(0);
  Wire.endTransmission(true);
}

void getData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);   // AcX register address
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true); // Request 14 bytes of data after AcX address
  AcX = Wire.read() << 8 | Wire.read(); // Connect two separate bytes into one and store them in each variable
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
}

void detect_dangerous_driving(){
  if(many_objects_around){
    initSensor();
    getAngleXY(); 
    if(angleAcY >= 40 || angleAcY <= -40){
      tilted_current_time = millis();
      if(tilted_current_time - tilted_previous_time <= 10000){
        tilted_count += 1;
        if(tilted_count == 1){
          tilted_reference_time = millis();            
        }
      }
      tilted_previous_time = tilted_current_time;
    }

    if((tilted_current_time - tilted_reference_time <= 60000) && (tilted_count >= 5)){
      if(on_the_crosswalk){
        tilted_warning_count += 1;
        tilted_recklessness_count += 1;

        Serial.print("근거리에 물체가 다수 포착, 난폭 운전 적발. 위험 운전 총합 3회 이상 적발 시 강력 제재 예정: ");
        Serial.print(tilted_warning_count);
        Serial.print(", ");
        Serial.println(tilted_recklessness_count);
        tilted_count = 0;
      }
      else{
        tilted_warning_count += 1;
        Serial.print("위험 운전 적발. 위험 운전 총합 3회 이상 적발 시 제재 예정: ");
        Serial.print(tilted_warning_count);
        Serial.print(", ");
        Serial.println(tilted_recklessness_count);
        tilted_count = 0;
      }
    }

    if(tilted_warning_count >= 3){
      if(tilted_recklessness_count >= 1){
        Serial.println("위험 운전 및 난폭 운전 적발: 3일 사용 제재 예정");
        tilted_warning_count = 0;
        tilted_recklessness_count = 0;
      }
      else{
        Serial.println("위험 운전 적발: 1일 사용 제재 예정");
        tilted_warning_count = 0;
        tilted_recklessness_count = 0;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
}

void loop() {
  if(driving_mode){
    detect_dangerous_driving();
  }

  if(parking_complete){
    tilted_warning_count = 0;
    tilted_recklessness_count = 0;
    current_time_for_object_detection = 0.0;
    previous_time_for_object_detection = 0.0;
    current_time_for_object_detection_end = 0.0;
    previous_time_for_object_detection_end = 0.0;
  }
}
