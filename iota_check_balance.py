#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  iota_check_balance.py

from iota import Iota
import requests
import calendar
import time; 
import json

# gmt stores current gmtime 
gmt = time.gmtime() 
  
# ts stores timestamp 
ts = calendar.timegm(gmt) 

api = Iota('https://nodes.iota.cafe:443')

#######################################################
# Print wellcome message
#######################################################
def cartel():
	print("""
  _____ ____ _______         _____     _     ______            _           
 |_   _/ __ \__   __|/\     |  __ \   | |   |  ____|          | |          
   | || |  | | | |  /  \    | |__) |__| |_  | |__ ___  ___  __| | ___ _ __ 
   | || |  | | | | / /\ \   |  ___/ _ \ __| |  __/ _ \/ _ \/ _` |/ _ \ '__|
  _| || |__| | | |/ ____ \  | |  |  __/ |_  | | |  __/  __/ (_| |  __/ |   
 |_____\____/  |_/_/    \_\ |_|   \___|\__| |_|  \___|\___|\__,_|\___|_|   
 
 Press ctrl + c to exit.
""")

def process_balance():
	global_balance = 0.0
	while True:
		with open("config.json", "r") as readfile:
			config_data = json.load(readfile)
		balance = api.get_balances(addresses=[config_data['channel']])['balances'][0]
		if balance > global_balance:
			print("-------------------------------------------")
			print(f"A new balance is {balance} iotas. Sending data to the tangle.")
			data = {"iot2tangle":[{"sensor":"Food","data":[{"cash":str(balance)}]}],"device": "ESP32-PETFEEDER","timestamp": str(ts)}
			global_balance = balance
			response = requests.post('http://127.0.0.1:3002/messages', json=data)
			if response.status_code == 201:
				print(f"Response code: {response.status_code} - OK")
			else:
				print(f"Response code: {response.status_code} - Error")
		time.sleep(10)

cartel()
process_balance()
