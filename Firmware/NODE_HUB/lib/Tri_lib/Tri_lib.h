#ifndef Tri_lib_h
#define Tri_lib_h

#include <Wire.h>
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

extern int tocdo , thetich, thang, index_1,  index_2, index_3, index_4, index_5, index_6;
extern String Rx_data;
extern char mang[20] ;
extern char mangEnergy[60];
extern char rate[15];
extern char volume[15];
extern char thismonth [15];

extern int v1, i1, f1, p1, e1, pf1;

extern char v[15];
extern char i[15];
extern char f[15];
extern char p[15];
extern char en[15];
extern char pf[15];

void Tachchuoi_Water(char* mang_char);
void Tachchuoi_Energy(char* mang_char);
void Tachchuoi_WaterMonth(char* mang_char);
void Tachchuoi_EnergyMonth(char* mang_char);

#endif