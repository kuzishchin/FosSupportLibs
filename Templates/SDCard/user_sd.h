/*
 * user_sd.h
 *
 * Example of initialization of SD card
 */

#ifndef APPLICATION_INITIALIZATION_DEVDRIVERS_SDCARD_USER_SD_H_
#define APPLICATION_INITIALIZATION_DEVDRIVERS_SDCARD_USER_SD_H_



//#define USER_SD_DUMMY
//#define USER_SD_TEST

#ifdef USER_SD_TEST
#define USER_SD_TEST_CNT         16
#define USER_SD_TEST_ADDR_BLOCK  1000
#define USER_SD_TEST_BLOCK_COUNT 32
#define USER_SD_TEST_DATA_LEN    USER_SD_TEST_BLOCK_COUNT * SD_BLOCK_SIZE
#endif


#ifndef USER_SD_DUMMY
#include "SDCard/sd.h"
#else
#include "SDCard/sd_types.h"
#include "SDCard/sd_platform.h"
#endif


// инициализация
sd_error_code_t USER_SD_Init();

// получить состояние
sd_state_t USER_SD_GetState();

// чтение данных
sd_error_code_t USER_SD_Read(uint32_t addres_blk, uint32_t block_count, uint8_t *data);

// запись данных
sd_error_code_t USER_SD_Write(uint32_t addres_blk, uint32_t block_count, uint8_t *data);

// останавливаем запись
sd_error_code_t USER_SD_StopWrite();

// тест SD карты
void USER_SD_TestReadWrite();

// функция блокировки потока
void USER_SD_BlockThread();

// функция освобожения потока
void USER_SD_ReleaseThread();



#endif /* APPLICATION_INITIALIZATION_DEVDRIVERS_SDCARD_USER_SD_H_ */
