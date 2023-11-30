//pin numbers should be matched in every code

#define LeftFrontpinTrig 30
#define LeftFrontpinEcho 32

// We temporarily implemented this code with only one sensor.
// It will be updated by 4 sensors version

// If users change the button to park, driving mode must be false.
bool driving_mode = true;
// If white color is repetitively detected, on_the_crosswalk flag is set.
bool on_the_crosswalk = true;
// If ultrasound is detected objects repetitively, many_objects_around flag is set.
bool many_objects_around_LeftFront = false;
// If parking is completed, parking completion flag is set.
bool parking_complete = false;

float current_time_for_object_detection = 0.0;
float previous_time_for_object_detection = 0.0;
float current_time_for_object_detection_end = 0.0;
float previous_time_for_object_detection_end = 0.0;

double measureDistanceCm_LeftFront(){
  digitalWrite(LeftFrontpinTrig, LOW);
  delayMicroseconds(5);
  digitalWrite(LeftFrontpinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(LeftFrontpinTrig, LOW);
  
  double duration_front = pulseIn(LeftFrontpinEcho, HIGH);
  double cm_front = 0.0343 * (duration_front/2);
  return cm_front;
}

void set_many_objects_around_LeftFront(){
  double distance_front = measureDistanceCm_LeftFront();
  if(distance_front < 1000){
    if(distance_front <= 50){
      current_time_for_object_detection = millis();
      if(current_time_for_object_detection - previous_time_for_object_detection < 5000){
        many_objects_around_LeftFront = true;
        Serial.println(distance_front);
        Serial.println("There are many objects around you.");
      }  
      previous_time_for_object_detection = current_time_for_object_detection;
    }
  }
  if(distance_front > 50){
    current_time_for_object_detection_end = millis();
    if(previous_time_for_object_detection == current_time_for_object_detection){
      if(current_time_for_object_detection_end - previous_time_for_object_detection_end >= 10000){
        many_objects_around_LeftFront = false;
        Serial.println("There are no objects that are detected.");
        previous_time_for_object_detection_end = current_time_for_object_detection_end;
      }
    }
    else{
      current_time_for_object_detection_end = 0;
      previous_time_for_object_detection_end = 0;
    }
  }
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LeftFrontpinTrig, OUTPUT);
  pinMode(LeftFrontpinEcho, INPUT);
  //pinMode(LeftBehindpinTrig, OUTPUT);
  //pinMode(LeftBehindpinEcho, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  set_many_objects_around_LeftFront();
}