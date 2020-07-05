EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 6
Title "FT81x TFT040 Driver Board"
Date "2020-07-03"
Rev "2007D"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Conn_01x10_Male J1
U 1 1 5F0472A1
P 5450 3800
F 0 "J1" H 5558 4289 50  0000 C CNN
F 1 "Conn_01x10_Male" H 5558 4290 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x10_P2.54mm_Vertical" H 5450 3800 50  0001 C CNN
F 3 "~" H 5450 3800 50  0001 C CNN
	1    5450 3800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR026
U 1 1 5F04918A
P 5650 3400
F 0 "#PWR026" H 5650 3250 50  0001 C CNN
F 1 "+5V" V 5650 3500 50  0000 L CNN
F 2 "" H 5650 3400 50  0001 C CNN
F 3 "" H 5650 3400 50  0001 C CNN
	1    5650 3400
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR027
U 1 1 5F0493A4
P 5650 3500
F 0 "#PWR027" H 5650 3350 50  0001 C CNN
F 1 "VCC" V 5650 3600 50  0000 L CNN
F 2 "" H 5650 3500 50  0001 C CNN
F 3 "" H 5650 3500 50  0001 C CNN
	1    5650 3500
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR028
U 1 1 5F04A2D5
P 5650 3600
F 0 "#PWR028" H 5650 3350 50  0001 C CNN
F 1 "GND" V 5650 3500 50  0000 R CNN
F 2 "" H 5650 3600 50  0001 C CNN
F 3 "" H 5650 3600 50  0001 C CNN
	1    5650 3600
	0    -1   -1   0   
$EndComp
Text GLabel 5650 3700 2    50   Output ~ 0
CLK
Text GLabel 5650 3800 2    50   Input ~ 0
MISO
Text GLabel 5650 3900 2    50   Output ~ 0
MOSI
Text GLabel 5650 4000 2    50   Output ~ 0
CS1
Text GLabel 5650 4100 2    50   Output ~ 0
CS2
Text GLabel 5650 4200 2    50   Output ~ 0
DC
Text GLabel 5650 4300 2    50   Input ~ 0
AUDIO
$EndSCHEMATC
