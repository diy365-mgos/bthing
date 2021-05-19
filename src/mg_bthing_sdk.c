#include <stdlib.h>
#include <string.h>
#include "mgos.h"
#include "mg_bthing_sdk.h"

/*****************************************
 * Cast Functions
 */

// Convert (mgos_bthing_t) into (struct mg_bthing *)
struct mg_bthing *MG_BTHING_CAST1(mgos_bthing_t thing) { 
  return (struct mg_bthing *)thing;
}

// Convert (struct mg_bthing *) into (mgos_bthing_t) 
mgos_bthing_t MG_BTHING_CAST2(struct mg_bthing *thing) {
  return (mgos_bthing_t)thing;
}
/*****************************************/

/* Excecution context instance */
struct mg_bthing_ctx *s_context;
struct mg_bthing_ctx *mg_bthing_context() {
  if (!s_context) {
    /* Initialize execution context */
    s_context = calloc(1, sizeof(struct mg_bthing_ctx));
    s_context->things.thing = NULL;
    s_context->things.next_item = NULL;
  }
  return s_context; 
}

bool mg_bthing_init(struct mg_bthing *thing,
                    const char *id, int type, 
                    enum mgos_bthing_pub_state_mode pub_state_mode) {
  if (thing && id && (strlen(id) > 0)) {
    thing->id = strdup(id);
    thing->type = type;
    thing->pub_state_mode = pub_state_mode;
    return true;
  }
  LOG(LL_ERROR, ("Error initializing the bThing '%s'. Invalid 'thing' or 'id' parameters.", (id ? id : "")));
  return false;
}

void mg_bthing_reset(struct mg_bthing *thing) {
  if (thing) {
    free(thing->id);
    thing->id = NULL;
    thing->type = 0;
    thing->pub_state_mode = MGOS_BTHING_PUB_STATE_MODE_NEVER;
  }
}

#if MGOS_BTHING_HAVE_SENSORS

/*****************************************
 * Cast Functions
 */

// Convert (mgos_bthing_t) into (struct mg_bthing_sens *) or NULL if conversion fails
struct mg_bthing_sens *MG_BTHING_SENS_CAST1(mgos_bthing_t thing) {
  return (mgos_bthing_is_typeof(thing, MGOS_BTHING_TYPE_SENSOR) ? (struct mg_bthing_sens *)thing : NULL);
}

// Convert (struct mg_bthing *) into (struct mg_bthing_sens *) or NULL if conversion fails
struct mg_bthing_sens *MG_BTHING_SENS_CAST2(struct mg_bthing *thing) {
  return (mgos_bthing_is_typeof(MG_BTHING_CAST2(thing), MGOS_BTHING_TYPE_SENSOR) ? (struct mg_bthing_sens *)thing : NULL);
}

// Convert (struct mg_bthing_sens *) into (struct mg_bthing *)
struct mg_bthing *MG_BTHING_SENS_CAST3(struct mg_bthing_sens *thing) {
  return &(thing->base);
}

// Convert (struct mg_bthing_sens *) into (mgos_bthing_t)
mgos_bthing_t MG_BTHING_SENS_CAST4(struct mg_bthing_sens *thing) {
  return MG_BTHING_CAST1(MG_BTHING_SENS_CAST3(thing));
}
/*****************************************/

enum MG_BTHING_STATE_RESULT mg_bthing_sens_getting_state_cb(struct mg_bthing_sens *thing,
                                                            mgos_bvar_t state,
                                                            void *userdata) {
  if (thing) {
    if (!thing->get_state_cb)
      return MG_BTHING_STATE_RESULT_NO_HANDLER;
    if (thing->get_state_cb(MG_BTHING_SENS_CAST4(thing), state, userdata))
      return MG_BTHING_STATE_RESULT_SUCCESS;
  }
  LOG(LL_ERROR, ("Error getting bThing '%s' state. The get-state handler returned 'false'.",
    mgos_bthing_get_id(MG_BTHING_SENS_CAST4(thing))));
  return MG_BTHING_STATE_RESULT_ERROR;
}

bool mg_bthing_sens_init(struct mg_bthing_sens *sens) {
  if (sens) {
    struct mg_bthing *t = MG_BTHING_SENS_CAST3(sens);
    if (!t->id) {
      LOG(LL_ERROR, ("bSensor init failed. Invoke 'mg_bthing_init()' function first."));
      return false;
    }

    // Update bThing type
    t->type = (t->type | MGOS_BTHING_TYPE_SENSOR);

    sens->cfg = NULL;
    mg_bthing_on_getting_state(sens, mg_bthing_sens_getting_state_cb);
    sens->get_state_cb = NULL;
    sens->get_state_ud = NULL;
    sens->updating_state_cb = NULL;
    sens->updating_state_ud = NULL;
    sens->is_updating = 0;
    sens->state = mgos_bvar_new();
    return true;
  }
  return false;
}

void mg_bthing_sens_reset(struct mg_bthing_sens *thing) {
  if (thing) {
    free(thing->cfg);
    thing->cfg = NULL;
    mg_bthing_on_getting_state(thing, NULL);
    thing->get_state_cb = NULL;
    thing->get_state_ud = NULL;
    thing->updating_state_cb = NULL;
    thing->updating_state_ud = NULL;
    thing->is_updating = 0;
    mgos_bvar_free(thing->state);
    thing->state = NULL;
  }
}

bool mg_bthing_get_state(struct mg_bthing_sens *thing, bool force_pub_state_mode) {
  if (!thing) return false;
  thing->is_updating += 1;
  if (thing->getting_state_cb) {
    if (thing->getting_state_cb(thing, thing->state, thing->get_state_ud) == MG_BTHING_STATE_RESULT_ERROR) {
      thing->is_updating -= 1;
      LOG(LL_ERROR, ("Error getting bThing '%s' state.", mgos_bthing_get_id(MG_BTHING_SENS_CAST4(thing))));
      return false;
    }
  }

  if (thing->updating_state_cb) {
    thing->updating_state_cb(thing, thing->state, thing->updating_state_ud);
  }
  mgos_event_trigger(MGOS_EV_BTHING_UPDATING_STATE, thing);

  enum mgos_bthing_pub_state_mode pub_state_mode = MG_BTHING_SENS_CAST3(thing)->pub_state_mode;
  if (pub_state_mode != MGOS_BTHING_PUB_STATE_MODE_NEVER) {
    if (force_pub_state_mode == true ||
        pub_state_mode == MGOS_BTHING_PUB_STATE_MODE_ALWAYS ||
        (pub_state_mode == MGOS_BTHING_PUB_STATE_MODE_CHANGED && (mgos_bvar_is_changed(thing->state)))) {
      mgos_event_trigger(MGOS_EV_BTHING_PUBLISHING_STATE, thing);
    }
  }

  mgos_bvar_set_unchanged(thing->state);
  thing->is_updating -= 1;
  return true;
}

mg_bthing_getting_state_handler_t mg_bthing_on_getting_state(struct mg_bthing_sens *thing, 
                                                             mg_bthing_getting_state_handler_t getting_state_cb) {
  if (!thing) return NULL;
  mg_bthing_getting_state_handler_t prev_h = thing->getting_state_cb;
  thing->getting_state_cb = getting_state_cb;
  return prev_h;
}

#endif // MGOS_BTHING_HAVE_SENSORS

#if MGOS_BTHING_HAVE_ACTUATORS

/*****************************************
 * Cast Functions
 */

// Convert (mgos_bthing_t) into (struct mg_bthing_actu *) or NULL if conversion fails
struct mg_bthing_actu *MG_BTHING_ACTU_CAST1(mgos_bthing_t thing) {
  return (mgos_bthing_is_typeof(thing, MGOS_BTHING_TYPE_ACTUATOR) ? (struct mg_bthing_actu *)thing : NULL);
}

// Convert (struct mg_bthing *) into (struct mg_bthing_actu *) or NULL if conversion fails
struct mg_bthing_actu *MG_BTHING_ACTU_CAST2(struct mg_bthing *thing) {
  return (mgos_bthing_is_typeof(MG_BTHING_CAST2(thing), MGOS_BTHING_TYPE_ACTUATOR) ? (struct mg_bthing_actu *)thing : NULL);
}

// Convert (struct mg_bthing_actu *) into (struct mg_bthing_sens *)
struct mg_bthing_sens *MG_BTHING_ACTU_CAST3(struct mg_bthing_actu *thing) {
  return &(thing->base);
}

// Convert (struct mg_bthing_actu *) into (struct mg_bthing *)
struct mg_bthing *MG_BTHING_ACTU_CAST4(struct mg_bthing_actu *thing) {
  return MG_BTHING_SENS_CAST3(MG_BTHING_ACTU_CAST3(thing));
}

// Convert (struct mg_bthing_actu *) into (mgos_bthing_t)
mgos_bthing_t MG_BTHING_ACTU_CAST5(struct mg_bthing_actu *thing) { return MG_BTHING_CAST1(MG_BTHING_ACTU_CAST4(thing)); }
/*****************************************/

enum MG_BTHING_STATE_RESULT mg_bthing_actu_setting_state_cb(struct mg_bthing_actu *thing,
                                                            mgos_bvarc_t state,
                                                            void *userdata) {
  if (thing) {
    if (!thing->set_state_cb)
      return MG_BTHING_STATE_RESULT_NO_HANDLER;
    if (thing->set_state_cb(MG_BTHING_ACTU_CAST5(thing), state, userdata))
      return MG_BTHING_STATE_RESULT_SUCCESS;
  }
  return MG_BTHING_STATE_RESULT_ERROR;
}

bool mg_bthing_actu_init(struct mg_bthing_actu *actu) {
  if (actu) {
    struct mg_bthing *t = MG_BTHING_ACTU_CAST4(actu);
    if (!t->id) {
      LOG(LL_ERROR, ("bActuator init failed. Invoke 'mg_bthing_init()' function first."));
      return false;
    }

    // Update the bThing type
    t->type = (t->type | MGOS_BTHING_TYPE_ACTUATOR);

    actu->cfg = NULL;
    mg_bthing_on_setting_state(actu, mg_bthing_actu_setting_state_cb); 
    actu->set_state_cb = NULL;
    actu->set_state_ud = NULL;
    return true;
  }
  return false;
}

void mg_bthing_actu_reset(struct mg_bthing_actu *actu) {
  if (actu) {
    free(actu->cfg);
    actu->cfg = NULL;
    mg_bthing_on_setting_state(actu, NULL); 
    actu->set_state_cb = NULL;
    actu->set_state_ud = NULL;
  }
}

bool mg_bthing_set_state(struct mg_bthing_actu *thing, mgos_bvarc_t state) {
  if (thing) {
    struct mg_bthing_sens *sens = MG_BTHING_ACTU_CAST3(thing);

    enum MG_BTHING_STATE_RESULT res = (!thing->setting_state_cb ? 
      MG_BTHING_STATE_RESULT_NO_HANDLER : thing->setting_state_cb(thing, state, thing->set_state_ud));
    
    if (res == MG_BTHING_STATE_RESULT_NO_HANDLER)
      return mgos_bvar_merge(state, sens->state);
    else if (res == MG_BTHING_STATE_RESULT_SUCCESS) {
      mg_bthing_get_state(sens, false);
      return true;
    }
  }
  LOG(LL_ERROR, ("Error setting the state of bActuator '%s'",
    mgos_bthing_get_id(MG_BTHING_ACTU_CAST5(thing))));
  return false;
}

mg_bthing_setting_state_handler_t mg_bthing_on_setting_state(struct mg_bthing_actu *thing, 
                                                             mg_bthing_setting_state_handler_t setting_state_cb) {
  if (!thing) return NULL;
  mg_bthing_setting_state_handler_t prev_h = thing->setting_state_cb;
  thing->setting_state_cb = setting_state_cb;
  return prev_h;
}

#endif // MGOS_BTHING_HAVE_ACTUATORS

bool mg_bthing_register(mgos_bthing_t thing) {
  if (!thing) return false;
  struct mg_bthing_enum *things = &(mg_bthing_context()->things);
  if (things->thing) {
    struct mg_bthing_enum *new_item = calloc(1, sizeof(struct mg_bthing_enum ));
    new_item->thing = things->thing;
    new_item->next_item = things->next_item;
    things->next_item = new_item;
  }
  things->thing = thing;
  mgos_event_trigger(MGOS_EV_BTHING_CREATED, thing);
  return true;
}

int mg_bthing_scount(const char *str1, const char* str2) {
  int count = 0;
  if (str1 != NULL && str2 != NULL) {
    int string_len = strlen(str2);
    for (const char *p = str1; *p != '\0';) {
      if (strncmp(p, str2, string_len) == 0) {
        ++count;
        p += string_len;
      } else {
        ++p;
      }
    }
  }
  return count;
}

bool mg_bthing_sreplace(const char *src,
                              const char *tag,
                              const char* replace_with,
                              char **out) {
  *out = NULL;

  /* count 'tag' occurrences in 'src' */
  int count = mg_bthing_scount(src, tag);
  if (count == 0) {
    return false;
  }

  int src_len = strlen(src);
  int tag_len = strlen(tag);
  int replace_with_len = (replace_with == NULL ? 0 : strlen(replace_with));

  /* allocate the buffer for the result string */
  int res_len = (src_len + (replace_with_len * count));
  *out = (char *)malloc(res_len + 1);
  strcpy(*out, src);

  /* allocate a temporary buffer */
  char *tmp_buf = (char *)malloc(src_len + 1);
  char *tmp_buf_to_free = tmp_buf;
  char *buf = *out;
      
  for (; *buf != '\0';) {
    if (strncmp(buf, tag, tag_len) == 0) {
      strcpy(tmp_buf, (buf + tag_len));
      strcpy(buf, (replace_with != NULL ? replace_with : ""));
      strcpy((buf + replace_with_len), tmp_buf);
      buf += replace_with_len;
    } else {
      ++buf;
    }
  }

  free(tmp_buf_to_free);
  return true;
}

bool mg_bthing_sreplaces(const char *src,
                               char **out,
                               int count,
                               ...) {
  *out = NULL;

  char *string = (char *)src;
  char *tag = NULL;
  char *tag_value = NULL;
  char *temp_out = NULL;

  va_list ap;
  va_start(ap, count);
  
  for (int i = 0; i < count; ++i) {
    tag = va_arg(ap, char*);
    tag_value = va_arg(ap, char*);
    
    if (mg_bthing_sreplace(string, tag, tag_value, &temp_out)) {
      if (string != src) free (string);
      string = *out = temp_out; 
    }
  }
  
  va_end(ap);
  return (*out != NULL);
}