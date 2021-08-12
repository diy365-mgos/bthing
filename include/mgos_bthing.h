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
#include "mgos_bvar.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mg_bthing;
struct mg_bthing_enum;


/* Generic and opaque bThing instance */
typedef struct mg_bthing *mgos_bthing_t;

/* bThings enumerator returned by mgos_bthing_get_all() */
typedef struct mg_bthing_enum *mgos_bthing_enum_t;

#define MGOS_BTHING_TYPE_ANY 0
#define MGOS_BTHING_TYPE_SENSOR 1
#define MGOS_BTHING_TYPE_ACTUATOR 3

#ifdef MGOS_BTHING_HAVE_ACTUATOR 
#define MGOS_BTHING_HAVE_SENSORS 1
#define MGOS_BTHING_HAVE_ACTUATORS 1
#else
  #ifdef MGOS_BTHING_HAVE_SENSOR
  #define MGOS_BTHING_HAVE_SENSORS 1
  #define MGOS_BTHING_HAVE_ACTUATORS 0
  #else
  #define MGOS_BTHING_HAVE_SENSORS 0
  #define MGOS_BTHING_HAVE_ACTUATORS 0
  #endif 
#endif

#define MGOS_BTHING_ENV_DEVICEID "${device_id}"
#define MGOS_BTHING_ENV_THINGID "${bthing_id}"

#define MGOS_BTHING_STR_ON "ON"
#define MGOS_BTHING_STR_OFF "OFF"

#define MGOS_BTHING_NO_TICKS 0
#define MGOS_BTHING_NO_PIN -1

#define MGOS_BTHING_EVENT_BASE MGOS_EVENT_BASE('B', 'T', 'G')
#define MGOS_EV_BTHING_ANY MGOS_BTHING_EVENT_BASE
enum mgos_bthing_event {
  MGOS_EV_BTHING_CREATED = MGOS_BTHING_EVENT_BASE,
  MGOS_EV_BTHING_STATE_CHANGING,
  MGOS_EV_BTHING_STATE_CHANGED,
  MGOS_EV_BTHING_STATE_UPDATED
};

/* Returns the ID of a *bThing*, or `NULL` if error. */
const char *mgos_bthing_get_id(mgos_bthing_t thing);

/* Returns the type of a *bThing*, or `0` if error. */
int mgos_bthing_get_type(mgos_bthing_t thing);

/* Returns `true` if the *bThing* type is `type`. */
bool mgos_bthing_is_typeof(mgos_bthing_t thing, int type);

/* Returns the *bThing* having the specified ID, or `NULL` otherwise. */
mgos_bthing_t mgos_bthing_get(const char* id);

/* Returns the enumerator for iterating all registered *bThings*, or `NULL` if error. */
mgos_bthing_enum_t mgos_bthing_get_all();

/* Gets the next *bThing* iterating registered ones. 
 * Returns `false` if the end of the enumerator is reached, or `true` otherwise.
 */
bool mgos_bthing_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing);
/* Gets the next *bThing* of given type, iterating registered ones. 
 * Returns `false` if the end of the enumerator is reached, or `true` otherwise.
 */
bool mgos_bthing_typeof_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing, int type);

#if MGOS_BTHING_HAVE_SENSORS

enum mgos_bthing_state_flag {
  MGOS_BTHING_STATE_FLAG_UNCHANGED = 0,       // 00000
  MGOS_BTHING_STATE_FLAG_CHANGING = 1,        // 00001
  MGOS_BTHING_STATE_FLAG_CHANGED = 3,         // 00011
  MGOS_BTHING_STATE_FLAG_INITIALIZING = 5,    // 00101
  MGOS_BTHING_STATE_FLAG_INITIALIZED = 15,    // 01111
  MGOS_BTHING_STATE_FLAG_UPD_REQUESTED = 16,  // 10000
};

struct mgos_bthing_state {
  mgos_bthing_t thing;
  enum mgos_bthing_state_flag state_flags;
  mgos_bvarc_t state;
  // --------------------------------------------------
  // NOTE: if you add new fields, you must update
  // following struct as well:
  //   - struct mgos_bthing_state_change
  // --------------------------------------------------
};

struct mgos_bthing_state_change {
  mgos_bthing_t thing;
  enum mgos_bthing_state_flag state_flags;
  mgos_bvarc_t cur_state;
  // --------------------------------------------------
  // NOTE 1: above fields must be the same of:
  //   - struct mgos_bthing_state 
  //
  // NOTE 2: New fields must be added below. 
  // --------------------------------------------------
  mgos_bvarc_t new_state;
};

typedef bool (*mgos_bthing_get_state_handler_t)(mgos_bthing_t thing, mgos_bvar_t state, void *userdata);

bool mgos_bthing_on_get_state(mgos_bthing_t thing,
                              mgos_bthing_get_state_handler_t get_state_cb,
                              void *userdata);

void mgos_bthing_on_event(mgos_bthing_t thing, enum mgos_bthing_event ev,
                          mgos_event_handler_t handler, void *userdata);

mgos_bvarc_t mgos_bthing_get_state(mgos_bthing_t thing);

bool mgos_bthing_update_state(mgos_bthing_t thing);

int mgos_bthing_update_states(int bthing_type);

#endif // MGOS_BTHING_HAVE_SENSORS

#if MGOS_BTHING_HAVE_ACTUATORS

typedef bool (*mgos_bthing_set_state_handler_t)(mgos_bthing_t thing, mgos_bvarc_t state, void *userdata);

bool mgos_bthing_on_set_state(mgos_bthing_t thing,
                              mgos_bthing_set_state_handler_t set_state_cb,
                              void *userdata);

bool mgos_bthing_set_state(mgos_bthing_t thing, mgos_bvarc_t state);

#endif // MGOS_BTHING_HAVE_ACTUATORS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_BTHING_H_ */
