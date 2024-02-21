/*
 * user_sd_platform.c
 *
 * Example of realizations of platform dependent functions
 */


#include "main.h"

#define SD_TIMEOUT 1000

SPI_HandleTypeDef* sd_spi_port = NULL;


// установить пин cs в состояние
void sd_platform_set_cs_state(uint8_t state)
{
	HAL_GPIO_WritePin(SPI3_CS_SD_GPIO_Port, SPI3_CS_SD_Pin, (GPIO_PinState)state);
}


// проверка наличия карты
// 0 - нет, 1 - есть
uint8_t sd_platform_is_inserted()
{
	return 1;
}


// регистрация порта
void sd_platform_port_reg(void* port)
{
	sd_spi_port = port;
}


// передать данные
// 0 - упех, 1 - ошибка
uint8_t sd_platform_spi_transmit(uint8_t *tx_data, uint16_t size)
{
	if(HAL_SPI_Transmit(sd_spi_port, tx_data, size, SD_TIMEOUT) != HAL_OK)
		return 1;
	return 0;
}


// передать данные асинхронно
// 0 - упех, 1 - ошибка
uint8_t sd_platform_spi_transmit_async(uint8_t *tx_data, uint16_t size)
{
	if(HAL_SPI_Transmit_DMA(sd_spi_port, tx_data, size) != HAL_OK)
		return 1;
	return 0;
}


// получить данные
// 0 - упех, 1 - ошибка
uint8_t sd_platform_spi_receive(uint8_t *rx_data, uint16_t size)
{
	if(HAL_SPI_Receive(sd_spi_port, rx_data, size, SD_TIMEOUT) != HAL_OK)
		return 1;
	return 0;
}


// передать и получить данные
// 0 - упех, 1 - ошибка
uint8_t sd_platform_spi_transmit_receive(uint8_t *tx_data, uint8_t *rx_data, uint16_t size)
{
	if(HAL_SPI_TransmitReceive(sd_spi_port, tx_data, rx_data, size, SD_TIMEOUT) != HAL_OK)
		return 1;
	return 0;
}


// передать и получить данные асинхронно
// 0 - упех, 1 - ошибка
uint8_t sd_platform_spi_transmit_receive_async(uint8_t *tx_data, uint8_t *rx_data, uint16_t size)
{
	if(HAL_SPI_TransmitReceive_DMA(sd_spi_port, tx_data, rx_data, size) != HAL_OK)
		return 1;
	return 0;
}


// отправить и приянть байт
uint8_t sd_platform_spi_tx_rx_byte(uint8_t tx)
{
	uint8_t rx;
	sd_platform_spi_transmit_receive(&tx, &rx, 1);
	return rx;
}


// отправить байт
void sd_platform_spi_tx_byte(uint8_t tx)
{
	sd_platform_spi_tx_rx_byte(tx);
}


// принять байт
uint8_t sd_platform_spi_rx_byte()
{
	return sd_platform_spi_tx_rx_byte(0xFF);
}


// освободить spi
void sd_platform_spi_release()
{
	sd_platform_spi_tx_rx_byte(0xFF);
}


// функция перевода шины spi на низкую скорость
// устанавливает скорость 164 Кбит/сек
// 0 - упех, 1 - ошибка
uint8_t sd_platform_spi_slow_mode()
{
	sd_spi_port->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	if (HAL_SPI_Init(sd_spi_port) != HAL_OK)
		return 1;
	return 0;
}


// функция переводашины spi на высокую скорось (по умолчанию: высокая скорость)
// устанваливает скорость 21 Мбит/сек
// 0 - упех, 1 - ошибка
uint8_t sd_platform_spi_fast_mode()
{
	sd_spi_port->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	if (HAL_SPI_Init(sd_spi_port) != HAL_OK)
		return 1;
	return 0;
}

