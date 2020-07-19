EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 8 8
Title "FT81x TFT040 Driver Board"
Date "2020-07-19"
Rev "2007E"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:TestPoint TP1
U 1 1 5F030E9E
P 5250 2650
F 0 "TP1" V 5250 3000 50  0000 C CNN
F 1 "TestPoint" V 5354 2722 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 2650 50  0001 C CNN
F 3 "~" H 5450 2650 50  0001 C CNN
	1    5250 2650
	0    -1   -1   0   
$EndComp
$Comp
L power:+12V #PWR068
U 1 1 5F0317D1
P 5250 2650
F 0 "#PWR068" H 5250 2500 50  0001 C CNN
F 1 "+12V" V 5250 2750 50  0000 L CNN
F 2 "" H 5250 2650 50  0001 C CNN
F 3 "" H 5250 2650 50  0001 C CNN
	1    5250 2650
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP3
U 1 1 5F0327EC
P 5250 2800
F 0 "TP3" V 5250 3150 50  0000 C CNN
F 1 "TestPoint" V 5354 2872 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 2800 50  0001 C CNN
F 3 "~" H 5450 2800 50  0001 C CNN
	1    5250 2800
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR070
U 1 1 5F032C4C
P 5250 2800
F 0 "#PWR070" H 5250 2650 50  0001 C CNN
F 1 "+3.3V" V 5250 2900 50  0000 L CNN
F 2 "" H 5250 2800 50  0001 C CNN
F 3 "" H 5250 2800 50  0001 C CNN
	1    5250 2800
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP5
U 1 1 5F03331D
P 5250 2950
F 0 "TP5" V 5250 3300 50  0000 C CNN
F 1 "TestPoint" V 5354 3022 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 2950 50  0001 C CNN
F 3 "~" H 5450 2950 50  0001 C CNN
	1    5250 2950
	0    -1   -1   0   
$EndComp
$Comp
L power:+3V0 #PWR072
U 1 1 5F033611
P 5250 2950
F 0 "#PWR072" H 5250 2800 50  0001 C CNN
F 1 "+3V0" V 5250 3050 50  0000 L CNN
F 2 "" H 5250 2950 50  0001 C CNN
F 3 "" H 5250 2950 50  0001 C CNN
	1    5250 2950
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP2
U 1 1 5F035E62
P 6300 2650
F 0 "TP2" V 6300 3000 50  0000 C CNN
F 1 "TestPoint" V 6404 2722 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.0x1.0mm" H 6500 2650 50  0001 C CNN
F 3 "~" H 6500 2650 50  0001 C CNN
	1    6300 2650
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR069
U 1 1 5F0362A5
P 6300 2650
F 0 "#PWR069" H 6300 2400 50  0001 C CNN
F 1 "GND" V 6300 2500 50  0000 R CNN
F 2 "" H 6300 2650 50  0001 C CNN
F 3 "" H 6300 2650 50  0001 C CNN
	1    6300 2650
	0    -1   -1   0   
$EndComp
$Comp
L Connector:TestPoint TP4
U 1 1 5F0363CB
P 6300 2800
F 0 "TP4" V 6300 3150 50  0000 C CNN
F 1 "TestPoint" V 6404 2872 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.0x1.0mm" H 6500 2800 50  0001 C CNN
F 3 "~" H 6500 2800 50  0001 C CNN
	1    6300 2800
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR071
U 1 1 5F0365F8
P 6300 2800
F 0 "#PWR071" H 6300 2550 50  0001 C CNN
F 1 "GND" V 6300 2650 50  0000 R CNN
F 2 "" H 6300 2800 50  0001 C CNN
F 3 "" H 6300 2800 50  0001 C CNN
	1    6300 2800
	0    -1   -1   0   
$EndComp
$Comp
L Connector:TestPoint TP6
U 1 1 5F042349
P 5250 3200
F 0 "TP6" V 5250 3550 50  0000 C CNN
F 1 "TestPoint" V 5354 3272 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 3200 50  0001 C CNN
F 3 "~" H 5450 3200 50  0001 C CNN
	1    5250 3200
	0    -1   -1   0   
$EndComp
Text GLabel 5250 3200 2    50   UnSpc ~ 0
SCK_3V
$Comp
L Connector:TestPoint TP8
U 1 1 5F042350
P 5250 3350
F 0 "TP8" V 5250 3700 50  0000 C CNN
F 1 "TestPoint" V 5354 3422 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 3350 50  0001 C CNN
F 3 "~" H 5450 3350 50  0001 C CNN
	1    5250 3350
	0    -1   -1   0   
$EndComp
Text GLabel 5250 3350 2    50   UnSpc ~ 0
CS1_3V
$Comp
L Connector:TestPoint TP10
U 1 1 5F042357
P 5250 3500
F 0 "TP10" V 5250 3850 50  0000 C CNN
F 1 "TestPoint" V 5354 3572 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 3500 50  0001 C CNN
F 3 "~" H 5450 3500 50  0001 C CNN
	1    5250 3500
	0    -1   -1   0   
$EndComp
Text GLabel 5250 3500 2    50   UnSpc ~ 0
CS2_3V
$Comp
L Connector:TestPoint TP12
U 1 1 5F04235E
P 5250 3650
F 0 "TP12" V 5250 4000 50  0000 C CNN
F 1 "TestPoint" V 5354 3722 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 3650 50  0001 C CNN
F 3 "~" H 5450 3650 50  0001 C CNN
	1    5250 3650
	0    -1   -1   0   
$EndComp
Text GLabel 5250 3650 2    50   UnSpc ~ 0
DC_3V
$Comp
L Connector:TestPoint TP14
U 1 1 5F042365
P 5250 3800
F 0 "TP14" V 5250 4150 50  0000 C CNN
F 1 "TestPoint" V 5354 3872 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 3800 50  0001 C CNN
F 3 "~" H 5450 3800 50  0001 C CNN
	1    5250 3800
	0    -1   -1   0   
$EndComp
Text GLabel 5250 3800 2    50   UnSpc ~ 0
SDI_3V
$Comp
L Connector:TestPoint TP16
U 1 1 5F04236C
P 5250 3950
F 0 "TP16" V 5250 4300 50  0000 C CNN
F 1 "TestPoint" V 5354 4022 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 3950 50  0001 C CNN
F 3 "~" H 5450 3950 50  0001 C CNN
	1    5250 3950
	0    -1   -1   0   
$EndComp
Text GLabel 5250 3950 2    50   UnSpc ~ 0
SDO_3V
$Comp
L Connector:TestPoint TP7
U 1 1 5F04BDAF
P 6300 3200
F 0 "TP7" V 6300 3550 50  0000 C CNN
F 1 "TestPoint" V 6404 3272 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 6500 3200 50  0001 C CNN
F 3 "~" H 6500 3200 50  0001 C CNN
	1    6300 3200
	0    -1   -1   0   
$EndComp
Text GLabel 6300 3200 2    50   UnSpc ~ 0
DE
$Comp
L Connector:TestPoint TP9
U 1 1 5F04C63C
P 6300 3350
F 0 "TP9" V 6300 3700 50  0000 C CNN
F 1 "TestPoint" V 6404 3422 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 6500 3350 50  0001 C CNN
F 3 "~" H 6500 3350 50  0001 C CNN
	1    6300 3350
	0    -1   -1   0   
$EndComp
Text GLabel 6300 3350 2    50   UnSpc ~ 0
HSYNC
$Comp
L Connector:TestPoint TP11
U 1 1 5F04CB5A
P 6300 3500
F 0 "TP11" V 6300 3850 50  0000 C CNN
F 1 "TestPoint" V 6404 3572 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 6500 3500 50  0001 C CNN
F 3 "~" H 6500 3500 50  0001 C CNN
	1    6300 3500
	0    -1   -1   0   
$EndComp
Text GLabel 6300 3500 2    50   UnSpc ~ 0
VSYNC
$Comp
L Connector:TestPoint TP13
U 1 1 5F04CF0A
P 6300 3650
F 0 "TP13" V 6300 4000 50  0000 C CNN
F 1 "TestPoint" V 6404 3722 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 6500 3650 50  0001 C CNN
F 3 "~" H 6500 3650 50  0001 C CNN
	1    6300 3650
	0    -1   -1   0   
$EndComp
Text GLabel 6300 3650 2    50   UnSpc ~ 0
PCLK
$Comp
L Connector:TestPoint TP15
U 1 1 5F04D431
P 6300 3800
F 0 "TP15" V 6300 4150 50  0000 C CNN
F 1 "TestPoint" V 6404 3872 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 6500 3800 50  0001 C CNN
F 3 "~" H 6500 3800 50  0001 C CNN
	1    6300 3800
	0    -1   -1   0   
$EndComp
Text GLabel 6300 3800 2    50   UnSpc ~ 0
DISPRES
$Comp
L Connector:TestPoint TP17
U 1 1 5F0486C1
P 6300 3950
F 0 "TP17" V 6300 4300 50  0000 C CNN
F 1 "TestPoint" V 6404 4022 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 6500 3950 50  0001 C CNN
F 3 "~" H 6500 3950 50  0001 C CNN
	1    6300 3950
	0    -1   -1   0   
$EndComp
Text GLabel 6300 3950 2    50   UnSpc ~ 0
FTRES
$Comp
L Connector:TestPoint TP18
U 1 1 5F15817D
P 5250 4100
F 0 "TP18" V 5250 4450 50  0000 C CNN
F 1 "TestPoint" V 5354 4172 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D0.5mm" H 5450 4100 50  0001 C CNN
F 3 "~" H 5450 4100 50  0001 C CNN
	1    5250 4100
	0    -1   -1   0   
$EndComp
Text GLabel 5250 4100 2    50   UnSpc ~ 0
IRQ_3V
$EndSCHEMATC
