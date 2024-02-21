/**************************************************************************//**
 * @file      sd.c
 * @brief     SD card driver. Source file.
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

#include "SDCard/sd.h"
#include "Platform/sl_platform.h"
#include <string.h>


// инициализация
sd_error_code_t SD_Init(sd_t *ptr, sd_init_t *init)
{
	if((ptr == NULL) || (init == NULL))
		return SD__NULL_POINTER;

	memcpy(&ptr->cset, &init->cset, sizeof(sd_cset_t));
	memset(&ptr->var, 0, sizeof(sd_var_t));
	memset(&ptr->dbg, 0, sizeof(sd_dbg_t));

	// проверка наличтя карты памяти
	if(sd_platform_is_inserted() == 0)
		return SD__NO_CARD;

	// инициализация протокала SPI
	sd_error_code_t ret = SD_SPI_Init();
	if(ret != SD__OK)
		return ret;

	// сброс карты памяти
	ret = Private_SD_Reset(ptr, SD_RETRY_RESET);
	if(ret != SD__OK)
		return ret;

	// проверка условий работы
	ret = Private_SD_CheckCond(ptr, SD_RETRY_CHECK);
	if(ret == SD__WRONG_COND)                          // если невреные условия
		return ret;                                    // выходим

	if(ret == SD__ILLEGAL_CMD)                         // если команда не поддерживается
	{
		/*
		 * TO DO:
		 * Сделать инициализацию старой версии
		 */
		ptr->var.ver = 1;

		// а пока выходим с ошибкой
		return ret;
	}

	/*
	 * Если дошли сюда значит считаем что карта новой версии
	 */
	ptr->var.ver = 2;

	/*
	 * Инициализация карты памяти
	 */
	ret = Private_SD_Init(ptr, SD_RETRY_INIT);
	if(ret != SD__OK)
		return ret;

	ret = Private_SD_ReadOCR(ptr, SD_RETRY_OCR);
	if(ret != SD__OK)
		return ret;

	ptr->var.state = SD__INIT;

	return SD__OK;
}


// получить состояние
sd_state_t SD_GetState(sd_t *ptr)
{
	if(ptr == NULL)
		return SD__NO_INIT;

	return ptr->var.state;
}


/*
 * Чтение данных
 * ptr - sd_t
 * addres_blk - начало чтения данных в блоках (блоки по SD_BLOCK_SIZE байт)
 * block_count - число блоков для чтения
 * data - прочитанные данные
 * return - успех или код  ошибки
 */
sd_error_code_t SD_Read(sd_t *ptr, uint32_t addres_blk, uint32_t block_count, uint8_t *data)
{
	if((ptr == NULL) || (data == NULL))
		return SD__NULL_POINTER;

	if(ptr->var.state != SD__INIT)
		return SD__WRONG_STATE;

	sd_error_code_t ret;

	// если стоит флаг потоковой записи
	if(ptr->var.stream_write == SD__SW_ON)
	{
		ret = SD_SPI_StopStreamWrite(ptr);            // останавливаем
		if(ret != SD__OK)
			return ret;
	}

	// считываем count блоков
	ret = SD_SPI_ReadBlockMult(ptr, addres_blk, block_count, data);
	if(ret != SD__OK)
		return ret;

	return SD__OK;
}


/*
 * Запись данных
 * ptr - sd_t
 * addres_blk - начало чтения данных в блоках (блоки по SD_BLOCK_SIZE байт)
 * block_count - число блоков для чтения
 * data - записываемые данные
 * stop_flag - сразу закончить потоковую запись
 * return - успех или код  ошибки
 */
sd_error_code_t SD_Write(sd_t *ptr, uint32_t addres_blk, uint32_t block_count, uint8_t *data, sd_sw_t stop_flag)
{
	if((ptr == NULL) || (data == NULL))
		return SD__NULL_POINTER;

	if(ptr->var.state != SD__INIT)
		return SD__WRONG_STATE;

	sd_error_code_t ret = SD__OK;

	/*
	 * Проверяем уловие потоковой отправки данных:
	 * если новый адрес не очередной, останавливаем поточную передачу данных
	 */
	if((ptr->var.next_addres_blk != addres_blk) && (ptr->var.stream_write == SD__SW_ON))
	{
		ret = SD_SPI_StopStreamWrite(ptr);
		if(ret != SD__OK)
			return ret;
	}


	// начинаем потоковую запись, если ее нет
	if(ptr->var.stream_write != SD__SW_ON)
	{
		ret = SD_SPI_StartStreamWrite(ptr, addres_blk, block_count);
		if(ret != SD__OK)
			return ret;
	}

	// пишем данные
	ret = SD_SPI_StreamWrite(ptr, block_count, data);
	if(ret != SD__OK)
		return ret;

	// останавливаем потоковую запись, если стоит флаг
	if((stop_flag == SD__SW_ON) && (ptr->var.stream_write == SD__SW_ON))
	{
		ret = SD_SPI_StopStreamWrite(ptr);
		if(ret != SD__OK)
			return ret;
	}

	// обновляем адрес следующего блока для проверки потоковой передачи данных
	ptr->var.next_addres_blk = addres_blk + block_count;

	return SD__OK;
}


// останавливаем запись
sd_error_code_t SD_StopWrite(sd_t *ptr)
{
	if(ptr->var.stream_write == SD__SW_ON)
	{
		sd_error_code_t ret = SD_SPI_StopStreamWrite(ptr);
		if(ret != SD__OK)
			return ret;
	}

	return SD__OK;
}


/*
 * Тестовая запись и чтение
 */
sd_error_code_t SD_TestReadWrite(sd_t *ptr, uint32_t addres_blk, uint32_t block_count, uint8_t *wr_data, uint8_t *rd_data, uint32_t* write_time, uint32_t* read_time)
{
	if((ptr == NULL) || (wr_data == NULL) || (rd_data == NULL) || (write_time == NULL) || (read_time == NULL))
		return SD__NULL_POINTER;

	uint32_t ts = SL_GetTick();

	sd_error_code_t ret = SD_Write(ptr, addres_blk, block_count, wr_data, SD__SW_OFF);
	if(ret != SD__OK)
		return ret;

	*write_time = SL_GetTick() - ts;

	ts = SL_GetTick();

	ret = SD_Read(ptr, addres_blk, block_count, rd_data);
	if(ret != SD__OK)
		return ret;

	*read_time = SL_GetTick() - ts;

	return SD__OK;
}


// сброс карты памяти
static sd_error_code_t Private_SD_Reset(sd_t *ptr, uint8_t retry)
{
	while(SD_SPI_SendCmd(ptr, SD_GO_IDLE_STATE, 0) != 0x01)
	    if(retry-- < 0x01)
	    	return SD__NO_ANSWER;

	return SD__OK;
}


// проверить условия работы карты
static sd_error_code_t Private_SD_CheckCond(sd_t *ptr, uint8_t retry)
{
	uint8_t responce;
	while(responce = SD_SPI_SendCmd(ptr, SD_SEND_IF_COND, SD_SEND_IF_COND_ARG) != 0x01)
	{
	    if(retry-- < 0x01)
	    {
	    	if(responce == 0xFF)
	    		return SD__WRONG_COND;
	    	else
	    		return SD__ILLEGAL_CMD;
	    }
	}

	return SD__OK;
}


// инициализация карты памяти
static sd_error_code_t Private_SD_Init(sd_t *ptr, uint16_t retry)
{

	while(SD_SPI_SendAcmd(ptr, SD_SD_SEND_OP_COND, SD_SD_SEND_OP_COND_ARG) != 0x00)
		if(retry-- < 0x01)
			return SD__INIT_ERROR;

	return SD__OK;
}


// чтение параметров карты памяти
static sd_error_code_t Private_SD_ReadOCR(sd_t *ptr, uint16_t retry)
{
	while(SD_SPI_SendCmd(ptr, SD_READ_OCR, 0) != 0x00)
		if(retry-- < 0x01)
			return SD__NO_ANSWER;

	return SD__OK;
}





