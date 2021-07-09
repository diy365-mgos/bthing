# bThings Core Library
## Overview
Mongoose OS core library for the bThings ecosystem.
|Sensors|Actuators|Others|
|--|--|--|
|[bSensors](https://github.com/diy365-mgos/bsensor), [bBinarySensors](https://github.com/diy365-mgos/bbinsensor), [bButtons](https://github.com/diy365-mgos/bbutton)|[bActuators](https://github.com/diy365-mgos/bactuator), [bBinaryActuators](https://github.com/diy365-mgos/bbinactuator), [bSwitches](https://github.com/diy365-mgos/bswitch)|[MQTT support](https://github.com/diy365-mgos/bthing-mqtt), [State-shadow suport](https://github.com/diy365-mgos/bthing-shadow), [GPIO support](https://github.com/diy365-mgos/bthing-gpio)|
## C/C++ APIs Reference
### enum mgos_bthing_event
```c
enum mgos_bthing_event {
  MGOS_EV_BTHING_ANY,
  MGOS_EV_BTHING_CREATED,
  MGOS_EV_BTHING_STATE_CHANGING,
  MGOS_EV_BTHING_STATE_CHANGED,
  MGOS_EV_BTHING_UPDATE_STATE
};
```
**Triggered events** - Events triggered by a bThing. Use [mgos_event_add_handler()](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_add_handler) or [mgos_event_add_group_handler(MGOS_EV_BTHING_ANY, ...)](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_add_group_handler) for subscribing to them.

|Event||
|--|--|
|MGOS_EV_BTHING_CREATED|Triggered when a new bThing is created. The event-data passed to the handler is a `mgos_bthing_t`.|
|MGOS_EV_BTHING_STATE_CHANGING|Triggered when the state value of a bThing is going to change. The event-data passed to the handler is a `struct mgos_bthing_state_changing_arg*`.|
|MGOS_EV_BTHING_STATE_CHANGED|Triggered when the state value of a bThing is changed. The event-data passed to the handler is a `struct mgos_bthing_state_changed_arg*`.|

**Listening-to events** - Events a bThing is listening to. You can send them using [mgos_event_trigger()](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_trigger). You can send an event to all registered bThings or to a specific one. See example below.
```c
// Send the event to all registered bThings
mgos_event_trigger(MGOS_EV_BTHING_UPDATE_STATE, NULL);
// Send the event to a specific bThing
mgos_event_trigger(MGOS_EV_BTHING_UPDATE_STATE, thing);
```
|Event||
|--|--|
|MGOS_EV_BTHING_UPDATE_STATE|Send it to force the bThing's state to be updated. After sending it, `MGOS_EV_BTHING_STATE_CHANGING` and `MGOS_EV_BTHING_STATE_CHANGED` events are forcibly triggered.|
### mgos_bthing_state_changed_arg
```c
struct mgos_bthing_state_changed_arg {
  mgos_bthing_t thing;
  mgos_bvarc_t state;
};
```
Event-data passed to `MGOS_EV_BTHING_STATE_CHANGED` event's handlers (see [mgos_event_handler_t](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_handler_t)).

|Fields||
|--|--|
|thing|The bThing which state is changed.|
|state|The state.|
### mgos_bthing_state_changing_arg
```c
struct mgos_bthing_state_changing_arg {
  mgos_bthing_t thing;
  mgos_bvarc_t cur_state;
  mgos_bvarc_t new_state;
};
```
Event-data passed to `MGOS_EV_BTHING_STATE_CHANGING` event's handlers (see [mgos_event_handler_t](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_handler_t)).

|Fields||
|--|--|
|thing|The bThing which state is going to change.|
|cur_state|The current state.|
|new_state|The new state.|
### mgos_bthing_get_id
```c
const char *mgos_bthing_get_id(mgos_bthing_t thing);
```
Returns the ID of a bThing, or `NULL` if error.

|Parameter||
|--|--|
|thing|A bThing.|
### mgos_bthing_get_type
```c
int mgos_bthing_get_type(mgos_bthing_t thing);
```
Returns the type of a bThing, or `0` if error.

|Parameter||
|--|--|
|thing|A bThing.|

Example:
```c
LOG(LL_INFO, ("The bThing type is: %d.", mgos_bthing_get_type(thing)));
```
### mgos_bthing_is_typeof
```c
bool mgos_bthing_is_typeof(mgos_bthing_t thing, int type);
```
Returns `true` if the bThing type is `type`.

|Parameter||
|--|--|
|thing|A bThing.|
|type|The type to check.|

Example:
```c
if (mgos_bthing_is_typeof(thing, MGOS_BTHING_TYPE_SENSOR))
  LOG(LL_INFO, ("The bThing is a sensor."));
else if (mgos_bthing_is_typeof(thing, MGOS_BTHING_TYPE_ACTUATOR))
  LOG(LL_INFO, ("The bThing is an actuator."));
else
  LOG(LL_INFO, ("Unknown bThing type."));
```
### mgos_bthing_get
```c
mgos_bthing_t mgos_bthing_get(const char* id);
```
Returns the bThing having the specified ID, or `NULL` if not found.

|Parameter||
|--|--|
|id|A bThing ID.|
### mgos_bthing_get_all
```c
mgos_bthing_enum_t mgos_bthing_get_all();
```
Returns the enumerator for iterating all registered bThings, or `NULL` if error. The enumerator can be used with `mgos_bthing_get_next()`.
### mgos_bthing_get_next
```c
bool mgos_bthing_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing);
```
Gets the next bThing iterating registered ones. Returns `false` if the end of the enumerator is reached, or `true` otherwise.

|Parameter||
|--|--|
|things_enum|A reference to a bThing enumerator returned by `mgos_bthing_get_all()`.|
|thing|The output bThing. Optional, if `NULL` no bThing is returned as output.|
### mgos_bthing_typeof_get_next
```c
bool mgos_bthing_typeof_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing, int type) ;
```
Gets the next bThing of given type, iterating registered ones. Returns `false` if the end of the enumerator is reached, or `true` otherwise.

|Parameter||
|--|--|
|things_enum|A reference to a bThing enumerator returned by `mgos_bthing_get_all()`.|
|thing|The output bThing. Optional, if `NULL` no bThing is returned as output.|
|type|The bThing type to take into account enumerating.|
### (*mgos_bthing_get_state_handler_t)
```c
typedef bool (*mgos_bthing_get_state_handler_t)(mgos_bthing_t thing, mgos_bvar_t state, void *userdata);
```
*Get-state* handler signature. Must return `true` on success, or `false` otherwise. The signature is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|The bThing for whom to return the status.|
|state|The state value to return.|
|userdata|The handler's *user-data*.|
### mgos_bthing_on_get_state
```c
bool mgos_bthing_on_get_state(mgos_bthing_t thing,
                              mgos_bthing_get_state_handler_t get_state_cb,
                              void *userdata);
```
Sets the *get-state* handler of a bThing sensor/actuator (`mgos_bthing_is_typeof(MGOS_BTHING_TYPE_SENSOR)`). Returns `true` on success, or `false` otherwise. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|A bThing sensor/actuator.|
|get_state_cb|The [get-state handler](#mgos_bthing_get_state_handler_t) or `NULL` to reset the handler.|
|userdata|The *user-data* to pass to the handler or `NULL`. Ignored if `get_state_cb` is `NULL`.|
### mgos_bthing_get_state
```c
mgos_bvarc_t mgos_bthing_get_state(mgos_bthing_t thing);
```
Returns the state of a bThing sensor/actuator, or `NULL` if error. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|A bThing sensor/actuator.|
### (*mgos_bthing_set_state_handler_t)
```c
typedef bool (*mgos_bthing_set_state_handler_t)(mgos_bthing_t thing, mgos_bvarc_t state, void *userdata);
```
*Set-state* handler signature. Must return `true` on success, or `false` otherwise. The signature is available only `#if MGOS_BTHING_HAVE_ACTUATORS`.

|Parameter||
|--|--|
|thing|The bThing to set the state for.|
|state|The state value to set.|
|userdata|The handler's *user-data*.|
### mgos_bthing_on_set_state
```c
bool mgos_bthing_on_set_state(mgos_bthing_t thing,
                              mgos_bthing_set_state_handler_t set_state_cb,
                              void *userdata);
```
Sets the *set-state* handler of a bThing actuator (`mgos_bthing_is_typeof(MGOS_BTHING_TYPE_ACTUATOR)`). Returns `true` on success, or `false` otherwise. This function is available only `#if MGOS_BTHING_HAVE_ACTUATORS`.

|Parameter||
|--|--|
|thing|A bThing actuator.|
|set_state_cb|The [set-state handler](#mgos_bthing_set_state_handler_t) or `NULL` to reset the handler.|
|userdata|The *user-data* to pass to the handlers or `NULL`. Ignored if `set_state_cb` is `NULL`.|
### mgos_bthing_set_state
```c
bool mgos_bthing_set_state(mgos_bthing_t thing, mgos_bvarc_t state);
```
Sets the state of a bThing actuator. Returns `true` on success, or `false` otherwise. This function is available only `#if MGOS_BTHING_HAVE_ACTUATORS`.

|Parameter||
|--|--|
|thing|A bThing actuator.|
|state|The state value to set.|
### (*mgos_bthing_state_changed_handler_t)
```c
typedef void (*mgos_bthing_state_changed_handler_t)(mgos_bthing_t thing,
                                                    mgos_bvarc_t state,
                                                    void *userdata);
```
Signature of *state-changed* handlers. (see `mgos_bthing_on_state_changed()` below). The signature is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|The bThing which state is changed.|
|state|The state value.|
|userdata|The handler's *user-data*.|
### mgos_bthing_on_state_changed
```c
void mgos_bthing_on_state_changed(mgos_bthing_t thing,
                                  mgos_bthing_state_changed_handler_t handler,
                                  void *userdata);
```
Adds a *state-changed* handler, only if the *handler/userdata* pair is not yet registered. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|A bThing.|
|handler|The [state-changed handler](#mgos_bthing_state_changed_handler_t) to add.|
|userdata|The handler's *user-data* or `NULL`.|
### (*mgos_bthing_state_changing_handler_t)
```c
typedef void (*mgos_bthing_state_changing_handler_t)(mgos_bthing_t thing,
                                                     mgos_bvarc_t cur_state,
                                                     mgos_bvarc_t new_state,
                                                     void *userdata);
```
Signature of *state-changing* handlers (see `mgos_bthing_on_state_changing()` below). The signature is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|The bThing which state is going to change.|
|cur_state|The current state value.|
|new_state|The new state value.|
|userdata|The handler's *user-data*.|
### mgos_bthing_on_state_changing
```c
void mgos_bthing_on_state_changing(mgos_bthing_t thing,
                                   mgos_bthing_state_changing_handler_t handler,
                                   void *userdata);
```
Adds a *state-changing* handler, only if the *handler/userdata* pair is not yet registered. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|A bThing.|
|handler|The [state-changing handler](#mgos_bthing_state_changing_handler_t) to add.|
|userdata|The handler's *user-data* or `NULL`.|
## JS APIs Reference
### bThing.EVENT
```javascript
bThing.EVENT: {
  ANY,
  CREATED,
  STATE_CHANGING,
  STATE_CHANGED,
  UPDATE_STATE
}
```
**Triggered events** - Events triggered by a bThing. Use [Event.addHandler()](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#eventaddhandler) or [Event.addGroupHandler(bThing.EVENT.ANY, ...)](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#eventaddgrouphandler) for subscribing to them.

|Event||
|--|--|
|CREATED|Triggered when a new bThing is created. The event-data passed to the handler is a `mgos_bthing_t`.|
|STATE_CHANGING|Triggered when the state value of a bThing is going to change. The event-data passed to the handler is a `struct mgos_bthing_state_changing_arg*`.|
|STATE_CHANGED|Triggered when the state value of a bThing is changed. The event-data passed to the handler is a `struct mgos_bthing_state_changed_arg*`.|

Example:
```javascript
Event.addGroupHandler(bThing.EVENT.ANY, function(ev, evdata, ud) {
  if (ev == bThing.EVENT.CREATED) {
    let thing = bThing.getFromHandle(evdata);
    // ...
  } else if (ev == bThing.EVENT.STATE_CHANGING) {
    // ...  
  } else if (ev == bThing.EVENT.STATE_CHANGED) {
    // ...  
  }
}, null);
```
**Listening-to events** - Events a bThing is listening to. You can send them using [Event.trigger()](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#eventtrigger). You can send an event to all registered bThings or to a specific one. See example below.
```javascript
// Send the event to all registered bThings
Event.trigger(bThing.EVENT.UPDATE_STATE, null);
// Send the event to a specific bThing
Event.trigger(bThing.EVENT.UPDATE_STATE, thing);
```
|Event||
|--|--|
|UPDATE_STATE|Send this event-command to force the bThing state to be updated. After sending it, events `bThing.EVENT.STATE_CHANGING` and `bThing.EVENT.STATE_CHANGED` are forcibly triggered.|
### bThing.getAllThings
```javascript
bThing.getAllThings();
```
Returns the array of all registered bThings.

Example:
```javascript
let things = bThing.getAllThings();
for (let i = 0; i < things.length; ++i) {
  let thing = things[i];
  // do something...
}
```
### bThing.getFromHandle
```javascript
bThing.getFromHandle(handle);
```
Returns a bThing using its C/C++ handle, or `null` if not found.

|Parameter||
|--|--|
|handle|A C/C++ bThing handle.|
### bThing.getFromId
```javascript
bThing.getFromId(id);
```
Returns the bThing having the specified ID, or `null` if not found.

|Parameter||
|--|--|
|id|A bThing ID.|
### .getId
```javascript
<obj>.getId();
```
Returns the ID of a bThing, or `null` if error.
### .getType
```javascript
<obj>.getType();
```
Returns the type of a bThing, or `0` if error.

Example:
```javascript
print("The bThing type is:", <obj>.getType());
```
### .isTypeOf
```javascript
<obj>.isTypeOf(type);
```
Returns `true` if the bThing type is `type`.

|Parameter||
|--|--|
|type|The type to check.|

Example:
```javascript
if (<obj>.isTypeOf(bThing.TYPE.SENSOR))
  print("The bThing is a sensor.");
else if (<obj>.isTypeOf(bThing.TYPE.ACTUATOR))
  print("The bThing is an actuator.");
else
  print("Unknown bThing type.");
```
## To Do
- Complete javascript APIs for [Mongoose OS MJS](https://github.com/mongoose-os-libs/mjs).