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
L Connector:Conn_01x11_Male J1
U 1 1 5EFE0C5D
P 5400 3850
F 0 "J1" H 5400 4450 50  0000 C CNN
F 1 "Conn_01x11_Male" H 5508 4440 50  0001 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x11_P2.54mm_Vertical" H 5400 3850 50  0001 C CNN
F 3 "~" H 5400 3850 50  0001 C CNN
	1    5400 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 3350 5850 3350
Wire Wire Line
	5600 3450 5850 3450
Wire Wire Line
	5600 3550 5850 3550
Wire Wire Line
	5600 3650 5850 3650
Wire Wire Line
	5600 3750 5850 3750
Wire Wire Line
	5600 3850 5850 3850
Wire Wire Line
	5600 3950 5850 3950
Wire Wire Line
	5600 4050 5850 4050
Wire Wire Line
	5600 4150 5850 4150
Wire Wire Line
	5600 4250 5850 4250
Wire Wire Line
	5600 4350 5850 4350
Text GLabel 5850 4350 2    50   Input ~ 0
AUDIO
Text GLabel 5850 4250 2    50   Output ~ 0
DC
Text GLabel 5850 4150 2    50   Output ~ 0
CS2
Text GLabel 5850 4050 2    50   Output ~ 0
CS1
Text GLabel 5850 3950 2    50   Output ~ 0
MOSI
Text GLabel 5850 3850 2    50   Input ~ 0
MISO
Text GLabel 5850 3750 2    50   Output ~ 0
CLK
$Comp
L power:GND #PWR015
U 1 1 5EFF5A7A
P 5850 3650
F 0 "#PWR015" H 5850 3400 50  0001 C CNN
F 1 "GND" V 5850 3550 50  0000 R CNN
F 2 "" H 5850 3650 50  0001 C CNN
F 3 "" H 5850 3650 50  0001 C CNN
	1    5850 3650
	0    -1   -1   0   
$EndComp
$Comp
L power:+5V #PWR014
U 1 1 5EFF5C1B
P 5850 3550
F 0 "#PWR014" H 5850 3400 50  0001 C CNN
F 1 "+5V" V 5850 3650 50  0000 L CNN
F 2 "" H 5850 3550 50  0001 C CNN
F 3 "" H 5850 3550 50  0001 C CNN
	1    5850 3550
	0    1    1    0   
$EndComp
Text GLabel 5850 3450 2    50   Output ~ 0
GND12V
Text GLabel 5850 3350 2    50   Output ~ 0
12V
$EndSCHEMATC
