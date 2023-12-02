#define S2 30        
#define S3 28      
#define sensorOut 26 


int R_Min = 5; 
int R_Max = 25; 
int G_Min = 4; 
int G_Max = 42;
int B_Min = 4; 
int B_Max = 45; 

/*Define int variables*/
int Red = 0;
int Green = 0;
int Blue = 0;
int previous_time = 0, current_time = 0;

int redValue;
int greenValue;
int blueValue;
int Frequency;

uint8_t park_flag = 0, totval = 0, count = 0;

bool driving_mode = true;
bool on_the_crosswalk = false;
bool previous_color_white = false;
bool crosswalk_flag = false;

void setup() {
  pinMode(S2, OUTPUT);    
  pinMode(S3, OUTPUT);      
  pinMode(sensorOut, INPUT); 
  Serial.begin(115200);      
  delay(1000);            
}

void loop() {
  Red = getRed();
  redValue = map(Red, R_Min,R_Max,255,0); // all of the values need to be calibrated. This is temporay value. 
  //We should add calibrating code or We should set the min & max value by hand.
  delay(10);
 
  Green = getGreen();
  greenValue = map(Green, G_Min,G_Max,255,0);
  delay(10);
 
  Blue = getBlue();
  blueValue = map(Blue, B_Min,B_Max,255,0); 
  delay(10);

  Serial.print("Red = ");
  Serial.print(redValue);  
  Serial.print("    ");
  Serial.print("Green = ");
  Serial.print(greenValue); 
  Serial.print("    ");
  Serial.print("Blue = ");
  Serial.println(blueValue);
  delay(200);   

  drive();
}



int getRed() {
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  Frequency = pulseIn(sensorOut, LOW);
  return Frequency;
}

int getGreen() {
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  Frequency = pulseIn(sensorOut, LOW);
  return Frequency;
}

int getBlue() {
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  Frequency = pulseIn(sensorOut, LOW);
  return Frequency;
}

int drive() {

  Serial.println("Now Driving");

  detecting_crosswalk();

}

int detecting_crosswalk(){
  
  totval = redValue + greenValue + blueValue;
  
  if(totval >= 660){ //흰색일시 시작
    current_time = millis();
    previous_color_white = true;

    totval = redValue + greenValue + blueValue; //check again

    while(current_time - previous_time <= 50000){ //50초 동안

      Red = getRed();
      redValue = map(Red, R_Min,R_Max,255,0); // all of the values need to be calibrated. This is temporay value. 
      //We should add calibrating code or We should set the min & max value by hand.
      delay(10);
 
      Green = getGreen();
      greenValue = map(Green, G_Min,G_Max,255,0);
      delay(10);
 
      Blue = getBlue();
      blueValue = map(Blue, B_Min,B_Max,255,0); 
      delay(10);
      

      totval = redValue + greenValue + blueValue;

      if(previous_color_white == true){
        if(totval <= 60){
          count+=1; //검정 감지시 +1
          previous_color_white = false;
        }
        else{
          count=0;
          break;
        }
      }
      if(previous_color_white == false){
        if(totval >= 660){
          count+=1;
          previous_color_white == true;
        }
        else{
          count=0;
          break;
        }
      }

      if(count>7){
        on_the_crosswalk = true;
        return on_the_crosswalk;
        break;
      }
      
      delay(1000);
    }
  }
}









