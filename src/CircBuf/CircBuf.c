/**************************************************************************//**
 * @file      CircBuf.c
 * @brief     Circular buffer for data. Source file.
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


#include "CircBuf.h"



// инициализация
void CircBuf_Init(circ_buf_t *ptr, circ_buf_init_t *init)
{
  if((ptr == NULL) || (init == NULL))
    return;

  ptr->buf_ptr = init->buf_ptr;
  ptr->buf_len = init->buf_len;

  ptr->start_ind = 0;
  ptr->end_ind = 0;
  ptr->data_size = 0;

  ptr->ovf_err_cnt = 0;
  ptr->lost_bytes = 0;
}


// получить размер буфера
uint16_t CircBuf_GetBufLen(circ_buf_t *ptr)
{
  if(ptr == NULL)
    return 0;

  return ptr->buf_len;
}


// получить число данных
uint16_t CircBuf_GetDataLen(circ_buf_t *ptr)
{
  if(ptr == NULL)
    return 0;

  return ptr->data_size;
}


// получить стартовый индекс
uint16_t CircBuf_GetStartIndex(circ_buf_t *ptr)
{
  if(ptr == NULL)
    return 0;

  return ptr->start_ind;
}


// получить указатель на буфер
uint8_t* CircBuf_GetBufPtr(circ_buf_t *ptr)
{
	if(ptr == NULL)
		return NULL;

	return ptr->buf_ptr;
}


// получить указатель на длину данных
uint16_t* CircBuf_GetDataLenPtr(circ_buf_t *ptr)
{
	if(ptr == NULL)
		return NULL;

	return &ptr->data_size;
}


// получить указатель на стартовый индекс
uint16_t* CircBuf_GetStartIndexPtr(circ_buf_t *ptr)
{
	if(ptr == NULL)
		return NULL;

	return &ptr->start_ind;
}


// добавить данные в кольцевой буфер
circ_buf_error_code_t CircBuf_AddData(circ_buf_t *ptr, uint8_t* data, uint16_t len)
{
	uint16_t len_to_border;

	if((ptr == NULL) || (data == NULL))
		return CIRC_BUF__NULL_POINTER;

	if(len == 0)
		return CIRC_BUF__WRONG_ARG;

	// проверка на переполение буфера
	if(len >= (ptr->buf_len - ptr->data_size))
	{
		ptr->ovf_err_cnt++;
		ptr->lost_bytes += len;

		return CIRC_BUF__OVF;
	}

	len_to_border = ptr->buf_len - ptr->end_ind;                                // расстояние до границы буфера

	// если данные не пересекают границу
	if(len <= len_to_border)
	{
		memcpy(&ptr->buf_ptr[ptr->end_ind], data, len);                         // копируем данные
		ptr->end_ind += len;                                                    // смещаем индек конца

		// проверка закольцовки
		if(ptr->end_ind >= ptr->buf_len)                                        // если перешли границу
			ptr->end_ind = 0;                                                   // кольцуем
	}else                                                                       // если данные пересекают границу
	{
		memcpy(&ptr->buf_ptr[ptr->end_ind], data, len_to_border);               // копируем до границы
		memcpy(&ptr->buf_ptr[0], &data[len_to_border], len - len_to_border);    // копируем оставшиеся данные
		ptr->end_ind = len - len_to_border;                                     // ставим индекс
	}

	Private_CircBuf_AddDataSizeValue(ptr, len);                                 // обновляем кол-во данных

	return CIRC_BUF__OK;
}


// добавить данные в кольцевой буфер потокобезопасно
circ_buf_error_code_t CircBuf_AddDataProtected(circ_buf_t *ptr, uint8_t* data, uint16_t len)
{
	uint32_t s;
	ENTER_CRITICAL(s);
	circ_buf_error_code_t ret = CircBuf_AddData(ptr, data, len);
	LEAVE_CRITICAL(s);
	return ret;
}


// установить стартовый индекс (удаляет данные из буфера путем переноса стартового индекса)
void CircBuf_SetStartInd(circ_buf_t *ptr, uint16_t index)
{
	int32_t pop_data;

	if(index >= ptr->buf_len)
		return;

	// вычисляем ко-во удаляемых данных
	pop_data = index - ptr->start_ind;

	// если удаляемых данных есть
	if(pop_data != 0)
	{
		// если перешли границу, корректируем
		if(pop_data < 0)
			pop_data += ptr->buf_len;

		// обновляем индек и кол-во данных
		ptr->start_ind = index;
		Private_CircBuf_AddDataSizeValue(ptr, -pop_data);
	}
}


// увеличить индекс
uint16_t CircBuf_AddIndValue(circ_buf_t *ptr, uint16_t val, uint16_t add)
{
	if(ptr == NULL)
		return 0;

	if(ptr->buf_len == 0)
		return 0;

	val += add;
	while(val >= ptr->buf_len)
		val -= ptr->buf_len;

	return val;
}


// увеличить индекс
uint16_t CircBuf_IncIndValue(circ_buf_t *ptr, uint16_t val)
{
	if(ptr == NULL)
		return 0;

	if(ptr->buf_len == 0)
		return 0;

	val++;
	while(val >= ptr->buf_len)
		val -= ptr->buf_len;

	return val;
}


// получить длинну данных мжду заданными индексами
uint16_t CircBuf_GetDataLenBetweenIndexes(circ_buf_t *ptr, uint16_t start_ind, uint16_t end_ind)
{
	if(ptr == NULL)
		return 0;

	if((start_ind >= ptr->buf_len) || (end_ind >= ptr->buf_len))
		return 0;

	int32_t len = (int32_t)end_ind - (int32_t)start_ind;
	if(len < 0)
		len += (int32_t)ptr->buf_len;

	return (uint16_t)len;
}


// скопировать данные между индексами
uint16_t CircBuf_DataCopyBetweenIndexes(circ_buf_t *ptr, uint8_t *dst, uint16_t start_ind, uint16_t end_ind)
{
	if((ptr == NULL) || (dst == NULL))
		return 0;

	if((start_ind >= ptr->buf_len) || (end_ind >= ptr->buf_len))
		return 0;

	int32_t  len = (int32_t)end_ind - (int32_t)start_ind;
	uint16_t part_len = 0;
	if(len < 0)
	{
		part_len = ptr->buf_len - start_ind;
		memcpy(dst, &ptr->buf_ptr[start_ind], part_len);
		memcpy(&dst[part_len], ptr->buf_ptr, end_ind);

		part_len += end_ind;
	}
	else{
		part_len = (uint16_t)len;
		memcpy(dst, &ptr->buf_ptr[start_ind], part_len);
	}

	return part_len;
}


// Установить данные между индексами
uint16_t CircBuf_DataSetBetweenIndexes(circ_buf_t *ptr, uint8_t val, uint16_t start_ind, uint16_t end_ind)
{
	if(ptr == NULL)
		return 0;

	if((start_ind >= ptr->buf_len) || (end_ind >= ptr->buf_len))
		return 0;

	int32_t  len = (int32_t)end_ind - (int32_t)start_ind;
	uint16_t part_len = 0;
	if(len < 0)
	{
		part_len = ptr->buf_len - start_ind;
		memset(&ptr->buf_ptr[start_ind], val, part_len);
		memset(ptr->buf_ptr, val, end_ind);

		part_len += end_ind;
	}
	else{
		part_len = (uint16_t)len;
		memset(&ptr->buf_ptr[start_ind], val, part_len);
	}

	return part_len;
}




// безопасная работа с переменной data_size
static void Private_CircBuf_AddDataSizeValue(circ_buf_t *ptr, int16_t add)
{
  uint32_t s;

  ENTER_CRITICAL(s);               // начало критической секции

  ptr->data_size += add;           // инкремент на заданное значени

  LEAVE_CRITICAL(s);               // окончание критической секции
}
