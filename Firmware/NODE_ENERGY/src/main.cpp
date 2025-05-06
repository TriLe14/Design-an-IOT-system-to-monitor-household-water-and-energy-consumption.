#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ADE7753.h"  
#include <SPI.h>   
#include <SoftwareSerial.h>

#define COLUMS           20   //LCD columns
#define ROWS             4    //LCD rows
#define LCD_SPACE_SYMBOL 0x20 //space symbol from LCD ROM, see p.9 of GDM2004D datasheet or english ASCII printable characters
#define RELAY PA0
#define BUTTON PB4
#define SPI_Speed 1000000

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

ADE7753 meter; 

float v1 = 0,i1 = 0, t1 = 0;
float e1 = 0, e2 = 0, e3 = 0,pf=0, pf_rms = 0;
byte relayStatus = LOW;

//Các hệ số calib
float kv = 12.27;                     
float ki1 = 393216;
float ki2 = 43532467;
float ke = 678493.2794;
float kt = 184.31;

void ISR(){
  detachInterrupt(BUTTON);
  relayStatus = !relayStatus; 
  digitalWrite(RELAY,relayStatus);
  delay(500);
  attachInterrupt(digitalPinToInterrupt(BUTTON), ISR, FALLING);
}

void setup()
{
  pinMode(RELAY,OUTPUT); 
  pinMode(BUTTON,INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(BUTTON), ISR, FALLING);
  lcd.begin(COLUMS, ROWS, LCD_5x8DOTS, PB7 , PB6, 400000);  //colums, rows, characters size, SDA, SCL, I2C speed in Hz
  /* prints static text */
  lcd.print(F("      HELLO!"));
  delay(2000);

  //meter.Init();
  meter.gainSetup(0x00,0x00,GAIN_1,GAIN_1); 
  meter.resetStatus();
  digitalWrite(RELAY,LOW);
}

void loop() { 

// Đo điện áp
 v1=meter.vrms();
 delay(100);
 
// Đo dòng điện
 i1=meter.irms();
 delay(100);

// Đo chu kỳ
  t1=meter.getPeriod();
  delay(100);

// Đo công suất hoạt động
  //meter.setMode(0x0080); // Chế độ tích lũy năng lượng ban đầu. ADE7753 được đặt ở chế độ tích lũy năng lượng chu kỳ đường dây bằng cách set bit 7 (CYCMODE) trong thanh ghi MODE. 
  //e1 = meter.setPotLine(100); // Số lượng nửa chu kỳ chiếm trong phép đo là cố định. 100 nửa chu kỳ tương đương với 1 giây làm việc trên mạng 50 Hz.
  //delay(20);  // Trích xuất năng lượng hoạt động tích lũy, đồng bộ hóa phép đo với điểm qua 0 của tín hiệu điện áp. 
  /*if(meter.setPotLine(100) == 0){
    e1 = 0;
  } else
  {
    //e1 = meter.getWatt();
    meter.setPotLine(100);
    e2 = meter.getVa();
    e3 = meter.getVar()*14;
    float i = e3/e2;
    pf = sqrt(1 - i*i);
    //e1 = (v1/kv)*((i1 - ki1)/ki2)*pf;
    delay(20);*/
  //}
  
  //meter.setMode(0x0080);
  //meter.setPotLine(200); // 2 giây đo.
  //e2=meter.getWatt();


  //meter.setMode(0x0080);
  //meter.setPotLine(300); // 3 giây đo
  //e3=meter.getWatt(); 
  
      lcd.setCursor(0,0);
      lcd.print("V = ");
      lcd.setCursor(4,0);
      lcd.print("            ");
      lcd.setCursor(4,0);
      lcd.print(v1);
      lcd.print(" V");
    	lcd.setCursor(0,1);
    	lcd.print("I = ");
      lcd.setCursor(4,1);
      lcd.print("       ");
      //if(i1!=ki1){
      lcd.setCursor(4,1);
    	//lcd.print(((i1 - ki1)/ki2)*1000);
      lcd.print("              ");
      lcd.setCursor(4,1);
      lcd.print(i1);
      lcd.print(" A");
      //} else {
      // lcd.setCursor(4,1);
      //lcd.print("              ");
      //lcd.setCursor(4,1);
      //i1=0;
      // lcd.print(i1); 
      // lcd.print(" A");
      //} 
    	lcd.setCursor(0,2);
    	lcd.print("f = ");
      lcd.setCursor(4,2);
      lcd.print("             ");
      lcd.setCursor(4,2);
    	lcd.print(t1); 
      lcd.print(" Hz");
      /* lcd.setCursor(0,3);
    	lcd.print("E = "); 
      lcd.setCursor(4,3);
      lcd.print("                ");
      lcd.setCursor(4,3);
    	lcd.print(pf); 
      lcd.print(" W"); */
}
