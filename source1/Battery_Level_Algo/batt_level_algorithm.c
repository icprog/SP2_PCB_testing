/******************************************************************************
 *
 *   Filename: $ batt_level_algorithm.c
 *   Created on: $Date: June 08, 2015 
 *   Revision: $3
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of battery level monitoring algorithm.
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#define PC_DEBUG 0
#if PC_DEBUG
	#include <windows.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdint.h>

#else if

	#include <mqx.h>
	#include <bsp.h>
	#include <psp.h>
	#include <math.h>
	#include <stdint.h>
	#include <stdlib.h>
	#include "config.h"
//	#include "common_headers.h"

#endif

#include "batt_level_algorithm.h"

/************************* VARIABLES **********************************/

 static const float battData[279]=
 {
 
 0.9914508018,0.9914031289,0.9913554561,0.9913078727,0.9912602894,0.991212706,0.9911651227,0.9911176288,0.9910701349,0.991022641,0.9909751472,
 0.9909277428,0.9908803384,0.9908328445,0.9907854401,0.9907380357,0.9906906313,0.9899817129,0.8725120014,0.7444020969,0.6835708361,0.519048079,
 0.4579248302,0.2992844067,0.2438514464,0.1672361774,0.0685074786,0.0305904508,0,-0.0577541513,-0.1123813887,0.9956431561,0.9956205409,
 0.9955979257,0.9955753105,0.9955526952,0.99553008,0.9955075058,0.9954848905,0.9954623163,0.9954397421,0.9954171678,0.9953945936,
 0.9953720193,0.7371741668,0.7004699461,0.5741048287,0.5466704183,0.4086518423,0.2741093442,0.2523432768,0.1297540516,0.1122265387,
 0,-0.0140310193,-0.0284498947,-0.0967407493,-0.1342588432,-0.1454890442,-0.1550866204,-0.1631856745,-0.172068779,0.9974699009,0.9974562653,
 0.9974426296,0.9974290175,0.9974154055,0.9974017934,0.9973881813,0.9973745692,0.9968866073,0.9962007309,0.9947087602,0.670580054,0.542796353,
 0.4696146428,0.4013931677,0.3790928094,0.3528500456,0.3195290448,0,-0.0099225572,-0.0665596659,-0.0781370207,-0.1270487333,-0.1351476474,
 -0.1806738134,-0.1875756517,-0.194439425,-0.2189671609,-0.2369236634,-0.242032574,-0.2474116219,0.9911010963,0.9910532992,0.9910055021,
 0.9909576166,0.9909098195,0.9908620224,0.9908141369,0.9907663397,0.9907185426,0.9906707455,0.99062286,0.9905750629,0.9905271774,0.9904792919,
 0.9904314064,0.8357127694,0.6911960028,0.5355089666,0.4093070175,0.2752222919,0.1822015418,0.0757661924,0,-0.0602270774,-0.126591094,-0.1766701401,
 -0.2106625305,-0.2386096656,-0.2641345271,-0.2843320381,-0.3026607978,0.9956831617,0.9956610453,0.9956389289,0.9956168125,0.9955946961,0.9955725797,
 0.9955505018,0.9955284238,0.9948888923,0.659662773,0.5322736494,0.4243025964,0.3074525439,0.2196786207,0.1331503922,0.0594188496,0,-0.0482134059,
 -0.0948260371,-0.1266766815,-0.1556390291,-0.1735372121,-0.1941865198,-0.2085368987,-0.2220468449,-0.2346537453,-0.2450870859,-0.2543623135,
 -0.261902237,-0.2680499161,-0.2728454055,0.9974916276,0.9974785743,0.997465521,0.9974524678,0.9970750801,0.9963385332,0.7019933884,0.5774263735,
 0.4600443156,0.3671294837,0.2700500024,0.1828655614,0.1100909725,0.0538145349,0,-0.0377398247,-0.0713819177,-0.0977975443,-0.1206810483,
 -0.1376277609,-0.1514353282,-0.1639438762,-0.1742141925,-0.1802833692,-0.1876363026,-0.1947932254,-0.2002182896,-0.2043828234,-0.207778951,
 -0.2099406973,-0.2121797386,0.991682839,0.9916389137,0.9915949884,0.9915510631,0.9915071378,0.9914632125,0.9914194375,0.8955891655,0.7503531137,
 0.6425806101,0.5391333969,0.4461297929,0.3639024314,0.2919760988,0.2148282936,0.1530445677,0.0963307782,0.046589188,0,-0.0339865816,-0.0650585634,
 -0.0896613764,-0.1114811759,-0.1285691223,-0.143519521,-0.1549422102,-0.1654498476,-0.1735463329,-0.1813804508,-0.186454714,-0.1916827097,
 0.9952776239,0.9952537593,0.9952298947,0.9952060301,0.9951821654,0.9951583008,0.993731742,0.6576377669,0.5425556024,0.443773321,0.3352475436,
 0.2013596015,0.1288485043,0.0641162147,0,-0.0548933485,-0.1081472282,-0.1498916566,-0.1836237845,-0.2143751899,-0.2386176249,-0.2589420064,
 -0.2762347825,-0.2903397373,-0.3017170379,-0.3102174449,-0.3186716283,-0.3250329302,-0.3297311798,-0.333562898,-0.3366350243,0.9975113589,
 0.997187266,0.9963888579,0.995112551,0.6835017347,0.5647512792,0.4572257191,0.3696290689,0.2839496106,0.2113313889,0.1424130903,0.079039482,
 0,-0.0443631941,-0.0706439463,-0.0921173716,-0.1120317549,-0.1279742948,-0.1402136594,-0.1502522544,-0.157692835, -0.1631318187,-0.1674478934,
 -0.170530291,-0.1726384031,-0.1740405083,-0.1751506123,-0.1761166986,-0.1767609085,-0.1772589823,-0.1776529189
 };
 
 /*-----------------------------------------------------------------------------* 
 * Function:    battData
 * Brief:       This function calculate Remaining battery capacity from Battery voltage, current and temperature.
 * Input:   	temperature,current, voltage
 * Output:      battery capacity remaining.
 -----------------------------------------------------------------------------*/
 void triBattInterp( float temperature, float current,float voltage,float *capacityLeft,float *capacityBin)
 {
	 
	 float tempRange[3] = {-18, 0, 18};
	 float loadRange[3] = {7.5, 15, 25};
	 float load = 0.0;
	 float tempD,loadD,voltD;
	 float c00,c10,c01,c11;
	 float c0,c1;
	 float voltC111,voltC112;
	 // float capacityLeft = 0.0;

	 int16_t 	C111,C112,C121,C122,C211,C212,C221,C222;
	 uint16_t 	index_0,index_1,i;
	 uint16_t 	volt_temp = 3000;
	 uint16_t 	volt_actual = (uint16_t)(voltage*1000);
	 int16_t loadC111,loadC121;


	 C111 = C112 = C121 = C122 = C211 = C212 = C221 = C222 = 0;
	 i = index_0 = index_1 = 0;
	 tempD = loadD = voltD = c00 = c10 = c01 = c11 = 0.0;

	 // ensure temperature is in correct range
	 if(temperature < TEMP_MIN) {temperature = TEMP_MIN;}
	 if(temperature > TEMP_MAX) {temperature = TEMP_MAX;}

	 //convert current to equivelent resistive load 
	 load = voltage/current;
	 //printf("\n load_0 %f\n",load);

	 //   % ensure load is in correct range
	 if(load > LOAD_MAX) { load = LOAD_MAX;}
	 if(load < LOAD_MIN) { load = LOAD_MIN;}

	 //printf("\n load_1 %f\n",load);

	 // % ensure voltage is in the correct range
	 if(voltage > VOLTAGE_MAX) {voltage = VOLTAGE_MAX;}
	 if(voltage < VOLTAGE_MIN) {voltage = VOLTAGE_MIN;}

	 // find range to search, indexs are based on structure of data file
	 if((temperature < TEMP_MIDLE) && (load < LOAD_MIDLE))  { index_0 = 0; index_1 = 30;} 
	 else	if((temperature >= TEMP_MIDLE) && (load < LOAD_MIDLE)) { index_0 = 93; index_1 = 123;}
	 else	if((temperature < TEMP_MIDLE) && (load >= LOAD_MIDLE)) { index_0 = 31; index_1 = 61;}
	 else	{ index_0 = 124; index_1 = 154;}

	 //printf("\n index_0%d\t index_1 %d\n",index_0,index_1);

	 for(i = index_0; i <= index_1 ; i++)
	 {	
		 if (volt_actual >= volt_temp)
		 {		
			 C111 = i;
			 C112 = i - 1;   // adjacent voltages are adjacent matrix entries
			 C121 = i + 31;  // adjacent currents are 31 entries apart
			 C122 = i + 30;
			 C211 = i + 93;  // adjacent temperatures are 93 entries apart
			 C212 = i + 92;
			 C221 = i + 124;
			 C222 = i + 123;

			 break;
		 }
		 volt_temp -=25;
	 }


	 voltC111 = 3.0 -(0.025 *((float)(C111 % VOLTAGE_VALUE_CNT)));
	 voltC112 = 3.0 -(0.025 *((float)(C112 % VOLTAGE_VALUE_CNT)));
	 loadC111 = ((int16_t)(C111/(LOAD_VALUE_CNT - 1))) % 3;
	 loadC121 = ((int16_t)(C121/(LOAD_VALUE_CNT - 1))) % 3;

	 tempD = (temperature - tempRange[C111/(TEMP_VALUE_CNT - 1)])/(tempRange[C211/(TEMP_VALUE_CNT - 1)] - tempRange[C111/(TEMP_VALUE_CNT - 1)]);
	 loadD = (load - loadRange[loadC111])/(loadRange[loadC121] - loadRange[loadC111]);
	 voltD = (voltage - voltC111)/( voltC112 - voltC111);

	 //printf("\n tempD %f\t loadD %f\t voltD %f\n",tempD,loadD,voltD);

	 c00 = battData[C111]*(1-tempD) + battData[C211]*tempD;
	 c10 = battData[C121]*(1-tempD) + battData[C221]*tempD;
	 c01 = battData[C112]*(1-tempD) + battData[C212]*tempD;
	 c11 = battData[C122]*(1-tempD) + battData[C222]*tempD; 

	 c0 = c00*(1-loadD) + c10*loadD;
	 c1 = c01*(1-loadD) + c11*loadD;

	 *capacityLeft = c0*(1-voltD) + c1*voltD;

	 /*truncate capacity remaining to */
	 if (*capacityLeft < 0) *capacityLeft = 0;

	 *capacityBin = (float)(((int32_t)((*capacityLeft-0.01)/0.25)+1)*0.25);
	 *capacityLeft *= 100;
	 //    *capacityRemaining = (uint8_t)((capacityLeft * 100) + 0.5);
 }
 
#if PC_DEBUG
 void main(void)
 {
 float temp,current,voltage,capacityLeft;
 float capacityBin;
 printf("\n Enter the data in the following format\n");
 
 triBattInterp( -12,  250, 2.5,&capacityLeft,&capacityBin);
 printf("\nOutputs\n");
 printf("\ncapacityLeft: %f\t capacityBin: %f\n",capacityLeft,capacityBin);
 
 while(1)
 {
 printf("\n**********************************************************************************\n");
 printf("\n Enter Temp\n");
 scanf("%f",&temp);
 printf("\n Enter current\n");
 scanf("%f",&current);
 printf("\n Enter voltage\n");
 scanf("%f",&voltage);
 
 printf("\nInputs\n");
 printf(" temp: %f\t current: %f\t voltage: %f\n",temp,current,voltage);
 triBattInterp( temp,  current, voltage,&capacityLeft,&capacityBin);
 printf("\nOutputs\n");
  printf("\ncapacityLeft: %f\t capacityBin: %f\n",capacityLeft,capacityBin);
 
 }
 
 }
#endif
