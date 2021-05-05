#include <mgos.h>
#include "mgos_bthing_sdk.h"
#include "mgos_bthing.h"


const char *mgos_bthing_get_id(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST(thing)->id : NULL);
}

int mgos_bthing_get_type(mgos_bthing_t thing) {
  return (thing ? MG_BTHING_CAST(thing)->type : 0);
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
