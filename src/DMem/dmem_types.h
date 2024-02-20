/**************************************************************************//**
 * @file      dmem_types.h
 * @brief     Dynamic memory distribution types declarations. Header file.
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

#ifndef APPLICATION_SUPPORTLIBS_DMEM_DMEM_TYPES_H_
#define APPLICATION_SUPPORTLIBS_DMEM_DMEM_TYPES_H_


#include <stdint.h>
#include <stddef.h>


#define DMEM_BLOCK_SIZE_BYTES    8        // размер блока памяти в байтах
#define DMEM_ENDED_PART          0xFFFF   // отметка указвывающая отсутсвие следующей записи
#define DMEM_DEF_PROC_PERIOD_MS  100      // период обработки основного цикла по умолчанию в мс


// коды возвратов
typedef enum
{
	DMEM_OK = 0,                 // все хорошо
	DMEM_NULL_POINTER = 1,       // обнаружен нулевой указатель
	DMEM_INIT_ERR = 2,           // ошибка при инициализации
	DMEM_WRONG_CRC = 3,          // неверный CRC
	DMEM_OUT_OF_HEAP = 4,        // указатель за пределами кучи
	DMEM_WRONG_ALLIG = 5,        // неверное выравнивание

} dmem_ret_t;


// состояние кучи
typedef enum
{
	DMEM_NO_INIT = 0,            // не инициализирована
	DMEM_INIT = 1,               // инициализирована
	DMEM_ERR  = 2,               // повреждена

} dmem_state_t;


#pragma pack(push,1)

// тип раздела
typedef enum
{
	DMEM_NO_INFO = 0,       // нет информации
	DMEM_FREE = 1,          // свободный раздел
	DMEM_ALLOC = 3,         // занятый раздел

} dmem_part_type_t;

// узел описания заголовка выделенного раздела памяти
typedef struct
{
	dmem_part_type_t part_type;  // тип раздела
	uint8_t          res;        // резерв

	uint16_t next_node;  // адрес следующего раздела в блоках от начала кучи
	                     // если next_node == DMEM_ENDED_PART, то этот раздел последний

	uint16_t size;       // размер раздела в блоках включая заголовок
	uint16_t crc16;      // контрольня сумма описания раздела

} dmem_node_t;

#pragma pack(pop)


// настройки кучи
typedef struct
{
	uint32_t proc_period_ms;   // период обработки основного цикла

} dmem_heap_set_t;


typedef void (*dmem_err_cbk_t)();           // прототип функции ошибки кучи

// константные настройки кучи
typedef struct
{
	uint16_t  heap_size;                    // размер кучи в блоках
	uint8_t  *heap_ptr;                     // указатель на начало кучи
	dmem_err_cbk_t  dmem_err_cbk_t;         // функция ошибки кучи

} dmem_heap_cset_t;


// структура для инициализации
typedef struct
{
	uint32_t        array_size_byte;        // размер массива для кучи в байтах
	uint8_t        *array_ptr;              // указатель на массив для кучи
	dmem_err_cbk_t  dmem_err_cbk_t;         // функция ошибки кучи

} dmem_heap_init_t;


#endif /* APPLICATION_SUPPORTLIBS_DMEM_DMEM_TYPES_H_ */









