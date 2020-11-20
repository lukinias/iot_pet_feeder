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
		
		try:
			balance = api.get_balances(addresses=[config_data['channel']])['balances'][0]
		except Exception as e:
			print(f'Exception ocurred - Keepy not found: {e}')
			
		if balance > global_balance:
			print("-------------------------------------------")
			print(f"The new balance is {balance} iotas.")
			data = {"iot2tangle":[{"sensor":"Food","data":[{"cash":str(balance)}]}],"device": config_data['device_id'],"timestamp": str(ts)}
			global_balance = balance
			
			try:
				response = requests.post(f"{config_data['keepy_address']}/messages", json=data)
			except Exception as e:
				print(f'Exception ocurred: {e}')
				
			if response.status_code == 201:
				print(f"Sending data to the tangle OK.")
			else:
				print(f"Sending data to the tangle Error (code: {response.status_code})")
		time.sleep(10)

cartel()
process_balance()
