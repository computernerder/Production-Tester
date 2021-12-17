#include "functions.h"


//timer[0][x] = current
//timer[1][x] = previous

double debounce = 1500;
int feeder_cycles = 0;
String command;

//interupt requests

void pin_ISR_0(){
  timer[0][0] = millis();
  if(timer[0][0] >= timer[1][0] + debounce){timer[1][0] = timer[0][0]; reset(0);}
  }
  
void pin_ISR_1(){
  timer[0][1] = millis();
  Serial.print("2");
  if(timer[0][1] >= timer[1][1] + debounce){timer[1][1] = timer[0][1]; reset(1);}
}
void pin_ISR_2(){
  timer[0][2] = millis();
  Serial.print("3");
  if(timer[0][2] >= timer[1][2] + debounce){timer[1][2] = timer[0][2]; reset(2);}
}

void pin_ISR_3(){
  timer[0][3] = millis();
  Serial.print("4");
  if(timer[0][3] >= timer[1][3] + debounce){timer[1][3] = timer[0][3]; reset(3);}
}


int pelletCounter[4]={0,0,0,0};
double pelletTimer[2][4] = {{0,0,0,0},{0,0,0,0}};

void rest(int lane_number){
  //Serial.println("Rest");
timer[0][lane_number] = millis();
      if(timer[0][lane_number] >= timer[1][lane_number] + debounce){timer[1][lane_number] = timer[0][lane_number]; reset(lane_number);}
}

void serial_control(){
  if(Serial.available()){
    command = Serial.readStringUntil('\n');
    //Serial.print("Avalible");
    if(command.equals("1")){rest(0);}
    if(command.equals("2")){rest(1);}
    if(command.equals("3")){rest(2);}
    if(command.equals("4")){rest(3);}
    if(command.equals("s")){for(int x = 0; x > 4; x++){rest(x);}}
    
      
}
}

void setup() {
  
  Serial.begin(9600);

  //Outputs set pinmode
  for(int x = 0; x<4; x++){
    pinMode(noid_pin[x], OUTPUT);
    pinMode(out_pin[x],OUTPUT);
    pinMode(red_led_pin[x], OUTPUT);
    pinMode(green_led_pin[x], OUTPUT);
    pinMode(verify_pin[x], INPUT);
    pinMode(response_pin[x], INPUT);
    pinMode(button_pin[x], INPUT_PULLUP);
    //define button pins as interupts 
   
  }
  
   attachInterrupt(digitalPinToInterrupt(button_pin[0]), &pin_ISR_0 ,RISING);
   attachInterrupt(digitalPinToInterrupt(button_pin[1]), &pin_ISR_1 ,RISING);
   attachInterrupt(digitalPinToInterrupt(button_pin[2]), &pin_ISR_2 ,RISING);
   attachInterrupt(digitalPinToInterrupt(button_pin[3]), &pin_ISR_3 ,RISING);

  
  //Initialize Displays
  oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  Wire.begin();

  //Clear all the display's
  for(int x = 0; x < 4; x++){
  tcaselect(x);
  oled.begin();
  oled.setCursor(0,15);
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.print("Lane:");
  oled.print(x+1);
  oled.display();

  
  
}
  
  
  //Ensures lever is in and solenoids are off
  for (int x = 0; x < 4; x++){
    digitalWrite(out_pin[x],HIGH);
    digitalWrite(noid_pin[x],LOW);
    } 

  //set all lanes starting count equal to the BCD count during startup
  for (int x = 0; x < 4; x++){
    lane[2][x] = bcd_reader() * 10;
  }

  Serial.println("Setup Complete");
  delay(2000);

}


void feeder_test(int feeder_number){

  if(digitalRead(response_pin[feeder_number])==false){
     pelletTimer[0][1] = millis();
    if(pelletTimer[0][1] >= pelletTimer[1][1] + 50){pelletTimer[1][1] = pelletTimer[0][1]; reset(1);pelletCounter[feeder_number]++;}
    
  }
  tcaselect(feeder_number);
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.setTextSize(3);
  oled.print(pelletCounter[feeder_number]);
  oled.print(" - ");
  oled.println(digitalRead(response_pin[feeder_number]));
  Serial.println(digitalRead(response_pin[feeder_number]));
  
  oled.display();



}

double timer_1 = 0;
int timer_2 = 0;







void loop() {

//if(timer_2 == 0){
  //timer_1 = millis();
  //Serial.print("This is the begining!!!!!");
  
  


    
    
      
  
   
   
  serial_control();

  cycle();
  //test_mode();
  /*
  Serial.print(digitalRead(verify_pin[3]));
  Serial.print(" / ");
  //feeder_test(3);
  Serial.print(millis() - timer_1);
Serial.print(" / ");
  Serial.println(pelletCounter[3]);
  if(digitalRead(verify_pin[3]) == 0){
    
    pelletCounter[3]++;
    delay(2000);
    }
*/
    
    
//}







}

