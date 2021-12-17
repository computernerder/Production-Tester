#include "functions.h"


//interupt requests
void pin_ISR_0(){reset(0);}
void pin_ISR_1(){reset(1);}
void pin_ISR_2(){reset(2);}
void pin_ISR_3(){reset(3);}


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
    pinMode(button_pin[x], INPUT);
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

  delay(2000);

}



void loop() {
//Serial.print("This is the begining!!!!!");
cycle();

}

