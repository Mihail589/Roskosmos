from schsat import *
lastError = 0       # Прошлая ошибка
Integrator = 0      # Интеграл (сумма всех ошибок)
PID = 0         # Величина управляющего воздействия
Integrator_max = 10     # Ограничение максимального значения интеграла 
Integrator_min = -10    # Ограничение минимального значения интеграла 
 
# Функции для определение новой скорости маховика.
# mtr_speed - текущая угловая скорость маховика, об/мин
# omega - текущая угловая скорость спутника, град/с
# omega_goal - целевая угловая скорость спутника, град/с
# mtr_new_speed - требуемая угловая скорость маховика, об/мин
kp = 2.5
ki = 2.5
kd = 2.5
def motor_new_speed_PD(mtr_speed, omega, omega_goal):
    global Integrator
    global lastError
    error = omega - omega_goal                  # Вычисление ошибки
    P = Kp * error                              # Вычисление воздействия пропорционального звена
    D = Kd * ( error - lastError) / time_step   # Вычисление воздействия дифференциального звена
    lastError = error                           # Запоминаем ошибку
    Integrator = Integrator + error * time_step     # Накапливаем суммарную ошибку
    if Integrator > Integrator_max:             # Сатурация (Ограничиваем максимальное значение накапливаемой ошибки)
        Integrator = Integrator_max
    elif Integrator < Integrator_min:
        Integrator = Integrator_min
 
    I = Integrator * Ki             # Вычисление воздействия интегрального звена
    PID = P + I + D             # Вычисление суммарного управляющего воздействия
 
    mtr_new_speed = int(mtr_speed + PID)
    if mtr_new_speed > mtr_max_speed:
        mtr_new_speed = mtr_max_speed
    elif mtr_new_speed < -mtr_max_speed:
        mtr_new_speed = -mtr_max_speed
    print "P= ", P, "I= ", I, "D= ", D, "PID= ", PID, "mtr_new_speed= ", mtr_new_speed  
    return mtr_new_speed

def initall():
	print "On Motor 1"
	motor_turn_on()
	print "On hyro subsystem"
	hyro_turn_on()

def offall():
	print "Off motor 1"
	motor_turn_off()
	print "Off Hyro subsystem"