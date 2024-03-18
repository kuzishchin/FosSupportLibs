/**************************************************************************//**
 * @file      sd_platform.c
 * @brief     SD card platform abstraction layer. Source file.
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


#include "SDCard/sd_platform.h"


// установить пин cs в состояние
__weak void sd_platform_set_cs_state(uint8_t state){}


// проверка наличия карты
// 0 - нет, 1 - есть
__weak uint8_t sd_platform_is_inserted()
{
	return 0;
}

// регистрация порта
__weak void sd_platform_port_reg(void* port){}

// передать данные
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_transmit(uint8_t *tx_data, uint16_t size)
{
	return 0;
}

// передать данные асинхронно
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_transmit_async(uint8_t *tx_data, uint16_t size)
{
	return 0;
}

// получить данные
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_receive(uint8_t *rx_data, uint16_t size)
{
	return 0;
}

// передать и получить данные
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_transmit_receive(uint8_t *tx_data, uint8_t *rx_data, uint16_t size)
{
	return 0;
}

// передать и получить данные асинхронно
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_transmit_receive_async(uint8_t *tx_data, uint8_t *rx_data, uint16_t size)
{
	return 0;
}

// отправить и приянть байт
__weak uint8_t sd_platform_spi_tx_rx_byte(uint8_t tx)
{
	return 0;
}

// отправить байт
__weak void sd_platform_spi_tx_byte(uint8_t tx){}

// принять байт
__weak uint8_t sd_platform_spi_rx_byte()
{
	return 0;
}

// освободить spi
__weak void sd_platform_spi_release(){}

// функция перевода шины spi на низкую скорость
// устанавливает скорость 164 Кбит/сек
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_slow_mode()
{
	return 0;
}

// функция переводашины spi на высокую скорось (по умолчанию: высокая скорость)
// устанваливает скорость 21 Мбит/сек
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_fast_mode()
{
	return 0;
}










