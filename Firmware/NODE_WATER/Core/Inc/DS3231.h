/*
 * DS3231.h
 *
 *  Created on: Mar 31, 2024
 *      Author: ACER
 */

#ifndef INC_DS3231_H_
#define INC_DS3231_H_

#include "stm32f1xx_hal.h"
#include "main.h"

#define DS3231_ADDRESS 0xD0 //Địa chỉ I2C của module DS3231
#define EEPROM_ADDRESS 0xAE //Địa chỉ I2C của AT24C32

extern float TEMP;
extern char buffer[15];
extern uint8_t seconds;
extern uint8_t minutes;
extern uint8_t hour;
extern uint8_t dayofweek;
extern uint8_t dayofmonth;
extern uint8_t month;
extern uint8_t year;
extern uint16_t addr;
extern uint8_t dayofmonth_t;
extern uint8_t month_t;
extern uint8_t year_t;

uint8_t bcdToDec(uint8_t val);//chuyển đổi mã BCD sang mã DEC mục đích để khi đọc dữ liệu từ DS3231 chúng ta sẽ chuyển sang hệ 10
uint8_t decToBcd(uint8_t val);//chuyển mã DEC sang mã BCD mục đích để khi cấu hình thời gian ban đầu
void Set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year);
void Get_Time (void);
float Get_Temp (void);
void force_temp_conv (void);
void Time_LCD();

void EEPROM_WriteByte(uint16_t addr, uint8_t data);
uint8_t EEPROM_ReadByte(uint16_t addr);

void EEPROM_WriteTwoBytes(uint16_t addr, uint16_t data);
uint16_t EEPROM_ReadTwoBytes(uint16_t addr);

void EEPROM_WriteFloat(uint16_t addr, float data);
float EEPROM_ReadFloat(uint16_t addr);

void Save_Month(float a);

#endif /* INC_DS3231_H_ */
