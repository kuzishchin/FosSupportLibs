/**************************************************************************//**
 * @file      sd.h
 * @brief     SD card driver. Header file.
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

#ifndef APPLICATION_SUPPORTLIBS_SDCARD_SD_H_
#define APPLICATION_SUPPORTLIBS_SDCARD_SD_H_


#include "SDCard/sd_spi.h"


// инициализация
sd_error_code_t SD_Init(sd_t *ptr, sd_init_t *init);

// получить состояние
sd_state_t SD_GetState(sd_t *ptr);

// чтение данных
sd_error_code_t SD_Read(sd_t *ptr, uint32_t addres_blk, uint32_t block_count, uint8_t *data);

// запись данных
sd_error_code_t SD_Write(sd_t *ptr, uint32_t addres_blk, uint32_t block_count, uint8_t *data, sd_sw_t stop_flag);

// останавливаем запись
sd_error_code_t SD_StopWrite(sd_t *ptr);

// тестовое чтение и запись
sd_error_code_t SD_TestReadWrite(sd_t *ptr, uint32_t addres_blk, uint32_t block_count, uint8_t *wr_data, uint8_t *rd_data, uint32_t* write_time, uint32_t* read_time);

// сброс карты памяти
static sd_error_code_t Private_SD_Reset(sd_t *ptr, uint8_t retry);

// проверить условия работы карты
static sd_error_code_t Private_SD_CheckCond(sd_t *ptr, uint8_t retry);

// инициализация карты памяти
static sd_error_code_t Private_SD_Init(sd_t *ptr, uint16_t retry);

// чтение параметров карты памяти
static sd_error_code_t Private_SD_ReadOCR(sd_t *ptr, uint16_t retry);




#endif /* APPLICATION_SUPPORTLIBS_SDCARD_SD_H_ */






