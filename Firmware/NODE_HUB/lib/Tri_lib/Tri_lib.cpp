#include "Tri_lib.h"
#include <Wire.h>
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>


int tocdo = 0, thetich = 0, thang = 0;
int index_1 = 0, index_2 = 0,  index_3 = 0, index_4 = 0, index_5 = 0, index_6 = 0;
int v1 = 0, i1 = 0, f1 = 0, p1 = 0, pf1 = 0, e1 = 0;

String Rx_data;
char mang[20] ;
char mangEnergy[60];
char rate[15];
char volume[15];
char thismonth [15];

char v[15];
char i[15];
char f[15];
char p[15];
char en[15];
char pf[15];

void Tachchuoi_Water(char* mang_char) {
    for(int m = 0; mang_char[m] != '\0'; m++){
      if(mang_char[m] == 'S'){
        thismonth[index_3] = '\0';
        volume[index_2] = '\0';
        rate[index_1] = '\0';
        index_1 = 0;
        index_2 = 0;
        index_3 = 0;
        thang = 0;
        break;
      }
      if(thang == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        thismonth[index_3] = mang_char[m];
        index_3++;
        }
      }
      if(mang_char[m] == 'M'){
        thang = 1;
        thetich = 0;
      }
      if(thetich == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        volume[index_2] = mang_char[m];
        index_2++;
        }
      }
      if(mang_char[m] == 'V'){
        thetich = 1;
        tocdo = 0 ;
      }
      if(tocdo == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        rate[index_1] = mang_char[m];
        index_1++;
        }
      }
      if(mang_char[m] == 'R'){
        tocdo= 1;
      }
    }
}

void Tachchuoi_Energy(char* mang_char){
      for(int m = 0; mang_char[m] != '\0'; m++){
      if(mang_char[m] == 'S'){
        en[index_6] = '\0';
        pf[index_5] = '\0';
        p[index_4] = '\0';
        f[index_3] = '\0';
        i[index_2] = '\0';
        v[index_1] = '\0';
        index_1 = 0;
        index_2 = 0;
        index_3 = 0;
        index_4 = 0;
        index_5 = 0;
        index_6 = 0;
        e1 = 0;
        break;
      }
      if(e1 == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        en[index_6] = mang_char[m];
        index_6++;
        } 
      }
      if(mang_char[m] == 'A'){
        e1 = 1;
        pf1 = 0;
      }
      if(pf1 == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        pf[index_5] = mang_char[m];
        index_5++;
        } 
      }
      if(mang_char[m] == 'H'){
        pf1 = 1;
        p1 = 0;
      }
      if(p1 == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        p[index_4] = mang_char[m];
        index_4++;
        }
      }
      if(mang_char[m] == 'P'){
        p1 = 1;
        f1 = 0;
      }
      if(f1 == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        f[index_3] = mang_char[m];
        index_3++;
        }
      }
      if(mang_char[m] == 'F'){
        f1 = 1;
        i1 = 0;
      }
      if(i1 == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        i[index_2] = mang_char[m];
        index_2++;
        }
      }
      if(mang_char[m] == 'I'){
        i1 = 1;
        v1 = 0 ;
      }
      if(v1 == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        v[index_1] = mang_char[m];
        index_1++;
        }
      }
      if(mang_char[m] == 'U'){
        v1= 1;
      }
    }
}

void Tachchuoi_WaterMonth(char* mang_char) {
    for(int m = 0; mang_char[m] != '\0'; m++){
      if(mang_char[m] == 'S'){
        thismonth[index_1] = '\0';
        thang = 0;
        break;
      }
      if(thang == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        thismonth[index_1] = mang_char[m];
        index_1++;
        }
      }
      if(mang_char[m] == 'L'){
        thang= 1;
      }
    }
}

void Tachchuoi_EnergyMonth(char* mang_char) {
    for(int m = 0; mang_char[m] != '\0'; m++){
      if(mang_char[m] == 'S'){
        en[index_1] = '\0';
        e1 = 0;
        break;
      }
      if(e1 == 1){
        if((mang_char[m] >= '0' && mang_char[m] <= '9') || mang_char[m] == '.'){
        en[index_1] = mang_char[m];
        index_1++;
        }
      }
      if(mang_char[m] == 'E'){
        e1= 1;
      }
    }
}

