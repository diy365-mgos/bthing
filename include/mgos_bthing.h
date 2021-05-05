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

#ifndef MGOS_BTHING_H_
#define MGOS_BTHING_H_

#include <stdbool.h>
#include "mgos_event.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mg_bthing;
struct mg_bthing_enum;


/* Generic and opaque bThing instance */
typedef struct mg_bthing *mgos_bthing_t;

/* bThing enumerator */
typedef struct mg_bthing_enum *mgos_bthing_enum_t;


#define MGOS_BTHING_TYPE_SENSOR 1
#define MGOS_BTHING_TYPE_ACTUATOR 3

#define MGOS_BTHING_ENV_DEVICEID "${device_id}"
#define MGOS_BTHING_ENV_THINGID "${thing_id}"

#define MGOS_BTHING_STR_ON "ON"
#define MGOS_BTHING_STR_OFF "OFF"

#define MGOS_BTHING_NO_TICKS 0
#define MGOS_BTHING_NO_PIN -1

#define MGOS_BTHING_EVENT_BASE MGOS_EVENT_BASE('B', 'T', 'N')
enum mgos_bthing_event {
  MGOS_EV_BTHING_ANY = MGOS_BTHING_EVENT_BASE, 
  MGOS_EV_BTHING_CREATED, 
  MGOS_EV_BTHING_STATE_UPDATED,
  MGOS_EV_BTHING_UPDATE_STATE
};

enum mgos_bthing_state_ev {
  MGOS_BTHING_STATE_EV_SET,
  MGOS_BTHING_STATE_EV_GET
};

enum mgos_bthing_notify_state {
  MGOS_BTHING_NOTIFY_STATE_NEVER,
  MGOS_BTHING_NOTIFY_STATE_ON_CHANGE,
  MGOS_BTHING_NOTIFY_STATE_ALWAYS,
};

/* Returns the ID of a *bThing*, or `NULL` if error. */
const char *mgos_bthing_get_id(mgos_bthing_t thing);

/* Returns the type of a *bThing*, or `0` if error. */
int mgos_bthing_get_type(mgos_bthing_t thing);

/* Returns the *bThing* having the specified ID, or `NULL` otherwise. */
mgos_bthing_t mgos_bthing_get(const char* id);

/* Returns the enumerator for iterating all registered *bThings*, or `NULL` if error. */
mgos_bthing_enum_t mgos_bthing_get_all();

/* Gets the next *bThing* iterating registered ones. 
 * Returns `false` if the end of the enumerator is reached, or `true` otherwise.
 */
bool mgos_bthing_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_BTHING_H_ */
