/**************************************************************************//**
 * @file      sd_types.h
 * @brief     SD card types declarations. Header file.
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

#ifndef APPLICATION_SUPPORTLIBS_SDCARD_SD_TYPES_H_
#define APPLICATION_SUPPORTLIBS_SDCARD_SD_TYPES_H_

// повторения
#define SD_RETRY_ACMD   0x10                                    // поыток на команду ACMD
#define SD_RETRY_RESET  0x20                                    // попыток на сброс
#define SD_RETRY_CHECK  0xFE                                    // попыток на проверку условий работы
#define SD_RETRY_INIT   0xFFE                                   // попыток на инициализацию
#define SD_RETRY_OCR    0xFE                                    // попыток начтение регистра OCR

// команды SD карте
#define SD_GO_IDLE_STATE        0 								// программная перезагрузка
#define SD_SEND_IF_COND         8 							    // для SDCard ver 2 - проверка условий работы
#define SD_STOP_TRANSMISSION    12                              // остановка чтения множества блоков
#define SD_READ_SINGLE_BLOCK    17 								// чтение блока данных
#define SD_READ_MULTIPLE_BLOCK  18                              // чтение множества блоков
#define SD_WRITE_SINGLE_BLOCK   24 								// запись блока данных
#define SD_WRITE_MULTIPLE_BLOCK 25                              // запись множества блоков
#define SD_SD_SEND_OP_COND      41 								// начало процесса инициализации
#define SD_APP_CMD              55 								// главная команда из ACMD команд
#define SD_READ_OCR             58 	                            // чтение регистра OCR
#define SD_ACMD23               23                              // установить число блоков для очистки перед записью

// токены
#define SD_START_TOKEN          0xFE                            // начало блока передачи данных
#define SD_START_MULT_TOKEN     0xFC                            // начало блока передачи данных в режиме многоблочной записи
#define SD_STOP_MULT_TOKEN      0xFD                            // останов передачи данных в режиме многоблочной записи

// аргументы
#define SD_SEND_IF_COND_ARG     0x000001AA                      // аргумент для команды провки условий работы
#define SD_SD_SEND_OP_COND_ARG  0x40000000                      // аргумен начала процесса инициализации

// размер блока данных в байтах
#define SD_BLOCK_SIZE           512


#include <stdint.h>
#include <stddef.h>

// выключатель
typedef enum
{
	SD__SW_OFF = 0,
	SD__SW_ON,

} sd_sw_t;


// тип карты памяти
typedef enum
{
	SD__UNKNOWN = 0,
	SD__SD,             // SD до 2 Гб включительно, FAT12 или FAT16, HS до 25 Мбит/сек
	SD__SDHC,           // HC от 2 до 32 Гб включительно, FAT32, HS и UHS-I до 104 Мбит/сек,
	                    // (больше контактов) UHS-II до 312 Мбит/сек, UHS-III до 624 Мбит/сек,
	                    // SD Express до 985 Мбит/сек
	SD__SDXC,           // XC от 32 Гб до 2 Тб включительно, exFAT, то же
	SD__SDUC,           // UC от 2 до 128 Тб, exFAT, то же

} sd_type_t;


// перечисление состояний
typedef enum
{
	SD__NO_INIT = 0,               // состояние до инициализации (разрешена только инициализация)
	SD__INIT,                      // инициализирован и в режиме ожидания

} sd_state_t;


// перечисление кодов ошибок
typedef enum
{
	SD__OK = 0,                       // ОК
	SD__NULL_POINTER,                 // нулевой указатель
	SD__NO_CARD,                      // нет карты
	SD__SPI_ERR,                      // ошибка SPI
	SD__NO_ANSWER,                    // нет ответа
	SD__ILLEGAL_CMD,                  // неверная команда
	SD__WRONG_COND,                   // неверные условия
	SD__INIT_ERROR,                   // ошибка инициализации
	SD__WRITE_ERR,                    // ошибка записи
	SD__WRONG_STATE,                  // неверное состояние
	SD__READ_ERROR,                   // ошибка чтения
	SD__WRONG_DATA,                   // неверные данные

} sd_error_code_t;


// описание callback функции блокировки потока
typedef void (*SD_CallbackBlockThread)();


// константные настройки
typedef struct
{
	SD_CallbackBlockThread callback_block_thread;        // callback функция блокировки потока

} sd_cset_t;


// инициализация
typedef struct
{
	sd_cset_t cset;

} sd_init_t;


// переменные
typedef struct
{
	uint8_t    ver;
	sd_type_t  type;
	sd_state_t state;
	sd_sw_t    stream_write;
	volatile uint8_t  lock_counter;
	volatile uint32_t next_addres_blk;

} sd_var_t;


// отладка
typedef struct
{
	volatile uint8_t t;

} sd_dbg_t;


// основная структура
typedef struct
{
	sd_cset_t cset;
	sd_var_t  var;
	sd_dbg_t  dbg;

} sd_t;



#endif /* APPLICATION_SUPPORTLIBS_SDCARD_SD_TYPES_H_ */





