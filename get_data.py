import json
import logging
import math
import random
import sys
import time
import os
import requests as req

def login():
	para={'email':'sejal.khatri5@gmail.com','password':'tapitdown'}
	res=req.post('https://api.datonis.io/api_sign_in',params=para)
	jsonStr=res.json()
	auth_token=jsonStr.get('auth_token')
	
	if not auth_token:
		print "login failed...."
	return auth_token


def getData(thing_key):
	auth_token=login()
	para={"thing_key":thing_key, "from":"2016/07/9 21:00:00", "to":"2016/07/10 21:00:00", "time_zone":"UTC" , "time_format":"str", "per":"10"}
	header={"X-Auth-Token":auth_token}
	res=req.post( 'https://api.datonis.io/api/v3/datonis_query/thing_data',params=para,headers=header)
	return res.content

x = getData("693fba9982")

print x




  


