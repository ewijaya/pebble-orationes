'use strict';

// Exercise the actual entry point and installed Clay parser. Mobile WebView
// timers require the global receiver; permissive Node timer mocks hide this.
var assert = require('assert');
var fs = require('fs');
var vm = require('vm');
var config = require('../src/pkjs/config');
var keys = {};
require('../package.json').pebble.messageKeys.forEach(function(name, index) {
  keys[name] = 10000 + index;
});
var stored = {};
function start() {
  var events = {}, messages = [], logs = [];
  var context = vm.createContext({
    console: {log: function(message) { logs.push(message); }},
    localStorage: {
      getItem: function(key) { return stored[key] || null; },
      setItem: function(key, value) { stored[key] = value; },
      removeItem: function(key) { delete stored[key]; }
    },
    Pebble: {
      addEventListener: function(name, handler) {
        (events[name] || (events[name] = [])).push(handler);
      },
      getActiveWatchInfo: function() { return {platform: 'emery'}; },
      getAccountToken: function() { return ''; },
      getWatchToken: function() { return ''; },
      openURL: function() {},
      sendAppMessage: function(message, success) { messages.push(message); success(); }
    }
  });
  vm.runInContext('var timers = {}, timerId = 0; ' +
    'function setTimeout(callback) { if (this !== globalThis) throw new TypeError("Illegal invocation"); ' +
    'timers[++timerId] = callback; return timerId; } ' +
    'function clearTimeout(id) { if (this !== globalThis) throw new TypeError("Illegal invocation"); delete timers[id]; }', context);
  function load(filename, resolver) {
    var module = {exports: {}};
    vm.runInContext('(function(require, module, exports) {\n' + fs.readFileSync(filename, 'utf8') + '\n})', context)(resolver, module, module.exports);
    return module.exports;
  }
  // Use the published bundle; dist/js is a copy created only by the Pebble SDK.
  var clay = load(require.resolve('@rebble/clay/src/js'), function(name) {
    assert.equal(name, 'message_keys'); return keys;
  });
  load(require.resolve('../src/pkjs/index'), function(name) {
    if (name === '@rebble/clay') return clay;
    if (name === 'message_keys') return keys;
    return require('../src/pkjs/' + name);
  });
  function emit(name, event) { events[name].forEach(function(handler) { handler(event); }); }
  emit('ready');
  return {emit: emit, messages: messages, logs: logs, context: context};
}
var response = {};
function defaults(items) {
  items.forEach(function(item) {
    if (item.items) defaults(item.items);
    if (item.messageKey) response[item.messageKey] = {value:
      item.serializeValueAs === 'integer' ? Number(item.defaultValue) : item.defaultValue};
  });
}
defaults(config);
response.NavigationHighlight.value = 3;
response.MainMenuSlot3.value = 23; // Reported failure: Slot 3 -> Aspirations.
var phone = start();
phone.emit('webviewclosed', {response: encodeURIComponent(JSON.stringify(response))});
assert.equal(phone.messages.length, 2, 'Save must send the settings, not throw: ' + phone.logs.join('; '));
var sent = phone.messages[1];
assert.equal(sent[keys.NavigationHighlight], 3);
assert.equal(sent[keys.MainMenuSlot3], 23);
assert.equal(sent[keys.RememberPlace], 1);
assert(Object.keys(sent).every(function(key) { return typeof sent[key] === 'number'; }));
assert(stored['orationes-pending-settings']);

// Replacing a pending save exercises cancellation as well as scheduling.
response.NavigationHighlight.value = 4;
phone.emit('webviewclosed', {response: JSON.stringify(response)});
assert.equal(phone.messages.length, 3);
var latest = phone.messages[2];
assert.equal(Object.keys(phone.context.timers).length, 1);
phone.emit('appmessage', {payload: {SettingsAck: sent[keys.SettingsTransaction], SettingsStatus: 0}});
assert(stored['orationes-pending-settings']);

// Unconfirmed settings survive a JS restart and are sent again.
phone = start();
assert.equal(phone.messages[0][keys.NavigationHighlight], 4);
var ack = {};
ack[keys.SettingsAck] = latest[keys.SettingsTransaction];
ack[keys.SettingsStatus] = 0;
ack[keys.NavigationHighlight] = 4;
phone.emit('appmessage', {payload: ack});
assert(!stored['orationes-pending-settings']);
assert.equal(Object.keys(phone.context.timers).length, 0);
assert.equal(JSON.parse(stored['clay-settings']).NavigationHighlight, 4);
assert.deepEqual(phone.logs, []);
console.log('Clay save, mobile timers, restart, and watch confirmation integration passed');
