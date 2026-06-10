time_step = 0.1     # Временной шаг работы алгоритма, с
omega_goal = 0.0    # Целевая угловая скорость спутника, град/с. Для режима стабилизации равна 0.0.
mtr_num = 1     # Номер маховика
mtr_max_speed = 3000    # Максимально допустимая скорость маховика, об/мин
hyr_num = 1     # Номер ДУС (датчика угловой скорости)
 
error = 0           # Ошибка
P = 0               # Воздействие пропорционального звена
D = 0               # Воздействие дифференциального звена
I = 0               # Воздействие интегрального звена
Kp = 100            # Пропорциональный коэффициент
Kd = 0.02           # Дифференциальный коэффициент
Ki = 0.9            # Интегральный коэффициент
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
 
# Функция включает все приборы, которые будут использоваться в основной программе.
def initialize_all():
    print "Enable motor №", mtr_num 
    motor_turn_on(mtr_num)
    sleep(1)
    print "Enable angular velocity sensor №", hyr_num 
    hyro_turn_on(hyr_num)
    sleep(1)
 
# Функция отключает все приборы,
# которые будут использоваться в основной программе.
def switch_off_all():
    print "Finishing..."
    print "Disable angular velocity sensor №", hyr_num
    hyro_turn_off(hyr_num)
    motor_set_speed(mtr_num, 0)
    sleep (1)
    motor_turn_off(mtr_num)
    print "Finish program"
 
# Основная функция программы, в которой вызываются остальные функции.
def control():
    initialize_all()    # Инициализируем все устройства
    mtr_state = 0       # Инициализируем статус маховика
    hyro_state = 0      # Инициализируем статус ДУС
    omega_goal = 0.0    # Целевой угол поворота
 
    for i in range(100):
        print "i = ", i
 
        # Опрос датчика угловой скорости и маховика.
        hyro_state, gx_raw, gy_raw, gz_raw = hyro_request_raw(hyr_num) 
        mtr_state, mtr_speed = motor_request_speed(mtr_num)
 
        # Обработка показаний датчика угловой скорости,
        # вычисление угловой скорости спутника по показаниям ДУС.
        # Если код ошибки ДУС равен 0, т.е. ошибки нет
        if not hyro_state:
            gx_degs = gx_raw * 0.00875
            gy_degs = gy_raw * 0.00875
            gz_degs = gz_raw * 0.00875
            # если ДУС установлен осью z вверх, то угловая скорость
            # спутника совпадает с показаниями ДУС по оси z, иначе
            # необходимо изменить знак: omega = - gz_degs
			omega = gz_degs
            print "gx_degs =", gx_degs, "gy_degs =", gy_degs, "gz_degs =", gz_degs
        elif hyro_state == 1:
            print "Fail because of access error, check the connection"
        elif hyro_state == 2:
            print "Fail because of interface error, check your code"
 
        #Обработка показаний маховика и установка требуемой угловой скорости.
        if not mtr_state:   # если код ошибки 0, т.е. ошибки нет
            print "Motor_speed: ", mtr_speed
            # установка новой скорости маховика
            mtr_new_speed = motor_new_speed_PD(mtr_speed,omega,omega_goal)
            motor_set_speed(mtr_num, mtr_new_speed)
 
        sleep(time_step)
 
    switch_off_all()