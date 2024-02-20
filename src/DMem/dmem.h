/**************************************************************************//**
 * @file      dmem.h
 * @brief     Dynamic memory distribution objects. Header file.
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

#ifndef APPLICATION_SUPPORTLIBS_DMEM_DMEM_H_
#define APPLICATION_SUPPORTLIBS_DMEM_DMEM_H_


#include "dmem_types.h"


// запись отладки
typedef struct
{
	uint16_t parts_cnt;           // число разделов
	uint32_t min_size_bytes;      // размер минимального раздела
	uint32_t max_size_bytes;      // размер максимального раздела
	uint32_t all_parts_bytes;     // размер всех разделов

} dmem_heap_dbg_note_t;


// отладка
typedef struct
{
	dmem_heap_dbg_note_t free;   // описание свободных разделов
	dmem_heap_dbg_note_t alloc;  // описание занятых разделов

	uint32_t heap_size_bytes;    // размер кукчи в байтах
	float    alloc_p;            // занято от кучи в процентах
	float    free_p;             // свободно от кучи в процентах

	uint16_t addres_err_part;    // адрес раздела с ошибкой

} dmem_heap_dbg_t;


// переменные кучи
typedef struct
{
	dmem_state_t state;          // состояние кучи
	uint32_t     ts;             // метка времени

} dmem_heap_var_t;


// описание кучи
typedef struct
{
	dmem_heap_var_t  var;       // переменные
	dmem_heap_set_t  set;       // настройки
	dmem_heap_cset_t cset;      // константные настройки
	dmem_heap_dbg_t  dbg;       // отладка

} dmem_heap_t;


// инициализация кучи
dmem_ret_t DMem_HeapInit(dmem_heap_t *p, dmem_heap_init_t *init);

// выделить память
void* DMem_Alloc(dmem_heap_t *p, uint32_t size_bytes);

// освободить память
dmem_ret_t DMem_Free(dmem_heap_t *p, void* ptr);

// обработчик основного цикла для кучи
void DMem_MainLoopProc(dmem_heap_t* p);

// получить указатель на раздел по адресу
static dmem_node_t* Private_DMem_GetPartPtr(dmem_heap_t* p, uint16_t addres);

// получить адрес раздела
static uint16_t Private_DMem_GetAddres(dmem_heap_t *p, dmem_node_t *node);

// проверить кучу
static dmem_ret_t Private_DMem_CheckHeap(dmem_heap_t* p);

// отладка
static void Private_DMem_DbgNode(dmem_node_t *p, dmem_heap_dbg_note_t *_free, dmem_heap_dbg_note_t *_alloc);

// отладка
static void Private_DMem_Dbg(dmem_heap_t* p);

// создать раздел
static dmem_node_t* Private_DMem_CreatePart(dmem_heap_t* p, uint16_t addres, uint16_t size, dmem_part_type_t type, uint16_t next_addres);

// обновляем CRC
static void Private_DMem_UpdCrc(dmem_node_t* p);

// проверяем CRC
static dmem_ret_t Private_DMem_CheckCrc(dmem_node_t* p);

// разделить рездел на две части, первая часть размером size
static void Private_DMem_SplitPart(dmem_heap_t* p, dmem_node_t* node, uint16_t size);

// слить указанный раздел со следующим с проверкой на наличие последнего и свобоность обоих
static void Private_DMem_MargePart(dmem_heap_t* p, dmem_node_t* node);

// ищем последовательно расположенные пустые блоки
static dmem_node_t* Private_DMem_FindLineFreeParts(dmem_heap_t* p);

// вызываем cllback ошибки кучи
static void Private_DMem_ErrCallback(dmem_heap_t* p);



#endif /* APPLICATION_SUPPORTLIBS_DMEM_DMEM_H_ */





