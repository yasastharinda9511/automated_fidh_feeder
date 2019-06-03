#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <TimerOne.h>

#define button_1 8
#define button_2 9
#define button_3 10
#define button_4  11
#define button_5  12

#define test_pin  13

#define address0_hour 0x01
#define address0_minute 0x02

#define address1_hour 0x03
#define address1_minute 0x04

#define address2_hour 0x05
#define address2_minute 0x06

#define I2C_1 16
#define I2C_2 2

#define motor_1 0
#define motor_2 1
#define motor_pwm 5

volatile int state=1;
int set_time_index=0;
int blink_index=1;
boolean blinking=true;

////////////////////in state 1/////////////////////////////////
// rotate start button_3
// rotate stop button_4

////////////////////in state 2/////////////////////////////////
// increase value button_3
// decrease value button_4
// index change button_5
// enter value button_2

boolean button1_pressed=false;
boolean button2_pressed=false;
boolean button3_pressed=false;
boolean button4_pressed=false;
boolean button5_pressed=false;
boolean instant=false;

volatile boolean changed_display=true;
volatile boolean double_dot_blink=true;
volatile int counter=0;
volatile int counter_minute=0;
volatile int counter_hour=12;
int cursor_1=0;
String time_index;
int print_minute;
int print_hour;
int count=0;


boolean timer_index_change=true;
byte mode_button_1_pre_state=0;
byte mode_button_1_current_state=0;
byte mode_button_2_pre_state=0;
byte mode_button_2_current_state=0;
byte mode_button_3_pre_state=0;
byte mode_button_3_current_state=0;
byte mode_button_4_pre_state=0;
byte mode_button_4_current_state=0;
byte mode_button_5_pre_state=0;
byte mode_button_5_current_state=0;

int total_minute;
int remaining_minute;
int remaining_hour;
int count_hour;
int count_minute;
int remaining1_minute;
int remaining1_hour;
int closest_minute;

LiquidCrystal_I2C lcd(0x3F,I2C_1,I2C_1);

struct set_time_1{
    int hour;
    int minute;
  };

struct set_time_1 time1;
struct set_time_1 time2;
struct set_time_1 time3;
void setup() {
  //Serial.begin(9600);
  
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(10,INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);

  pinMode(motor_1,OUTPUT);
  pinMode(motor_2,OUTPUT);
  pinMode(motor_pwm,OUTPUT);
  
  pinMode(test_pin,OUTPUT);

  pinMode(8,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  EEPROM.write(address0_hour, 8);
  EEPROM.write(address0_minute, 0);

  EEPROM.write(address1_hour, 12);
  EEPROM.write(address1_minute, 1);

  EEPROM.write(address2_hour, 4);
  EEPROM.write(address2_minute, 30);

  
  lcd.begin();
  lcd.setCursor(0,0);
  lcd.print("   Automated  ");
  lcd.setCursor(0,1);
  lcd.print("  Fish Feeder  ");

  //digitalWrite(test_pin,HIGH);

  delay(3000);
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(blinkLED);
}

void loop() {
  mode_button_1_current_state=digitalRead(button_1);
  mode_button_2_current_state=digitalRead(button_2);
  mode_button_3_current_state=digitalRead(button_3);
  mode_button_4_current_state=digitalRead(button_4);
  mode_button_5_current_state=digitalRead(button_5);
  
  if(mode_button_1_current_state!= mode_button_1_pre_state and mode_button_1_current_state==0){
    //Serial.println("YASAS THARINDA BUTTON_1 PRESSED");
    button1_pressed=true;
    }
   else{
    button1_pressed=false;
    }

  if(mode_button_2_current_state!= mode_button_2_pre_state and mode_button_2_current_state==0){
    //Serial.println("YASAS THARINDA BUTTON_2 PRESSED");
    button2_pressed=true;
    }
   else{
    button2_pressed=false;
    }

    if(mode_button_3_current_state!= mode_button_3_pre_state and mode_button_3_current_state==0){
    //Serial.println("YASAS THARINDA BUTTON_3 PRESSED");
    button3_pressed=true;
    }
   else{
    button3_pressed=false;
    }

    if(mode_button_4_current_state!= mode_button_4_pre_state and mode_button_4_current_state==0){
    //Serial.println("YASAS THARINDA BUTTON_4 PRESSED");
    button4_pressed=true;
    }
   else{
    button4_pressed=false;
    }

    if(mode_button_5_current_state!= mode_button_5_pre_state and mode_button_5_current_state==0){
    //Serial.println("YASAS THARINDA BUTTON_5 PRESSED");
    button5_pressed=true;
    }
   else{
    button5_pressed=false;
    }

  /*Serial.print("button1_pressed :");
  Serial.println(button1_pressed);

  Serial.print("button2_pressed :");
  Serial.println(button2_pressed);

  Serial.print("button3_pressed :");
  Serial.println(button3_pressed);

  Serial.print("button4_pressed :");
  Serial.println(button4_pressed);

  Serial.print("button5_pressed :");
  Serial.println(button5_pressed);*/

  //Serial.println(mode_button_3_pre_state);
  //Serial.println(mode_button_3_current_state);
  print_lcd();
  state_machine();
  time_equal_check();
  mode_button_1_pre_state=mode_button_1_current_state;
  mode_button_2_pre_state=mode_button_2_current_state;
  mode_button_3_pre_state=mode_button_3_current_state;
  mode_button_4_pre_state=mode_button_4_current_state;
  mode_button_5_pre_state=mode_button_5_current_state;

  
  delay(50);
   
   
  /*if(digitalRead(button_4)){
    digitalWrite(motor_1,HIGH);
    digitalWrite(motor_2,LOW);
    delay(3000);
    }
  if(digitalRead(button_5)){
    digitalWrite(motor_1,LOW);
    digitalWrite(motor_2,LOW);
    delay(3000);
    }*/
}
void state_machine(){
  if(button1_pressed){ // button 1 is mode button
          if(state==1){
            changed_display=true;
            timer_index_change=true; 
            lcd.clear();
            state=2;
            }
          else if(state==2){
            changed_display=true;
            lcd.clear();
            if(count==2){
              state=1;
              set_time_index=0;
              count=0;
            }else{
              count=count+1;
              }
            }
          }
  }

void print_lcd(){
    
    if (state==1){
        if(double_dot_blink){
          //Serial.print("Yasas ");
          lcd.setCursor(2,1);
          lcd.print(":");
          }else{
           //Serial.print("Tharinda");
           lcd.setCursor(2,1);
           lcd.print(" ");
            }

        if(button2_pressed){
          //Serial.println("Yasasasasasasasasasassasasasaasasa");
          digitalWrite(motor_1,HIGH);
          digitalWrite(motor_2,LOW); 
          digitalWrite(motor_pwm,HIGH);
          digitalWrite(test_pin,HIGH); 
          delay(2000);
          digitalWrite(motor_1,LOW);
          digitalWrite(motor_2,LOW); 
          digitalWrite(motor_pwm,LOW); 
          digitalWrite(test_pin,LOW);
          }

        
  }
  
  if (state==1 && changed_display){
        changed_display=false;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Rem time to rel:");
        lcd.setCursor(0,1);
        if(remaining1_hour<10){
          lcd.print("0");
          }
          
        lcd.print(remaining1_hour);
        
        lcd.print(":");
        if(remaining1_minute<10){
          lcd.print("0");
          }
        lcd.print(remaining1_minute);
        //lcd.print(counter)
  }
    else if(state==2){
        //lcd.clear();
        changed_display=true;
        if(set_time_index==0 && timer_index_change){
          time_index="time1";
          print_hour= EEPROM.read(address0_hour);
          print_minute= EEPROM.read(address0_minute);  
          timer_index_change=false; 
          lcd.clear(); 
          lcd.setCursor(0,0); 
          lcd.print(time_index);     
          }
        else if(set_time_index==1 && timer_index_change){
          time_index="time2";
          print_hour= EEPROM.read(address1_hour);
          print_minute=EEPROM.read(address1_minute);
          timer_index_change=false;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(time_index);        
          }
        else if(set_time_index==2 && timer_index_change){
          time_index="time3";
          print_hour=EEPROM.read(address2_hour);
          print_minute= EEPROM.read(address2_minute);  
          timer_index_change=false;
          lcd.clear();
          lcd.setCursor(0,0);   
          lcd.print(time_index);     
          }
        lcd.setCursor(0,1);
        if(blink_index==0){
          //Serial.println("Yasasasasasasasasassasasasasasasasass");
          if(blinking){
               if(print_hour<10){
                  lcd.print("0");
                }
              // lcd.setCursor(1,6);
               lcd.print(print_hour);
               blinking=false;
               delay(100);
          }else{
            lcd.print("  ");
            blinking=true;
            delay(100);
            }
        }else{
          
          if(print_hour<10){
             lcd.print("0");
          }
          lcd.print(print_hour);
          }
        lcd.print(":");
        if (blink_index==1){
          if(blinking){
            if(print_minute<10){
              lcd.print("0");
              }
            lcd.print(print_minute);
            blinking=false;
            delay(100);
          }else{
            lcd.print("  ");
            delay(100);
            blinking=true;
            }
        }else{
            if(print_minute<10){
              lcd.print("0");
              }
            lcd.print(print_minute);
          }
       // Serial.println(blink_index);
        //Serial.println(blinking);
        // increasing values////////////////
        if(button3_pressed){
          if(blink_index==0){
            if(print_hour<24){
              print_hour=print_hour+1;
              }else{
                print_hour=0;
                }
            }
           else if(blink_index==1){
             if(print_minute<59){
              print_minute=print_minute+1;
              }else{
                print_minute=0;
                }
            }
            }

        if(button4_pressed){
          if(blink_index==0){
            if(print_hour>0){
              print_hour=print_hour-1;
              }else{
                print_hour=24;
                }
            }
           else if(blink_index==1){
             if(print_minute>0){
              print_minute=print_minute-1;
              }else{
                print_minute=59;
                }
            }
            }
        //////////////////////////////////////
        if(button1_pressed){
            set_time_index=set_time_index+1;
            timer_index_change=true; 
          }
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        if(button2_pressed){
            //lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("set the timer");
            if(set_time_index==0){
              EEPROM.write(address0_hour,print_hour);
              EEPROM.write(address0_minute,print_minute);
              
              timer_index_change=true;        
              }
            else if(set_time_index==1){
              EEPROM.write(address1_hour,print_hour);
              EEPROM.write(address1_minute,print_minute);
              timer_index_change=true;       
              }
            else if(set_time_index==2){
              EEPROM.write(address2_hour,print_hour);
              EEPROM.write(address2_minute,print_minute);
              timer_index_change=true;         
              }
            delay(500);
          }
        //////////////////////////////////////
        if(button5_pressed){
            if(blink_index==1){
              blink_index=0;
            }else{
              blink_index=1;
              }
          
          }
        ////////////////////////////////////////////////////////////////////
        }
    }
  
void blinkLED(void)
{
  if(double_dot_blink){
    double_dot_blink=false;
    }else{
      double_dot_blink=true;
      }
      
  if (counter==60){
    changed_display=true;
    counter=0;
    if(counter_minute==59){
      counter_minute=0;
      counter_hour=counter_hour+1;
      }else{
       counter_minute=counter_minute+1; 
        }

        
        
    }else{
      counter=counter+1;
      }
    search_remaining();
}
void search_remaining(){
  total_minute = counter_minute + counter_hour*60;
  count_hour= EEPROM.read(address0_hour);
  count_minute= EEPROM.read(address0_minute); 
  remaining_minute=count_minute+count_hour*60;


  if(remaining_minute<total_minute){
    closest_minute=60*24+ remaining_minute -total_minute;
    }else{
      closest_minute=remaining_minute -total_minute;
      }
  //////////////////////////////////////////////////////////////////////////
  count_hour= EEPROM.read(address1_hour);
  count_minute= EEPROM.read(address1_minute); 
  remaining_minute=count_minute+count_hour*60;


  if(remaining_minute<total_minute){
    remaining_minute=60*24+ remaining_minute -total_minute;
    if(remaining_minute<closest_minute){
      closest_minute=remaining_minute;
      }
    }else{
      remaining_minute=remaining_minute -total_minute;
      if(remaining_minute<closest_minute){
      closest_minute=remaining_minute;
      }
      }
  ////////////////////////////////////////////////////////////////////
  count_hour= EEPROM.read(address2_hour);
  count_minute= EEPROM.read(address2_minute); 
  remaining_minute=count_minute+count_hour*60;


  if(remaining_minute<total_minute){
    remaining_minute=60*24+ remaining_minute -total_minute;
    if(remaining_minute<closest_minute){
      closest_minute=remaining_minute;
      }
    }else{
      remaining_minute=remaining_minute -total_minute;
      if(remaining_minute<closest_minute){
      closest_minute=remaining_minute;
      }
      }  

  ////////////////////////////////////////////////////////////////////////
  remaining1_minute=closest_minute%60;
  remaining1_hour=closest_minute/60;

  /*Serial.print(remaining1_hour);
  Serial.print(":");
  Serial.print(remaining1_minute);
  Serial.println("");
  Serial.println(total_minute);*/
  
  
  }
void time_equal_check(){
  
  if(remaining1_hour==0 and remaining1_minute==0 and counter<5){
      digitalWrite(motor_1,HIGH);
      digitalWrite(motor_2,LOW);
      digitalWrite(motor_pwm,HIGH);
    }else {
      digitalWrite(motor_1,LOW);
      digitalWrite(motor_2,LOW); 
      digitalWrite(motor_pwm,LOW);
      }
  }
