#include <Wire.h>

const int MPU_ADDR = 0x68;    // MPU6050 address for I2C communication
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;   // Acceleration & Gyro 
double angleAcX;
double angleAcY;
// double angleAcZ;
const double RADIAN_TO_DEGREE = 180 / 3.14159;

// If users change the button to park, driving mode must be false.
bool driving_mode = true;
// If white color is repetitively detected, on_the_crosswalk flag is set.
// It names "on_the_crosswalk", but it is also setted when user is changing lane repetitivley.
bool on_the_crosswalk = true;
// If ultrasound is detected objects repetitively, many_objects_around flag is set.
bool many_objects_around = true;
// If parking is completed, parking completion flag is set.
bool parking_complete = false;

// variables that need to count the number of reckless driving on crosswalk or lane.
const float zero_time = 0.0;
float previous_time = 0.0;
float current_time = 0.0;
uint8_t count = 0;
uint8_t warning_count = 0;
//recklessness: Dangerous driving even if there are detected so many objects(person, vehicle, dog, whatever ... )
uint8_t recklessness_count = 0;

double getAngleXY() {
  getData();  
  // Roll angle
  angleAcX = atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2)));
  angleAcX *= RADIAN_TO_DEGREE;
  // Pitch angle
  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;
}

// Initiate I2C communication between imu sensor and arduino.
void initSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);   // Address for I2C communication
  Wire.write(0x6B);    // write on 0x6B for communication with MPU6050
  Wire.write(0);
  Wire.endTransmission(true);
}

void getData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);   // AcX register address
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);  // AcX 주소 이후의 14byte의 데이터를 요청
  AcX = Wire.read() << 8 | Wire.read(); //두 개의 나뉘어진 바이트를 하나로 이어 붙여서 각 변수에 저장
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}

// We only use SCL, SDA pins because we will only use I2C communication.
void setup() {
  Serial.begin(9600);
}

void loop() {
  if(driving_mode && on_the_crosswalk){
    initSensor();
    getAngleXY(); 

    if(angleAcY >= 40 || angleAcY <= -40){
      current_time = millis();
      if(current_time - previous_time <= 10000){
        count+=1;
        Serial.print("Angle:");
        Serial.println(angleAcY);
        Serial.print("count:");
        Serial.println(count);
        Serial.println("-----------------------------------------------");
      }
      previous_time = current_time;
      delay(1000);
    }

    if(current_time - zero_time <= 60000 && count >= 5){
      if(many_objects_around){
        warning_count += 1;
        recklessness_count += 1;

        Serial.print("근거리에 물체가 다수 포착, 난폭 운전 적발. 위험 운전 총합 3회 이상 적발 시 강력 제재 예정: ");
        Serial.println(warning_count);
        current_time = 0.0;
        previous_time = 0.0;
        count = 0;
      }
      else{
        warning_count += 1;
        
        Serial.print("위험 운전 적발. 위험 운전 총합 3회 이상 적발 시 제재 예정");
        Serial.println(warning_count);
        current_time = 0.0;
        previous_time = 0.0;
        count = 0;
      }
    }

    if(warning_count >= 3){
      if(recklessness_count >= 2){
        Serial.println("위험 운전 및 난폭 운전 적발: 3일 사용 제재 예정");
        warning_count = 0;
        recklessness_count = 0;
      }
      else{
        Serial.println("위험 운전 적발: 1일 사용 제재 예정");
        warning_count = 0;
        recklessness_count = 0;
      }
    }

    if(parking_complete == true){
      warning_count = 0;
      recklessness_count = 0;
    }
  }
}


