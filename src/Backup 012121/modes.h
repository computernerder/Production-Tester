//here there be dragons!!!

//The below works for a single lane just if I screw it up when trying to make all four lanes work.

#include "functions.h"

int volatile count = 1;

int volatile verify_1 = 0;
int volatile response_1 = 0;
int volatile position_1 = 0;
byte volatile reset_was_done = true;

bool message_sent = false;

void pin_ISR(){
  Serial.print("\nInterrupt");
  Serial.println(lane_active);  
  if(lane_active == true){lane_active = false;}
  else if(lane_active == false){
    lane_active = true;
    Serial.print("\nReset");
    count = 1;
    response_1 = 0;
    verify_1 = 0;
    position_1 = 0;
    message_sent = false;
     reset_was_done = true;    
  }
 
    


}
 
volatile byte state = LOW;

void setup() {

Serial.begin(9600);

 //Outputs set pinmode
for(int x = 0;x<4;x++){
  pinMode(noid_pin[x], OUTPUT);
  pinMode(out_pin[x],OUTPUT);
  pinMode(red_led_pin[x], OUTPUT);
  pinMode(green_led_pin[x], OUTPUT);
  pinMode(verify_pin[x], INPUT);
  pinMode(response_pin[x], INPUT);
  pinMode(button_pin[x], INPUT);
}

pinMode(buzzer_pin, OUTPUT);

attachInterrupt(digitalPinToInterrupt(button_pin[0]), &pin_ISR ,RISING);

/*
//Initialize Displays
oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
Wire.begin();

//Clear all the display's
tcaselect(0);
oled.begin();
oled.clearDisplay();
oled.display();
tcaselect(1);
oled.begin();
oled.clearDisplay();
oled.display();
tcaselect(2);
oled.begin();
oled.clearDisplay();
oled.display();
tcaselect(3);
oled.begin();
oled.clearDisplay();
oled.display();

// Display a start text.
for(int x = 0; x<4; x++){
  tcaselect(x);                 
  oled.begin();
  oled.setTextSize(1);
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(20,0);
  oled.println("4 Oled");
  oled.setCursor(5,16);
  oled.print("Display test: ");
  oled.setCursor(100,0);
  oled.setTextSize(3);
  oled.println(x + 1);
  oled.display();
}
*/
//Ensures lever is in and solenoids are off
for (int x = 0; x < 4; x++){
  digitalWrite(out_pin[x],HIGH);
  digitalWrite(noid_pin[x],LOW);
  } 


for (int x = 0; x < 4; x++){
  lane[2][x] = number_of_cycles;
}

delay(2000);

}



void reset_all(){
    if((digitalRead(button_pin[0])==LOW) && (reset_was_done == false) ){
      Serial.print("\nReset During Operation");
      digitalWrite(noid_pin[0], LOW);
      digitalWrite(out_pin[0], HIGH);
      count = number_of_cycles+1;
      message_sent = false;
      reset_was_done = true;

    }  
    
}





// the loop function runs over and over again forever
void loop() {

number_of_cycles = bcd_reader() * 10;



reset_all();

if((count < number_of_cycles+1) && (lane_active == true)){
  Serial.print ("\nBeginning of Cycle");
  Serial.print("\nreset_was_done:  ");
  Serial.print(reset_was_done);
  
  //turn on Green LED
  digitalWrite(red_led_pin[0], LOW);
  digitalWrite(green_led_pin[0], LOW);
 
  //setup for cycle
  //eject lever
  digitalWrite(out_pin[0],LOW);
  delay(500);
 
  //verify lever ejected
  if(digitalRead(verify_pin[0]) == LOW){
    verify_1++;
  }
  //verify position input
  if(digitalRead(position_pin[0]) == LOW){
    position_1++;
  }
  delay(500);
  
  //use solenoid
  digitalWrite(noid_pin[0],HIGH);

  delay(250);

  //check for response
  if(digitalRead(response_pin[0]) == LOW){
    response_1++;
  }

  digitalWrite(noid_pin[0], LOW);

  delay(500);
  

  Serial.print("\nVerify:  ");
  Serial.print(verify_1);
  Serial.print("\nPosition:   ");
  Serial.print(position_1);
  Serial.print("\nResponse:   ");
  Serial.print(response_1);
  Serial.print("\nCount:   ");
  Serial.print(count);
   Serial.print("\n---------------");

  digitalWrite(out_pin[0], HIGH);

  delay(500);

  count++;


 
}   //This happens when the number of cycles = the expected number
else{
    //digitalWrite(red_led_pin[0], LOW);
    //digitalWrite(green_led_pin[0], LOW);
  }

  if(((count - 1) == response_1) && ((count - 1) == position_1) && ((count - 1) == verify_1) && ((count - 1) == number_of_cycles)){
  digitalWrite(red_led_pin[0], LOW);
  digitalWrite(green_led_pin[0], HIGH);
    if(message_sent == false){
      Serial.print("\nAll are equal!");
      message_sent = true;
      //lane_active = false;
      
    }
    
  
}//This is the failure condition where all counts are not equal.
 else if (((count - 1) == number_of_cycles) && ((number_of_cycles != response_1) || (number_of_cycles != position_1) || (number_of_cycles != verify_1))){


      digitalWrite(red_led_pin[0], HIGH);
      digitalWrite(green_led_pin[0], LOW);
      if(message_sent == false){
        Serial.print("\nThis is a failure condition");
        //lane_active = false;
      message_sent = true;
      }
      
 }


}





/*
lane x row 1 active IE lane[0][x]
lane x row 2 current count IE lane[1][x]
lane x row 3 max count count IE lane[2][x]
lane x row 4 lane failure count IE lane[3][x]
lane x row 5 response count IE lane[4][x]
lane x row 6 position countIE lane[5][x]
lane x row 7 verify countIE lane[6][x]

*/


