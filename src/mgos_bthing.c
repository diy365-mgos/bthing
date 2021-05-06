#include <mgos.h>
#include "mgos_bthing_sdk.h"
#include "mgos_bthing.h"


const char *mgos_bthing_get_id(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST(thing)->id : NULL);
}

int mgos_bthing_get_type(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST(thing)->type : 0);
}

bool mgos_bthing_is_typeof(mgos_bthing_t thing, int type) {
  return ((mgos_bthing_get_type(thing) & type) == type);
}

mgos_bthing_t mgos_bthing_get(const char* id) {
  if (id) {
    struct mg_bthing_enum *things = &s_context.things;
    while (things && things->thing) {
      if (0 == strcasecmp(id, MG_BTHING_CAST(things->thing)->id)) return things->thing;
      things = things->next_item;
    }
  }
  return NULL;
}

mgos_bthing_enum_t mgos_bthing_get_all() {
  return (mgos_bthing_enum_t)&s_context.things;
}

bool mgos_bthing_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing) {
  if (!things_enum || !(*things_enum) || !(*things_enum)->thing) {
    if (thing) *thing = NULL;
    return false;
  } else {
    if (thing) *thing = (*things_enum)->thing;
    *things_enum = (*things_enum)->next_item;
    return true;
  }
}

bool mgos_bthing_set_state_handlers(mgos_bthing_t thing,
                                    mgos_bthing_set_state_handler_t set_state_cb, 
                                    mgos_bthing_get_state_handler_t get_state_cb,
                                    void *userdata) {
  struct mg_bthing_actu *t = MG_BTHING_ACTU_CAST(thing);
  if (t && get_state_cb) {
    if (mgos_bthing_set_state_handler(thing, set_state_cb, userdata)) {
      t->get_state_cb = get_state_cb;
      return true;
    }
  }
  return false;
}

bool mgos_bthing_set_state_handler(mgos_bthing_t thing,
                                   mgos_bthing_set_state_handler_t set_state_cb,
                                   void *userdata) {
  struct mg_bthing_sens *t = MG_BTHING_SENS_CAST(thing);
  if (t && set_state_cb) {
    t->set_state_cb = set_state_cb;
    t->state_cb_ud = userdata;
    return true;
  }
  return false;
}


bool mgos_bthing_init() {
  /* Initialize execution context */
  s_context.things.thing = NULL;
  s_context.things.next_item = NULL;

  if (!mgos_event_register_base(MGOS_BTHING_EVENT_BASE, "bThing events")) {
    return false;
  }
  
  LOG(LL_INFO, ("MGOS_BTHING_EVENT_BASE %d", MGOS_BTHING_EVENT_BASE));
    
  return true;
}
