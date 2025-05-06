/*
 * DS3231.c
 *
 *  Created on: Mar 31, 2024
 *      Author: ACER
 */
#include "DS3231.h"
#include "stdio.h"
#include "string.h"

extern I2C_HandleTypeDef hi2c2;

float TEMP;
char buffer[15];
uint8_t seconds;
uint8_t minutes;
uint8_t hour;
uint8_t dayofweek;
uint8_t dayofmonth;
uint8_t month;
uint8_t year;
uint16_t addr;
uint8_t dayofmonth_t;
uint8_t month_t;
uint8_t year_t;

uint8_t decToBcd(uint8_t val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}

uint8_t bcdToDec(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}

void Set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
	uint8_t set_time[7];
	set_time[0] = decToBcd(sec);
	set_time[1] = decToBcd(min);
	set_time[2] = decToBcd(hour);
	set_time[3] = decToBcd(dow);
	set_time[4] = decToBcd(dom);
	set_time[5] = decToBcd(month);
	set_time[6] = decToBcd(year);

	HAL_I2C_Mem_Write(&hi2c2, DS3231_ADDRESS, 0x00, 1, set_time, 7, 1000);
	//HAL_I2C_Mem_Write để ghi dữ liệu trực tiếp vào địa chỉ đã cho. Ở đây đã sử dụng một địa chỉ duy nhất (0x00) và ghi 7 byte dữ liệu vào đó. Hoạt động này được gọi là Multi-Write và được hầu hết mọi thiết bị bộ nhớ hỗ trợ. Địa chỉ sẽ tự động tăng lên khi bạn ghi byte dữ liệu tiếp theo. Cách này tương tự như việc ghi 1 byte dữ liệu vào mỗi thanh ghi trong số 7 thanh ghi đầu tiên (00h đến 06h).
}

void Get_Time (void)
{
	uint8_t get_time[7];
	HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x00, 1, get_time, 7, 1000);
	//seconds = bcdToDec(get_time[0]);
	//minutes = bcdToDec(get_time[1]);
	//hour = bcdToDec(get_time[2]);
	//dayofweek = bcdToDec(get_time[3]);
	dayofmonth = bcdToDec(get_time[4]);
	month = bcdToDec(get_time[5]);
	year = bcdToDec(get_time[6]);
}

float Get_Temp (void)
{
	uint8_t temp[2];

	HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x11, 1, temp, 2, 1000);
	return ((temp[0])+(temp[1]>>6)/4.0);
}

void force_temp_conv (void)
{
	uint8_t status=0;
	uint8_t control=0;
	HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x0F, 1, &status, 1, 100);  // read status register
	if (!(status&0x04))
	{
		HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x0E, 1, &control, 1, 100);  // read control register
		HAL_I2C_Mem_Write(&hi2c2, DS3231_ADDRESS, 0x0E, 1, (uint8_t *)(control|(0x20)), 1, 100);
	}
}

void EEPROM_WriteByte(uint16_t addr, uint8_t data) {
  uint8_t i2cData[4];

	  i2cData[0] = (addr >> 8) & 0xFF;  // MSB of address
	  i2cData[1] = addr & 0xFF;         // LSB of address
	  i2cData[2] = data;                 // Data to be written. Chỉ cần 1 byte để lưu biến kiểu int

	  HAL_I2C_Master_Transmit(&hi2c2, EEPROM_ADDRESS, i2cData, 3, HAL_MAX_DELAY);

  // Wait for write cycle to complete
  HAL_Delay(10);
}


uint8_t EEPROM_ReadByte(uint16_t addr) {
  uint8_t i2cData[2];
  uint8_t data;

  i2cData[0] = (addr >> 8) & 0xFF;  // MSB of address
  i2cData[1] = addr & 0xFF;         // LSB of address

  HAL_I2C_Master_Transmit(&hi2c2, EEPROM_ADDRESS, i2cData, 2, HAL_MAX_DELAY);
  HAL_I2C_Master_Receive(&hi2c2, EEPROM_ADDRESS, &data, 1, HAL_MAX_DELAY);


  return data;
}

void EEPROM_WriteTwoBytes(uint16_t addr, uint16_t data) {
  uint8_t i2cData[4];

	  i2cData[0] = (addr >> 8) & 0xFF;  // MSB of address
	  i2cData[1] = addr & 0xFF;         // LSB of address
	  i2cData[2] = (data >> 8) & 0xFF;  // Data to be written. Dữ liệu lúc này >255 nên cần dùng 2 byte để lưu trữ
	  i2cData[3] = data & 0xFF;

	  HAL_I2C_Master_Transmit(&hi2c2, EEPROM_ADDRESS, i2cData, 4, HAL_MAX_DELAY);
  // Wait for write cycle to complete
  HAL_Delay(10);
}


uint16_t EEPROM_ReadTwoBytes(uint16_t addr) {
  uint8_t i2cData[2];
  uint8_t data[2];

  i2cData[0] = (addr >> 8) & 0xFF;  // MSB of address
  i2cData[1] = addr & 0xFF;         // LSB of address

  HAL_I2C_Master_Transmit(&hi2c2, EEPROM_ADDRESS, i2cData, 2, HAL_MAX_DELAY);
  HAL_I2C_Master_Receive(&hi2c2, EEPROM_ADDRESS, &data, 2, HAL_MAX_DELAY);

  uint16_t result = (data[0] << 8) | data[1];

  return result;
}

void EEPROM_WriteFloat(uint16_t addr, float data) {
  uint8_t i2cData[6];

  // Chuyển đổi số float thành mảng byte
  uint8_t* floatBytes = (uint8_t*)&data;

  i2cData[0] = (addr >> 8) & 0xFF;         // MSB of address
  i2cData[1] = addr & 0xFF;                // LSB of address
  i2cData[2] = floatBytes[0];              // Byte 0 of float. Cần 4 byte để có thể lưu trữ 1 biến kiểu float
  i2cData[3] = floatBytes[1];              // Byte 1 of float
  i2cData[4] = floatBytes[2];              // Byte 2 of float
  i2cData[5] = floatBytes[3];              // Byte 3 of float

  HAL_I2C_Master_Transmit(&hi2c2, EEPROM_ADDRESS, i2cData, 6, HAL_MAX_DELAY);

  // Wait for write cycle to complete
  HAL_Delay(10);
}

float EEPROM_ReadFloat(uint16_t addr) {
  uint8_t i2cData[2];
  uint8_t floatBytes[4];

  i2cData[0] = (addr >> 8) & 0xFF;         // MSB of address
  i2cData[1] = addr & 0xFF;                // LSB of address

  HAL_I2C_Master_Transmit(&hi2c2, EEPROM_ADDRESS, i2cData, 2, HAL_MAX_DELAY);
  HAL_I2C_Master_Receive(&hi2c2, EEPROM_ADDRESS, floatBytes, 4, HAL_MAX_DELAY);

  float result;
  memcpy(&result, floatBytes, sizeof(float));

  return result;
}

void Save_Month(float a){
	addr = 0x0020 + (month-1)*4;
	EEPROM_WriteFloat(addr,a);
}

void Time_LCD(){//Hàm hiển thị thời gian lên LCD
	dayofmonth_t = dayofmonth;
	month_t = month;
	year_t = year;
	sprintf (buffer, "%02d/%02d/20%02d", dayofmonth, month, year);
	lcd_goto_XY (1,0);
	lcd_send_string(buffer);
}




