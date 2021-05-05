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
```c
int type = mgos_bthing_get_type(thing);
if ((type & MGOS_BTHING_TYPE_SENSOR) == MGOS_BTHING_TYPE_SENSOR)
  printf("The bThing is a sensor.");
else if ((type & MGOS_BTHING_TYPE_ACTUATOR) == MGOS_BTHING_TYPE_ACTUATOR)
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
## JS APIs Reference
### bThing.EVENT
```js
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
|STATE_UPDATED|Triggered when the state of a *bThing* has been updated. This event is triggered according the *bThing* state notification setting (see [bThing.EVENT.STATE](#bthingnotify_state) below).|

Example:
```js
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
|UPDATE_STATE|Send this event-command to force the *bThing* state to be updated. This can be sent to all registered *bThings* or to a specific one. After sending it, a `bThing.EVENT.STATE_UPDATED` event is forcibly triggered unless the *bThings* is configured as `bThing.NOTIFY_STATE.NEVER` (see [bThing.EVENT.STATE](#bthingnotify_state) below).|

Example:
```js
// Send the message to all registered bThings
Event.trigger(bThing.EVENT.UPDATE_STATE, null);
// Send the message to a specific bThing
Event.trigger(bThing.EVENT.UPDATE_STATE, thing);
```
### bThing.NOTIFY_STATE
```js
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
```js
bThing.getAllThings();
```
Returns the array of all registered *bThings*.

Example:
```js
let things = bThing.getAllThings();
for (let i = 0; i < things.length; ++i) {
  let thing = things[i];
  // do something...
}
```
### bThing.getFromHandle
```js
bThing.getFromHandle(handle);
```
Returns a *bThing* using its C/C++ handle, or `null` if not found.

|Parameter||
|--|--|
|handle|A C/C++ *bThing* handle.|
### bThing.getFromId
```js
bThing.getFromId(id);
```
Returns the *bThing* having the specified ID, or `null` if not found.

|Parameter||
|--|--|
|id|A *bThing* ID.|
### .getId
```js
<obj>.getId();
```
Returns the ID of a *bThing*, or `null` if error.
### .getType
```js
<obj>.getType();
```
Returns the type of a *bThing*, or `0` if error.

Example:
```js
let type = <obj>.getType();
if ((type & bThing.TYPE.SENSOR) == bThing.TYPE.SENSOR)
  print("The bThing is a sensor.");
else if ((type & bThing.TYPE.ACTUATOR) == bThing.TYPE.ACTUATOR)
  print("The bThing is an actuator.");
else
  print("Unknown bThing type.");
```
