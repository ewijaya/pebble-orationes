'use strict';
var assert = require('assert');
var createSync = require('../src/pkjs/settings-sync');
var values = {};
var messages = [];
var timers = {};
var sequence = 0;
var options = {
  keys: {SettingsTransaction: 100, SettingsAck: 101, SettingsStatus: 102},
  storage: {
    getItem: function(key) { return values[key] || null; },
    setItem: function(key, value) { values[key] = value; },
    removeItem: function(key) { delete values[key]; }
  },
  send: function(message) { messages.push(message); },
  schedule: function(callback) { timers[++sequence] = callback; return sequence; },
  cancel: function(id) { delete timers[id]; }
};
var sync = createSync(options);
sync.start();
sync.submit({1: 1});
var first = messages[messages.length - 1][100];
assert(values['orationes-pending-settings']); // Delivery isn't a durable ACK.
sync.receive({101: first + 1, 102: 0});
assert(values['orationes-pending-settings']);
sync.receive({101: first, 102: 0});
assert(!values['orationes-pending-settings']);
assert.equal(Object.keys(timers).length, 0);
sync.submit({1: 0});
var second = messages[messages.length - 1][100];
sync.submit({1: 1});
var third = messages[messages.length - 1][100];
sync.receive({SettingsAck: second, SettingsStatus: 0});
assert(values['orationes-pending-settings']); // Old ACK cannot clear new work.
sync.receive({SettingsAck: third, SettingsStatus: 1});
assert(sync.status().includes('could not save'));
var count = messages.length;
sync = createSync(options);
sync.start();
assert.equal(messages.length, count); // Rejection requires an explicit new save.
sync.submit({1: 0});
timers = {}; // JS process ends, localStorage survives.
sync = createSync(options);
sync.start();
assert.equal(messages[messages.length - 1][1], 0);
for (var i = 0; i < 6; i++) {
  var id = Object.keys(timers)[0];
  if (id) { var callback = timers[id]; delete timers[id]; callback(); }
}
assert.equal(Object.keys(timers).length, 0);
assert(values['orationes-pending-settings']); // Bounded retries retain work.
console.log('Settings synchronization regression tests passed');
