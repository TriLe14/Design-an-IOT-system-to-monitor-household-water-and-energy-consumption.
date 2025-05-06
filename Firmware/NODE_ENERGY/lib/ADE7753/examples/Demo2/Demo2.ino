

/* El siguiente programa es una demostración de las mediciones que el Energy Shield es capaz de realizar. Se abarcan medidas de Voltaje RMS 
* Corriente RMS , Energía Activa Acumulada durante un período de tiempo particular y Período de la señal de voltaje (Canal 2), cabe destacar que la energía acumulada durante un segundo 
* es igual a la potencia activa.

* Para ejecutar las mediciones se ocupó la proporcionalidad existente entre el valor entregado por el Energy Shield y el valor real de la magnitud 
* calculada. 

* Las constantes de proporcionalidad fueron calculadas de forma experimental (una demostración de aquellas pruebas se encuentran en el sketch DetConst), para el caso del Período la 
constante viene definida desde el datasheet.

*Las constante son las siguientes:

* Voltaje: kv=0.000000171849155 [volt/LSB]
* Corriente: ki=0.00000039122624397277 [amp/LSB]
* Energía Activa Acumulada: ke=0.0.00041576242446899414 [J/LSB]
* Período: kt=2.2*pow(10,-6) [seg/LSB]
*/

#include "ADE7753.h"  

#include <SPI.h>   
#include <Wire.h>
#include <SoftwareSerial.h>

void setup(){
}

void loop() { 

ADE7753 meter;  

meter.gainSetup(GAIN_1,GAIN_1, 0  ,0,   0,0); 
meter.resetStatus();

long v1,i1,e1,e2,e3,t1;

float kv,ki,ke,kt;

//Constantes de proporcionalidad.

kv=0.000000171849155;                        
ki=0.00000039122624397277;
ke=0.00041576242446899414;
kt=2.2*pow(10,-6);

while(1){

  
// Do dien ap
  v1=meter.vrms();

// Do dong dien
  i1=meter.irms();
  
// Do chu ky
  t1=meter.getPeriod();
  
// Do nang luong tich luy
meter.setMode(0x0080); // Chế độ tích lũy năng lượng ban đầu
meter.setLineCyc(100); // Số lượng nửa chu kỳ chiếm trong phép đo là cố định. 100 nửa chu kỳ tương đương với 1 giây làm việc trên mạng 50 Hz.
e1=meter.getLAENERGY(); // Trích xuất năng lượng hoạt động tích lũy, đồng bộ hóa phép đo với điểm qua 0 của tín hiệu điện áp. 

meter.setMode(0x0080);
meter.setLineCyc(200); // 2 giây đo.
e2=meter.getLAENERGY();

meter.setMode(0x0080);
meter.setLineCyc(300); // 3 giây đo
e3=meter.getLAENERGY();
  
  Serial.println("\n------Phép đo mới------");
  Serial.print("\nVoltage RMS [V]: ");
  Serial.print(kv*v1*51,15);
  Serial.print("\nCurrent RMS [A]: ");
  Serial.print(ki*i1*51,15);
  Serial.print("\nEnergia Activa Acumulada [J] en 1 seg : "); // Năng lượng hoạt động tích lũy trong 1s
  Serial.print(ke*(e3-e2)*51,15);                           // Ta lấy sự chênh lệch giữa năng lượng hoạt động tích lũy trong 3 giây và năng lượng tích lũy trong 2 giây. Năng lượng hoạt động tích lũy ở giây đầu bị bỏ qua do các giá trị đo ban đầu không phải lúc nào cũng chính xác.
  Serial.print("\nPeriodo de la senial de voltaje [seg]: ");
  Serial.println(t1*kt,5);     
}
}

