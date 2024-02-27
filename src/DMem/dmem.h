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



#endif /* APPLICATION_SUPPORTLIBS_DMEM_DMEM_H_ */





