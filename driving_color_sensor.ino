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
// It names "on_the_crosswalk", but it will be also setted when user is changing lane repetitivley.
bool on_the_crosswalk = false;
// If ultrasound is detected objects repetitively, many_objects_around flag is set.
bool many_objects_around = true;
//----------------------------------------------------------------------------------------------------------------------//

int R_Min = 5; 
int R_Max = 25; 
int G_Min = 4; 
int G_Max = 42;
int B_Min = 4; 
int B_Max = 45; 

/*Define int variables*/
int Red1, Red2, Red3, Red4;
int Green1, Green2, Green3, Green4;
int Blue1, Blue2, Blue3, Blue4;
int color_previous_time = 0, color_current_time = 0;

int redValue1, redValue2, redValue3, redValue4;
int greenValue1, greenValue2, greenValue3, greenValue4;
int blueValue1, blueValue2, blueValue3, blueValue4;
int Frequency;

float white_time = 0.0, black_time = 0.0, white_start_time = 0.0, black_start_time = 0.0, crosswalk_time;
uint8_t crosswalk_count = 0, crosswalk_warning_count=0, tmp_crosswalk_count;
uint16_t totval1, totval2;

bool white_flag = false;

void setup() {
  pinMode(S2, OUTPUT);    
  pinMode(S3, OUTPUT);      
  pinMode(sensorOut1, INPUT); 

  pinMode(S22, OUTPUT);    
  pinMode(S32, OUTPUT);      
  pinMode(sensorOut2, INPUT);

  pinMode(S23, OUTPUT);    
  pinMode(S33, OUTPUT);      
  pinMode(sensorOut3, INPUT); 

  pinMode(S24, OUTPUT);    
  pinMode(S34, OUTPUT);      
  pinMode(sensorOut4, INPUT);  

  Serial.begin(115200);      
  delay(1000);
}

void loop() {
  if(driving_mode){
    detect_crosswalk();
  }
}

int getRed(int x) {
  if(x==1){
    digitalWrite(S2,LOW);
    digitalWrite(S3,LOW);
    Frequency = pulseIn(sensorOut1, LOW);
  }
  else if(x==2){
    digitalWrite(S22,LOW);
    digitalWrite(S32,LOW);
    Frequency = pulseIn(sensorOut2, LOW);
  }
  else if(x==3){
    digitalWrite(S23,LOW);
    digitalWrite(S33,LOW);
    Frequency = pulseIn(sensorOut3, LOW);
  }
  else if(x==4){
    digitalWrite(S24,LOW);
    digitalWrite(S34,LOW);
    Frequency = pulseIn(sensorOut4, LOW);
  }
  return Frequency;
}

int getGreen(int x) {
  if(x==1){
    digitalWrite(S2,HIGH);
    digitalWrite(S3,HIGH);
    Frequency = pulseIn(sensorOut1, LOW);
  }
  else if(x==2){
    digitalWrite(S22,HIGH);
    digitalWrite(S32,HIGH);
    Frequency = pulseIn(sensorOut2, LOW);
  }
  else if(x==3){
    digitalWrite(S23,HIGH);
    digitalWrite(S33,HIGH);
    Frequency = pulseIn(sensorOut3, LOW);
  }
  else if(x==4){
    digitalWrite(S24,HIGH);
    digitalWrite(S34,HIGH);
    Frequency = pulseIn(sensorOut4, LOW);
  }
  return Frequency;
}

int getBlue(int x) {
  if(x==1){
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  Frequency = pulseIn(sensorOut1, LOW);
  }
  else if(x==2){
  digitalWrite(S22,LOW);
  digitalWrite(S32,HIGH);
  Frequency = pulseIn(sensorOut2, LOW);
  }
  else if(x==3){
  digitalWrite(S23,LOW);
  digitalWrite(S33,HIGH);
  Frequency = pulseIn(sensorOut3, LOW);
  }
  else if(x==4){
    digitalWrite(S24,LOW);
    digitalWrite(S34,HIGH);
    Frequency = pulseIn(sensorOut4, LOW);
  }
  return Frequency;
}

void detect_crosswalk(){
  Red1 = getRed(1);
  Red2 = getRed(2);
  redValue1 = map(Red1, R_Min,R_Max,255,0);
  redValue2 = map(Red2, R_Min,R_Max,255,0); // all of the values need to be calibrated. This is temporay value. 
  //We should add calibrating code or We should set the min & max value by hand.
  delay(10);
  Green1 = getGreen(1);
  Green2 = getGreen(2);
  greenValue1 = map(Green1, G_Min,G_Max,255,0);
  greenValue2 = map(Green2, G_Min,G_Max,255,0);
  delay(10);
  Blue1 = getBlue(1);
  Blue2 = getBlue(2);
  blueValue1 = map(Blue1, B_Min,B_Max,255,0); 
  blueValue2 = map(Blue2, B_Min,B_Max,255,0); 
  delay(10);  

  //킼보드 전면 쪽에 부착된 두 컬러 센서를 사용
  totval1 = redValue1 + greenValue1 + blueValue1;
  totval2 = redValue2 + greenValue2 + blueValue2;
  
  if(!white_flag && (totval1 >= 660) || (totval2 >= 660)){
    white_flag = true;
    black_time = millis() - black_start_time;
    crosswalk_count += 1;
    white_start_time = millis();
  }

  else if(white_flag && ((totval1 <= 60) || (totval2 <= 60))){
    white_flag = false;
    white_time = millis() - white_start_time;
    crosswalk_count += 1;
    black_start_time = millis();
  }
  
  // 성인 남성의 평균 도보 속도: 4.8km/h = 1.33m/s
  // 횡단보도 흰 블럭 규격: 45~50cm -> 안정성을 위해 최소값인 45cm를 기준으로 삼음
  // 횡단보도 흰 블럭 간 간격 규격: 1.5 * (흰 블럭 길이) -> 안정성을 위해 최소값인 45cm의 1.5배를 기준으로 삼음
  // 성인 남성의 평균 도보 속도로 횡단보도의 흰 블럭을 건너는 데 걸리는 시간: (거리) / (속력) = 0.45/1.33 = 0.338초
  // 성인 남성의 평균 도보 속도로 횡단보도의 흰 블럭 간격을 건너는 데 걸리는 시간: 0.508초
  if((white_time>0) && (white_time<338)){
    crosswalk_warning_count += 1;
    Serial.print("횡단보도 내에서의 빠른 움직임 혹은 지나치게 잦은 차선 변경 감지. 3회 이상 적발 시 강력 제재 예정: ");
    Serial.println(crosswalk_warning_count);
  }

  if((black_time>0) && (black_time<508)){
    crosswalk_warning_count += 1;
    Serial.print("횡단보도 내에서의 빠른 움직임 혹은 지나치게 잦은 차선 변경 감지. 3회 이상 적발 시 강력 제재 예정: ");
    Serial.println(crosswalk_warning_count);
  }
  
  if(crosswalk_count>=3){
    //on the crosswalk or lane change repeatitively
    on_the_crosswalk = true;
    if(crosswalk_count == 3){   
      tmp_crosswalk_count = crosswalk_count;
      crosswalk_time = millis();
    }

    if(crosswalk_warning_count>=3){
      Serial.println("난폭 운전 적발: 3일 사용 제재 예정");
    }

    if(crosswalk_count - tmp_crosswalk_count > 0){
      tmp_crosswalk_count = crosswalk_count;
      crosswalk_time = millis();
    }

    else if((millis()-crosswalk_time > 3000) && (crosswalk_count == tmp_crosswalk_count)){
      on_the_crosswalk = false;
      crosswalk_count = 0;
      tmp_crosswalk_count = 0;
    }
  }
}


