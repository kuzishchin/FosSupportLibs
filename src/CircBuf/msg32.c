/**************************************************************************//**
 * @file      msg.h
 * @brief     Message buffer for 32 bits data. Source file.
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


#include "msg32.h"
#include <string.h>
#include "Platform/compiler_macros.h"


// inc index
static uint16_t Msg32_IncIndex(uint16_t buf_size, uint16_t ind);


// initialize
msg32_ret_t Msg32_Initialize(msg32_t* p, uint32_t* buf_ptr, uint16_t buf_size)
{
	if((p == NULL) || (buf_ptr == NULL))
		return MSG32__NULL_POINTER;
	if(buf_size == 0)
		return MSG32__WRONG_ARG;

	memset(p, 0, sizeof(msg32_t));

	p->buf_ptr  = buf_ptr;
	p->buf_size = buf_size;

	return MSG32__OK;
}


// write data
msg32_ret_t Msg32_WriteData(msg32_t* p, uint32_t data)
{
	if(p == NULL)
		return MSG32__NULL_POINTER;

	if(p->cnt >= p->buf_size)
		return MSG32__OVF;

	p->buf_ptr[p->end_ind] = data;
	p->cnt++;
	p->end_ind = Msg32_IncIndex(p->buf_size, p->end_ind);

	return MSG32__OK;
}


// read index
msg32_ret_t Msg32_ReadData(msg32_t* p, uint32_t* data_ptr)
{
	if((p == NULL) || (data_ptr == NULL))
		return MSG32__NULL_POINTER;

	if(p->cnt == 0)
		return MSG32__EMPTY;

	*data_ptr = p->buf_ptr[p->start_ind];
	p->cnt--;
	p->start_ind = Msg32_IncIndex(p->buf_size, p->start_ind);

	return MSG32__OK;
}


// write in thread safe mode
msg32_ret_t Msg32_WriteDataProtected(msg32_t* p, uint32_t data)
{
	uint32_t s;
	ENTER_CRITICAL(s);
	msg32_ret_t ret = Msg32_WriteData(p, data);
	LEAVE_CRITICAL(s);
	return ret;
}


// read in thread safe mode
msg32_ret_t Msg32_WriteReadProtected(msg32_t* p, uint32_t* data_ptr)
{
	uint32_t s;
	ENTER_CRITICAL(s);
	msg32_ret_t ret = Msg32_ReadData(p, data_ptr);
	LEAVE_CRITICAL(s);
	return ret;
}


// inc index
static uint16_t Msg32_IncIndex(uint16_t buf_size, uint16_t ind)
{
	ind++;
	if(ind >= buf_size)
		ind = 0;
	return ind;
}









