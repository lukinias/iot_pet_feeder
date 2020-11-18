#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  iota_check_balance.py

from iota import Iota
import requests
import calendar
import time; 
  
# gmt stores current gmtime 
gmt = time.gmtime() 
  
# ts stores timestamp 
ts = calendar.timegm(gmt) 

api = Iota('https://nodes.iota.cafe:443')
global_balance = 0.0

while True:
	
	balance = api.get_balances(addresses=['XOWGPCTLGXHHEFQXJIAAXXRKDJBQYEYIFEDTTNJJOVGLFAWYDKCLTKJQBADZPYGMIPHTLMCTN9TM9LZICOIOQMQACW'])['balances'][0]
	
	if balance > global_balance:
		print("-------------------------------------------")
		print(f"A new balance is {balance} iotas")
		print(f"Sending data to the tangle.")
		data = {"iot2tangle":[{"sensor":"Food","data":[{"cash":str(balance)}]}],"device": "ESP32-PETFEEDER","timestamp": str(ts)}
		response = requests.post('http://127.0.0.1:3002/messages', json=data)
		global_balance = balance
		print(f"Response code: {response.status_code}")
		print("-------------------------------------------")

	time.sleep(10)
