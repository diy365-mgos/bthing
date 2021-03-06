let bThing = {
  _free: ffi('void free(void *)'),
  _gty: ffi('int mgos_bthing_get_type(void *)'),
  _tof: ffi('bool mgos_bthing_is_typeof(void *, int)'),
  _gid: ffi('char *mgos_bthing_get_uid(void *)'),

  _getSafe: function(obj, val) {
  	return ((obj === null || obj === undefined) ? val : obj);
  },

  _onCreate: [],
  // register on-create handler
  _onCreateSub: function(f) {
    this._onCreate.push(f);
  },

  // raise on-create event
  _onCreatePub: function(t) {
    for (let i = 0; i < this._onCreate.length; ++i) {
    	this._onCreate[i](t);
    }
  },

  _things: [],
  // register a bThing
  _regT: function(t) {
    if (t === null || t === undefined) return false;
    this._things.push(t);
    return true;
  },

  getAllThings: function() {
    return this._things;
  },

  getFromHandle: function(h) {
    for (let i = 0; i < this._things.length; ++i) {
      if (h === this._things[i].handle) {
        return this._things[i];
      }
    }
    return null;
  },

  getFromId: function(id) {
    for (let i = 0; i < this._things.length; ++i) {
      if (id === this._things[i].getId()) {
        return this._things[i];
      }
    }
    return null;
  },

  NO_TICKS: 0,
  NO_PIN: -1,

  TYPE: {
    SENSOR: 1,
    ACTUATOR: 3,
  },
  
  // Event codes 
  EVENT: {
    ANY: 0, // TODO: update
    CREATED: 0,
    STATE_CHANGING: 0,
    STATE_CHANGED: 0,
    STATE_UPDATED: 0,
  },
  
  _proto: {
    getHandle: function() {
      return this.handle;
    },

    getId: function() {
      return bThing._gid(this.handle);
    },

    getType: function() {
      return bThing._gty(this.handle);
    },

    isTypeOf: function(type) {
      return bThing._tof(this.handle, type);
    },
  },
};

bThing._onCreateSub(function(t) {
  bThing._regT(t);
});
