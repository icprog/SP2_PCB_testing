/*
 * sp2_string.c
 *
 *  Created on: Jul 15, 2015
 *      Author: Brett Nicholas
 */
#include "sp2_string.h"


//SP2_STRING_T str = str_BRASILIA;

int getStringStructSize(void)
{
	int string_struct_size = 0;
	for(int i = 0; i < NUM_STRINGS; i++)
	{
		for(int j = 0; j < NUM_LANGUAGES; j++)
		{
			string_struct_size += string_array[i].string_lens[j] + 2; //the plus 2 is for the 16bit integer storing the length of the string
		}
	}
	return string_struct_size;
}




