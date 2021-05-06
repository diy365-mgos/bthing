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

#ifndef MGOS_BTHING_SDK_H_
#define MGOS_BTHING_SDK_H_

#include <stdbool.h>
#include "mgos_bthing.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mg_bthing {
  char *id;
  int type;
  enum mgos_bthing_notify_state notify_state;
};

struct mg_bthing_sens {
  struct mg_bthing base;
  void *state_cb_ud;
  mgos_bthing_get_state_handler_t get_state_cb;
  unsigned char is_updating;
  mgos_bvar_t state;
};

struct mg_bthing_actu {
  struct mg_bthing_sens base;
  mgos_bthing_set_state_handler_t set_state_cb;
};

struct mg_bthing_enum {
  mgos_bthing_t thing;
  struct mg_bthing_enum *next_item;
};

struct mg_bthing_ctx {
  struct mg_bthing_enum things;
};

/* Excecution context instance */
static struct mg_bthing_ctx s_context;

struct mg_bthing *MG_BTHING_CAST(mgos_bthing_t thing);
struct mg_bthing_sens *MG_BTHING_SENS_CAST(mgos_bthing_t thing);
struct mg_bthing_actu *MG_BTHING_ACTU_CAST(mgos_bthing_t thing);

inline struct mg_bthing *MG_BTHING_SENS_BASE_CAST(struct mg_bthing_sens *thing) { return (struct mg_bthing *)thing; }
inline struct mg_bthing_sens *MG_BTHING_ACTU_BASE_CAST(struct mg_bthing_actu *thing) { return (struct mg_bthing_sens *)thing; }

bool mg_bthing_init(struct mg_bthing *thing,
                    const char *id, int type, 
                    enum mgos_bthing_notify_state notify_state);
bool mg_bthing_sens_init(struct mg_bthing_sens *thing,
                         const char *id, int type, 
                         enum mgos_bthing_notify_state notify_state);
bool mg_bthing_actu_init(struct mg_bthing_actu *thing,
                         const char *id, int type, 
                         enum mgos_bthing_notify_state notify_state);

bool mg_bthing_get_state(struct mg_bthing_sens *thing, bool force_notify_state);
bool mg_bthing_set_state(struct mg_bthing_actu *thing, mgos_bvarc_t state);

/* Register the bThing */
bool mg_bthing_register(mgos_bthing_t thing);

/* Count recurrences of str2 in str1 */
int mg_bthing_scount(const char *str1, const char* str2);

/* TODO: comment */
bool mg_bthing_sreplace(const char *src, const char *str,
                        const char* replace_with, char **out);

/* TODO: comment */
bool mg_bthing_sreplaces(const char *src, char **out, int count, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_BTHING_SDK_H_ */