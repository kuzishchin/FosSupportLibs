/**************************************************************************//**
 * @file      sl_string.c
 * @brief     Common string functions. Source file.
 * @version   V1.0.00
 * @date      12.01.2024
 ******************************************************************************/
/*
* Copyright 2024 Yury A. Kuzishchin and Vitaly A. Kostarev. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


#include "Libs/sl_string.h"



// число в строку по основанию
void user_itoa(int32_t _val, uint8_t base, uint8_t min_len, char *bufstr, uint8_t ur_flag)
{
	uint8_t buf[32] = {0};
	uint32_t val;
	int i = 30;
	int j=1;

	// елси знчение равно нулю
	if(_val == 0)
	{
		bufstr[0] = '0';
		while(j < min_len)
		{
		    bufstr[j] = ' ';
		    j++;
		}
		bufstr[j] = 0;
		return;
	}

	// если значение отрицательное
	if(_val < 0)
	{
		val = -_val;
		bufstr[0] = '-';
		j=1;
	}else   // если положительное
	{
		val = _val;
		j=0;
	}

	// вычисляем символы
	for(; val && i ; --i, val /= base)
	{
		if(ur_flag == 0)
			buf[i] = "0123456789abcdef"[val % base];
		else
			buf[i] = "0123456789ABCDEF"[val % base];
	}
	i++;

	// копируем символы
	while (buf[i]!=0)
	{
		bufstr[j]=buf[i];
		i++;
		j++;
	}

	// затираем лишнее
	while(j < min_len)
	{
	    bufstr[j] = ' ';
	    j++;
	}

	// нуль терминатор
	bufstr[j] = 0;
}


// парсинг имени первой папки из пути к фалу
// возвращает:
// 0 - имя не найдено;
// 1 - найдено крайнее имя в пути: 2 - то же но путь заканчивается знаком '/'
// 3 - найдено очередное имя в пути
// 4 - достигнута максимальная длина пути
uint8_t parse_first_node_name(const char* path, char* folder, char* remained_path, uint8_t str_len)
{
	char c;
	uint8_t mode = 0;
	uint8_t j = 0;

	char tmp[255];
	strcpy(tmp, path);

	memset(folder, 0, str_len);
	memset(remained_path, 0, str_len);


	for(uint8_t i = 0; i < str_len; i++)
	{
		c = tmp[i];
		if(c == 0)
			return mode;

		if(mode == 2)
			mode = 3;

		switch(mode)
		{
		case 0:
			if(c != '/')
			{
				folder[j] = c;
				j++;
				mode = 1;
			}

		break;

		case 1:
			if(c != '/')
			{
				folder[j] = c;
				j++;
			}else
			{
				mode = 2;
				j = 0;
			}

		break;

		case 3:
			remained_path[j] = c;
			j++;

		break;

		}
	}

	return 4;
}











