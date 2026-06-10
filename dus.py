def control():
	if hyro_turn_on(1):
		print "Ошибка"
	else:
		sleep(2)
		print(hyro_request_raw(1))
		hyro_turn_off(1) 