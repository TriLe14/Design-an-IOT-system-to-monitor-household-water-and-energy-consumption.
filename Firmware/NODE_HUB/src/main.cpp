#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>
#include "Arduino.h"
#include "uRTCLib.h"
#include "Tri_lib.h"

#define SDA_2  19
#define SCL_2  18


uRTCLib rtc(0x68);

LiquidCrystal_I2C lcd(0x27, 20, 4); 
HardwareSerial SerialPort(2); // use UART2


char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};



int Led = 13; 
int len = 25; int gtlen; 
int xuong = 27; int gtxuong; 
int menu = 26; int gtmenu ; 
int home = 0;
int demmenu = 0; int a = 1;
int date; int month; int year;
int hour; int minute; int second; int thu;
int homepage = 0;
String data;
char Rx_data1, Tx_data[20]; 
int b = 1;
int gio = 0, phut = 0, state = 0, state2 = 0, state3 = 0;
char mqtt_message[128];
int check = 0; // Được dùng trong hàm consump để kiểm tra xem có nhận được dữ liệu Zigbee hay chưa


//------Wifi Information---------------
//const char* ssid = "Tri le";      //Wifi connect
//const char* password = "14052002";   //Password
const char* ssid = "Tri Le Le";      //Wifi connect
const char* password = "14052002";   //Password
//const char* ssid = "Tri Le";      //Wifi connect
//const char* password = "14052002";   //Password

//------HiveMQ Information---------------
const char* mqtt_server = "d1086457e78e4e08a5aa4848ff84c83d.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "TriLe140502"; //User
const char* mqtt_password = "Ryoginno140502"; //Password
//--------------------------------------------------
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  //Serial.println();
  lcd.print("CONNECTING");
  //Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }

  randomSeed(micros());
  lcd.clear();
  //Serial.println("");
  lcd.print("WIFI CONNECTED");
  delay(1500);
  lcd.clear();
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

//------------Connect to MQTT Broker-----------------------------
void reconnect() {
  while (!client.connected()) {
    lcd.clear();
    lcd.print("MQTT CONNECTION...");
    String clientID =  "ESPClient-";
    clientID += String(random(0xffff),HEX);
    if (client.connect(clientID.c_str(), mqtt_username, mqtt_password)) {
      lcd.clear();
      lcd.print("MQTT CONNECTED");
      client.subscribe("Esp32/NodeEnergy");
      delay(1500);
    } else {
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Failed");
      //Serial.print(client.state());
      lcd.setCursor(0,2);
      lcd.print(" Try again in 2 seconds");
      delay(2000);
    } 
  }
}

void CheckState(){
      data = "P2P 7F5D S"; // Hỏi trạng thái hoạt động của Node Energy
      SerialPort.print(data);
      state2 = 2;
      int cnt = 1;
      while(state2 == 2){
      cnt ++;
      while (SerialPort.available()){
      Rx_data1 = SerialPort.read();
      if(Rx_data1 == 'Y'){ //ON 
        state2 = 0;
      } else if (Rx_data1 == 'N'){ //OFF
        state2 = 1;
      }} 
      if(cnt == 10) {break;}
      delay(10);
      }
      delay(20);
}

void CheckState2(){
      data = "P2P 7F5D S"; // Hỏi trạng thái hoạt động của Node Energy
      SerialPort.print(data);
      state2 = 2;
      int cnt = 1;
      while(state2 == 2){
      cnt ++;
      while (SerialPort.available()) {
      Rx_data1 = SerialPort.read();
      if(Rx_data1 == 'Y'){ //ON 
        state2 = 0;
      } else if (Rx_data1 == 'N'){ //OFF
        state2 = 1;
      }} 
      if(cnt == 100) {break;}
      delay(10);
      }
      delay(50);
}

//-----Method for Publishing MQTT Messages---------
void publishMessage(const char* topic, String payload, boolean retained){
  if(client.publish(topic,payload.c_str(),true)){}
    //Serial.println("Message published ["+String(topic)+"]: "+payload);
}

//-----Call back Method for Receiving MQTT message---------
void callback(char* topic, byte *payload, unsigned int length) {
  String incommingMessage = "";
  //Serial.println("-------new message from broker-----");
  //Serial.print("topic: ");
  //Serial.println(topic);
  //Serial.print("message: ");
  //Serial.write(payload, length);
  //Serial.println();
  for(int i=0; i<length;i++) {incommingMessage += (char)payload[i];} 
  if(incommingMessage == "B"){
        for(int q = 0; q < 20; q++){
        CheckState();
        if(state2 == 0){
          data = "P2P 7F5D B";
          SerialPort.print(data); // Gửi tín hiệu ON/OFF cho Node Energy
        } 
        delay(20);} 
  } else if(incommingMessage == "T"){
        for(int q = 0; q < 20; q++){
        CheckState();
        if(state2 == 1){
          data = "P2P 7F5D T";
          SerialPort.print(data); // Gửi tín hiệu ON/OFF cho Node Energy
        }
        delay(20);} 
      }
  }


void Water_Monitor(){// Hàm này đọc dữ liệu gửi về từ Node Water
      data = "P2P 8291 R";
      SerialPort.print(data);
      if (SerialPort.available()) {
      Rx_data = SerialPort.readString();
      Rx_data.toCharArray(mang, sizeof(mang));
      Tachchuoi_Water(mang);
      //publishMessage("NodeWater/Volume", Rx_data, true);
      lcd.setCursor(7,0);
      lcd.print("         ");
      lcd.setCursor(7,0);
      lcd.print(rate);
      lcd.print(" L/M");
      lcd.setCursor(9,1);
      lcd.print("            ");
      lcd.setCursor(9,1);
      lcd.print(volume);
      lcd.print(" L");
      lcd.setCursor(8,2);
      lcd.print("            ");
      lcd.setCursor(8,2);
      lcd.print(thismonth);
      lcd.print(" L");
      }
      delay(200);
}


void Energy_Monitor(){
      data = "P2P 7F5D E";
      SerialPort.print(data);
      if (SerialPort.available()) {
      Rx_data = SerialPort.readString();
      Rx_data.toCharArray(mangEnergy, sizeof(mangEnergy));
      Tachchuoi_Energy(mangEnergy);
      lcd.setCursor(3,0);
      lcd.print("        ");
      lcd.setCursor(3,0);
      lcd.print(v);
      lcd.print("V");
      lcd.setCursor(3,1);
      lcd.print("        ");
      lcd.setCursor(3,1);
      lcd.print(i);
      lcd.print("A");
      lcd.setCursor(3,2);
      lcd.print("       ");
      lcd.setCursor(3,2);
      lcd.print(p);
      lcd.print("W");
      lcd.setCursor(3,3);
      lcd.print("        ");
      lcd.setCursor(3,3);
      lcd.print(en);
      lcd.print("kWh");
      lcd.setCursor(13,1);
      lcd.print("      ");
      lcd.setCursor(13,1);
      lcd.print(pf);
      lcd.setCursor(13,2);
      lcd.print("       ");
      lcd.setCursor(13,2);
      lcd.print(f); 
      lcd.print("Hz");
      }
      delay(200);
}

void Energy_Consump(){//Cập nhật các giá trị đọc được từ Node Energy lên Web
      data = "P2P 7F5D E";
      SerialPort.print(data);
      for(int m = 0; m < 30; m++){
      if (SerialPort.available()) {
      check = 1;
      Rx_data = SerialPort.readString();
      Rx_data.toCharArray(mangEnergy, sizeof(mangEnergy));
      Tachchuoi_Energy(mangEnergy);
      publishMessage("NodeEnergy/Voltage", v, true);
      publishMessage("NodeEnergy/Current", i, true);
      publishMessage("NodeEnergy/Frequency", f, true);
      publishMessage("NodeEnergy/Power", p, true);
      publishMessage("NodeEnergy/Powerfactor", pf, true);
      publishMessage("NodeEnergy/Energy", en, true);
      }
      if(check == 1){
        check = 0;
        break;
      }
      delay(50);
      }
}

void Water_Consump(){//Cập nhật các giá trị đọc được từ Node Water lên Web
      data = "P2P 8291 R";
      SerialPort.print(data);
      for(int m = 0; m < 15; m++){
      if (SerialPort.available()) {
      check = 1;
      Rx_data = SerialPort.readString();
      Rx_data.toCharArray(mang, sizeof(mang));
      Tachchuoi_Water(mang);
      publishMessage("NodeWater/Rate", rate, true);
      publishMessage("NodeWater/Volume", volume, true);
      delay(20);
      }
      if(check == 1){
        check = 0; 
        break;
      }
      delay(20);
      }   
}

void Water_Month(){ //Qua tháng mới sẽ tiến hành đọc giá trị lưu lượng của tháng đó và gửi lên Web
      if(month <10){
        data = "P2P 8291" + String(month);
      } else if (month == 10){
        data = "P2P 8291 A";
      } else if (month == 11){
        data = "P2P 8291 B";
      } else {data = "P2P 8291 C";}
      SerialPort.print(data);
      for(int m = 0; m < 15; m++){
      if (SerialPort.available()) {
      check = 1;
      Rx_data = SerialPort.readString();
      Rx_data.toCharArray(mang, sizeof(mang));
      Tachchuoi_WaterMonth(mang);
      data = "NodeWater/" + String(month);
      data.toCharArray(Tx_data, sizeof(Tx_data));
      publishMessage(Tx_data, thismonth, true);
      delay(20);
      }
      if(check == 1){
        check = 0; 
        break;
      }
      delay(20);
      }   
} 

void Energy_Month(){ //Qua tháng mới sẽ tiến hành đọc giá trị lưu lượng của tháng đó và gửi lên Web
      if(month <10){
        data = "P2P 7F5D" + String(month);
      } else if (month == 10){
        data = "P2P 7F5D A";
      } else if (month == 11){
        data = "P2P 7F5D B";
      } else {data = "P2P 7F5D C";}
      SerialPort.print(data);
      for(int m = 0; m < 15; m++){
      if (SerialPort.available()) {
      check = 1;
      Rx_data = SerialPort.readString();
      Rx_data.toCharArray(mang, sizeof(mang));
      Tachchuoi_EnergyMonth(mang);
      data = "NodeEnergy/" + String(month);
      data.toCharArray(Tx_data, sizeof(Tx_data));
      publishMessage(Tx_data, en, true);
      delay(20);
      }
      if(check == 1){
        check = 0; 
        break;
      }
      delay(20);
      }   
} 

void IRAM_ATTR isr0(){
  home = 1;
  homepage = 0;
  a = 1;
  detachInterrupt(menu);
}

void IRAM_ATTR isr1(){ //Thoát ra khỏi chức năng hiện tại về lại Menu Monitor
  home = 2;
  homepage = 0;
  a = 1;
  detachInterrupt(menu);
}

void Time(){
  rtc.refresh();
  if(date != rtc.day() || month != rtc.month() || year != rtc.year()){
    if(month != rtc.month()){ // Qua tháng mới
      Energy_Month(); //Đọc giá trị tháng từ DS3231 + Gửi tín hiệu hỏi data thismonth + Gửi dữ liệu đó lên Web
      Water_Month();
    }

  lcd.setCursor(9,0);
  lcd.print("                    ");
  date = rtc.day();
  month = rtc.month();
  year = rtc.year();
  }

  if(thu != rtc.dayOfWeek()){
  lcd.setCursor(0,0);
  lcd.print("        ");
  thu = rtc.dayOfWeek();}
  lcd.setCursor(0,0);
  lcd.print(daysOfTheWeek[rtc.dayOfWeek()-1]);
  lcd.print(" ");

  lcd.print(rtc.day());
  lcd.print("/");
  lcd.print(rtc.month());
  lcd.print("/");
  lcd.print("20");
  lcd.print(rtc.year());
 
  if(hour != rtc.hour()){
    lcd.setCursor(0,1);
    lcd.print("  ");
    hour = rtc.hour();
  }
    if(minute != rtc.minute()){
    lcd.setCursor(3,1);
    lcd.print("  ");
    minute = rtc.minute();
  }
      if(second != rtc.second()){
    lcd.setCursor(6,1);
    lcd.print("  ");
    second = rtc.second();
  }
  if(hour < 10){
  lcd.setCursor(0,1);
  lcd.print("0");
  lcd.print(hour);
  } else{
  lcd.setCursor(0,1);
  lcd.print(hour);}
  lcd.setCursor(2,1);
  lcd.print(":");
  if(minute<10){
  lcd.setCursor(3,1);
  lcd.print("0");
  lcd.print(minute);
  } else{
  lcd.setCursor(3,1);
  lcd.print(minute);}
  lcd.setCursor(5,1);
  lcd.print(":");
  lcd.print(second); 

  lcd.setCursor(10,1);
  lcd.print("T: ");
  lcd.setCursor(13,1);
  lcd.print(rtc.temp()  / 100);
  lcd.print("*");   //shows degrees character
  lcd.print("C"); 

    if(state != 0 && gio == hour && phut == minute ){ // Tính năng hẹn giờ của Node Hub: Kiểm tra xem đã đúng thời gian được cái đặt hay chưa
      CheckState2();
      //state3 = 1 ;
      if(state2 == 0){
        if(state == 1){
          data = "P2P 7F5D B";
          SerialPort.print(data); // Gửi tín hiệu ON/OFF cho Node Energy
        }
        else{
          state3 = state;
          state = 0;
        }
      } else if(state2 == 1){
        if(state == 2){
            data = "P2P 7F5D T";
            SerialPort.print(data); // Gửi tín hiệu ON/OFF cho Node Energy 
      } else{
          state3 = state;
          state = 0;
        }
      } 
  }
  if(gio == hour && phut < minute ){
    state = state3;
  }
}

void menuTong() 
{
  if (demmenu == 0)
  {
    lcd.setCursor(0, 0);
    lcd.print(">ON/OFF");
    lcd.setCursor(0, 1);
    lcd.print(" MONITOR");
    lcd.setCursor(0, 2);
    lcd.print(" TIMER");
    lcd.setCursor(0, 3); 
    lcd.print(" BACK");
  }
  else if (demmenu == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print(" ON/OFF");
    lcd.setCursor(0, 1);
    lcd.print(">MONITOR");
    lcd.setCursor(0, 2);
    lcd.print(" TIMER");
    lcd.setCursor(0, 3); 
    lcd.print(" BACK"); 
  }
  else if (demmenu == 2)
  {
    lcd.setCursor(0, 0);
    lcd.print(" ON/OFF");
    lcd.setCursor(0, 1);
    lcd.print(" MONITOR");
    lcd.setCursor(0, 2);
    lcd.print(">TIMER");
    lcd.setCursor(0, 3); 
    lcd.print(" BACK");   
  }
  else if (demmenu == 3)
  {
    lcd.setCursor(0, 0);
    lcd.print(" ON/OFF");
    lcd.setCursor(0, 1);
    lcd.print(" MONITOR");
    lcd.setCursor(0, 2);
    lcd.print(" TIMER");
    lcd.setCursor(0, 3); 
    lcd.print(">BACK");
  }
}

void menu1(){
  if (demmenu == 0)
  {
    lcd.setCursor(0, 0);
    lcd.print(">NODE ENERGY:");
    lcd.setCursor(0, 1);
    lcd.print(" BACK");
  }
  else if (demmenu == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print(" NODE ENERGY");
    lcd.setCursor(0, 1);
    lcd.print(">BACK");
  }
}

void menuMonitor() 
{
  if (demmenu == 0)
  {
    lcd.setCursor(0, 0);
    lcd.print(">NODE WATER");
    lcd.setCursor(0, 1);
    lcd.print(" NODE ENERGY");
    lcd.setCursor(0, 2);
    lcd.print(" BACK");
  }
  else if (demmenu == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print(" NODE WATER");
    lcd.setCursor(0, 1);
    lcd.print(">NODE ENERGY");
    lcd.setCursor(0, 2);
    lcd.print(" BACK");
  }
  else if(demmenu == 2){
    lcd.setCursor(0, 0);
    lcd.print(" NODE WATER");
    lcd.setCursor(0, 1);
    lcd.print(" NODE ENERGY");
    lcd.setCursor(0, 2);
    lcd.print(">BACK");
  }
}

void menuTimer(){
  if (demmenu == 0)
  {
    lcd.setCursor(0, 0);
    lcd.print(">STATE : ");
    lcd.setCursor(0, 1);
    lcd.print(" HOUR");
    lcd.setCursor(0, 2);
    lcd.print(" MINUTE");
    lcd.setCursor(0, 3); 
    lcd.print(" BACK");
  }
  else if (demmenu == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print(" STATE : ");
    lcd.setCursor(0, 1);
    lcd.print(">HOUR");
    lcd.setCursor(0, 2);
    lcd.print(" MINUTE");
    lcd.setCursor(0, 3); 
    lcd.print(" BACK"); 
  }
  else if (demmenu == 2)
  {
    lcd.setCursor(0, 0);
    lcd.print(" STATE : ");
    lcd.setCursor(0, 1);
    lcd.print(" HOUR");
    lcd.setCursor(0, 2);
    lcd.print(">MINUTE");
    lcd.setCursor(0, 3); 
    lcd.print(" BACK");   
  }
  else if (demmenu == 3)
  {
    lcd.setCursor(0, 0);
    lcd.print(" STATE : ");
    lcd.setCursor(0, 1);
    lcd.print(" HOUR");
    lcd.setCursor(0, 2);
    lcd.print(" MINUTE");
    lcd.setCursor(0, 3); 
    lcd.print(">BACK");
  }
}

void chonmenuTong() 
{
  switch (demmenu) 
  {
    case 0:
      home = 3;
      demmenu = 0;
      delay(400);
      break;
    case 1:
      home = 2;
      homepage = 0;
      demmenu = 0;
      delay(400);
      break;
    case 2:
      home = 4;
      demmenu = 0;
      homepage = 0;
      delay(400);
      break;
    case 3:
      home = 0;
      demmenu = 0;
      delay(500);
      attachInterrupt(menu, isr0, FALLING);
      lcd.clear();
      break;
  }
}

void chonmenuMonitor() 
{
  switch (demmenu) 
  {
    case 0:
      home = 2;
      homepage = 1;
      delay(400);
      break;
    case 1:
      home = 2;
      homepage = 2;
      delay(400);
      break;
    case 2:
      home = 1;
      demmenu = 1;
      lcd.clear();
      delay(400);
      break;
  }
}

void chonmenuTimer() 
{
  switch (demmenu) 
  {
    case 0:
      state = state + 1;
      EEPROM.write(2, state);
      EEPROM.commit();
      if(state>2){ state = 0;}
      if(state == 0){
        lcd.setCursor(9,0);
        lcd.print("    ");
        lcd.setCursor(9,0);
        lcd.print("NONE");
      } else if (state == 1){
        lcd.setCursor(9,0);
        lcd.print("    ");
        lcd.setCursor(9,0);
        lcd.print("ON");
      } else if(state == 2){
        lcd.setCursor(9,0);
        lcd.print("    ");
        lcd.setCursor(9,0);
        lcd.print("OFF");
      }
      delay(300);
      break;
    case 1:
      homepage = 1;
      lcd.clear();
      lcd.print("HOUR : ");
      delay(400);
      break;
    case 2:
      homepage = 2;
      lcd.clear();
      lcd.print("MINUTE : ");
      delay(400);
      break;
    case 3:
      home = 1;
      demmenu = 2;
      b = 1;
      lcd.clear();
      delay(400);
      //state3 = 0;
      break;
  }
}

void chonmenu1() // Menu bên trong chức năng ON/OFF
{
  switch (demmenu) 
  {
    case 0:
      data = "P2P 7F5D C";
      SerialPort.print(data); // Gửi tín hiệu ON/OFF cho Node Energy
      break;
    case 1:
      home = 1;
      demmenu = 0;
      lcd.clear();
      delay(400);
      break;
  }
}



void setup() {
  Wire1.begin(SDA_2, SCL_2, 100000);
  //rtc.set(10, 11, 11, 3, 28, 5, 24);
  // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year)
  // set day of week (1=Sunday, 7=Saturday)
  EEPROM.begin(512);
  gio = EEPROM.read(0);
  phut = EEPROM.read(1);
  state = EEPROM.read(2);
  lcd.init();                    
  lcd.backlight();
  Serial.begin(9600);
  while(!Serial) delay(1);
  SerialPort.begin(38400, SERIAL_8N1, 16, 17); //Set up UART2 for Zigbee
  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  pinMode(Led, OUTPUT);
  pinMode(len, INPUT_PULLUP);
  pinMode(xuong, INPUT_PULLUP);
  pinMode(menu, INPUT_PULLUP);
  digitalWrite(Led,HIGH);
  attachInterrupt(menu, isr0, FALLING);
  date = rtc.day();
  month = rtc.month();
  year = rtc.year();
  hour = rtc.hour();
  minute = rtc.minute();
  second = rtc.second();
  thu = rtc.dayOfWeek();
}

void loop() {
  if (home == 0){

  client.loop();

  if (!client.connected()) {
    reconnect();
    lcd.clear();
  }

  Time();

  for(int q=0; q<20;q++){
  CheckState();
  if(state2 == 0){
    String p = "1";
    publishMessage("Esp32/Status", p, true);
  } else if(state2 == 1){
    String p = "0";
    publishMessage("Esp32/Status", p, true);
  }
  delay(10);
  }

  Energy_Consump();

  Water_Consump();

  }

  else if(home == 1){
    if(a == 1){
    a = 0;
    lcd.clear();
    delay(400);
   }
    menuTong();
    gtlen = digitalRead(len);
    gtxuong = digitalRead(xuong);
    gtmenu = digitalRead(menu);
    if (gtlen == 0){
      while(!gtlen == 0);
      if (demmenu == 0)
      { demmenu = 3;  }
      else if(demmenu == 1)
      { demmenu = 0;  }
      else if(demmenu == 2)
      { demmenu = 1;  }     
      else if(demmenu == 3)
      { demmenu = 2;  }   
      menuTong();
      delay(200);
  }
    if (gtxuong == 0){
    while(!gtxuong == 0);
      if (demmenu == 0)
      { demmenu = 1;  }
      else if(demmenu == 1)
      { demmenu = 2;  }
      else if(demmenu == 2)
      { demmenu = 3;  }
      else if(demmenu == 3)
      { demmenu = 0;  }
      menuTong();
      delay(200);
  }
    if (gtmenu == 0){
    while(!gtmenu == 0); 
    chonmenuTong();
    lcd.clear();
  }
  }

  else if (home == 2){
    if(homepage == 0){
    if(a == 1){
    a = 0;
    lcd.clear();
  }
    menuMonitor();
    gtlen = digitalRead(len);
    gtxuong = digitalRead(xuong);
    gtmenu = digitalRead(menu);
      if (gtlen == 0){
      while(!gtlen == 0);
      if (demmenu == 0)
      { demmenu = 2;  }
      else if(demmenu == 1)
      { demmenu = 0;  }
      else if(demmenu == 2)
      { demmenu = 1;  }    
      menuMonitor();
      delay(200);
  }
    if (gtxuong == 0){
    while(!gtxuong == 0);
      if (demmenu == 0)
      { demmenu = 1;  }
      else if(demmenu == 1)
      { demmenu = 2;  }
      else if(demmenu == 2)
      { demmenu = 0;  }
      menuMonitor();
      delay(200);
  }
    if (gtmenu == 0){
    while(!gtmenu == 0); 
    chonmenuMonitor();
    lcd.clear();
  }}
  else if(homepage == 1){
    if(a == 0){
    a = 1;
    attachInterrupt(menu, isr1, FALLING);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("RATE = ");
    lcd.setCursor(0,1);
    lcd.print("VOLUME = ");
    lcd.setCursor(0,2);
    lcd.print("MONTH = ");
  }
    Water_Monitor();
  }
  else if(homepage == 2){
    if(a == 0){
    a = 1;
    attachInterrupt(menu, isr1, FALLING);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("U: ");
    lcd.setCursor(0,1);
    lcd.print("I: ");
    lcd.setCursor(0,2);
    lcd.print("P: ");
    lcd.setCursor(0,3);
    lcd.print("A: ");
  }
    Energy_Monitor();
  }
  } else if (home == 3){
    if(a == 1){
    a = 0;
    lcd.clear();
  }
    menu1();
    CheckState();
    if(state2 == 0){
      lcd.setCursor(14,0);
      lcd.print("      ");
      lcd.setCursor(14,0);
      lcd.print("OFF");
      //delay(50);
    }
    if(state2 == 1){
      lcd.setCursor(14,0);
      lcd.print("      ");
      lcd.setCursor(14,0);
      lcd.print("ON");
      //delay(50);
    }
    gtlen = digitalRead(len);
    gtxuong = digitalRead(xuong);
    gtmenu = digitalRead(menu);
      if (gtlen == 0){
      while(!gtlen == 0);
      if (demmenu == 0)
      { demmenu = 1;  }
      else if(demmenu == 1)
      { demmenu = 0;  }    
      menu1();
      delay(200);
  }
    if (gtxuong == 0){
    while(!gtxuong == 0);
      if (demmenu == 0)
      { demmenu = 1;  }
      else if(demmenu == 1)
      { demmenu = 0;  }
      menu1();
      delay(200);
  }
    if (gtmenu == 0){
    while(!gtmenu == 0); 
    chonmenu1();
    //delay(50);
  }
  } else if (home == 4){
    if(homepage == 0){
    if(b == 1){
      b = 0;
      if(state == 0){
      lcd.setCursor(9,0);
      lcd.print("    ");
      lcd.setCursor(9,0);
      lcd.print("NONE");
      } else if (state == 1){
      lcd.setCursor(9,0);
      lcd.print("    ");
      lcd.setCursor(9,0);
      lcd.print("ON");
      } else if(state == 2){
      lcd.setCursor(9,0);
      lcd.print("    ");
      lcd.setCursor(9,0);
      lcd.print("OFF");
      }
    }
    menuTimer();
    gtlen = digitalRead(len);
    gtxuong = digitalRead(xuong);
    gtmenu = digitalRead(menu);
    if (gtlen == 0){
      while(!gtlen == 0);
      if (demmenu == 0)
      { demmenu = 3;  }
      else if(demmenu == 1)
      { demmenu = 0;  }
      else if(demmenu == 2)
      { demmenu = 1;  }     
      else if(demmenu == 3)
      { demmenu = 2;  }   
      menuTimer();
      delay(200);
  }
    if (gtxuong == 0){
    while(!gtxuong == 0);
      if (demmenu == 0)
      { demmenu = 1;  }
      else if(demmenu == 1)
      { demmenu = 2;  }
      else if(demmenu == 2)
      { demmenu = 3;  }
      else if(demmenu == 3)
      { demmenu = 0;  }
      menuTimer();
      delay(200);
  }
    if (gtmenu == 0){
    while(!gtmenu == 0); 
    chonmenuTimer();
    //lcd.clear();
  }} else if (homepage == 1){
  gtlen = digitalRead(len);
  gtxuong = digitalRead(xuong);
  gtmenu = digitalRead(menu); 
  lcd.setCursor(7,0);
  lcd.print("   ");
  lcd.setCursor(7,0); 
  lcd.print(gio); 
  if (gtlen == 0)
  {
    if(gio == 23){gio = 0;}
    gio = gio + 1;  
  }   
  if (gtxuong == 0)
  {
    if(gio == 0) {gio = 23;}
    gio = gio - 1;
  }
  if (gtmenu == 0)
  {
    homepage = 0;
    EEPROM.write(0, gio);
    EEPROM.commit();
    b = 1;       
  }
  delay (150);
  } else if (homepage == 2){
    gtlen = digitalRead(len);
    gtxuong = digitalRead(xuong);
    gtmenu = digitalRead(menu); 
    lcd.setCursor(9,0);
    lcd.print("   ");
    lcd.setCursor(9,0); 
    lcd.print(phut);  
    if (gtlen == 0)
    {
    if(phut == 59){phut = 0;}
      phut = phut + 1;   
    } 
    if (gtxuong == 0)
    {
    if(phut == 0) {phut = 59;}
      phut = phut - 1;
    }
    if (gtmenu == 0)
    {
      homepage = 0; 
      EEPROM.write(1, phut);
      EEPROM.commit();
      b = 1;     
    }
    delay (150);
  }
  } 
}
