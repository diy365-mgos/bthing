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

#ifndef MG_BTHING_SDK_H_
#define MG_BTHING_SDK_H_

#include <stdbool.h>
#include <stdarg.h>
#include "mgos_bthing.h"

#ifdef __cplusplus
extern "C" {
#endif

enum mg_bthing_state_result {
  MG_BTHING_STATE_RESULT_ERROR,
  MG_BTHING_STATE_RESULT_SUCCESS,
  MG_BTHING_STATE_RESULT_UNHANDLED
};

enum mg_bthing_flag {
  MG_BTHING_FLAG_NONE = 0,              // 0000-0000
  MG_BTHING_FLAG_ISPRIVATE = 1,         // 0000-0001
  MG_BTHING_FLAG_REGISTERED = 2,        // 0000-0010
  MG_BTHING_FLAG_UPDATING_STATE = 4,    // 0000-0100
  MG_BTHING_FLAG_STATE_INITIALIZED = 8, // 0000-1000
  MG_BTHING_FLAG_FORCE_STATE_PUB = 16   // 0001-0000
};

struct mg_bthing {
  char *uid;
  char *id;
  char *domain;
  int type;
  char flags;
};

/*****************************************
 * Cast Functions
 */

// Convert (mgos_bthing_t) into (struct mg_bthing *)
struct mg_bthing *MG_BTHING_CAST1(mgos_bthing_t thing);

// Convert (struct mg_bthing *) into (mgos_bthing_t) 
 mgos_bthing_t MG_BTHING_CAST2(struct mg_bthing *thing);
/*****************************************/

#if MGOS_BTHING_HAVE_SENSORS

struct mg_bthing_sens;
typedef enum mg_bthing_state_result (*mg_bthing_getting_state_handler_t)(struct mg_bthing_sens *thing,
                                                                         mgos_bvar_t state,
                                                                         void *userdata);

struct mg_bthing_on_event_handler {
  struct mg_bthing_on_event_handler *next;
  enum mgos_bthing_event event;
  mgos_event_handler_t handler;
  void *userdata;
};

struct mg_bthing_sens {
  struct mg_bthing base;
  void *cfg;
  mg_bthing_getting_state_handler_t getting_state_cb;
  mgos_bthing_get_state_handler_t get_state_cb;
  void *get_state_ud;
  struct mg_bthing_on_event_handler *on_event;
  mgos_bvar_t state;
  mgos_bvar_t tmp_state;
};

/*****************************************
 * Cast Functions
 */

// Convert (mgos_bthing_t) into (struct mg_bthing_sens *) or NULL if conversion fails
struct mg_bthing_sens *MG_BTHING_SENS_CAST1(mgos_bthing_t thing);

// Convert (struct mg_bthing *) into (struct mg_bthing_sens *) or NULL if conversion fails
struct mg_bthing_sens *MG_BTHING_SENS_CAST2(struct mg_bthing *thing);

// Convert (struct mg_bthing_sens *) into (struct mg_bthing *)
struct mg_bthing *MG_BTHING_SENS_CAST3(struct mg_bthing_sens *thing);

// Convert (struct mg_bthing_sens *) into (mgos_bthing_t)
mgos_bthing_t MG_BTHING_SENS_CAST4(struct mg_bthing_sens *thing);
/*****************************************/

#define MG_BTHING_SENS_NEW(s) s = (struct mg_bthing_sens *)calloc(1, sizeof(struct mg_bthing_sens))

#endif // MGOS_BTHING_HAVE_SENSORS

#if MGOS_BTHING_HAVE_ACTUATORS

struct mg_bthing_actu;
typedef enum mg_bthing_state_result (*mg_bthing_setting_state_handler_t)(struct mg_bthing_actu *thing,
                                                                         mgos_bvarc_t state,
                                                                         void *userdata);

struct mg_bthing_actu {
  struct mg_bthing_sens base;
  void *cfg;
  mg_bthing_setting_state_handler_t setting_state_cb;
  mgos_bthing_set_state_handler_t set_state_cb;
  void *set_state_ud;
};

/*****************************************
 * Cast Functions
 */

// Convert (mgos_bthing_t) into (struct mg_bthing_actu *) or NULL if conversion fails
struct mg_bthing_actu *MG_BTHING_ACTU_CAST1(mgos_bthing_t thing);

// Convert (struct mg_bthing *) into (struct mg_bthing_actu *) or NULL if conversion fails
struct mg_bthing_actu *MG_BTHING_ACTU_CAST2(struct mg_bthing *thing);

// Convert (struct mg_bthing_actu *) into (struct mg_bthing_sens *)
struct mg_bthing_sens *MG_BTHING_ACTU_CAST3(struct mg_bthing_actu *thing);

// Convert (struct mg_bthing_actu *) into (struct mg_bthing *)
struct mg_bthing *MG_BTHING_ACTU_CAST4(struct mg_bthing_actu *thing);

// Convert (struct mg_bthing_actu *) into (mgos_bthing_t)
mgos_bthing_t MG_BTHING_ACTU_CAST5(struct mg_bthing_actu *thing);
/*****************************************/

#define MG_BTHING_ACTU_NEW(a) a = (struct mg_bthing_actu *)calloc(1, sizeof(struct mg_bthing_actu))

#endif // MGOS_BTHING_HAVE_ACTUATORS

struct mg_bthing_enum {
  mgos_bthing_t thing;
  struct mg_bthing_enum *next_item;
};

struct mg_bthing_ctx {
  struct mg_bthing_enum things;
};

struct mg_bthing_ctx *mg_bthing_context();

bool mg_bthing_init(struct mg_bthing *thing, const char *id, int type, const char *domain);

void mg_bthing_reset(struct mg_bthing *thing);

void mg_bthing_set_flag(mgos_bthing_t thing, enum mg_bthing_flag flag);
void mg_bthing_reset_flag(mgos_bthing_t thing, enum mg_bthing_flag flag);
bool mg_bthing_has_flag(mgos_bthing_t thing, enum mg_bthing_flag flag);

#if MGOS_BTHING_HAVE_SENSORS

bool mg_bthing_sens_init(struct mg_bthing_sens *sens, void *cfg);

void mg_bthing_sens_reset(struct mg_bthing_sens *sens);

// Returns the readonly in-memory state of the bThing
mgos_bvarc_t mg_bthing_get_raw_state(mgos_bthing_t thing);

// Returns the updatable in-memory state of the bThing
mgos_bvar_t mg_bthing_get_state_4update(mgos_bthing_t thing);

mg_bthing_getting_state_handler_t mg_bthing_on_getting_state(struct mg_bthing_sens *sens, 
                                                             mg_bthing_getting_state_handler_t getting_state_cb);

int mg_bthing_update_states_ap(bool force_pub, enum mgos_bthing_filter_by filter, va_list ap);

#endif // MGOS_BTHING_HAVE_SENSORS

#if MGOS_BTHING_HAVE_ACTUATORS

bool mg_bthing_actu_init(struct mg_bthing_actu *actu, void *cfg);

void mg_bthing_actu_reset(struct mg_bthing_actu *actu);

bool mg_bthing_set_state(struct mg_bthing_actu *actu, mgos_bvarc_t state);

mg_bthing_setting_state_handler_t mg_bthing_on_setting_state(struct mg_bthing_actu *actu, 
                                                             mg_bthing_setting_state_handler_t setting_state_cb);

#endif // MGOS_BTHING_HAVE_ACTUATORS

/* Register the bThing */
bool mg_bthing_register(mgos_bthing_t thing);

/* Count recurrences of str2 in str1 */
int mg_bthing_scount(const char *str1, const char* str2);

/* TODO: comment */
bool mg_bthing_sreplace(const char *src, const char *str,
                        const char* replace_with, char **out);

/* TODO: comment */
bool mg_bthing_sreplaces(const char *src, char **out, int count, ...);

int64_t mg_bthing_duration_micro(int64_t t1, int64_t t2);

int mg_bthing_path_indexof(const char *path, int path_len, char sep, const char *str);

bool mg_bthing_path_get_segment(const char *path, int path_len, char sep,
                               int seg_idx, const char **seg_val, int *seg_len);

char *mgos_bthing_sjoin(const char *sep, int count, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MG_BTHING_SDK_H_ */