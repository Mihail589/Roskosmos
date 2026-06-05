import argparse 
import os, zipfile, shutil
import subprocess
from subprocess import Popen, PIPE
import string
def gen(file):
	shutil.copy(file, file.split(".py")[0])
	print("\r\nMaking skeleton...\r\n") 

	prog = ""
	#notepad.activateFile("main")
	prog += "#! /bin/bash\n"; 
	prog += 'cd "$(dirname "${0}")"\n';
	prog += 'TIFS=$IFS\n';
	prog += "IFS='"'&'"'\n"
	prog += 'OPTIONS=($QUERY_STRING)\n'
	prog += 'IFS=$TIFS\n'
	prog += "tty="'"${1:-/dev/ttyUSB0}"'"\n"
	prog += "python ./%s -t 5 -f ftp://192.168.42.42/ ${OPTIONS[@]} $tty 2>&1\n"% file.split(".py")[0]
	with open('main', "w") as f:
		f.write(prog)
	

	#start copied to short_filename
	outp = open(file.split(".py")[0], "a")
	inp = open("main_start", "r")
	outp.write (inp.read())
	#contents appended to short_filename (without ext)
	
	inp = open('main_end', "r")	
	outp.write (inp.read())
	outp.close()
	inp.close()
	
	print("Skeleton Ok!\r\n") 

		
	print('\r\nStarting ZIP\r\n')
	with zipfile.ZipFile(f'{file.split(".py")[0]}.zip', 'w') as zf:
		zf.write("main")
		zf.write(file.split(".py")[0])
	print("ZIP Ok!\r\n")
		
	print("\r\nCleaning\r\n")
	os.remove('main')
	os.remove(file.split(".py")[0])
parser = argparse.ArgumentParser(description = "Скрипт для подготвки архива для спутника")
parser.add_argument("filename", help = "Основной файл")
args = parser.parse_args()
gen(args.filename)


	

# Создаём новый архив (режим 'w')
