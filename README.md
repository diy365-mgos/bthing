# bThings Core Library
## Overview
Mongoose OS core library for the bThings ecosystem.
|Sensors|Actuators|Others|
|--|--|--|
|[bSensors](https://github.com/diy365-mgos/bsensor), [bBinarySensors](https://github.com/diy365-mgos/bbinsensor), [bButtons](https://github.com/diy365-mgos/bbutton)|[bActuators](https://github.com/diy365-mgos/bactuator), [bBinaryActuators](https://github.com/diy365-mgos/bbinactuator), [bSwitches](https://github.com/diy365-mgos/bswitch)|[MQTT support](https://github.com/diy365-mgos/bthing-mqtt), [State-shadow suport](https://github.com/diy365-mgos/bthing-shadow), [GPIO support](https://github.com/diy365-mgos/bthing-gpio)|
## C/C++ APIs Reference
### mgos_bthing_event
```c
enum mgos_bthing_event {
  MGOS_EV_BTHING_CREATED,
  MGOS_EV_BTHING_STATE_CHANGING,
  MGOS_EV_BTHING_STATE_CHANGED,
  MGOS_EV_BTHING_STATE_UPDATED,
  MGOS_EV_BTHING_STATE_PUBLISHING
};
```
Events triggered by a bThing. Use following functions to subscribe to these events:
* [mgos_event_add_handler()](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_add_handler)
* [mgos_event_add_group_handler(MGOS_EV_BTHING_ANY, ...)](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_add_group_handler)
* [mgos_bthing_on_event()](https://github.com/diy365-mgos/bthing#mgos_bthing_on_event)

|Event||
|--|--|
|MGOS_EV_BTHING_CREATED|Triggered when a new bThing is created. The event-data passed to the handler is a `mgos_bthing_t`.|
|MGOS_EV_BTHING_STATE_CHANGING|Triggered when the state of a bThing is going to change. The event-data passed to the handler is a `struct mgos_bthing_state_change*`.|
|MGOS_EV_BTHING_STATE_CHANGED|Triggered when the state of a bThing is changed. The event-data passed to the handler is a `struct mgos_bthing_state*`.|
|MGOS_EV_BTHING_STATE_UPDATED|Triggered when the state of a bThing has been updated after invoking `mgos_bthing_update_state()` or '`mgos_bthing_update_states()`' function. It is triggered also if the state is not changed. The event-data passed to the handler is a `struct mgos_bthing_state*`.|
|MGOS_EV_BTHING_STATE_PUBLISHING|...|
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
|state_flags|State's flags. It could be a combination of more flags (see `enum mgos_bthing_state_flag` below).|
|state|The state.|
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
|state_flags|State's flags. It could be a combination of more flags (see `enum mgos_bthing_state_flag` below).|
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
### mgos_bthing_state_flag
```c
enum mgos_bthing_state_flag {
  MGOS_BTHING_STATE_FLAG_UNCHANGED = 1,     // 0000001
  MGOS_BTHING_STATE_FLAG_INITIALIZING = 6,  // 0000110
  MGOS_BTHING_STATE_FLAG_CHANGING = 4,      // 0000100
  MGOS_BTHING_STATE_FLAG_INITIALIZED = 24,  // 0011000
  MGOS_BTHING_STATE_FLAG_CHANGED = 16,      // 0010000
  MGOS_BTHING_STATE_FLAG_UPDATED = 32       // 0100000
  MGOS_BTHING_STATE_FLAG_FORCED_PUB = 64    // 1000000
};
```
|Flag||
|--|--|
|`MGOS_BTHING_STATE_FLAG_UNCHANGED`|The state was not changed.|
|`MGOS_BTHING_STATE_FLAG_INITIALIZING`|The state is going to be initialized. This flag includes `MGOS_BTHING_STATE_FLAG_CHANGING`.|
|`MGOS_BTHING_STATE_FLAG_CHANGING`|The state is going to be changed.|
|`MGOS_BTHING_STATE_FLAG_INITIALIZED`|The state has been initialized. This flag includes  `MGOS_BTHING_STATE_FLAG_CHANGED`.|
|`MGOS_BTHING_STATE_FLAG_CHANGED`|The state has been changed.|
|`MGOS_BTHING_STATE_FLAG_UPDATED`|The state has been updated, but not necessary changed. This flag is set when the `MGOS_EV_BTHING_STATE_UPDATED` event is triggered after invoking `mgos_bthing_update_state()` or '`mgos_bthing_update_states()`' function.|
|`MGOS_BTHING_STATE_FLAG_FORCED_PUB`|...|
### mgos_bthing_get_uid
```c
const char *mgos_bthing_get_uid(mgos_bthing_t thing);
```
Returns the uniue ID of a bThing, or `NULL` if error.

|Parameter||
|--|--|
|thing|A bThing.|
### mgos_bthing_get_id
```c
const char *mgos_bthing_get_id(mgos_bthing_t thing);
```
Returns the ID of a bThing, or `NULL` if error.

|Parameter||
|--|--|
|thing|A bThing.|
### mgos_bthing_get_domain
```c
const char *mgos_bthing_get_domain(mgos_bthing_t thing);
```
Returns the bThing's domain name, or `NULL` if the bThing doesn't belong to any domain (see `mgos_bthing_set_domain()`).

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
### mgos_bthing_get_by_uid
```c
mgos_bthing_t mgos_bthing_get_by_uid(const char* uid);
```
Returns the bThing having the specified unique ID, or `NULL` if not found.

|Parameter||
|--|--|
|uid|A bThing unique ID.|
### mgos_bthing_get_by_id
```c
mgos_bthing_t mgos_bthing_get_by_id(const char* id, const char *domain);
```
Returns the bThing having the specified ID and domain, or `NULL` if not found.

|Parameter||
|--|--|
|id|A bThing ID.|
|domain|The domain name.|
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
### mgos_bthing_filter_get_next
```c
bool mgos_bthing_filter_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing,
                                 enum mgos_bthing_filter_by filter, ...);
```
Gets the next filtered bThing, iterating registered ones. Returns `false` if the end of the enumerator is reached, or `true` otherwise.

|Parameter||
|--|--|
|things_enum|A reference to a bThing enumerator returned by `mgos_bthing_get_all()`.|
|thing|The output bThing. Optional, if `NULL` no bThing is returned as output.|
|filter|The filter type to apply.|
|...|Dynamic filter value.|

Examples:
```c
// Example #1 - Loop on all instances
mgos_bthing_filter_get_next(things_enum, thing, MGOS_BTHING_FILTER_BY_NOTHING);
// Example #2 - Loop on all in "lights" domain instances
mgos_bthing_filter_get_next(things_enum, thing, MGOS_BTHING_FILTER_BY_DOMAIN, "lights");
// Example #3 - Loop on all bSwitches
mgos_bthing_filter_get_next(things_enum, thing, MGOS_BTHING_FILTER_BY_TYPE, MGOS_BSWITCH_TYPE);
```
### mgos_bthing_filter_by
```c
enum mgos_bthing_filter_by {
  MGOS_BTHING_FILTER_BY_NOTHING = 0,
  MGOS_BTHING_FILTER_BY_TYPE = 1,
  MGOS_BTHING_FILTER_BY_DOMAIN = 2
}
```
Filters used by `mgos_bthing_filter_get_next()` itherating registered bThings.
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
int mgos_bthing_update_states(enum mgos_bthing_filter_by filter, ...);
```
Updates the state of all bThings matching the provided filter. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`. Returns the count of successfully updated states.

|Parameter||
|--|--|
|filter|The filter type to apply.|
|...|Dynamic filter value.|

Examples:
```c
// Example #1 - Update all states
mgos_bthing_update_states(MGOS_BTHING_FILTER_BY_NOTHING);
// Example #2 - Update all in "lights" domain states
mgos_bthing_update_states(MGOS_BTHING_FILTER_BY_DOMAIN, "lights");
// Example #3 - Update the state of all bSwitches
mgos_bthing_update_states(MGOS_BTHING_FILTER_BY_TYPE, MGOS_BSWITCH_TYPE);
```
### mgos_bthing_updatable_state
```c
struct mgos_bthing_updatable_state {
  mgos_bthing_t owner;
  mgos_bvar_t value;
};
```
Updatable in-memory state value of a bThing. It is used by [mgos_bthing_start_update_state()](#mgos_bthing_start_update_state) and [mgos_bthing_end_update_state()](#mgos_bthing_end_update_state) functions.

|Field||
|--|--|
|owner|The bThing owner of the state.|
|value|The updatable state value.|
### mgos_bthing_start_update_state
```c
bool mgos_bthing_start_update_state(mgos_bthing_t thing, struct mgos_bthing_updatable_state *state);
```
Gets the updatable in-memory state value of a bThing. Returns `true` on success, or `false` otherwise. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|A bThing sensor/actuator.|
|state|Output in-memory [updatable state](#mgos_bthing_updatable_state).|

**Remarks**

The provided updatable in-memory state value can be updated using [bVar functions](https://github.com/diy365-mgos/bvar). The [mgos_bthing_end_update_state()](#mgos_bthing_end_update_state) function must be invoked to complete the state modification process.

Example:
```c
struct mgos_bthing_updatable_state state;
if (mgos_bthing_start_update_state(thing, &state)) {
  mgos_bvar_set_decimal(state.value, 36.72);
  mgos_bthing_end_update_state(state);
}
```
### mgos_bthing_end_update_state
```c
bool mgos_bthing_end_update_state(struct mgos_bthing_updatable_state state);
```
Completes and closes the state update process initiated by the [mgos_bthing_start_update_state()](#mgos_bthing_start_update_state) function. Returns `true` on success, or `false` otherwise. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|state|The updated in-memory [updatable state](#mgos_bthing_updatable_state).|
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
|ev|The [event](#mgos_bthing_event).|
|handler|The event handler to add (see [mgos_event_handler_t](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_event.h.md#mgos_event_handler_t)).|
|userdata|The handler's *user-data* or `NULL`.|
### mgos_bthing_make_private
```c
void mgos_bthing_make_private(mgos_bthing_t thing);
```
Makes the bThing private. A private instance is not included into the shadow state (see [bThings Shadow library](https://github.com/diy365-mgos/bthing-shadow)) and it is not published via MQTT (see [bThings MQTT Library](https://github.com/diy365-mgos/bthing-mqtt)) or HTTP.

|Parameter||
|--|--|
|thing|A bThing.|
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
|STATE_UPDATED|Triggered when the state of a bThing has been updated after invoking `mgos_bthing_update_state()` or '`mgos_bthing_update_states()`' function. It is triggered also if the state is not changed. The event-data passed to the handler is a `struct mgos_bthing_state*`.|

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