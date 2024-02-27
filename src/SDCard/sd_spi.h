/**************************************************************************//**
 * @file      sd_spi.h
 * @brief     SD card protocol over SPI. Header file.
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

#ifndef APPLICATION_SUPPORTLIBS_SDCARD_SD_SPI_H_
#define APPLICATION_SUPPORTLIBS_SDCARD_SD_SPI_H_



#include "SDCard/sd_platform.h"


// инициализация протокола обмена данными с SD картой по SPI
sd_error_code_t SD_SPI_Init();

// xтение множеста блоков
sd_error_code_t SD_SPI_ReadBlockMult(sd_t *ptr, uint32_t addres_blk, uint32_t block_count, uint8_t *data);

// переход в режим потоковой записи
sd_error_code_t SD_SPI_StartStreamWrite(sd_t *ptr, uint32_t addres_blk, uint32_t block_count);

// запись блоков в потоковом режиме
sd_error_code_t SD_SPI_StreamWrite(sd_t *ptr, uint32_t block_count, uint8_t *data);

// останов поточной записи данных
sd_error_code_t SD_SPI_StopStreamWrite(sd_t *ptr);

// послать команду
uint8_t SD_SPI_SendCmd(sd_t *ptr, uint8_t cmd, uint32_t arg);

// послать расширенную команду
uint8_t SD_SPI_SendAcmd(sd_t *ptr, uint8_t acmd, uint32_t arg);




#endif /* APPLICATION_SUPPORTLIBS_SDCARD_SD_SPI_H_ */




