# bThings Core Library
## Overview
Mongoose OS core library for the bThings ecosystem.
## C/C++ APIs Reference
### enum mgos_bthing_event
```c
enum mgos_bthing_event {
  MGOS_EV_BTHING_ANY,
  MGOS_EV_BTHING_CREATED, 
  MGOS_EV_BTHING_STATE_UPDATED,
  MGOS_EV_BTHING_UPDATE_STATE
};
```
Events triggered by a*bThing* or on which it is listening to. Use `mgos_event_add_group_handler(MGOS_EV_BTHING_ANY, ...)` for handling all triggered events in one shot.

|Event||
|--|--|
|MGOS_EV_BTHING_CREATED|Triggered when a new *bThing* is created.|
|MGOS_EV_BTHING_STATE_UPDATED|Triggered when the state of a *bThing* has been updated. This event is triggered according the *bThing* state notification setting (see [mgos_bthing_notify_state](#enum-mgos_bthing_notify_state) below).|

Example:
```c
static void bthing_events_cb(int ev, void *ev_data, void *userdata) {
  mgos_bthing_t thing = (mgos_bthing_t)ev_data;
  if (ev == case MGOS_EV_BTHING_CREATED) {
    // ...
  } else if (ev == case MGOS_EV_BTHING_STATE_UPDATED) {
    // ...
  }
  (void) userdata;
}

mgos_event_add_group_handler(MGOS_EV_BTHING_ANY, bthing_events_cb, NULL);
```
|Event||
|--|--|
|MGOS_EV_BTHING_UPDATE_STATE|Send this event-command to force the *bThing* state to be updated. This can be sent to all registered *bThings* or to a specific one. After sending it, a `MGOS_EV_BTHING_STATE_UPDATED` event is forcibly triggered unless the *bThings* is configured as `MGOS_BTHING_NOTIFY_STATE_NEVER` (see [mgos_bthing_notify_state](#enum-mgos_bthing_notify_state) below).|

Example:
```c
// Send the message to all registered bThings
mgos_event_trigger(MGOS_EV_BTHING_UPDATE_STATE, NULL);
// Send the message to a specific bThing
mgos_event_trigger(MGOS_EV_BTHING_UPDATE_STATE, thing);
```
### enum mgos_bthing_notify_state
```c
enum mgos_bthing_notify_state {
  MGOS_BTHING_NOTIFY_STATE_NEVER,
  MGOS_BTHING_NOTIFY_STATE_ON_CHANGE,
  MGOS_BTHING_NOTIFY_STATE_ALWAYS
};
```
Ways a *bThing* can trigger the `MGOS_EV_BTHING_STATE_UPDATED` [event](#enum-mgos_bthing_event).

|Value||
|--|--|
|MGOS_BTHING_NOTIFY_STATE_NEVER|The event is never triggered.|
|MGOS_BTHING_NOTIFY_STATE_ON_CHANGE|The event is triggered only when the state is updated and its value is changed. |
|MGOS_BTHING_NOTIFY_STATE_ALWAYS|The event is triggered every time the state is updated.|
### mgos_bthing_get_id
```c
const char *mgos_bthing_get_id(mgos_bthing_t thing)
```
Returns the ID of a *bThing*, or `NULL` if error.

|Parameter||
|--|--|
|thing|A *bThing*.|
### mgos_bthing_get_type
```c
int mgos_bthing_get_type(mgos_bthing_t thing)
```
Returns the type of a *bThing*, or `0` if error.

|Parameter||
|--|--|
|thing|A *bThing*.|

Example:
```c
printf("The bThing type is: %d.", mgos_bthing_get_type(thing));
```
### mgos_bthing_is_typeof
```c
bool mgos_bthing_is_typeof(mgos_bthing_t thing, int type)
```
Returns `true` if the *bThing* type is `type`.

|Parameter||
|--|--|
|thing|A *bThing*.|
|type|The type to check.|

Example:
```c
if (mgos_bthing_is_typeof(thing, MGOS_BTHING_TYPE_SENSOR))
  printf("The bThing is a sensor.");
else if (mgos_bthing_is_typeof(thing, MGOS_BTHING_TYPE_ACTUATOR))
  printf("The bThing is an actuator.");
else
  printf("Unknown bThing type.");
```
### mgos_bthing_get
```c
mgos_bthing_t mgos_bthing_get(const char* id)
```
Returns the *bThing* having the specified ID, or `NULL` if not found.

|Parameter||
|--|--|
|id|A *bThing* ID.|
### mgos_bthing_get_all
```c
mgos_bthing_enum_t mgos_bthing_get_all()
```
Returns the enumerator for iterating all registered *bThings*, or `NULL` if error. The enumerator can be used with `mgos_bthing_get_next()`.
### mgos_bthing_get_next
```c
bool mgos_bthing_get_next(mgos_bthing_enum_t *things_enum, mgos_bthing_t *thing);
```
Gets the next *bThing* iterating registered ones. Returns `false` if the end of the enumerator is reached, or `true` otherwise.

|Parameter||
|--|--|
|things_enum|A reference to a *bThing* enumerator returned by `mgos_bthing_get_all()`.|
|thing|The output *bThing*. Optional, if `NULL` no *bThing* is returned as output.|
### (*mgos_bthing_get_state_handler_t)
```c
typedef bool (*mgos_bthing_get_state_handler_t)(mgos_bthing_t thing, mgos_bvar_t state, void *userdata);
```
*Get-state* handler signature. Must return `true` on success, or `false` otherwise. The signature is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|The *bThing* for whom to return the status.|
|state|The state value to return.|
|userdata|The handler's *user-data*.|
### (*mgos_bthing_set_state_handler_t)
```c
typedef bool (*mgos_bthing_set_state_handler_t)(mgos_bthing_t thing, mgos_bvarc_t state, void *userdata);
```
*Set-state* handler signature. Must return `true` on success, or `false` otherwise. The signature is available only `#if MGOS_BTHING_HAVE_ACTUATORS`.

|Parameter||
|--|--|
|thing|The *bThing* to set the state for.|
|state|The state value to set.|
|userdata|The handler's *user-data*.|
### mgos_bthing_set_state_handler
```c
bool mgos_bthing_on_get_state(mgos_bthing_t thing,
                              mgos_bthing_get_state_handler_t get_state_cb,
                              void *userdata);
```
Sets the *get-state* handler of a *bThing* sensor/actuator (`mgos_bthing_is_typeof(MGOS_BTHING_TYPE_SENSOR)`). Returns `true` on success, or `false` otherwise. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|A *bThing* sensor/actuator.|
|get_state_cb|The [get-state handler](#mgos_bthing_get_state_handler_t) or `NULL` to reset the handler.|
|userdata|The *user-data* to pass to the handler or `NULL`. Ignored if `get_state_cb` is `NULL`.|
### mgos_bthing_on_set_state
```c
bool mgos_bthing_on_set_state(mgos_bthing_t thing,
                              mgos_bthing_set_state_handler_t set_state_cb,
                              void *userdata);
```
Sets the *set-state* handler of a *bThing* actuator (`mgos_bthing_is_typeof(MGOS_BTHING_TYPE_ACTUATOR)`). Returns `true` on success, or `false` otherwise. This function is available only `#if MGOS_BTHING_HAVE_ACTUATORS`.

|Parameter||
|--|--|
|thing|A *bThing* actuator.|
|set_state_cb|The [set-state handler](#mgos_bthing_set_state_handler_t) or `NULL` to reset the handler.|
|userdata|The *user-data* to pass to the handlers or `NULL`. Ignored if `set_state_cb` is `NULL`.|
### mgos_bthing_get_state
```c
mgos_bvarc_t mgos_bthing_get_state(mgos_bthing_t thing);
```
Returns the state of a *bThing* sensor/actuator, or `NULL` if error. This function is available only `#if MGOS_BTHING_HAVE_SENSORS`.

|Parameter||
|--|--|
|thing|A *bThing* sensor/actuator.|
### mgos_bthing_set_state
```c
bool mgos_bthing_set_state(mgos_bthing_t thing, mgos_bvarc_t state);
```
Sets the state of a *bThing* actuator. Returns `true` on success, or `false` otherwise. This function is available only `#if MGOS_BTHING_HAVE_ACTUATORS`.

|Parameter||
|--|--|
|thing|A *bThing* actuator.|
|state|The state value to set.|
## JS APIs Reference
### bThing.EVENT
```javascript
bThing.EVENT: {
  ANY,
  CREATED,
  STATE_UPDATED,
  UPDATE_STATE
}
```
Events triggered by a*bThing* or on which it is listening to. Use `Event.addGroupHandler(bThing.EVENT.ANY, ...)` for handling all triggered events in one shot.

|Event||
|--|--|
|CREATED|Triggered when a new *bThing* is created.|
|STATE_UPDATED|Triggered when the state of a *bThing* has been updated. This event is triggered according the *bThing* state notification setting (see [bThing.NOTIFY_STATE](#bthingnotify_state) below).|

Example:
```javascript
Event.addGroupHandler(bThing.EVENT.ANY, function(ev, evdata, ud) {
  let thing = bThing.getFromHandle(evdata);
  if (ev == bThing.EVENT.CREATED) {
    // ...
  } else if (ev == bThing.EVENT.STATE_UPDATED) {
    // ...  
  }
}, null);
```
|Event||
|--|--|
|UPDATE_STATE|Send this event-command to force the *bThing* state to be updated. This can be sent to all registered *bThings* or to a specific one. After sending it, a `bThing.EVENT.STATE_UPDATED` event is forcibly triggered unless the *bThings* is configured as `bThing.NOTIFY_STATE.NEVER` (see [bThing.NOTIFY_STATE](#bthingnotify_state) below).|

Example:
```javascript
// Send the message to all registered bThings
Event.trigger(bThing.EVENT.UPDATE_STATE, null);
// Send the message to a specific bThing
Event.trigger(bThing.EVENT.UPDATE_STATE, thing);
```
### bThing.NOTIFY_STATE
```javascript
bThing.NOTIFY_STATE: {
  NEVER,
  ON_CHANGE,
  ALWAYS
}
```
Ways a *bThing* can trigger the `bThing.EVENT.STATE_UPDATED` [event](#bthingevent).

|Value||
|--|--|
|NEVER|The event is never triggered.|
|ON_CHANGE|The event is triggered only when the state is updated and its value is changed. |
|ALWAYS|The event is triggered every time the state is updated.|
### bThing.getAllThings
```javascript
bThing.getAllThings();
```
Returns the array of all registered *bThings*.

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
Returns a *bThing* using its C/C++ handle, or `null` if not found.

|Parameter||
|--|--|
|handle|A C/C++ *bThing* handle.|
### bThing.getFromId
```javascript
bThing.getFromId(id);
```
Returns the *bThing* having the specified ID, or `null` if not found.

|Parameter||
|--|--|
|id|A *bThing* ID.|
### .getId
```javascript
<obj>.getId();
```
Returns the ID of a *bThing*, or `null` if error.
### .getType
```javascript
<obj>.getType();
```
Returns the type of a *bThing*, or `0` if error.

Example:
```javascript
print("The bThing type is:", <obj>.getType());
```
### .isTypeOf
```javascript
<obj>.isTypeOf(type);
```
Returns `true` if the *bThing* type is `type`.

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
