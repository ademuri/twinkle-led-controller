EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 11
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L homebrew:HT7K1201 U4
U 1 1 5F5EEE19
P 2600 1800
AR Path="/5F5EB333/5F5EEE19" Ref="U4"  Part="1" 
AR Path="/5F66E22A/5F5EEE19" Ref="U?"  Part="1" 
AR Path="/5F685DB4/5F5EEE19" Ref="U5"  Part="1" 
AR Path="/5F69405E/5F5EEE19" Ref="U6"  Part="1" 
AR Path="/5F694066/5F5EEE19" Ref="U7"  Part="1" 
AR Path="/5F69B53B/5F5EEE19" Ref="U8"  Part="1" 
AR Path="/5F6A41A2/5F5EEE19" Ref="U9"  Part="1" 
AR Path="/5F6A41AA/5F5EEE19" Ref="U10"  Part="1" 
AR Path="/5F6A41B2/5F5EEE19" Ref="U11"  Part="1" 
AR Path="/5F6A41BA/5F5EEE19" Ref="U12"  Part="1" 
AR Path="/5F6A41C2/5F5EEE19" Ref="U13"  Part="1" 
F 0 "U4" H 2300 2100 50  0000 C CNN
F 1 "HT7K1201" H 2850 2100 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 2150 1450 50  0001 C CNN
F 3 "https://www.holtek.com/documents/10179/116711/HT7K1201v140.pdf" H 2150 1450 50  0001 C CNN
F 4 "C182041" H 2400 2100 50  0001 C CNN "LCSC"
	1    2600 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J3
U 1 1 5F5EEE1F
P 3750 1750
AR Path="/5F5EB333/5F5EEE1F" Ref="J3"  Part="1" 
AR Path="/5F66E22A/5F5EEE1F" Ref="J?"  Part="1" 
AR Path="/5F685DB4/5F5EEE1F" Ref="J4"  Part="1" 
AR Path="/5F69405E/5F5EEE1F" Ref="J5"  Part="1" 
AR Path="/5F694066/5F5EEE1F" Ref="J6"  Part="1" 
AR Path="/5F69B53B/5F5EEE1F" Ref="J7"  Part="1" 
AR Path="/5F6A41A2/5F5EEE1F" Ref="J8"  Part="1" 
AR Path="/5F6A41AA/5F5EEE1F" Ref="J9"  Part="1" 
AR Path="/5F6A41B2/5F5EEE1F" Ref="J10"  Part="1" 
AR Path="/5F6A41BA/5F5EEE1F" Ref="J11"  Part="1" 
AR Path="/5F6A41C2/5F5EEE1F" Ref="J12"  Part="1" 
F 0 "J3" H 3830 1742 50  0000 L CNN
F 1 "Screw_Terminal_01x02" H 3500 1550 50  0000 L CNN
F 2 "homebrew:Wurth_ScrewTerminal_1x02_P5.00mm_691137710002" H 3750 1750 50  0001 C CNN
F 3 "~" H 3750 1750 50  0001 C CNN
	1    3750 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 1900 3300 1900
Wire Wire Line
	3300 1900 3300 1850
Wire Wire Line
	3300 1850 3550 1850
$Comp
L power:GND #PWR0102
U 1 1 5F5EEE28
P 2600 2250
AR Path="/5F5EB333/5F5EEE28" Ref="#PWR0102"  Part="1" 
AR Path="/5F66E22A/5F5EEE28" Ref="#PWR?"  Part="1" 
AR Path="/5F685DB4/5F5EEE28" Ref="#PWR027"  Part="1" 
AR Path="/5F69405E/5F5EEE28" Ref="#PWR030"  Part="1" 
AR Path="/5F694066/5F5EEE28" Ref="#PWR033"  Part="1" 
AR Path="/5F69B53B/5F5EEE28" Ref="#PWR036"  Part="1" 
AR Path="/5F6A41A2/5F5EEE28" Ref="#PWR039"  Part="1" 
AR Path="/5F6A41AA/5F5EEE28" Ref="#PWR042"  Part="1" 
AR Path="/5F6A41B2/5F5EEE28" Ref="#PWR045"  Part="1" 
AR Path="/5F6A41BA/5F5EEE28" Ref="#PWR048"  Part="1" 
AR Path="/5F6A41C2/5F5EEE28" Ref="#PWR051"  Part="1" 
F 0 "#PWR0102" H 2600 2000 50  0001 C CNN
F 1 "GND" H 2605 2077 50  0000 C CNN
F 2 "" H 2600 2250 50  0001 C CNN
F 3 "" H 2600 2250 50  0001 C CNN
	1    2600 2250
	1    0    0    -1  
$EndComp
$Comp
L Device:C C8
U 1 1 5F5EEE31
P 1400 1550
AR Path="/5F5EB333/5F5EEE31" Ref="C8"  Part="1" 
AR Path="/5F66E22A/5F5EEE31" Ref="C?"  Part="1" 
AR Path="/5F685DB4/5F5EEE31" Ref="C9"  Part="1" 
AR Path="/5F69405E/5F5EEE31" Ref="C10"  Part="1" 
AR Path="/5F694066/5F5EEE31" Ref="C11"  Part="1" 
AR Path="/5F69B53B/5F5EEE31" Ref="C12"  Part="1" 
AR Path="/5F6A41A2/5F5EEE31" Ref="C13"  Part="1" 
AR Path="/5F6A41AA/5F5EEE31" Ref="C14"  Part="1" 
AR Path="/5F6A41B2/5F5EEE31" Ref="C15"  Part="1" 
AR Path="/5F6A41BA/5F5EEE31" Ref="C16"  Part="1" 
AR Path="/5F6A41C2/5F5EEE31" Ref="C17"  Part="1" 
F 0 "C8" H 1515 1596 50  0000 L CNN
F 1 "0.1uF" H 1515 1505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 1438 1400 50  0001 C CNN
F 3 "~" H 1400 1550 50  0001 C CNN
F 4 "C1525" H 1400 1550 50  0001 C CNN "LCSC"
	1    1400 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 1350 2600 1350
$Comp
L power:GND #PWR0111
U 1 1 5F5EEE38
P 1400 1700
AR Path="/5F5EB333/5F5EEE38" Ref="#PWR0111"  Part="1" 
AR Path="/5F66E22A/5F5EEE38" Ref="#PWR?"  Part="1" 
AR Path="/5F685DB4/5F5EEE38" Ref="#PWR025"  Part="1" 
AR Path="/5F69405E/5F5EEE38" Ref="#PWR028"  Part="1" 
AR Path="/5F694066/5F5EEE38" Ref="#PWR031"  Part="1" 
AR Path="/5F69B53B/5F5EEE38" Ref="#PWR034"  Part="1" 
AR Path="/5F6A41A2/5F5EEE38" Ref="#PWR037"  Part="1" 
AR Path="/5F6A41AA/5F5EEE38" Ref="#PWR040"  Part="1" 
AR Path="/5F6A41B2/5F5EEE38" Ref="#PWR043"  Part="1" 
AR Path="/5F6A41BA/5F5EEE38" Ref="#PWR046"  Part="1" 
AR Path="/5F6A41C2/5F5EEE38" Ref="#PWR049"  Part="1" 
F 0 "#PWR0111" H 1400 1450 50  0001 C CNN
F 1 "GND" H 1405 1527 50  0000 C CNN
F 2 "" H 1400 1700 50  0001 C CNN
F 3 "" H 1400 1700 50  0001 C CNN
	1    1400 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 1400 1400 1350
$Comp
L power:+5V #PWR0112
U 1 1 5F5EEE3F
P 2600 1350
AR Path="/5F5EB333/5F5EEE3F" Ref="#PWR0112"  Part="1" 
AR Path="/5F66E22A/5F5EEE3F" Ref="#PWR?"  Part="1" 
AR Path="/5F685DB4/5F5EEE3F" Ref="#PWR026"  Part="1" 
AR Path="/5F69405E/5F5EEE3F" Ref="#PWR029"  Part="1" 
AR Path="/5F694066/5F5EEE3F" Ref="#PWR032"  Part="1" 
AR Path="/5F69B53B/5F5EEE3F" Ref="#PWR035"  Part="1" 
AR Path="/5F6A41A2/5F5EEE3F" Ref="#PWR038"  Part="1" 
AR Path="/5F6A41AA/5F5EEE3F" Ref="#PWR041"  Part="1" 
AR Path="/5F6A41B2/5F5EEE3F" Ref="#PWR044"  Part="1" 
AR Path="/5F6A41BA/5F5EEE3F" Ref="#PWR047"  Part="1" 
AR Path="/5F6A41C2/5F5EEE3F" Ref="#PWR050"  Part="1" 
F 0 "#PWR0112" H 2600 1200 50  0001 C CNN
F 1 "+5V" H 2700 1450 50  0000 C CNN
F 2 "" H 2600 1350 50  0001 C CNN
F 3 "" H 2600 1350 50  0001 C CNN
	1    2600 1350
	1    0    0    -1  
$EndComp
Connection ~ 2600 1350
$Comp
L Device:R R4
U 1 1 5F5EEE47
P 3350 1750
AR Path="/5F5EB333/5F5EEE47" Ref="R4"  Part="1" 
AR Path="/5F66E22A/5F5EEE47" Ref="R?"  Part="1" 
AR Path="/5F685DB4/5F5EEE47" Ref="R5"  Part="1" 
AR Path="/5F69405E/5F5EEE47" Ref="R6"  Part="1" 
AR Path="/5F694066/5F5EEE47" Ref="R7"  Part="1" 
AR Path="/5F69B53B/5F5EEE47" Ref="R8"  Part="1" 
AR Path="/5F6A41A2/5F5EEE47" Ref="R9"  Part="1" 
AR Path="/5F6A41AA/5F5EEE47" Ref="R10"  Part="1" 
AR Path="/5F6A41B2/5F5EEE47" Ref="R11"  Part="1" 
AR Path="/5F6A41BA/5F5EEE47" Ref="R12"  Part="1" 
AR Path="/5F6A41C2/5F5EEE47" Ref="R13"  Part="1" 
F 0 "R4" V 3143 1750 50  0000 C CNN
F 1 "180" V 3234 1750 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 3280 1750 50  0001 C CNN
F 3 "~" H 3350 1750 50  0001 C CNN
F 4 "C17924" V 3350 1750 50  0001 C CNN "LCSC"
	1    3350 1750
	0    1    1    0   
$EndComp
Wire Wire Line
	3550 1750 3500 1750
Wire Wire Line
	3200 1750 3150 1750
Text HLabel 2050 1750 0    50   Input ~ 0
LED_A
Text HLabel 2050 1900 0    50   Input ~ 0
LED_B
$EndSCHEMATC
