#include "Arduino.h"
#include "pins.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_Address 0x3C
Adafruit_SSD1306 oled(1);
#include "Wire.h"
#define TCAADDR 0x70
int verified_inputs = 0;

//               Lane Act | Curr Cnt |   Max Cycles   | Error   |Response |Position | Verify
int volatile lane[][4] = {{0,0,0,0},{0,0,0,0},{100,100,100,100},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};


void final_test(int lane_num, int inputs1){
  int stuck_inputs = 0;
  int total_inputs = 0;

  if(lane[7][lane_num]==false){
  verified_inputs = 0;
  delay(1000);
  Serial.println("Eject Lever");
  digitalWrite(out_pin[lane_num], LOW);    //put

  delay(300);

  for(int x = 0; x < 10; x++){
    
    delay(200);
    
    digitalWrite(noid_pin[lane_num], HIGH);//compress solenoid
    delay(200);
    
    if(digitalRead(response_pin[lane_num])==LOW){
      verified_inputs++;
      Serial.print(lane_num);
      Serial.print(" Verified Inputs: ");
      Serial.println(verified_inputs);
    }
    delay(200);
    
    digitalWrite(noid_pin[lane_num], LOW);
    delay(300);
        
    if(digitalRead(response_pin[lane_num])==LOW){
      stuck_inputs++;
      Serial.print(lane_num);
      Serial.print(" Stuck Inputs: ");
      Serial.println(stuck_inputs);
    }   
    
  }
  
  digitalWrite(out_pin[lane_num], HIGH);  
  delay(1000);
  digitalWrite(noid_pin[lane_num], LOW);
  total_inputs = verified_inputs - stuck_inputs;
  Serial.print(total_inputs);
  verified_inputs = total_inputs;
  //Serial.print("Verified Inputs: ");
  //Serial.println(verified_inputs);
    
  }

}


//Sets up to use Multiplexer
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission(); 
}


//used to reset on button presses
void reset(int lane_number){
    //Serial.print("\nInterrupt "); Serial.print(lane_number);
    if(lane[0][lane_number] == true){
      lane[0][lane_number] = false;
      digitalWrite(noid_pin[lane_number], LOW);  //makes sure lever and solenoid are in default states and not stuck on.
      digitalWrite(out_pin[lane_number], HIGH);
      
    
    
    }  //lane deactive

    else if(lane[0][lane_number] == false){
      lane[0][lane_number] = true;  //lane active
      //Serial.print("\nReset");
      lane[1][lane_number] = 0;  //count
      lane[4][lane_number] = 0;  //response
      lane[6][lane_number] = 0;  //verify
      lane[5][lane_number] = 0;  //position
      lane[7][lane_number] = 0;  //message sent
      lane[8][lane_number] = 1;  //reset done
    }
}


//This function return the value of the BCD counter.
int bcd_reader(){
    int temp = 0;
    if(digitalRead(bcd_pin[0])==HIGH){temp +=  pow(2,0);}
    if(digitalRead(bcd_pin[1])==HIGH){temp +=  pow(2,1);}
    if(digitalRead(bcd_pin[2])==HIGH){temp +=  pow(2,2);}
    if(digitalRead(bcd_pin[3])==HIGH){temp +=  pow(2,3);}
    switch(temp){
      case 0:
        temp = 1000;
        break;
      case 1:
        temp = 100;
        break;
      case 2:
        temp = 200;
        break;
      case 3:
        temp = 300;
        break;
      case 4:
        temp = 400;
        break;
      case 5:
        temp = 500;
        break;
      case 6:
        temp = 600;
        break;
      case 7:
        temp = 700;
        break;
      case 8:
        temp = 2;
        break;
      case 9:
        temp = 10;
        break;
    }
    return temp;

}
//used for input testing
/*
void test_mode(){
delay(500);
for (int x = 0; x < 4; x++){
    
    digitalWrite(red_led_pin[x],HIGH);
    
    
    if (digitalRead(response_pin[x])==LOW) {Serial.print(x + "Response:  On");}
    else {Serial.print(x+1);
    Serial.print(" Response:  Off");}

    if (digitalRead(position_pin[x])==LOW) {Serial.print("   |   Position:  On");}
    else {Serial.print("   |   Position:  Off");}

    if (digitalRead(verify_pin[x])==LOW) {Serial.print("   |   Verify:  On");}
    else {Serial.print("   |   Verify:  Off");}

    if (digitalRead(button_pin[x])==LOW) {Serial.print("   |   Button:  On\n");digitalWrite(red_led_pin[x],HIGH);}
    else {Serial.print("   |    Button:  Off\n");digitalWrite(red_led_pin[x],LOW);}
    
}

Serial.write("---------------------------------------------------------------------------------------\n");

}
*/


void cycle(){


  //if lane is inactive set the lane max count to BCD reader
  for(int x = 0; x < 4; x++){
    if (lane[0][x] == false){
      lane[2][x] = bcd_reader();
    }
  }


  //The cycle will start  if it has not reached the proper number of cycles and the lane is true.
  //Serial.print ("\nBeginning of Cycle");
  
  //turn on both LED's off
  for(int x = 0; x < 4; x++){
    if(lane[0][x] == true){
      digitalWrite(red_led_pin[x], LOW);
      digitalWrite(green_led_pin[x], LOW);
    }
  }
 
  //eject lever
  for(int x = 0; x < 4; x++){
    if(lane[0][x] == 1){
      digitalWrite(out_pin[x],LOW);
    }
  }
  delay(300);  //300 max
 
  //verify lever ejected
  for(int x = 0; x < 4; x++){
    if(lane[0][x]==1){
      if(digitalRead(verify_pin[x]) == LOW){
        lane[6][x]++;  //verify
      }
    }  
  }
  //verify position input
  for(int x = 0; x < 4; x++){
    if(lane[0][x]==1){  
      if(digitalRead(position_pin[x]) == LOW){
        lane[5][x]++;  //position
      }
    }
  }
  delay(100);
  
  //use solenoid
  for(int x = 0; x < 4; x++){
    if(lane[0][x]==1){
      digitalWrite(noid_pin[x],HIGH);
    }
  }

  delay(100);

  //check for response turn off solenoid
  for(int x = 0; x < 4; x++){
    if(lane[0][x]==1){
      if(digitalRead(response_pin[x]) == LOW){
        
        digitalWrite(noid_pin[x], LOW);
        delay(100);
        if(digitalRead(response_pin[x])==HIGH){
          //digitalWrite(noid_pin[x],HIGH);
          delay(100);
          if(digitalRead(response_pin[x])==HIGH){lane[4][x]++;}

        }
      }
      digitalWrite(noid_pin[x], LOW);
    }
  }

  delay(100);
 
 //activate output for lane
  for(int x = 0; x < 4; x++){
    if(lane[0][x] == 1){
      digitalWrite(out_pin[x], HIGH);
    }
  }

  delay(100);
  
  for(int x = 0; x < 4; x++){
    if(lane[0][x]==1){
      tcaselect(x);
      oled.clearDisplay();
      oled.setCursor(0,0);
      oled.setTextSize(2);
      oled.print("P"); oled.print(lane[5][x]);
      oled.print(" R"); oled.print(lane[4][x]);
      oled.print("\nV"); oled.print(lane[6][x]);
      oled.print(" C");
      oled.print(lane[2][x]);
      oled.display();
    }
  }


  
  for (int x = 0; x < 4; x++){
    if(lane[1][x] >= lane[2][x]){ //if count => max count
      lane[0][x] = false;       //set lane to be inactive
      //if response, verify, and position are equal turn on green light
      if((lane[1][x] == lane[2][x]) && (lane[2][x] == lane[4][x]) && (lane[2][x] == lane[5][x]) && (lane[2][x] == lane[6][x])){
          
          //final_test(x, verified_inputs);
          
          
          
            digitalWrite(green_led_pin[x], HIGH);
            digitalWrite(red_led_pin[x], LOW);
              if(lane[7][x]==false){
                //Serial.print("\nLane ");  Serial.print(x); Serial.print(" has completed successfully");
                tcaselect(x);
                oled.clearDisplay();
                oled.setCursor(0,0);
                oled.setTextSize(2);
                oled.print("This lane has passed successfully!");
                oled.display();
                lane[7][x] = true;
              }
          }
          
      else{
          digitalWrite(green_led_pin[x], LOW);
          digitalWrite(red_led_pin[x], HIGH);
            if(lane[7][x] == false){
              //Serial.print("\nLane ");  Serial.print(x); Serial.print(" has failed");
              tcaselect(x);
              oled.clearDisplay();
              oled.setCursor(0,0);
              oled.setTextSize(1);
              if(lane[2][x] != lane[4][x]){
                oled.print("Response:  ");
                oled.print(lane[2][x] - lane[4][x]);
              }
              if(lane[2][x] != lane[5][x]){
                oled.print("\nPosition:  ");
                oled.print(lane[2][x] - lane[5][x]);
              }
              if(lane[2][x] != lane[6][x]){
                oled.print("\nVerify:  ");
                oled.print(lane[2][x] - lane[6][x]);
              }
              
              //oled.print("This lane has failed.");
              oled.display();
              lane[7][x] = true;
            }
      }
      
    }
    
  }

  for(int x = 0; x < 4; x++){if(lane[0][x]==1){lane[1][x]++;}}  //if lane active increase the count
}



