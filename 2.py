# Коэффициент дифференциальной обратной связи.
# Коэффициент положительный, если маховик расположен осью z вверх
# и ДУС расположен осью z также вверх.
# Коэффициент подбирается экспериментально в зависимости от формы
# и массы вашего спутника.
kd = 100
# Временной шаг работы алгоритма, с
time_step = 0.1
# Целевая угловая скорость спутника, град/с.
# Для режима стабилизации равна 0.0.
omega_goal = 10.0
# Номер маховика
mtr_num = 1
# Номер ДУС (датчика угловой скорости)
hyr_num = 1
 
 
# Функции для определение новой скорости маховика.
# Новая скорость маховика складывается из
# текущей скорости маховика и приращения скорости.
# Приращение скорости пропорционально ошибке по углу
# и ошибке по угловой скорости.
# mtr_speed - текущая угловая скорость маховика, об/мин
# omega - текущая угловая скорость спутника, град/с
# omega_goal - целевая угловая скорость спутника, град/с
# mtr_new_speed - требуемая угловая скорость маховика, об/мин
def motor_new_speed_PD(mtr_speed, omega, omega_goal):
	mtr_new_speed = int(mtr_speed
						+ kd*(omega-omega_goal)
						)
	return mtr_new_speed
 
 
# Функция включает все приборы,
# которые будут использоваться в основной программе.
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
	initialize_all()
	# Инициализируем статус маховика
	mtr_state = 0
	# Инициализируем статус ДУС	
	hyro_state = 0
	
	flag = True
	while flag:
 
		for i in range(600):
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
				print "gx_degs =", gx_degs, \
					"gy_degs =", gy_degs, "gz_degs =", gz_degs
			elif hyro_state == 1:
				print "Fail because of access error, check the connection"
			elif hyro_state == 2:
				print "Fail because of interface error, check your code"
 
		#Обработка показаний маховика и установка трубемой угловой скорости.
			if not mtr_state:	# если код ошибки 0, т.е. ошибки нет
				print "Motor_speed: ", mtr_speed
			# установка новой скорости маховика
				mtr_new_speed = motor_new_speed_PD(mtr_speed,omega,omega_goal)
				motor_set_speed(mtr_num, -mtr_new_speed)
 
			sleep(time_step)
		kd = float(input())
 
	switch_off_all()