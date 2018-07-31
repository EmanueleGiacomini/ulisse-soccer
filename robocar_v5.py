#------ROBOCAR VISION CODE-----
# V5.1
# Image stream, line detect
# CNN Recog, 
# Blob for signal detect in HSV
# Serial to an Arduino
#------------------------------
# USAGE
# python robocar_v5.py --model four_classes.hdf5 --serial -1

from keras.models import load_model
from imutils import paths

import cv2
import numpy as np
import time
from math import trunc
from math import atan2
from math import degrees
import argparse
import serial

################################
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.OUT) # STOP
GPIO.setup(18, GPIO.OUT) # DX
GPIO.setup(27, GPIO.OUT) # SX

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-m", "--model", required=True,help="path to pre-trained model")
args = vars(ap.parse_args())

# ADDRESS = '/dev/ttyACM0'

# #if (args["serial"] > '0') :
# uno = serial.Serial(ADDRESS, 115200, timeout = 3)  # open serial port
# print("[INFO] using", uno.name) # check which port was really used 

#########TRIM EVERY TIME#######
# define the list of boundaries
# CAUTION WE'RE WORKING IN HSV

red_lower = (140, 50, 50)
red_upper = (179, 255, 255)

blue_lower = (110,50,50) #110,50,50
blue_upper = (130,255,255) #130,255,255

minLineLength = 80
maxLineGap = 10

############################################

# initialize the class labels
classLabels = ["dx","none","stop","sx"]
dataSend = ['2','1','0','3']
classPrint = ["DX","NONE","STOP","SX"]
data = '1'

# load the pre-trained network
print("[INFO] loading pre-trained network...")
model = load_model(args["model"])

############################################

cap = cv2.VideoCapture(0)

#--------------VARS------------

font = cv2.FONT_HERSHEY_SIMPLEX # simple font
fps = None # numeric var for fps
milliseconds = 0.0

red_detected = False
blue_detected = False

#------------------------------

while(1) :

	start = time.clock()
	
	# Take each frame
	_, frame = cap.read() 
	#frame = cv2.flip(frame, flipCode=-1) # remove if necessary
	frame = cv2.resize(frame, (300, 200), interpolation=cv2.INTER_LINEAR)
	#cv2.imshow("unprocessed",frame)
	blobs = frame.copy()
	blobs = cv2.cvtColor(blobs, cv2.COLOR_BGR2HSV)

	#---------------LINES-------------

	gray = frame.copy()
	gray = gray[100:200,0:300] #y1 to y2, x1 to x2
	#cv2.imshow('half', gray)
	gray = cv2.cvtColor(gray,cv2.COLOR_BGR2GRAY)
	canned = cv2.Canny(gray,100,100)
	lines = cv2.HoughLinesP(canned,1,np.pi/180,100,minLineLength,maxLineGap)

	if lines is not None : # security measure to prevent block
		for x1,y1,x2,y2 in lines[0] :
			cv2.line(frame,(x1,y1+100),(x2,y2+100),(255,0,255),2)

			# calc of relative coordinates
			X = x2 - x1
			Y = y2 - y1
			alpha = atan2(X, Y)	# atan 2
			alpha = degrees(alpha) # convert to deg
			alpha = trunc(alpha) # truncate 
			alpha = str(alpha) + " deg." # save string
	
	#----------BLOBS&CNN--------------

	red_mask = cv2.inRange(blobs, red_lower, red_upper)
	red_mask = cv2.erode(red_mask, None, iterations=2)
	red_mask = cv2.dilate(red_mask, None, iterations=2)
	red_cnts = cv2.findContours(red_mask, cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)[-2]

	blue_mask = cv2.inRange(blobs, blue_lower, blue_upper)
	blue_mask = cv2.erode(blue_mask, None, iterations=2)
	blue_mask = cv2.dilate(blue_mask, None, iterations=2)
	blue_cnts = cv2.findContours(blue_mask, cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)[-2]

	img = [] # create array
	#img = blobs

	if (len(red_cnts) > 0) :

		red_c = max(red_cnts, key=cv2.contourArea)
		red_x,red_y,red_w,red_h = cv2.boundingRect(red_c)

		if (red_w >= 20) and (red_h >= 20) :
			cv2.rectangle(frame,(red_x,red_y),(red_x+red_w,red_y+red_h),(0,255,0),2)
			# crop from frame instead of crop from blobs
			red_blobs = frame[red_y:red_y+red_h, red_x:red_x+red_w] # img[y: y + h, x: x + w]
			img = cv2.resize(red_blobs, (32, 32), interpolation=cv2.INTER_LINEAR) #resize
			red_detected = True
		else : red_detected = False
	else : red_detected = False

	if (len(blue_cnts) > 0) :

		blue_c = max(blue_cnts, key=cv2.contourArea)
		blue_x,blue_y,blue_w,blue_h = cv2.boundingRect(blue_c)

		if (blue_w >= 20) and (blue_h >= 20) :
			cv2.rectangle(frame,(blue_x,blue_y),(blue_x+blue_w,blue_y+blue_h),(0,255,0),2)
			# crop from frame instead of crop from blobs
			blue_blobs = frame[blue_y:blue_y+blue_h, blue_x:blue_x+blue_w] # img[y: y + h, x: x + w]
			img = cv2.resize(blue_blobs, (32, 32), interpolation=cv2.INTER_LINEAR) #resize
			blue_detected = True
		else : blue_detected = False
	else : blue_detected = False

	if (red_detected == True) or (blue_detected == True) :

		############################################

		cv2.imshow("cropped", img)
		img = img.astype("float") / 255.0 # scale values
		img = np.expand_dims(img, axis=0) # add a dimension

		# make predictions on the images
		print("\n[INFO] predicting...")
		preds_max = model.predict(img, batch_size=32).argmax(axis=1)
		preds = model.predict(img, batch_size=32)
		preds_max = int(preds_max) # convert to int
		dx_pred, none_pred, stop_pred, sx_pred = preds[0]
		
		#print("DX: %.2f" % dx_pred," STOP: %.2f" % stop_pred," SX: %.2f" % sx_pred)
		infos = 'DX: %.2f' % dx_pred+' STOP: %.2f' % stop_pred+' SX: %.2f' % sx_pred+' None: %.2f' % none_pred
		infos = str(infos)
		print(infos)
		print("Predicted signal: ",classLabels[preds_max])

		cv2.putText(frame,str(classPrint[preds_max]),(100,190), font, 0.5,(0,255,0),1,cv2.LINE_AA)

		data = dataSend[preds_max]
		#data_send = chr(data)
		#print("Sended: ",data)

	#else : 
		#data = '1'
		#print("Sended: ",data)

		############################################

	# #------------SERIAL-----------

	# code_readed = uno.read()   

	# if code_readed == '1' : # if the code is 01 then send alpha value
	# 	data_send = str(data)
	# 	print(type(data_send))
	# 	uno.write(data_send)

	# #-----------------------------

		if (classLabels[preds_max] == "stop") :
			GPIO.output(17, GPIO.LOW)
			GPIO.output(18, GPIO.HIGH)
			GPIO.output(27, GPIO.HIGH)
			print("STOP on pin 17")

		if (classLabels[preds_max] == "dx") :
			GPIO.output(17, GPIO.HIGH)
			GPIO.output(18, GPIO.LOW)
			GPIO.output(27, GPIO.HIGH)
			print("DX on pin 18")

		if (classLabels[preds_max] == "sx") :
			GPIO.output(17, GPIO.HIGH)
			GPIO.output(18, GPIO.HIGH)
			GPIO.output(27, GPIO.LOW)
			print("SX on pin 27")

		if (classLabels[preds_max] == "none") :
			GPIO.output(17, GPIO.HIGH)
			GPIO.output(18, GPIO.HIGH)
			GPIO.output(27, GPIO.HIGH)
			print("None")

##	print("None")
##	GPIO.output(17, GPIO.HIGH)
##	GPIO.output(18, GPIO.HIGH)
##	GPIO.output(27, GPIO.HIGH)

	framerate = "FPS: " + str(fps) # framerate string
	time_elapsed = str("Pr: " + "%.2f" % milliseconds) + " ms"
	print(framerate)
	cv2.putText(frame,framerate,(10,190), font, 0.5,(255,255,255),1,cv2.LINE_AA)
	
	cv2.imshow("raw", frame)
	cv2.imshow("reds", red_mask)
	cv2.imshow("blues", blue_mask)
	cv2.imshow("canny", canned)

	# fps calc stuff
	stop = time.clock()

	fps = 1 / (stop - start)
	milliseconds = (stop - start) * 1000
	fps = trunc(fps)

	key = cv2.waitKey(1) & 0xFF

	if key == ord("q"):
		break

cap.release()
cv2.destroyAllWindows()