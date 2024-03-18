/*
 * user_sd.c
 *
 * Example of initialization of SD card
 */


#include "SDCard/user_sd.h"

#ifndef USER_SD_DUMMY
sd_t sd;

#ifdef USER_SD_TEST
uint8_t sd_test_write_buf[USER_SD_TEST_DATA_LEN];
uint8_t sd_test_read_buf[USER_SD_TEST_DATA_LEN];

float sd_write_speed = 0.0f;
float sd_read_speed = 0.0f;

sd_error_code_t sd_test_ret;
uint32_t err_data_ind = 0;
uint8_t  err_period = 0;

uint32_t test_addres_blk = USER_SD_TEST_ADDR_BLOCK;
#endif
#endif

// инициализация
sd_error_code_t USER_SD_Init()
{
#ifndef USER_SD_DUMMY
	sd_init_t init = {0};
	init.cset.callback_block_thread = USER_SD_BlockThread;
	return SD_Init(&sd, &init);
#else
	return SD__OK;
#endif
}


// получить состояние
sd_state_t USER_SD_GetState()
{
#ifndef USER_SD_DUMMY
	return SD_GetState(&sd);
#else
	return SD__NO_INIT;
#endif
}


// чтение данных
sd_error_code_t USER_SD_Read(uint32_t addres_blk, uint32_t block_count, uint8_t *data)
{
#ifndef USER_SD_DUMMY
	return SD_Read(&sd, addres_blk, block_count, data);
#else
	return SD__OK;
#endif
}


// запись данных
sd_error_code_t USER_SD_Write(uint32_t addres_blk, uint32_t block_count, uint8_t *data)
{
#ifndef USER_SD_DUMMY
	return SD_Write(&sd, addres_blk, block_count, data, SD__SW_OFF);
#else
	return SD__OK;
#endif
}


// останавливаем запись
sd_error_code_t USER_SD_StopWrite()
{
#ifndef USER_SD_DUMMY
	return SD_StopWrite(&sd);
#else
	return SD__OK;
#endif
}


// тест SD карты
void USER_SD_TestReadWrite()
{
#ifndef USER_SD_DUMMY
#ifdef USER_SD_TEST
	uint32_t sd_write_time = 0;
	uint32_t sd_read_time = 0;


	for(uint32_t i = 0; i < USER_SD_TEST_DATA_LEN; i++)
		sd_test_write_buf[i] = (uint8_t)i;

	for(uint32_t i = 0; i < USER_SD_TEST_CNT; i++)
	{
		sd_test_ret = SD_TestReadWrite(&sd, test_addres_blk, USER_SD_TEST_BLOCK_COUNT, sd_test_write_buf, sd_test_read_buf, &sd_write_time, &sd_read_time);
		if(sd_test_ret != SD__OK)
			return;

		for(uint32_t j = 0; j < USER_SD_TEST_DATA_LEN; j++)
		{
			if(sd_test_write_buf[j] != sd_test_read_buf[j])
			{
				sd_test_ret = SD__WRONG_DATA;
				err_data_ind = j;
				err_period = i;
				return;
			}
		}
	}

	test_addres_blk++;


	if((!sd_write_time) || (!sd_read_time))
		return;

	sd_write_speed = (float)USER_SD_TEST_DATA_LEN / (float)sd_write_time;
	sd_read_speed = (float)USER_SD_TEST_DATA_LEN / (float)sd_read_time;
#endif
#endif
}


// функция блокировки потока
void USER_SD_BlockThread()
{
#ifndef USER_SD_DUMMY
	while(!sd.var.lock_counter);
	if(sd.var.lock_counter)
		sd.var.lock_counter--;
#endif
}


// функция освобожения потока
void USER_SD_ReleaseThread()
{
#ifndef USER_SD_DUMMY
	sd.var.lock_counter++;
#endif
}






