/*
 * msg32.h
 *
 *  Created on: 19 янв. 2026 г.
 *      Author: yriyk
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























