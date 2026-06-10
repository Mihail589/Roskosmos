#define LSS_OK 0 
#define LSS_ERROR 1 
#define LSS_BREAK 2
 
#include <stdio.h>
#include <stdint.h>
#include "libschsat.h"
int control() // Основная функция программы, в которой вызываем остальные функции
{
 int16_t mgn_result[] = {0,0,0,0}; // Инициализируем mgn_result
 uint16_t num = 1; // номер магнитометра
 int i;
 printf("Rnable  magnetometer №%d\n", num);
 magnetometer_turn_on(num);
 Sleep(1);
 printf("Get  RAW data from magnetometer\n");
 for (i = 0; i < 1000; i++)
 {
  mgn_result[0] = magnetometer_request_raw(num,& mgn_result[1],& mgn_result[2],& mgn_result[3]);
  if (!mgn_result[0])// если датчик вернул сообщение об ошибке,
  {
   printf("\nstate:%d,  \nx_raw =%d,\ny_raw =%d, \nz_raw =%d\n",i, mgn_result[1], mgn_result[2], mgn_result[3]); //Обратите внимание на символ переноса строки!
  }
  else if (mgn_result[0] == 1){
   printf("Fail because of access error, check the connection");
  }
  else if (mgn_result[0] == 2){
   printf("Fail  because of interdace error, check your code");
  }
  Sleep(0.1);
 }
 printf("Disable  magnetometer №%d\n", num);
 magnetometer_turn_off(num);
 return 0;
}
