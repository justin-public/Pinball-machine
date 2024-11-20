/*
 * Portfolio 용 코드
 * TARGET : Arduino
 * Date : 2021.11.18
 * writer : justin
 * epicgram co.ltd
*/
#include "LED.h"
#include "thread.h"
#include "mstimer.h"

#define reset_pin 39
#define start_sensor 53 
#define finish_sensor 52
//#define finish_sensor1 50

int sensor_value = 1;
int sensor_value1 = 0;
int sensor_value2 = 0;

int Game_start = 0;
int Best_score = 0;

int sel = 0;
int led_motion = 0;
int sound_motion = 0;
int music_play = 0;

static int8_t Send_buf[15] = {0};
static int8_t Send_buf1[4] = {0};

int count_time;
int hourc,minc,secc;
int secc_1,secc_1_1,secc_2,secc_2_10;
int minc_1,minc_1_1,minc_2,minc_2_10;
int secc_1_1_b,secc_2_10_b;
int minc_1_1_b,minc_2_10_b; 

#define Pop1_input  24      // 53 ??        
#define Pop1_output 22      // 51 ??

#define Pop2_input  28
#define Pop2_output 26

int chattering;
int chattering1;

int duplicate_check;

void setup() 
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  pixels1.begin();
  pixels2.begin();
  pixels3.begin();
  pixels4.begin();
  pixels5.begin();
  pixels6.begin();
  pixels7.begin();
  pixels8.begin(); 
  pixels9.begin();
  pixels10.begin();
  pixels11.begin();
  
  //cs_39_37.set_CS_AutocaL_Millis(0xFFFFFFFF);
  
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial3.begin(9600);
  
  MsTimer2::set(1000,count_timer);
  MsTimer2::start();

  pinMode(start_sensor,INPUT);
  pinMode(finish_sensor,INPUT);
  //pinMode(finish_sensor1,INPUT);

  pinMode(Pop1_input,INPUT);
  pinMode(Pop1_output,OUTPUT);

  pinMode(Pop2_input,INPUT);
  pinMode(Pop2_output,OUTPUT);

  Game_start = 3;      // 게임 초기화면       
  count_time = 0;
  led_motion = 0;
  duplicate_check = 0;
  sound_motion = 0;

  hourc = 0;
  minc = 0;
  secc = 0;
  secc_1 = 0;
  secc_1_1 = 0;
  secc_2 = 0;
  secc_2_10 = 0;
  minc_1 = 0;
  minc_1_1 = 0;
  minc_2 = 0;
  minc_2_10 = 0;
  secc_1_1_b = 0;
  secc_2_10_b = 0;
  minc_1_1_b = 0;
  minc_2_10_b = 0;
  
  Best_score = 0;
  sel = 0;

  digitalWrite(Pop1_output,HIGH);
  digitalWrite(Pop2_output,HIGH);
  delay(500);
  digitalWrite(reset_pin,LOW);
  delay(200);
  digitalWrite(reset_pin,HIGH);
  Serial.println("Game start!!");
  delay(500);
  
  module1->onRun(finish_module);
  module1->setInterval(5);         // 5       
  module2.onRun(start_module);
  module2.setInterval(10);         // 10
  module3.onRun(led_animation);
  module3.setInterval(15);         // 15 
  module4.onRun(sound_effect);
  module4.setInterval(20);         // 20 
  module5.onRun(Pop_bumper);
  module5.setInterval(25);         // 25
  module6.onRun(loop_task); 
  module6.setInterval(30);
  
  controll.add(module1);
  controll.add(&module2);
  controll.add(&module3);
  controll.add(&module4);
  controll.add(&module5);
  controll.add(&module6);
}

void sendCommand(char sound_num, int channel)
{
  delay(20);
  Send_buf[0] = 0xAA;
  Send_buf[1] = 0x08;
  Send_buf[2] = 0x0B;
  Send_buf[3] = 0x01;
  Send_buf[4] = 0x2F;
  Send_buf[5] = 0x30;
  Send_buf[6] = 0x30;
  Send_buf[7] = 0x30;
  Send_buf[8] = 0x30;
  Send_buf[9] = sound_num & 0xff;
  Send_buf[10] = 0x2A;
  Send_buf[11] = 0x4D;
  Send_buf[12] = 0x50;
  Send_buf[13] = 0x33;
  Send_buf[14] = 0xD8 + channel;

  for(uint8_t i=0; i<15; i++)
  {
    Serial3.write(Send_buf[i]);
  }
}

void soundpause()
{
  delay(20);
  Send_buf1[0] = 0xAA;
  Send_buf1[1] = 0x03;
  Send_buf1[2] = 0x00;
  Send_buf1[3] = 0xAD;

  for(uint8_t i=0; i<4; i++)
  {
    Serial3.write(Send_buf1[i]);
  }
}

void sound_effect()
{
  int tick = 0;
  if(sound_motion == 1)
  {
    sendCommand('1',0);         // 00001.MP3  BGM
    delay(150);
    sound_motion = 0;     
  }
  if(sound_motion == 2)
  {
    sendCommand('2',1);         // 00002.MP3  Shooter         
    delay(500);                 // 150    180 
    sound_motion = 1;
  }
  if(sound_motion == 3)         // 00003.MP3 Rank up
  {
    sendCommand('3',2);
    delay(1000);
    sound_motion = 0;
  }
  if(sound_motion == 4)
  {
    sendCommand('4',3);
    delay(1000);
    sound_motion = 0;
  }
}

void Pop_bumper()
{
  int state_pin = digitalRead(Pop1_input);
  int state_pin1 = digitalRead(Pop2_input);
  if(state_pin == 0)digitalWrite(Pop1_output,LOW);
  if(state_pin == 1)digitalWrite(Pop1_output,HIGH);
  if(state_pin1 == 0)digitalWrite(Pop2_output,LOW);
  if(state_pin1 == 1)digitalWrite(Pop2_output,HIGH);
}

void led_animation()
{
  if(led_motion == 0)
  {
    LED1(250,0,250);
    LED2(250,0,250);
    LED3(250,0,250);
    LED4(250,0,250);
    LED5(250,0,250);
    LED6(250,0,250);
    LED7(250,0,250);
    LED8(250,0,250);
    LED9(250,0,250);
    LED10(250,0,250);
    LED11(250,0,250);
    LED12(250,0,250);
  }
  if(led_motion == 1)
  {
    LED4(0,0,0);
    LED8(0,0,0);
    delay(100);
    LED4(0,250,0);
    LED8(0,250,0);
    delay(100);
    LED4(0,0,0);
    LED8(0,0,0);
    delay(100);
    LED4(0,250,0);
    LED8(0,250,0);
    delay(100);
    LED4(0,0,0);
    LED8(0,0,0);
    delay(100);
    LED4(0,250,0);
    LED8(0,250,0);
    delay(100);
    led_motion = 0;
  }
  if(led_motion == 2)
  {
    LED5(0,0,0);
    LED7(0,0,0);
    delay(100);
    LED5(250,0,0);
    LED7(250,0,0);
    delay(100);
    LED5(0,0,0);
    LED7(0,0,0);
    delay(100);
    LED5(250,0,0);
    LED7(250,0,0);
    delay(100);
    LED5(0,0,0);
    LED7(0,0,0);
    delay(100);
    LED5(250,0,0);
    LED7(250,0,0);
    delay(100);
    led_motion = 0;
  }
  if(led_motion == 3)
  {
    LED1(0,0,0);
    LED2(0,0,0);
    LED3(0,0,0);
    LED4(0,0,0);
    LED5(0,0,0);
    LED6(0,0,0);
    LED7(0,0,0);
    LED8(0,0,0);
    LED9(0,0,0);
    LED10(0,0,0);
    LED11(0,0,0);
    LED12(0,0,0);
    delay(100);
    LED1(0,250,250);
    LED2(0,250,250);
    LED3(0,250,250);
    LED4(0,250,250);
    LED5(0,250,250);
    LED6(0,250,250);
    LED7(0,250,250);
    LED8(0,250,250);
    LED9(0,250,250);
    LED10(0,250,250);
    LED11(0,250,250);
    LED12(0,250,250);
    delay(100);
    LED1(0,0,0);
    LED2(0,0,0);
    LED3(0,0,0);
    LED4(0,0,0);
    LED5(0,0,0);
    LED6(0,0,0);
    LED7(0,0,0);
    LED8(0,0,0);
    LED9(0,0,0);
    LED10(0,0,0);
    LED11(0,0,0);
    LED12(0,0,0);
    delay(100);
    LED1(0,250,250);
    LED2(0,250,250);
    LED3(0,250,250);
    LED4(0,250,250);
    LED5(0,250,250);
    LED6(0,250,250);
    LED7(0,250,250);
    LED8(0,250,250);
    LED9(0,250,250);
    LED10(0,250,250);
    LED11(0,250,250);
    LED12(0,250,250);
    delay(100);
    LED1(0,0,0);
    LED2(0,0,0);
    LED3(0,0,0);
    LED4(0,0,0);
    LED5(0,0,0);
    LED6(0,0,0);
    LED7(0,0,0);
    LED8(0,0,0);
    LED9(0,0,0);
    LED10(0,0,0);
    LED11(0,0,0);
    LED12(0,0,0);
    delay(100);
    led_motion = 0;
  }
}

void start_module()
{
  sensor_value = digitalRead(start_sensor);
  if(sensor_value == 1){
    chattering = 0;
  }
  if(sensor_value == 0)
  {
    chattering++;
    if(chattering == 1)
    {
      duplicate_check++;
      if(duplicate_check == 1)     
      {
        Game_start = 1;
        led_motion = 1;
        sound_motion = 2;       // 게임 시작시 사운드  //
      }
      if(duplicate_check > 1){
        //sensor_value = 0;
        count_time = 0;
        Game_start = 1;
        led_motion = 1;
        sound_motion = 2;
      }
    }
  }
}

void finish_module()
{
  sensor_value1 = digitalRead(finish_sensor);
  if(sensor_value1 == 1){
    chattering1 = 0;
  }
  if(sensor_value1 == 0){
    chattering1++;
    if(chattering1 == 1){
      Game_start = 2;
    }
  }
  /*
  sensor_value1 = digitalRead(finish_sensor);
  sensor_value2 = digitalRead(finish_sensor1);
  
  if(sensor_value1 == LOW && sensor_value2 == LOW)
  {
    chattering1 = 0;
  }
  if(sensor_value1 == HIGH && sensor_value2 == HIGH)
  {
    chattering1++;
    if(chattering1 == 1){
      Game_start = 2;
    }
  }
  */
}

void loop_task()
{
  if(Game_start == 0){
    count_time = 0;
    Serial1.print("n0.val=");
    Serial1.print(secc_1_1);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n1.val=");
    Serial1.print(secc_2_10);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n2.val=");
    Serial1.print(minc_1_1);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n3.val=");
    Serial1.print(minc_2_10);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    ///*
    Serial1.print("n4.val=");
    Serial1.print(secc_1_1_b);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n5.val=");
    Serial1.print(secc_2_10_b);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n6.val=");
    Serial1.print(minc_1_1_b);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n7.val=");
    Serial1.print(minc_2_10_b);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    //*/    
  }
  if(Game_start == 1){
    Serial1.print("n0.val=");
    Serial1.print(secc_1_1);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n1.val=");
    Serial1.print(secc_2_10);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n2.val=");
    Serial1.print(minc_1_1);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n3.val=");
    Serial1.print(minc_2_10);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
  }
  if(Game_start == 2){
    if(Best_score <= count_time){
      Best_score = count_time;       // 레코드용 변수
      secc_1_1_b = secc_1_1;
      secc_2_10_b = secc_2_10;
      minc_1_1_b = minc_1_1;       
      minc_2_10_b = minc_2_10;
      sel = 1;
    }
    if(Best_score > count_time){
      sel = 2;
    }
    if(sel == 1){
      Serial1.print("page 1");
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      delay(100);
      Serial1.print("n8.val=");
      Serial1.print(secc_1_1_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n9.val=");
      Serial1.print(secc_2_10_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n10.val=");
      Serial1.print(minc_1_1_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n11.val=");
      Serial1.print(minc_2_10_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      
      delay(2000);
      Serial1.print("page 0");
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      delay(100);
      Serial1.print("n4.val=");
      Serial1.print(secc_1_1_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n5.val=");
      Serial1.print(secc_2_10_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n6.val=");
      Serial1.print(minc_1_1_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n7.val=");
      Serial1.print(minc_2_10_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
     
      sel = 0;
      Game_start = 0;
      led_motion = 3;
      sound_motion = 3;
      duplicate_check = 0;
    }
    if(sel == 2){
      Serial1.print("page 0");
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      delay(100);
      Serial1.print("n4.val=");
      Serial1.print(secc_1_1_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n5.val=");
      Serial1.print(secc_2_10_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n6.val=");
      Serial1.print(minc_1_1_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n7.val=");
      Serial1.print(minc_2_10_b);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      delay(100);
      
      Serial1.print("n0.val=");
      Serial1.print(secc_1_1);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n1.val=");
      Serial1.print(secc_2_10);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n2.val=");
      Serial1.print(minc_1_1);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);

      Serial1.print("n3.val=");
      Serial1.print(minc_2_10);
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      sel = 0;
      Game_start = 0;
      led_motion = 2;
      sound_motion = 4;
      duplicate_check = 0; 
    }
  }
  if(Game_start == 3)
  {
    Serial1.print("n4.val=");
    Serial1.print(0);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n5.val=");
    Serial1.print(0);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n6.val=");
    Serial1.print(0);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n7.val=");
    Serial1.print(0);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    
    delay(100);
    Serial1.print("n0.val=");
    Serial1.print(0);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n1.val=");
    Serial1.print(0);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n2.val=");
    Serial1.print(0);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    Serial1.print("n3.val=");
    Serial1.print(0);
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);

    delay(500);
  }
}

void loop() 
{
  controll.run();
}

void count_timer()
{
  count_time++;
  
  hourc = count_time / 3600;
  minc = (count_time % 3600) / 60;
  secc = (count_time % 3600) % 60;

  minc_2 = minc % 100;
  minc_2_10 = minc_2 / 10;

  minc_1 = minc % 10;
  minc_1_1 = minc_1 / 1;
    
  secc_2 = secc % 100;
  secc_2_10 = secc_2 / 10;  

  secc_1 = secc % 10;
  secc_1_1 = secc_1 / 1;

  music_play++;
  if(music_play == 30 && Game_start == 1)
  {
    sound_motion = 1;
    music_play = 0;
  }
}
