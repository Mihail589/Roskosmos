#include <stdio.h>
#include <stdint.h>
#include "libschsat.h"
#define LSS_OK 0 
#define LSS_ERROR 1 
#define LSS_BREAK 2
#include <math.h>
 
const float time_step = 100;        // Временной шаг работы алгоритма, мс
const int16_t mtr_max_speed = 3000;     // Максимально допустимая скорость маховика, об/мин
const uint16_t mtr_num = 1;     // Номер маховика
const uint16_t hyr_num = 1;     // Номер ДУС
const uint16_t mag_num = 1;     // Номер магнитометра
int16_t confirm = 0;            //Переменная для функции motor_set_speed
 
float errora = 0;               // Ошибка по углу
float Pa = 0;               // Воздействие пропорционального звена
float Da = 0;               // Воздействие дифференциального звена
float Ia = 0;               // Воздействие интегрального звена
float Kpa = 1.5;         // Пропорциональный коэффициент ошибки по углу
float Kda = 0.15;            // Дифференциальный коэффициент
float Kia = 2.5;              // Интегральный коэффициент
float lastErrora = 0;           // Прошлая ошибка по углу
float Integratora = 0;          // Интеграл (сумма всех ошибок по углу)
float PID_alpha = 0;            // Величина управляющего воздействия
float Integrator_maxa = 10;     // Ограничение максимального значения интеграла
float Integrator_mina = -10;    // Ограничение минимального значения интеграла
 
float error = 0;            // Ошибка по угловой скорости
float P = 0;                // Воздействие пропорционального звена
float D = 0;                // Воздействие дифференциального звена
float I = 0;                // Воздействие интегрального звена
float Kp = 10;              // Пропорциональный коэффициент
float Kd = 0.1;             // Дифференциальный коэффициент
float Ki = 1;               // Интегральный коэффициент
float lastError = 0;        // Прошлая ошибка по угловой скорости
float Integrator = 0;       // Интеграл (сумма всех ошибок по угловой скорости)
float PID_omega = 0;        // Величина управляющего воздействия
float Integrator_max = 10;      // Ограничение максимального значения интеграла
float Integrator_min = -10;     // Ограничение минимального значения интеграла
 
int16_t mtr_new_speed;
 
int mag_calibrated(int16_t *magx, int16_t *magy, int16_t *magz ){
//Функция mag_calibrated вносит поправки в показания магнитометра с учетом калибровочных коэффициентов
    float magx_cal;
    magx_cal = 1.668867*(*magx + -38.326818) + -0.007649*(*magy + 20.442007) + 0.113348*(*magz + -69.659602);
    float magy_cal;
    magy_cal = -0.007649*(*magx + -38.326818) + 1.749597*(*magy + 20.442007) + 0.130999*(*magz + -69.659602);
    float magz_cal;
    magz_cal = 0.113348*(*magx + -38.326818) + 0.130999*(*magy + 20.442007) + 1.577785*(*magz + -69.659602);
    *magx = (int16_t) magx_cal;
    *magy = (int16_t) magy_cal;
    *magz = (int16_t) magz_cal; 
    return 0;
}
 
int motor_new_speed_PD(int mtr_speed, int alpha, int alpha_goal, int omega, int omega_goal){
 /* Функция для определения новой скорости маховика.
 mtr_speed - текущая угловая скорость маховика, об/мин
 omega - текущая угловая скорость спутника, град/с
 omega_goal - целевая угловая скорость спутника, град/с
 mtr_new_speed - требуемая угловая скорость маховика, об/мин*/
 
    error = omega - omega_goal;                 // Вычисление ошибки
    P = Kp * error;                             // Вычисление воздействия пропорционального звена
    D = Kd * (error - lastError) / time_step;   // Вычисление воздействия дифференциального звена
    lastError = error;                          // Запоминаем ошибку
    Integrator = Integrator + error * time_step;    // Накапливаем суммарную ошибку
    if (Integrator > Integrator_max){           // Сатурация (Ограничиваем максимальное значение накапливаемой ошибки)
        Integrator = Integrator_max;
    }
    else if (Integrator < Integrator_min){
        Integrator = Integrator_min;
    }
    I = Integrator * Ki;                    // Вычисление воздействия интегрального звена
    PID_omega = P + I + D;              // Вычисление суммарного управляющего воздействия
 
    errora = alpha - alpha_goal;                // Вычисление ошибки
    Pa = Kpa * errora;                          // Вычисление воздействия пропорционального звена
    Da = Kda * (errora - lastErrora) / time_step;   // Вычисление воздействия дифференциального звена
    lastErrora = errora;                        // Запоминаем ошибку
    Integratora = Integratora + errora * time_step; // Накапливаем суммарную ошибку
    if (Integratora > Integrator_maxa){             // Сатурация (Ограничиваем максимальное значение накапливаемой ошибки)
        Integratora = Integrator_maxa;
    }
    else if (Integratora < Integrator_mina){
        Integratora = Integrator_mina;
    }
    Ia = Integratora * Kia;                     // Вычисление воздействия интегрального звена
    PID_alpha = Pa + Ia + Da;               // Вычисление суммарного управляющего воздействия   
 
    mtr_new_speed = (int16_t)(mtr_speed + PID_omega);
 
    if (mtr_new_speed > mtr_max_speed){
        mtr_new_speed = mtr_max_speed;
    }
    else if (mtr_new_speed < -mtr_max_speed){
        mtr_new_speed = -mtr_max_speed;
    }
    printf("P=%f\t I=%f\t D=%f\t PID_omega=%f\t mtr_new_speed=%d\n", P,  I,  D, PID_omega, mtr_new_speed);   
    printf("Pa=%f\t Ia=%f\t Da=%f\t PID_alpha=%f\t", Pa, Ia, Da, PID_alpha); 
    return mtr_new_speed;
}
 
int angle_transformation(int alpha, int alpha_goal){
    if (alpha <= (alpha_goal - 180)){
        alpha = alpha + 360;
    }
    else if (alpha>(alpha_goal +180)){
        alpha = alpha - 360;
    }
    return alpha;
}
 
void initialize_all(void){
/*  Функция инициализации всех систем,
    включает все приборы,которые будут использоваться в основной программе.*/
    printf ("Enable motor №%d\n", mtr_num);
    motor_turn_on(mtr_num);
    Sleep(1);
    printf("Enable angular velocity sensor №%d\n", hyr_num); 
    hyro_turn_on(hyr_num); // Включаем ДУС
    Sleep(1);
    printf("Enable magnetometer %d\n", mag_num);
    magnetometer_turn_on(mag_num);
    Sleep(1);
}
 
int switch_off_all(){
//Функция отключает все приборы,которые будут использоваться в основной программе.
    printf("Finishing...\n");
    printf("Disable angular velocity sensor №%d\n", hyr_num);
    hyro_turn_off(hyr_num);
    printf("Disable magnetometer №%d\n", mag_num);
    magnetometer_turn_off(mag_num);
    motor_set_speed(mtr_num, 0, &confirm);
    Sleep (1);
    motor_turn_off(mtr_num);
    printf("Finish program\n");
    return 0;
}
 
int control(){
    //данные магнитометра
    int16_t mgx_cal=0;
    int16_t mgy_cal=0;
    int16_t mgz_cal=0;
    int16_t *magx_raw = &mgx_cal;
    int16_t *magy_raw = &mgy_cal;
    int16_t *magz_raw = &mgz_cal;
    //данные ДУС
    int16_t gx_raw;
    int16_t gy_raw;
    int16_t gz_raw;
    int16_t *hyrox_raw = &gx_raw;
    int16_t *hyroy_raw = &gy_raw;
    int16_t *hyroz_raw = &gz_raw;
    int16_t mtr_speed;
    int16_t omega = 0;
    int16_t omega_goal = 0;  // Целевая угловая скорость
 
    initialize_all();
 
    int mtr_state = 0;  // Инициализируем статус маховика
    int hyro_state = 0;     // Инициализируем статус ДУС
    int mag_state = 0;  // Инициализируем статус магнитометра
    int alpha_goal = 90;    // Целевой угол     
    double mag_alpha = 0;
    int i;
    for (i = 0; i < 1000; i++){
        mag_state = magnetometer_request_raw(mag_num, magx_raw, magy_raw, magz_raw);
        hyro_state = hyro_request_raw(hyr_num,hyrox_raw,hyroy_raw,hyroz_raw); 
        gx_raw = *hyrox_raw; 
        gy_raw = *hyroy_raw; 
        gz_raw = *hyroz_raw; 
        mtr_state = motor_request_speed(mtr_num, &mtr_speed);
        if (!mag_state){
            mag_calibrated(magx_raw,magy_raw,magz_raw);
            /* *magy_raw = - *magy_raw; переходим из левой системы координат,
            которая изображена на магнитометре в правую, для того чтобы 
            положительное направление угла было против часовой стрелки*/
            mag_alpha = atan2(mgy_cal, mgx_cal)/M_PI*180;
            mag_alpha = angle_transformation(mag_alpha, alpha_goal);
            //printf("magx_cal = %d", mgx_cal);
            //printf(" magy_cal = %d", mgy_cal);
            //printf(" magz_cal = %d", mgz_cal);
            printf("mag_alpha = %f", mag_alpha);
        }
        else if (mag_state == 1){
            printf("Fail because of access error, check the connection\n");
        }
        else if (mag_state == 2){
            printf("Fail because of interface error, check your code\n");
        }
 
        if (!hyro_state){
            //float gx_degs = gx_raw * 0.00875;
            //float gy_degs = gy_raw * 0.00875;
            float gz_degs = gz_raw * 0.00875;
            /* если ДУС установлен осью z вверх, то угловая скорость
            // спутника совпадает с показаниями ДУС по оси z, иначе
             необходимо изменить знак: omega = - gz_degs */
            omega = gz_degs;
            //printf("\ngx_degs = %f", gx_degs);
            //printf(" gy_degs = %f", gy_degs);
            printf(" gz_degs = %f\n", gz_degs);
        }
        else if (hyro_state == 1){
            printf("Fail because of access error, check the connection\n");
        }
        else if (hyro_state == 2){
            printf("Fail because of interface error, check your code\n");
        }
 
        //Обработка показаний маховика и установка требуемой угловой скорости.
        if (!mtr_state) {
            // если код ошибки 0, т.е. ошибки нет
            int16_t mtr_speed=0;
            motor_request_speed(mtr_num, &mtr_speed);
            //printf("\nMotor_speed: %d\n", mtr_speed); 
            // установка новой скорости маховика
            mtr_new_speed = motor_new_speed_PD(mtr_speed, mag_alpha, alpha_goal, omega, omega_goal);
            motor_set_speed(mtr_num, mtr_new_speed, &confirm);
        }   
        mSleep(time_step);  
    }
    switch_off_all();
    return 0;
}