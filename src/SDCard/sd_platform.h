/**************************************************************************//**
 * @file      sd_platform.h
 * @brief     SD card platform abstraction layer. Header file.
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

#ifndef APPLICATION_SUPPORTLIBS_SDCARD_SD_PLATFORM_H_
#define APPLICATION_SUPPORTLIBS_SDCARD_SD_PLATFORM_H_


#include "SDCard/sd_types.h"


// установить пин cs в состояние
__weak void sd_platform_set_cs_state(uint8_t state);

// проверка наличия карты
// 0 - нет, 1 - есть
__weak uint8_t sd_platform_is_inserted();

// регистрация порта
__weak void sd_platform_port_reg(void* port);

// передать данные
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_transmit(uint8_t *tx_data, uint16_t size);

// передать данные асинхронно
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_transmit_async(uint8_t *tx_data, uint16_t size);

// получить данные
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_receive(uint8_t *rx_data, uint16_t size);

// передать и получить данные
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_transmit_receive(uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

// передать и получить данные асинхронно
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_transmit_receive_async(uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

// отправить и приянть байт
__weak uint8_t sd_platform_spi_tx_rx_byte(uint8_t tx);

// отправить байт
__weak void sd_platform_spi_tx_byte(uint8_t tx);

// принять байт
__weak uint8_t sd_platform_spi_rx_byte();

// освободить spi
__weak void sd_platform_spi_release();

// функция перевода шины spi на низкую скорость
// устанавливает скорость 164 Кбит/сек
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_slow_mode();

// функция переводашины spi на высокую скорось (по умолчанию: высокая скорость)
// устанваливает скорость 21 Мбит/сек
// 0 - упех, 1 - ошибка
__weak uint8_t sd_platform_spi_fast_mode();


// макросы
#define SD_CS_Enable    sd_platform_set_cs_state(0);      // CS Enable
#define SD_CS_Disable	sd_platform_set_cs_state(1);      // CS Disable



#endif /* APPLICATION_SUPPORTLIBS_SDCARD_SD_PLATFORM_H_ */






