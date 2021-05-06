#include <mgos.h>
#include "mgos_bthing_sdk.h"
#include "mgos_bthing.h"


const char *mgos_bthing_get_id(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST(thing)->id : NULL);
}

int mgos_bthing_get_type(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST(thing)->type : 0);
}

bool mgos_bthing_typeof(mgos_bthing_t thing, int type) {
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

#if MGOS_BTHING_HAVE_SENSORS

bool mgos_bthing_set_state_handler(mgos_bthing_t thing,
                                   mgos_bthing_get_state_handler_t get_state_cb,
                                   void *userdata) {
  struct mg_bthing_sens *sens = MG_BTHING_SENS_CAST(thing);
  if (sens && get_state_cb) {
    sens->get_state_cb = get_state_cb;
    sens->state_cb_ud = userdata;
    return true;
  }
  return false;
}

mgos_bvarc_t mgos_bthing_get_state(mgos_bthing_t thing) {
  struct mg_bthing_sens *sens = MG_BTHING_SENS_CAST(thing);
  bool get_ok = (!sens ? false : (sens->is_updating == 0 ? mg_bthing_get_state(sens, false) : true));
  return (get_ok ? MGOS_BVAR_CONST(sens->state) : NULL);
}

static void mg_bthing_update_state_cb(int ev, void *ev_data, void *userdata) {
  if (ev != MGOS_EV_BTHING_UPDATE_STATE) return;
  if (ev_data) {
    mg_bthing_get_state(MG_BTHING_SENS_CAST((mgos_bthing_t)ev_data), true);
  } else {
    mgos_bthing_t thing;
    mgos_bthing_enum_t things = mgos_bthing_get_all();

    while(mgos_bthing_get_next(&things, &thing)) {
      struct mg_bthing_sens *sensor = MG_BTHING_SENS_CAST(thing);
      mg_bthing_get_state(sensor, true);
    }
  }

  (void) userdata;
}

#endif // MGOS_BTHING_HAVE_SENSORS

#if MGOS_BTHING_HAVE_ACTUATORS

bool mgos_bthing_set_state_handlers(mgos_bthing_t thing,
                                    mgos_bthing_get_state_handler_t get_state_cb, 
                                    mgos_bthing_set_state_handler_t set_state_cb,
                                    void *userdata) {
  struct mg_bthing_actu *actu = MG_BTHING_ACTU_CAST(thing);
  if (actu && set_state_cb) {
    if (mgos_bthing_set_state_handler(thing, get_state_cb, userdata)) {
      actu->set_state_cb = set_state_cb;
      return true;
    }
  }
  return false;
}

bool mgos_bthing_set_state(mgos_bthing_t thing, mgos_bvarc_t state) {
  return mg_bthing_set_state(MG_BTHING_ACTU_CAST(thing), state);
}

#endif // MGOS_BTHING_HAVE_ACTUATORS


bool mgos_bthing_init() {
  /* Initialize execution context */
  s_context.things.thing = NULL;
  s_context.things.next_item = NULL;

  if (!mgos_event_register_base(MGOS_BTHING_EVENT_BASE, "bThing events")) return false;

  #if MGOS_BTHING_HAVE_SENSORS
  if (!mgos_event_add_handler(MGOS_EV_BTHING_UPDATE_STATE, mg_bthing_update_state_cb, NULL)) return false;
  #endif
  
  LOG(LL_INFO, ("MGOS_BTHING_EVENT_BASE %d", MGOS_BTHING_EVENT_BASE));
    
  return true;
}
