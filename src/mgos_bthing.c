#include <mgos.h>
#include "mg_bthing_sdk.h"

#ifdef MGOS_HAVE_MJS
#include "mjs.h"
#endif

const char *mgos_bthing_get_uid(mgos_bthing_t thing) {
  struct mg_bthing *t = MG_BTHING_CAST1(thing);
  if (!t) return NULL;
  if (!t->uid) {
    const char *dev_id = mgos_sys_config_get_device_id();
    t->uid = calloc(strlen(t->id) + (t->domain ? strlen(t->domain) : 0) + strlen(dev_id) + 3, sizeof(char));
    strcat(t->uid, dev_id); strcat(t->uid, ".");
    if (t->domain) {
      strcat(t->uid, t->domain); strcat(t->uid, ".");
    }
    strcat(t->uid, t->id);
  }
  return t->uid;
}

const char *mgos_bthing_get_id(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST1(thing)->id : NULL);
}

const char *mgos_bthing_get_domain(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST1(thing)->domain : NULL);
}

int mgos_bthing_get_type(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST1(thing)->type : 0);
}

bool mgos_bthing_is_typeof(mgos_bthing_t thing, int type) {
  return ((mgos_bthing_get_type(thing) & type) == type);
}

mgos_bthing_t mgos_bthing_get_by_uid(const char* uid) {
  if (uid) {
    struct mg_bthing_enum *things = &(mg_bthing_context()->things);
    while (things && things->thing) {
      if (0 == strcasecmp(uid, mgos_bthing_get_uid(things->thing))) return things->thing;
      things = things->next_item;
    }
  }
  return NULL;
}

mgos_bthing_t mgos_bthing_get_by_id(const char* id, const char *domain) {
  if (id) {
    struct mg_bthing_enum *things = &(mg_bthing_context()->things);
    while (things && things->thing) {
      if (0 == strcasecmp(id, mgos_bthing_get_id(things->thing))) {
        const char *dom = mgos_bthing_get_domain(things->thing);
        if ((dom && domain && (strcasecmp(dom, domain) == 0)) ||(!dom && !domain)) {
          return things->thing;
        }
      } 
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

bool mgos_bthing_filter_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing,
                                 enum mgos_bthing_filter_by filter, ...) {
  mgos_bthing_t my_t = NULL;
  if (mgos_bthing_get_next(things_enum, &my_t)) {
    va_list ap;
    va_start(ap, filter); 
    switch (filter)
    {
      case MGOS_BTHING_FILTER_BY_NOTHING:
        break;
      case MGOS_BTHING_FILTER_BY_TYPE: {
          int type = va_arg(ap, int);
          if (!mgos_bthing_is_typeof(my_t, type))
            return mgos_bthing_filter_get_next(things_enum, thing, filter, type);
        }
        break;
        case MGOS_BTHING_FILTER_BY_DOMAIN: {
          const char *dom = va_arg(ap, const char *);
          const char *my_dom = mgos_bthing_get_domain(my_t);
          if ((!dom && my_dom) || (dom && !my_dom) || (dom && my_dom && (strcasecmp(dom, my_dom) != 0))) 
            return mgos_bthing_filter_get_next(things_enum, thing, filter, dom);
        }
        break;
      default:
        return false;
    };

    va_end(ap);
    if (thing) *thing = my_t;
    return true;
  }

  if (thing) *thing = NULL;
  return false;
}

void mgos_bthing_make_private(mgos_bthing_t thing) {
  if (thing) {
    mg_bthing_set_flag(thing, MG_BTHING_FLAG_ISPRIVATE);
    mgos_event_trigger(MGOS_EV_BTHING_MADE_PRIVATE, thing);
  }
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
    LOG(LL_ERROR, ("The get-state handler of bThing '%s' is already configured.", mgos_bthing_get_uid(thing)));
  }
  return false;
}

bool mg_bthing_get_state(struct mg_bthing_sens *);

mgos_bvarc_t mgos_bthing_get_state(mgos_bthing_t thing) {
  bool get_ok = false;
  struct mg_bthing_sens *sens = MG_BTHING_SENS_CAST1(thing);
  if (thing && sens) {
    if (mg_bthing_has_flag(thing, MG_BTHING_FLAG_UPDATING_STATE)) {
      get_ok = true;
    } else {
      get_ok = mg_bthing_get_state(sens);
    }
  }
  return (get_ok ? MGOS_BVAR_CONST(sens->state) : NULL);
}

bool mgos_bthing_update_state(mgos_bthing_t thing, bool force_pub) {
  //return mg_bthing_update_state(thing, true);
  //return mg_bthing_update_state(thing);
  bool prev_force_pub = mg_bthing_has_flag(thing, MG_BTHING_FLAG_FORCE_STATE_PUB);
  if (force_pub) {
    mg_bthing_set_flag(thing, MG_BTHING_FLAG_FORCE_STATE_PUB);
  } else {
    mg_bthing_reset_flag(thing, MG_BTHING_FLAG_FORCE_STATE_PUB);
  }

  bool success = (mgos_bthing_get_state(thing) != NULL);

  if (prev_force_pub) {
    mg_bthing_set_flag(thing, MG_BTHING_FLAG_FORCE_STATE_PUB);
  } else {
    mg_bthing_reset_flag(thing, MG_BTHING_FLAG_FORCE_STATE_PUB);
  }
  return success;
}

int mgos_bthing_update_states(bool force_pub, enum mgos_bthing_filter_by filter, ...) {
  va_list ap;
  va_start(ap, filter);
  //int count = mg_bthing_update_states_ap(true, filter, ap);
  int count = mg_bthing_update_states_ap(force_pub, filter, ap);
  va_end(ap);
  return count;
}

bool mgos_bthing_start_update_state(mgos_bthing_t thing, struct mgos_bthing_updatable_state *state) {
  if (thing && state) {
    state->value = mg_bthing_get_state_4update(thing);
    if (state->value != NULL) {
      state->owner = thing;
      mg_bthing_set_flag(state->owner, MG_BTHING_FLAG_UPDATING_STATE);
      return true;
    }
  }
  return false;
}

bool mgos_bthing_end_update_state(struct mgos_bthing_updatable_state state) {
  if (state.owner && state.value) {
    if (mg_bthing_has_flag(state.owner, MG_BTHING_FLAG_UPDATING_STATE)) {
      mg_bthing_reset_flag(state.owner, MG_BTHING_FLAG_UPDATING_STATE);
      //return mg_bthing_update_state(state.owner, false);
      //return mg_bthing_update_state(state.owner);
      return mgos_bthing_update_state(state.owner, false);
    }
  }
  return false;
}

void mgos_bthing_on_event(mgos_bthing_t thing, enum mgos_bthing_event ev,
                          mgos_event_handler_t handler, void *userdata) {
  struct mg_bthing_sens *sens = MG_BTHING_SENS_CAST1(thing);
  if (sens) {
    struct mg_bthing_on_event_handler *stc = sens->on_event;
    while (stc) {
      if ((stc->event == ev) && (stc->handler == handler) && (stc->userdata == userdata)) return;
      stc = stc->next;
    }
 
    stc = calloc(1, sizeof(struct mg_bthing_on_event_handler));
    stc->event = ev;
    stc->handler = handler;
    stc->userdata = userdata;

    if (sens->on_event) stc->next = sens->on_event;
    sens->on_event = stc;
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
    LOG(LL_ERROR, ("The set-state handler of bThing '%s' is already configured.", mgos_bthing_get_uid(thing)));
  }
  return false;
}

bool mgos_bthing_set_state(mgos_bthing_t thing, mgos_bvarc_t state) {
  return mg_bthing_set_state(MG_BTHING_ACTU_CAST1(thing), state);
}

#endif // MGOS_BTHING_HAVE_ACTUATORS


bool mgos_bthing_init() {
  if (!mgos_event_register_base(MGOS_BTHING_EVENT_BASE, "bThing events")) return false;

  LOG(LL_DEBUG, ("MGOS_BTHING_EVENT_BASE %d", MGOS_BTHING_EVENT_BASE));
    
  return true;
}
