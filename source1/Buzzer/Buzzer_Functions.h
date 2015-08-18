
/******************************************************************************
 *
 *     Filename:       $Buzzer_Functions.h
 *     Created on:     $Date: Mar 24, 2014    (5:03:01 PM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file contains function definition of buzzer related contents
 *
 *     Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/




#ifndef BUZZER_FUNCTIONS_H_
#define BUZZER_FUNCTIONS_H_

#define SHORT_BEEP_TIME               200
#define SHORT_BEEP_OFF_TIME  100

void Buzzer_Init(void);
void Buzzer_toggle(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Short_Beep(uint8_t count);
void Buzzer_Long_Beep(void);

#endif /* BUZZER_FUNCTIONS_H_ */
/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
