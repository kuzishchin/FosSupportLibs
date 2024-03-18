/**************************************************************************//**
 * @file      sd_spi.c
 * @brief     SD card protocol over SPI. Source file.
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

#include "SDCard/sd_spi.h"


// ожидание пока карта занята
static sd_error_code_t Private_SD_SPI_WaitWhileBusy(uint16_t retry);

// ожидание отклика
static uint8_t Private_SD_SPI_WaitResponse(uint8_t retry);

// ожидание данных
static sd_error_code_t Private_SD_SPI_WhaitData(uint16_t timeout);


// инициализация протокола обмена данными с SD картой по SPI
sd_error_code_t SD_SPI_Init()
{
	if(sd_platform_spi_slow_mode() != 0)       // переход к низкой скорости
		return SD__SPI_ERR;

	for(uint8_t i = 0; i < 10; i++)            // сигнали синхронизации не менее 74 раз
		sd_platform_spi_release();

	if(sd_platform_spi_fast_mode() != 0)       // возвращаем высокую скорость
		return SD__SPI_ERR;

	return SD__OK;
}


/*
 * Чтение множеста блоков
 * ptr - sd_t
 * addres_blk - адрес начального блока для чтения в блоках (блоки по SD_BLOCK_SIZE байт)
 * block_count - число блоков для чтения
 * data - прочитанные данные
 * return - успех или код  ошибки
 */
sd_error_code_t SD_SPI_ReadBlockMult(sd_t *ptr, uint32_t addres_blk, uint32_t block_count, uint8_t *data)
{
	static uint8_t ans[SD_BLOCK_SIZE] = {0xFF};

	if(ptr == NULL)
		return SD__NULL_POINTER;

	if(ptr->var.stream_write == SD__SW_ON)
		return SD__WRONG_STATE;

	sd_error_code_t ret;

	// отправляем команду на чтение блоков данных
	if(SD_SPI_SendCmd(ptr, SD_READ_MULTIPLE_BLOCK, addres_blk))
		return SD__READ_ERROR;

	SD_CS_Enable;         // начало сеанса связи

	// чтение данных
	for(uint32_t j = 0;  j < block_count; j++)
	{
		// ожидание данных
		ret = Private_SD_SPI_WhaitData(0xFFFF);
		if(ret != SD__OK)
		{
			SD_CS_Disable
			return ret;
		}

		// читаем данные
		if(ptr->cset.callback_block_thread == NULL)
		{
			sd_platform_spi_transmit_receive(ans, &data[j*SD_BLOCK_SIZE], SD_BLOCK_SIZE);
		}else
		{
			sd_platform_spi_transmit_receive_async(ans, &data[j*SD_BLOCK_SIZE], SD_BLOCK_SIZE);
			ptr->cset.callback_block_thread();
		}

		// приём пустых байт
		sd_platform_spi_release();
		sd_platform_spi_release();
		sd_platform_spi_release();
	}

	// отправляем команду останова чтения блоков
	SD_SPI_SendCmd(ptr, SD_STOP_TRANSMISSION, 0);
	SD_CS_Disable;

	return SD__OK;
}


/*
 * Переход в режим потоковой записи
 * ptr - sd_t
 * addres_blk - адрес начала записи в номере блока (блоки по SD_BLOCK_SIZE байт)
 * block_count - число юлоков на запись
 * return - успех или код  ошибки
 */
sd_error_code_t SD_SPI_StartStreamWrite(sd_t *ptr, uint32_t addres_blk, uint32_t block_count)
{
	if(ptr == NULL)
		return SD__NULL_POINTER;

	if(ptr->var.stream_write == SD__SW_ON)
		return SD__WRONG_STATE;

	// отправляем команду для предварительной очистки блоков перед записью
	if(SD_SPI_SendAcmd(ptr, SD_ACMD23, block_count))
		return SD__WRITE_ERR;

	// отправляем команду записи с адресом в блоках
	if(SD_SPI_SendCmd(ptr, SD_WRITE_MULTIPLE_BLOCK, addres_blk))
		return SD__WRITE_ERR;

	SD_CS_Enable;         // начало сеанса связи

	ptr->var.stream_write = SD__SW_ON;      // ставим флаг потокового режима

	return SD__OK;
}



/*
 * Запись блоков в потоковом режиме
 * ptr - sd_t
 * block_count - число блоков на запись (блоки по SD_BLOCK_SIZE байт)
 * data - даные для записи
 * return - успех или код  ошибки
 */
sd_error_code_t SD_SPI_StreamWrite(sd_t *ptr, uint32_t block_count, uint8_t *data)
{
	if(ptr == NULL)
		return SD__NULL_POINTER;

	if(ptr->var.stream_write != SD__SW_ON)          // проверяем флаг потокового режима
		return SD__WRONG_STATE;

	sd_error_code_t ret;
	uint8_t responce;

	// записываем count блоков
	for(uint32_t i = 0; i < block_count; i++)
	{
		//ожидаем окончание записи блока данных
		ret = Private_SD_SPI_WaitWhileBusy(0xFFFF);
		if(ret != SD__OK)
			return ret;

		sd_platform_spi_tx_byte(SD_START_MULT_TOKEN);                        // START токен

		// передаём данные
		if(ptr->cset.callback_block_thread == NULL)
		{
			sd_platform_spi_transmit(&data[i*SD_BLOCK_SIZE], SD_BLOCK_SIZE);
		}else
		{
			sd_platform_spi_transmit_async(&data[i*SD_BLOCK_SIZE], SD_BLOCK_SIZE);
			ptr->cset.callback_block_thread();
		}

		//читаем 2 байта CRC без его проверки
		sd_platform_spi_release();
		sd_platform_spi_release();

		responce = sd_platform_spi_rx_byte();            // получаем ответ

		// обработка ошибки
		if((responce & 0x1F) != 0x05)
		{
			SD_CS_Disable;
			return SD__WRITE_ERR;
		}
	}

	return SD__OK;
}


/*
 * Останов поточной записи данных
 */
sd_error_code_t SD_SPI_StopStreamWrite(sd_t *ptr)
{
	if(ptr == NULL)
		return SD__NULL_POINTER;

	if(ptr->var.stream_write != SD__SW_ON)          // проверяем флаг потокового режима
		return SD__WRONG_STATE;

	sd_error_code_t ret;

	//ожидаем окончание записи блока данных
	ret = Private_SD_SPI_WaitWhileBusy(0xFFFF);
	if(ret != SD__OK)
	{
		SD_CS_Disable;
		return ret;
	}

	sd_platform_spi_release();                       // задержка
	sd_platform_spi_tx_byte(SD_STOP_MULT_TOKEN);     // СТОП токен

	// ожидание ответа
	if(Private_SD_SPI_WaitResponse(0xFF) == 0xFF)    // если таймаут
	{
		SD_CS_Disable;                               // завершение сенса связи
		return SD__NO_ANSWER;                        // возвращаем ошибку
	}

	// ожидание заврешения работы с картой
	ret = Private_SD_SPI_WaitWhileBusy(0xFFFF);
	if(ret != SD__OK)     // если таймаут
	{
		SD_CS_Disable;                               // завершение сенса связи
		return ret;                                  // возвращаем ошибку
	}

	SD_CS_Disable;                                   // завершение сенса связи

	ptr->var.stream_write = SD__SW_OFF;              // сбрасываем флаг

	return SD__OK;
}

/**
 * Послать команду
 * ptr - sd_t
 * cmd - команда
 * arg - аргумент
 * return - отклик карты
 */
uint8_t SD_SPI_SendCmd(sd_t *ptr, uint8_t cmd, uint32_t arg)
{
	uint8_t response, tmp8;
	uint8_t crc_byte;
	uint32_t tmp32;

	sd_var_t *v = &ptr->var;

	SD_CS_Enable;                   // активируем сеанс связи

	// коррекция адреса для SD карт
	if(v->type == SD__SD)
		if(cmd == SD_READ_SINGLE_BLOCK || cmd == SD_WRITE_SINGLE_BLOCK || cmd == SD_READ_MULTIPLE_BLOCK || cmd == SD_WRITE_MULTIPLE_BLOCK)
			arg = arg << 9;

	// определяем CRC (важно для двух команд)
	if(cmd == SD_SEND_IF_COND)
		crc_byte = 0x87;
	else
		crc_byte = 0x95;

	// проверка занятости карты
	sd_error_code_t err = Private_SD_SPI_WaitWhileBusy(0xFFFF);
	if(err != SD__OK)
		return 0xFF;

	/*
	 * Формат команды
	 * Бит 47: всегда 0
	 * Бит 46: всегда 1
	 * Биты 45-40: индекс команды
	 * Биты 38-8: аргумент
	 * Биты: 7-1: CRC7
	 * Бит 0: всегда 1
	 */

	// передаем команду, аргумент и CRC
	sd_platform_spi_tx_byte(cmd | 0x40);
	sd_platform_spi_tx_byte(arg>>24);
	sd_platform_spi_tx_byte(arg>>16);
	sd_platform_spi_tx_byte(arg>>8);
	sd_platform_spi_tx_byte(arg);
	sd_platform_spi_tx_byte(crc_byte);

	// ожидание ответа
	response = Private_SD_SPI_WaitResponse(0xFF);

	// проверка ответа если посылалась команда SD_SEND_IF_COND
	if(response == 0x01 && cmd == SD_SEND_IF_COND)
	{
		tmp32 = sd_platform_spi_rx_byte() << 24;
		tmp32 |= sd_platform_spi_rx_byte() << 16;
		tmp32 |= sd_platform_spi_rx_byte() << 8;
		tmp32 |= sd_platform_spi_rx_byte();

		// проверка ответа
		if(tmp32 != SD_SEND_IF_COND_ARG)
			return 0xFF;
	}

	// проверка ответа если посылалась команда SD_READ_OCR
	if(response == 0x00 && cmd == SD_READ_OCR)
	{
		tmp8 = sd_platform_spi_rx_byte();  //прочитать один байт регистра OCR

		 //прочитать три оставшихся байта регистра OCR
	    sd_platform_spi_rx_byte();
	    sd_platform_spi_rx_byte();
	    sd_platform_spi_rx_byte();

	    // если не установилось питание
	    if(!(tmp8 & 0x80))
	    	return 0xFF;

		// узнаем тип карты
	    if(tmp8 & 0x40)
	    	v->type = SD__SDHC;            // обнаружена карта SDHC
	    else
	    	v->type = SD__SD;              // обнаружена карта SD
	 }

	SD_CS_Disable;      // завершение сеанса связи

	return response;    // возвращает ответ карты
}


/*
 * Послать расширенную команду
 * ptr - sd_t
 * acmd - индекс команды
 * arg  - аргумент
 * return - отклик карты
 */
uint8_t SD_SPI_SendAcmd(sd_t *ptr, uint8_t acmd, uint32_t arg)
{
/*	uint8_t retry = SD_RETRY_ACMD;

	while(SD_SPI_SendCmd(ptr, SD_APP_CMD, 0) != 0x01)
	    if(retry-- < 0x01)
	    	return 0xFF;*/
	SD_SPI_SendCmd(ptr, SD_APP_CMD, 0);
	return SD_SPI_SendCmd(ptr, acmd, arg);
}


// ожидание пока карта занята
static sd_error_code_t Private_SD_SPI_WaitWhileBusy(uint16_t retry)
{
	while(!sd_platform_spi_rx_byte())        // до тех пор пока карта возвращает ноль
		if(retry-- < 0x0001)                 // таймаут
			return SD__NO_ANSWER;            // возвращаем ошибку

	return SD__OK;                           // возвращаем ОК
}


// ожидание отклика
static uint8_t Private_SD_SPI_WaitResponse(uint8_t retry)
{
	uint8_t response;                                         // отклик

	while((response = sd_platform_spi_rx_byte()) == 0xFF)     // ожидание отлчиного от 0xFF отклика
		if(retry-- < 0x01)                                    // если таймаут
			return response;                                  // возвращаем 0xFF

	return response;                                          // возвращаеи ответ
}


// ожидание данных
static sd_error_code_t Private_SD_SPI_WhaitData(uint16_t timeout)
{
	while(sd_platform_spi_rx_byte() != SD_START_TOKEN)  // ожидание токена
		if(timeout-- < 0x0001)                          // таймаут
			return SD__NO_ANSWER;                       // возвращаем ошибку

	return SD__OK;                                      // возвращаем ОК
}




