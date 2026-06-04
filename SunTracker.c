#include <stdio.h>
#include <stdint.h>
#include "libschsat.h"
#define LSS_OK 0 
#define LSS_ERROR 1 
#define LSS_BREAK 2
 
int control(){ //Основная функция программы
    uint16_t sun_result[] = {0, 0, 0}; // инициализируем sun_result
    uint16_t num = 1; // номер солнечного датчика
    printf("Enable sun sensor №%d\n", num);
    sun_sensor_turn_on(num); //включаем датчик  
    Sleep(1); //Ждем включения 1 секунду
    printf("Get RAW data from sun sensor №%d\n", num);
    for (int i = 0; i < 10; i++) //считываем показаия 10 раз
    {
        sun_result[0] = sun_sensor_request_raw(num,& sun_result[1],& sun_result[2]);/*проверить как работает, очень странно,  что работает
        если не работает задать sun_result[0] */
        if (!sun_result[0]){ //если датчик не вернул сообщение об ошибке,
            printf("state: %d raw = %d, %d\n", i, sun_result[1], sun_result[2]);
            }
 
        else if (sun_result[0] == 1) { //если датчик вернул сообщение об ошибке 1
            printf("Fail because of access error, check the connection\n");
            }
        else if (sun_result[0] == 2) { //если датчик вернул сообщение об ошибке 2
            printf("Fail because of interface error, check you code\n");
            }
        Sleep(1); //показания считываются раз в секунду
 
        }
    printf("Disable sun sensor №%d\n", num);
    sun_sensor_turn_off(num); //выключаем солнечный датчик
    return 0;
}