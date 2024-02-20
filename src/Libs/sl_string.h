/**************************************************************************//**
 * @file      sl_string.h
 * @brief     Common string functions. Header file.
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

#ifndef APPLICATION_SUPPORTLIBS_LIBS_SL_STRING_H_
#define APPLICATION_SUPPORTLIBS_LIBS_SL_STRING_H_


#include <stdint.h>
#include <stddef.h>
#include <string.h>


// число в строку по основанию
void user_itoa(int32_t _val, uint8_t base, uint8_t min_len, char *bufstr, uint8_t ur_flag);


// парсинг имени первой папки из пути к фалу
// возвращает:
// 0 - имя не найдено;
// 1 - найдено крайнее имя в пути: 2 - то же но путь заканчивается знаком '/'
// 3 - найдено очередное имя в пути
// 4 - достигнута максимальная длина пути
uint8_t parse_first_node_name(const char* path, char* folder, char* remained_path, uint8_t str_len);


#endif /* APPLICATION_SUPPORTLIBS_LIBS_SL_STRING_H_ */






