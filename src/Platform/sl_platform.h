/**************************************************************************//**
 * @file      sl_platform.h
 * @brief     Platform abstraction layer. Header file.
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

#ifndef APPLICATION_SUPPORTLIBS_PLATFORM_SL_PLATFORM_H_
#define APPLICATION_SUPPORTLIBS_PLATFORM_SL_PLATFORM_H_


#include <stdint.h>
#include "Platform/compiler_macros.h"


// получть системное время в мс
__weak uint32_t SL_GetTick();

// задержка
__weak void SL_Delay(uint32_t ms);

// получть системное время в мкс
__weak uint32_t SL_GetTick_us();



#endif /* APPLICATION_SUPPORTLIBS_PLATFORM_SL_PLATFORM_H_ */
