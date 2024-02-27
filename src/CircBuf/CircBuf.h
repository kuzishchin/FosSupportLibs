/**************************************************************************//**
 * @file      CircBuf.h
 * @brief     Circular buffer for data. Header file.
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

#ifndef APPLICATION_SUPPORTLIBS_CIRCBUF_H_
#define APPLICATION_SUPPORTLIBS_CIRCBUF_H_


#include <stdint.h>
#include <string.h>                     // работа со строками
#include "Platform/compiler_macros.h"



// перечисление кодов ошибок
typedef enum
{
	CIRC_BUF__OK = 0,                      // ОК
	CIRC_BUF__NULL_POINTER,                // нулевой указатель
	CIRC_BUF__WRONG_ARG,                   // неверный аргумент
	CIRC_BUF__OVF,                         // переполнение

} circ_buf_error_code_t;


// структура для инициализации
typedef struct
{
	uint8_t* buf_ptr;                                                             // указатель на буфер приёма
	uint16_t buf_len;                                                             // длина буфера

} circ_buf_init_t;


// описание структуры кольцевого буфера
typedef struct
{
	uint8_t* buf_ptr;                                                             // указатель на буфер приёма
	uint16_t buf_len;                                                             // длина буфера

	uint16_t start_ind;                                                           // начальный индекс данных
	uint16_t end_ind;                                                             // конечный индекс данных
	uint16_t data_size;                                                           // заполнение буфера в байтах

	uint32_t ovf_err_cnt;                                                         // счётчик переполнения буфера
	uint32_t lost_bytes;                                                          // потеряно байт

} circ_buf_t;


// инициализация
void CircBuf_Init(circ_buf_t *ptr, circ_buf_init_t *init);

// получить размер буфера
uint16_t CircBuf_GetBufLen(circ_buf_t *ptr);

// получить число данных
uint16_t CircBuf_GetDataLen(circ_buf_t *ptr);

// получить стартовый индекс
uint16_t CircBuf_GetStartIndex(circ_buf_t *ptr);

// получить указатель на буфер
uint8_t* CircBuf_GetBufPtr(circ_buf_t *ptr);

// получить указатель на буфер
uint16_t* CircBuf_GetDataLenPtr(circ_buf_t *ptr);

// получить указатель на стартовый индекс
uint16_t* CircBuf_GetStartIndexPtr(circ_buf_t *ptr);

// добавить данные в кольцевой буфер
circ_buf_error_code_t CircBuf_AddData(circ_buf_t *ptr, uint8_t* data, uint16_t len);

// добавить данные в кольцевой буфер потокобезопасно
circ_buf_error_code_t CircBuf_AddDataProtected(circ_buf_t *ptr, uint8_t* data, uint16_t len);

// установить стартовый индекс (удаляет данные из буфера путем переноса стартового индекса)
void CircBuf_SetStartInd(circ_buf_t *ptr, uint16_t index);

// увеличить индекс
uint16_t CircBuf_AddIndValue(circ_buf_t *ptr, uint16_t val, uint16_t add);

// увеличить индекс
uint16_t CircBuf_IncIndValue(circ_buf_t *ptr, uint16_t val);

// получить длинну данных мжду заданными индексами
uint16_t CircBuf_GetDataLenBetweenIndexes(circ_buf_t *ptr, uint16_t start_ind, uint16_t end_ind);

// скопировать данные между индексами
uint16_t CircBuf_DataCopyBetweenIndexes(circ_buf_t *ptr, uint8_t *dst, uint16_t start_ind, uint16_t end_ind);

// Установить данные между индексами
uint16_t CircBuf_DataSetBetweenIndexes(circ_buf_t *ptr, uint8_t val, uint16_t start_ind, uint16_t end_ind);



#endif /* APPLICATION_SUPPORTLIBS_CIRCBUF_H_ */
