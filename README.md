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
  MGOS_EV_BTHING_CREATED,
  MGOS_EV_BTHING_STATE_CHANGING,
  MGOS_EV_BTHING_STATE_CHANGED,
  MGOS_EV_BTHING_STATE_UPDATED
};
```
Events triggered by a bThing. Use [mgos_event_add_handler()](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_add_handler) or [mgos_event_add_group_handler(MGOS_EV_BTHING_ANY, ...)](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_add_group_handler) for subscribing to them.

|Event||
|--|--|
|MGOS_EV_BTHING_CREATED|Triggered when a new bThing is created. The event-data passed to the handler is a `mgos_bthing_t`.|
|MGOS_EV_BTHING_STATE_CHANGING|Triggered when the state of a bThing is going to change. The event-data passed to the handler is a `struct mgos_bthing_state_change*`.|
|MGOS_EV_BTHING_STATE_CHANGED|Triggered when the state of a bThing is changed. The event-data passed to the handler is a `struct mgos_bthing_state*`.|
|MGOS_EV_BTHING_STATE_UPDATED|Triggered when the state of a bThing has been updated. It is triggered also if the state is not changed. The event-data passed to the handler is a `struct mgos_bthing_state*`.|
### mgos_bthing_state
```c
struct mgos_bthing_state {
  mgos_bthing_t thing;
  enum mgos_bthing_state_flag state_flags;
  mgos_bvarc_t state;
};
```
Event-data passed to `MGOS_EV_BTHING_STATE_UPDATED` and `MGOS_EV_BTHING_STATE_CHANGED` event's handlers (see [mgos_event_handler_t](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_handler_t)).

|Field||
|--|--|
|thing|The bThing which state has been updated.|
|state_flags|The combination of one or more state's flags (see `enum mgos_bthing_state_flag` below).|
|state|The state.|
```
### mgos_bthing_state_change
```c
struct mgos_bthing_state_change {
  mgos_bthing_t thing;
  enum mgos_bthing_state_flag state_flags;
  mgos_bvarc_t cur_state;
  mgos_bvarc_t new_state;
};
```
Event-data passed to `MGOS_EV_BTHING_STATE_CHANGING` event's handler (see [mgos_event_handler_t](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_handler_t)).

|Field||
|--|--|
|thing|The bThing which state is going to change.|
|state_flags|The combination of one or more state's flags (see `enum mgos_bthing_state_flag` below).|
|cur_state|The current state.|
|new_state|The new state.|

**Remarks**

This struct inherits from `struct mgos_bthing_state`, so it can be casted to it.
```c
// struct mgos_bthing_state
//   -> struct mgos_bthing_state_change

struct mgos_bthing_state_change arg;
struct mgos_bthing_state *upd_arg = (struct mgos_bthing_state *)&arg;
```
### enum mgos_bthing_state_flag
```c
enum mgos_bthing_state_flag {
  MGOS_BTHING_STATE_FLAG_UNCHANGED = 0,     // 0000
  MGOS_BTHING_STATE_FLAG_CHANGING = 1,      // 0001
  MGOS_BTHING_STATE_FLAG_CHANGED = 3,       // 0011
  MGOS_BTHING_STATE_FLAG_INITIALIZING = 5,  // 0101
  MGOS_BTHING_STATE_FLAG_INITIALIZED = 15,  // 1111
};
```
|Flag||
|--|--|
|`MGOS_BTHING_STATE_FLAG_UNCHANGED`|The state was not changed.|
|`MGOS_BTHING_STATE_FLAG_CHANGING`|The state is going to be changed.|
|`MGOS_BTHING_STATE_FLAG_CHANGED`|The state has been changed. This flag includes `MGOS_BTHING_STATE_FLAG_CHANGING`.|
|`MGOS_BTHING_STATE_FLAG_INITIALIZING`|The state is going to be initialized. This flag includes `MGOS_BTHING_STATE_FLAG_CHANGING`.|
|`MGOS_BTHING_STATE_FLAG_INITIALIZED`|The state has been initialized. This flag includes `MGOS_BTHING_STATE_FLAG_INITIALIZING` and `MGOS_BTHING_STATE_FLAG_CHANGED`.|
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
|state|The state to return.|
|userdata|The handler's *user-data*.|
### mgos_bthing_update_state
```c
bool mgos_bthing_update_state(mgos_bthing_t thing);
```
Updates the state of a bThing sensor/actuator (`mgos_bthing_is_typeof(MGOS_BTHING_TYPE_SENSOR)`). Returns `true` on success, or `false` otherwise. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|A bThing sensor/actuator.|
### mgos_bthing_update_states
```c
void mgos_bthing_update_states(int bthing_type);
```
Updates the state of all bThings of type `bthing_type`. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|bthing_type|The type of bThings or `MGOS_BTHING_TYPE_ANY`.|
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
|state|The state to set.|
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
|state|The state to set.|
### mgos_bthing_on_event
```c
void mgos_bthing_on_event(mgos_bthing_t thing, enum mgos_bthing_event ev,
                          mgos_event_handler_t handler, void *userdata);
```
Adds en event handler for the bThing. If the handler (*ev + handler + userdata*) is already registered nothing is done. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|A bThing.|
|ev|The event.|
|handler|The event handler to add.|
|userdata|The handler's *user-data* or `NULL`.|
## JS APIs Reference
### bThing.EVENT
```javascript
bThing.EVENT: {
  ANY,
  CREATED,
  STATE_CHANGING,
  STATE_CHANGED,
  STATE_UPDATED
}
```
Events triggered by a bThing. Use [Event.addHandler()](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#eventaddhandler) or [Event.addGroupHandler(bThing.EVENT.ANY, ...)](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#eventaddgrouphandler) for subscribing to them.

|Event||
|--|--|
|CREATED|Triggered when a new bThing is created. The event-data passed to the handler is a `mgos_bthing_t`.|
|STATE_CHANGING|Triggered when the state of a bThing is going to change. The event-data passed to the handler is a `struct mgos_bthing_state_change*`.|
|STATE_CHANGED|Triggered when the state of a bThing is changed. The event-data passed to the handler is a `struct mgos_bthing_state*`.|
|STATE_UPDATED|Triggered when the state of a bThing has been updated. It is triggered also if the state is not changed. The event-data passed to the handler is a `struct mgos_bthing_state*`.|

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
  } else if (ev == bThing.EVENT.STATE_UPDATED) {
    // ...
  }
}, null);
```
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