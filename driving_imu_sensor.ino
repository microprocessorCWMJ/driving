#include <Wire.h>

const int MPU_ADDR = 0x68;    // I2C통신을 위한 MPU6050의 주소
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;   // 가속도(Acceleration)와 자이로(Gyro)
double angleAcX;
double angleAcY;
// double angleAcZ;
const double RADIAN_TO_DEGREE = 180 / 3.14159;

// If users change the button to park, driving mode must be false.
bool driving_mode = true;

// If white color is repetitively detected, on_the_crosswalk flag is set.
bool on_the_crosswalk = true;

// variables that need to count the number of reckless driving on crosswalk or lane.
const float zero_time = 0.0;
float previous_time = 0.0;
float current_time = 0.0;
uint8_t count = 0;

double getAngleXY() {
  getData();  
  // 삼각함수를 이용한 롤(Roll)의 각도 구하기 
  angleAcX = atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2)));
  angleAcX *= RADIAN_TO_DEGREE;
  // 삼각함수를 이용한 피치(Pitch)의 각도 구하기
  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;
  // angleAcZ값(Yaw)은 아래의 삼각함수 공식은 있으나, 가속도 센서만 이용해서는 원하는 데이터를 얻을 수 없어 생략
  // angleAcZ = atan(sqrt(pow(AcX, 2) + pow(AcY, 2)) / AcZ );
  // angleAcZ *= RADIAN_TO_DEGREE;
}

// 기울기 센서와 아두이노 간 I2C 통신을 시작하게 해준다.
void initSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);   // I2C 통신용 어드레스(주소)
  Wire.write(0x6B);    // MPU6050과 통신을 시작하기 위해서는 0x6B번지에    
  Wire.write(0);
  Wire.endTransmission(true);
}

void getData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);   // AcX 레지스터 위치(주소)를 지칭합니다
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

// I2C 통신을 통해 가속도와 자이로 값을 불러오기 때문에 SCL, SDA pin만을 사용한다.
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
      Serial.println("횡단보도에서의 위험 운전 혹은 지나치게 반복적인 차선 변경이 검출되어 해당 사용자의 킥보드 사용을 제재할 예정입니다.");
      current_time = 0.0;
      previous_time = 0.0;
      count = 0;
    }
  }
  delay(100);
}


