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
bool on_the_crosswalk = true;
// If ultrasound is detected objects repetitively, many_objects_around flag is set.
bool many_objects_around = false;
// If parking is completed, parking completion flag is set.
bool parking_complete = false;
//----------------------------------------------------------------------------------------------------------------------//

float current_time_for_object_detection = 0.0;
float previous_time_for_object_detection = 0.0;
float current_time_for_object_detection_end = 0.0;
float previous_time_for_object_detection_end = 0.0;

double measureDistanceCm(uint8_t pinTrig, uint8_t pinEcho){
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(5);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);
  double duration = pulseIn(pinEcho, HIGH);
  double cm = 0.0343 * (duration/2);
  return cm;
}

// Determine whether many_objects_around_LeftFront flag sets, or clears.
void detect_objects_around(){
  double distance_LeftFront = measureDistanceCm(LeftFrontpinTrig, LeftFrontpinEcho);
  double distance_LeftBehind = measureDistanceCm(LeftBehindpinTrig, LeftBehindpinEcho);
  double distance_RightFront = measureDistanceCm(RightFrontpinTrig, RightFrontpinEcho);
  double distance_RightBehind = measureDistanceCm(RightBehindpinTrig, RightBehindpinEcho);

  //Ensuring that there is no distance error or The area that you are in is not an open land.
  if((distance_LeftFront < 1000 && distance_LeftFront > 0) && (distance_LeftBehind < 1000 && distance_LeftBehind > 0) && (distance_RightFront < 1000 && distance_RightFront > 0) && (distance_RightBehind < 1000 && distance_RightBehind > 0)){
    
    // There is an object, or many objects around you!
    if((distance_LeftFront <= 30) || (distance_LeftBehind <= 30) || (distance_RightFront <= 30) || (distance_RightBehind <= 30)){
      current_time_for_object_detection = millis();
      if(current_time_for_object_detection - previous_time_for_object_detection < 5000){
        many_objects_around = true;
      }
      previous_time_for_object_detection = current_time_for_object_detection;
    }
    
    // There is no objects around you
    else if((distance_LeftFront > 30) || (distance_LeftBehind > 30) || (distance_RightFront > 30) || (distance_RightBehind > 30)){
      current_time_for_object_detection_end = millis();
      if(current_time_for_object_detection_end - previous_time_for_object_detection_end >= 10000){
        many_objects_around = false;
      }
      previous_time_for_object_detection_end = current_time_for_object_detection_end;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LeftFrontpinTrig, OUTPUT);
  pinMode(LeftFrontpinEcho, INPUT);
  pinMode(LeftBehindpinTrig, OUTPUT);
  pinMode(LeftBehindpinEcho, INPUT);
  pinMode(RightFrontpinTrig, OUTPUT);
  pinMode(RightFrontpinEcho, INPUT);
  pinMode(RightBehindpinTrig, OUTPUT);
  pinMode(RightBehindpinEcho, INPUT);
  Serial.begin(115200);
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(driving_mode){
    detect_objects_around();
  }

  if(parking_complete){
    current_time_for_object_detection = 0.0;
    previous_time_for_object_detection = 0.0;
    current_time_for_object_detection_end = 0.0;
    previous_time_for_object_detection_end = 0.0;
  }
}