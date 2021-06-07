#include <mgos.h>
#include "mg_bthing_sdk.h"

#ifdef MGOS_HAVE_MJS
#include "mjs.h"
#endif

const char *mgos_bthing_get_id(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST1(thing)->id : NULL);
}

int mgos_bthing_get_type(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST1(thing)->type : 0);
}

bool mgos_bthing_is_typeof(mgos_bthing_t thing, int type) {
  return ((mgos_bthing_get_type(thing) & type) == type);
}

mgos_bthing_t mgos_bthing_get(const char* id) {
  if (id) {
    struct mg_bthing_enum *things = &(mg_bthing_context()->things);
    while (things && things->thing) {
      if (0 == strcasecmp(id, MG_BTHING_CAST1(things->thing)->id)) return things->thing;
      things = things->next_item;
    }
  }
  return NULL;
}

mgos_bthing_enum_t mgos_bthing_get_all() {
  return (mgos_bthing_enum_t)&(mg_bthing_context()->things);
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

bool mgos_bthing_typeof_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing, int type) {
  if (!mgos_bthing_get_next(things_enum, thing)) return false;
  if (mgos_bthing_is_typeof(*thing, type)) return true;
  return mgos_bthing_typeof_get_next(things_enum, thing, type);
}

#if MGOS_BTHING_HAVE_SENSORS

bool mgos_bthing_on_get_state(mgos_bthing_t thing,
                              mgos_bthing_get_state_handler_t get_state_cb,
                              void *userdata) {
  struct mg_bthing_sens *sens = MG_BTHING_SENS_CAST1(thing);
  if (sens) {
    if (!sens->get_state_cb || !get_state_cb) {
      sens->get_state_cb = get_state_cb;
      sens->get_state_ud = (get_state_cb ? userdata : NULL);
      return true;
    }
    LOG(LL_ERROR, ("The get-state handler of bThing '%s' is already configured.", mgos_bthing_get_id(thing)));
  }
  return false;
}

mgos_bvarc_t mgos_bthing_get_state(mgos_bthing_t thing) {
  struct mg_bthing_sens *sens = MG_BTHING_SENS_CAST1(thing);
  bool get_ok = (!sens ? false : (sens->is_updating == 0 ? mg_bthing_get_state(sens, false) : true));
  return (get_ok ? MGOS_BVAR_CONST(sens->state) : NULL);
}

static void mg_bthing_update_state_cb(int ev, void *ev_data, void *userdata) {
  if (ev != MGOS_EV_BTHING_UPDATE_STATE) return;
  if (ev_data) {
    mg_bthing_get_state(MG_BTHING_SENS_CAST1((mgos_bthing_t)ev_data), true);
  } else {
    mgos_bthing_t thing;
    mgos_bthing_enum_t things = mgos_bthing_get_all();

    while(mgos_bthing_get_next(&things, &thing)) {
      mg_bthing_get_state(MG_BTHING_SENS_CAST1(thing), true);
    }
  }

  (void) userdata;
}

void mgos_bthing_on_state_changed(mgos_bthing_t thing,
                                   mgos_bthing_state_changed_handler_t state_changed_cb,
                                   void *userdata) {
  struct mg_bthing_sens *sens = MG_BTHING_SENS_CAST1(thing);
  if (sens) {
    struct mg_bthing_state_changed_handlers *sc = sens->state_changed;
    while (sc) {
      if (sc->callback == state_changed_cb && sc->userdata == userdata) return;
      sc = sc->next;
    }
 
    sc = calloc(1, sizeof(struct mg_bthing_state_changed_handlers));
    sc->callback = state_changed_cb;
    sc->userdata = userdata;

    if (sens->state_changed) sc->next = sens->state_changed;
    sens->state_changed = sc;
  }
}

#endif // MGOS_BTHING_HAVE_SENSORS

#if MGOS_BTHING_HAVE_ACTUATORS

bool mgos_bthing_on_set_state(mgos_bthing_t thing,
                              mgos_bthing_set_state_handler_t set_state_cb,
                              void *userdata) {
  struct mg_bthing_actu *actu = MG_BTHING_ACTU_CAST1(thing);
  if (actu) {
    if (!actu->set_state_cb || !set_state_cb) {
      actu->set_state_cb = set_state_cb;
      actu->set_state_ud = (set_state_cb ? userdata : NULL);
      return true;
    }
    LOG(LL_ERROR, ("The set-state handler of bThing '%s' is already configured.", mgos_bthing_get_id(thing)));
  }
  return false;
}

bool mgos_bthing_set_state(mgos_bthing_t thing, mgos_bvarc_t state) {
  return mg_bthing_set_state(MG_BTHING_ACTU_CAST1(thing), state);
}

#endif // MGOS_BTHING_HAVE_ACTUATORS


bool mgos_bthing_init() {
  if (!mgos_event_register_base(MGOS_BTHING_EVENT_BASE, "bThing events")) return false;

  #if MGOS_BTHING_HAVE_SENSORS
  if (!mgos_event_add_handler(MGOS_EV_BTHING_UPDATE_STATE, mg_bthing_update_state_cb, NULL)) return false;
  #endif
  
  LOG(LL_DEBUG, ("MGOS_BTHING_EVENT_BASE %d", MGOS_BTHING_EVENT_BASE));
    
  return true;
}
