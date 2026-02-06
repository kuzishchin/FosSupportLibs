/**************************************************************************//**
 * @file      msg.h
 * @brief     Message buffer for 32 bits data. Header file.
 * @version   V1.0.00
 * @date      06.02.2026
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

#ifndef CIRCBUF_MSG32_H_
#define CIRCBUF_MSG32_H_


#include <stdint.h>
#include <stddef.h>


// перечисление кодов ошибок
typedef enum
{
	MSG32__OK = 0,                      // ОК
	MSG32__NULL_POINTER,                // null pointer
	MSG32__WRONG_ARG,                   // wrong arg
	MSG32__OVF,                         // overflow
	MSG32__EMPTY,                       // no data

} msg32_ret_t;


typedef struct
{
	uint32_t *buf_ptr;      // puffer pointer
	uint16_t buf_size;      // buffer size in uint32_t elements

	uint16_t start_ind;     // index to read data
	uint16_t end_ind;       // index to write data
	uint16_t cnt;           // counter of elements

} msg32_t;


// initialize
msg32_ret_t Msg32_Initialize(msg32_t* p, uint32_t* buf_ptr, uint16_t buf_size);

// write data
msg32_ret_t Msg32_WriteData(msg32_t* p, uint32_t data);

// read index
msg32_ret_t Msg32_ReadData(msg32_t* p, uint32_t* data_ptr);

// write in thread safe mode
msg32_ret_t Msg32_WriteDataProtected(msg32_t* p, uint32_t data);

// read in thread safe mode
msg32_ret_t Msg32_WriteReadProtected(msg32_t* p, uint32_t* data_ptr);



#endif /* CIRCBUF_MSG32_H_ */























