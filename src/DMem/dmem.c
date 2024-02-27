/**************************************************************************//**
 * @file      dmem.c
 * @brief     Dynamic memory distribution objects. Source file.
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

#include "dmem.h"
#include "CRC/CRC16.h"
#include "Platform/sl_platform.h"
#include <string.h>


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


// инициализация кучи
dmem_ret_t DMem_HeapInit(dmem_heap_t *p, dmem_heap_init_t *init)
{
	if((p == NULL) || (init == NULL))
		return DMEM_NULL_POINTER;

	if(p->var.state != DMEM_NO_INIT)   // если куча инициализирована, выходим
		return DMEM_INIT_ERR;
	if(init->array_ptr == NULL)        // если нулевой указатель, выходим
		return DMEM_INIT_ERR;
	if(init->array_size_byte == 0)     // если нулевой размер, выходим
		return DMEM_INIT_ERR;

	memset(&p->var, 0, sizeof(dmem_heap_var_t));                 // сброс переменных

	p->cset.dmem_err_cbk_t = init->dmem_err_cbk_t;               // регистрируем функцию ошибки кучи

	p->cset.heap_ptr = init->array_ptr;                                   // помещаем кучу в массив
	p->cset.heap_size = init->array_size_byte / DMEM_BLOCK_SIZE_BYTES;    // вычисляем размер кучи в блоках

	p->set.proc_period_ms = DMEM_DEF_PROC_PERIOD_MS;                      // период обработки по умолчанию

	Private_DMem_CreatePart(p, 0, p->cset.heap_size, DMEM_FREE, DMEM_ENDED_PART);  // создаем первый раздел

	p->var.state = DMEM_INIT;                                    // ставим флаг инициализации

	return DMEM_OK;
}


// выделить память
void* DMem_Alloc(dmem_heap_t *p, uint32_t size_bytes)
{
	if(p == NULL)
		return NULL;

	if((p->var.state == DMEM_NO_INIT) || (size_bytes == 0))
		return NULL;

	uint16_t size_blk = size_bytes / DMEM_BLOCK_SIZE_BYTES;    // вычисляем требуемый размер раздела в блоках
	if(size_bytes%DMEM_BLOCK_SIZE_BYTES)                       // округляем вверх
		size_blk++;
	size_blk++;                                                // учитываем заголовок

	if(size_blk > p->cset.heap_size)                           // проверка размера
		return NULL;

	dmem_ret_t ret = DMEM_OK;
	uint16_t addres = 0;
	dmem_node_t* node_ptr = NULL;

	uint16_t min_size_blk = 0xFFFF;
	dmem_node_t* sel_node_ptr = NULL;

	// перебираем все разделы
	while(addres != DMEM_ENDED_PART)
	{
		node_ptr = Private_DMem_GetPartPtr(p, addres);   // получаем указатель на описание раздела

		ret = Private_DMem_CheckCrc(node_ptr);           // проверяем CRC
		if(ret != DMEM_OK)
		{
			p->dbg.addres_err_part = addres;
			p->var.state = DMEM_ERR;
			Private_DMem_ErrCallback(p);
			return NULL;
		}

		addres = node_ptr->next_node;                    // получем адрес следующего раздела

		if(node_ptr->part_type == DMEM_ALLOC)            // если раздел занятый, переходим к началу цикла
			continue;
		if(node_ptr->size < size_blk)                    // если размер раздела меньше требуемого, переходим к началу цикла
			continue;
		if(node_ptr->size >= min_size_blk)               // если раздел не самый маленький, переходим к началу цикла
			continue;

		/*
		 * Сохраняем данные подходящего размера
		 */
		min_size_blk = node_ptr->size;
		sel_node_ptr = node_ptr;
	}

	if(sel_node_ptr == NULL)         // проверяем наличие подходящего раздела
		return NULL;

	Private_DMem_SplitPart(p, sel_node_ptr, size_blk);  // разделяем раздел

	sel_node_ptr->part_type = DMEM_ALLOC;               // занимаем раздел
	Private_DMem_UpdCrc(sel_node_ptr);                  // обновляем CRC

	uint32_t node_mem_addres = (uint32_t)sel_node_ptr;  // получаем адрес раздела в памяти
	node_mem_addres += DMEM_BLOCK_SIZE_BYTES;           // смещаемся на заголовок

	return (void*)node_mem_addres;                      // возвращаем указатель на эту область
}


// освободить память
dmem_ret_t DMem_Free(dmem_heap_t *p, void* ptr)
{
	if((p == NULL) || (ptr == NULL))
		return DMEM_NULL_POINTER;

	if(p->var.state == DMEM_NO_INIT)
		return DMEM_INIT_ERR;

	uint32_t shift = (uint32_t)ptr - (uint32_t)p->cset.heap_ptr;    // смещение в байтах
	if(shift%DMEM_BLOCK_SIZE_BYTES)                                 // проверка на выравнивание
		return DMEM_WRONG_ALLIG;

	shift /= DMEM_BLOCK_SIZE_BYTES;                                 // смещение в блоках
	if(shift > p->cset.heap_size)                                   // проверка на принадлежность куче
		return DMEM_OUT_OF_HEAP;
	shift--;                                                        // учитываем размер заголовка

	dmem_ret_t ret = DMEM_OK;
	uint16_t addres = 0;
	dmem_node_t* node_ptr = NULL;

	uint16_t checked_addres = 0;

	// перебираем все разделы
	while(addres != DMEM_ENDED_PART)
	{
		node_ptr = Private_DMem_GetPartPtr(p, addres);   // получаем указатель на описание раздела

		ret = Private_DMem_CheckCrc(node_ptr);           // проверяем CRC
		if(ret != DMEM_OK)
		{
			p->dbg.addres_err_part = addres;
			p->var.state = DMEM_ERR;
			Private_DMem_ErrCallback(p);
			return ret;
		}

		checked_addres = addres;                         // сохраняем адрес проверяемого раздела
		addres = node_ptr->next_node;                    // получем адрес следующего раздела

		if(node_ptr->part_type == DMEM_FREE)             // если раздел свободный, переходим к началу цикла
			continue;

		if(checked_addres == shift)                      // если адрес и смещение совпали, выходим из цикла
			break;
	}

	node_ptr->part_type = DMEM_FREE;                     // освобождаем раздел
	Private_DMem_UpdCrc(node_ptr);                       // обновляем CRC

	node_ptr = Private_DMem_FindLineFreeParts(p);        // ищем последовательность пустых разделов и возвращаем первый из них
	if(node_ptr)
	{
		Private_DMem_MargePart(p, node_ptr);
		Private_DMem_MargePart(p, node_ptr);             // ели было три последовательных блока
	}

	return DMEM_OK;
}


// обработчик основного цикла для кучи
void DMem_MainLoopProc(dmem_heap_t* p)
{
	if(p == NULL)
		return;

	if(p->var.state == DMEM_NO_INIT)
		return;

	if((SL_GetTick() - p->var.ts) < p->set.proc_period_ms)
		return;
	p->var.ts = SL_GetTick();

	Private_DMem_CheckHeap(p);
}


// проверить кучу
static dmem_ret_t Private_DMem_CheckHeap(dmem_heap_t* p)
{
	dmem_ret_t ret = DMEM_OK;
	uint16_t addres = 0;
	dmem_node_t* node_ptr = NULL;

	dmem_heap_dbg_note_t _free = {0};
	dmem_heap_dbg_note_t _alloc = {0};

	_free.min_size_bytes = 0xFFFFFFFF;
	_alloc.min_size_bytes = 0xFFFFFFFF;

	// перебираем все разделы
	while(addres != DMEM_ENDED_PART)
	{
		node_ptr = Private_DMem_GetPartPtr(p, addres);     // получаем указатель на описание раздела

		ret = Private_DMem_CheckCrc(node_ptr);             // проверяем CRC
		if(ret != DMEM_OK)
		{
			p->dbg.addres_err_part = addres;
			p->var.state = DMEM_ERR;
			Private_DMem_ErrCallback(p);
			return ret;
		}

		addres = node_ptr->next_node;                      // получем адрес следующего раздела

		Private_DMem_DbgNode(node_ptr, &_free, &_alloc);   // отладка
	}

	memcpy(&p->dbg.free, &_free, sizeof(dmem_heap_dbg_note_t));
	memcpy(&p->dbg.alloc, &_alloc, sizeof(dmem_heap_dbg_note_t));
	Private_DMem_Dbg(p);

	p->dbg.addres_err_part = DMEM_ENDED_PART;

	return ret;
}


// отладка
static void Private_DMem_DbgNode(dmem_node_t *p, dmem_heap_dbg_note_t *_free, dmem_heap_dbg_note_t *_alloc)
{
	dmem_heap_dbg_note_t *dn = NULL;

	switch(p->part_type)
	{
	case DMEM_FREE:
		dn = _free;
		break;

	case DMEM_ALLOC:
		dn = _alloc;
		break;

	default:
		return;
	}

	dn->parts_cnt++;

	uint32_t size_bytes = p->size * DMEM_BLOCK_SIZE_BYTES;
	dn->all_parts_bytes += size_bytes;

	if(size_bytes > dn->max_size_bytes)
		dn->max_size_bytes = size_bytes;

	if(size_bytes < dn->min_size_bytes)
		dn->min_size_bytes = size_bytes;
}


// отладка
static void Private_DMem_Dbg(dmem_heap_t* p)
{
	p->dbg.heap_size_bytes = p->cset.heap_size * DMEM_BLOCK_SIZE_BYTES;
	p->dbg.free_p = 100.0f * (float)p->dbg.free.all_parts_bytes / (float)p->dbg.heap_size_bytes;
	p->dbg.alloc_p = 100.0f * (float)p->dbg.alloc.all_parts_bytes / (float)p->dbg.heap_size_bytes;
}


// получить указатель на раздел по адресу
static dmem_node_t* Private_DMem_GetPartPtr(dmem_heap_t* p, uint16_t addres)
{
	uint32_t shift = addres * DMEM_BLOCK_SIZE_BYTES;                  // вычисляем смещение в массиве кучи в байтах
	return (dmem_node_t*)&p->cset.heap_ptr[shift];                    // возвращаем указатель на раздел
}


// получить адрес раздела
static uint16_t Private_DMem_GetAddres(dmem_heap_t *p, dmem_node_t *node)
{
	uint32_t shift = (uint32_t)node - (uint32_t)p->cset.heap_ptr;
	return shift / DMEM_BLOCK_SIZE_BYTES;
}


// создать раздел
static dmem_node_t* Private_DMem_CreatePart(dmem_heap_t* p, uint16_t addres, uint16_t size, dmem_part_type_t type, uint16_t next_addres)
{
	dmem_node_t* node_ptr = Private_DMem_GetPartPtr(p, addres);     // получаем указатель на раздел по адресу

	node_ptr->part_type = type;           // тип блока
	node_ptr->res = 0;                    // резерв
	node_ptr->next_node = next_addres;    // адрес следующего блока
	node_ptr->size = size;                // размер
	node_ptr->crc16 = 0;                  // пока 0

	Private_DMem_UpdCrc(node_ptr);        // обновляем CRC

	return node_ptr;                      // возвращаем указатель
}


// обновляем CRC
static void Private_DMem_UpdCrc(dmem_node_t* p)
{
	p->crc16 = CRC16_Calc((uint8_t*)p, sizeof(dmem_node_t) - 2);   // вычисляем CRC16
}


// проверяем CRC
static dmem_ret_t Private_DMem_CheckCrc(dmem_node_t* p)
{
	uint16_t crc16 = CRC16_Calc((uint8_t*)p, sizeof(dmem_node_t) - 2);   // вычисляем CRC16
	if(crc16 != p->crc16)                                                // проверяем
		return DMEM_WRONG_CRC;
	return DMEM_OK;
}


// разделить рездел на две части, первая часть размером size
static void Private_DMem_SplitPart(dmem_heap_t* p, dmem_node_t* node, uint16_t size)
{
	if(node->size <= size)       // если раздел разделять не инужно, выходим
		return;

	uint16_t addres = Private_DMem_GetAddres(p, node);   // получаем адрес
	addres += size;                                      // смещаемся

	Private_DMem_CreatePart(p, addres, node->size - size, DMEM_FREE, node->next_node);  // создаем второй раздел
	node->next_node = addres;         // первый раздел нацеливаем на второй
	node->size      = size;           // меняем размер первого раздела
	Private_DMem_UpdCrc(node);        // обновляем CRC первого раздела
}


// слить указанный раздел со следующим с проверкой на наличие последнего и свобоность обоих
static void Private_DMem_MargePart(dmem_heap_t* p, dmem_node_t* node)
{
	if(node->next_node == DMEM_ENDED_PART)       // если раздео крайний, выходим
		return;

	dmem_node_t *next_node_ptr = Private_DMem_GetPartPtr(p, node->next_node);      // получаем указатель на следующий раздел

	if((node->part_type != DMEM_FREE) || (next_node_ptr->part_type != DMEM_FREE))  // проверяем что разделы свободные
		return;

	/*
	 * Вырезаем раздел next_node_ptr
	 */
	node->next_node = next_node_ptr->next_node;
	node->size += next_node_ptr->size;

	Private_DMem_UpdCrc(node);        // обновляем CRC
}


// ищем последовательно расположенные пустые блоки
static dmem_node_t* Private_DMem_FindLineFreeParts(dmem_heap_t* p)
{
	uint16_t addres = 0;
	dmem_node_t* node_ptr = NULL;

	dmem_part_type_t last_part_type = DMEM_ALLOC;
	dmem_node_t* last_node_ptr = NULL;

	// перебираем все разделы
	while(addres != DMEM_ENDED_PART)
	{
		node_ptr = Private_DMem_GetPartPtr(p, addres);     // получаем указатель на описание раздела
		addres = node_ptr->next_node;                      // получем адрес следующего раздела

		if((node_ptr->part_type == DMEM_FREE) && (last_part_type == DMEM_FREE))   // если два последовательных блока пустые
			return last_node_ptr;                                                 // возвращаем первый

		last_part_type = node_ptr->part_type;         // сохраняем тип
		last_node_ptr  = node_ptr;                    // сохраняем указатель
	}

	return NULL;       // если не нашли
}


// вызываем cllback ошибки кучи
static void Private_DMem_ErrCallback(dmem_heap_t* p)
{
	if(p->cset.dmem_err_cbk_t)
		p->cset.dmem_err_cbk_t();
}






