/*
 * Copyright (c) 2021 DIY365
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MG_BTHING_TYPES_H_
#define MG_BTHING_TYPES_H_


#ifdef __cplusplus
extern "C" {
#endif

#define MGOS_BTHING_TYPE_ANY 0        // 00
#define MGOS_BTHING_TYPE_SENSOR 1     // 01
#define MGOS_BTHING_TYPE_ACTUATOR 3   // 11

/****************************************
 * SENSORS
 * 00000000-00000000-00000000-0000|00|01
 ****************************************/

// The MGOS_BSENSOR_TYPE is the base for all other sensor's TYPES
// 00000000-00000000-00000000-0000|01|00
#define MGOS_BSENSOR_TYPE (4 | MGOS_BTHING_TYPE_SENSOR)

// 00000000-00000000-00000000-0001|00|00
#define MGOS_BBSENSOR_TYPE (16 | MGOS_BSENSOR_TYPE)

// 00000000-00000000-00000000-0010|00|00
#define MGOS_BBUTTON_TYPE (32 | MGOS_BSENSOR_TYPE)

// 00000000-00000000-00000000-0100|00|00
#define MGOS_BFLOWSENSOR_TYPE (64 | MGOS_BSENSOR_TYPE)



/****************************************
 * ACTUATORS
 * 00000000-00000000-00000000-0000|00|11
 ****************************************/

// The MGOS_BACTUATOR_TYPE is the base for all other actuator's TYPES
// 00000000-00000000-00000000-0000|10|00
#define MGOS_BACTUATOR_TYPE (8 | MGOS_BTHING_TYPE_ACTUATOR | MGOS_BSENSOR_TYPE)

// 00000000-00000000-00000000-0001|00|00
#define MGOS_BBACTUATOR_TYPE (16 | MGOS_BACTUATOR_TYPE)

// 00000000-00000000-00000000-0010|00|00
#define MGOS_BSWITCH_TYPE (32 | MGOS_BACTUATOR_TYPE)

// 00000000-00000000-00000000-0100|00|00
#define MGOS_BVALVE_TYPE (64 | MGOS_BACTUATOR_TYPE)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MG_BTHING_TYPES_H_ */