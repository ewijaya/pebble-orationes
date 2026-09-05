'use strict';
var assert = require('assert');
var catalog = require('../src/pkjs/catalog');
var customClay = require('../src/pkjs/custom-clay');
var items = {};
function item(value) {
  var handlers = {};
  return {
    get: function() { return value; },
    set: function(next) { value = next; if (handlers.change) handlers.change.call(this); },
    on: function(name, callback) { handlers[name] = callback; },
    click: function() { handlers.click.call(this); }
  };
}
catalog.defaults.forEach(function(value, index) { items['MainMenuSlot' + (index + 1)] = item(String(value)); });
items.Appearance = item(0);
items.AccentColor = item(0);
items['accent-preview'] = item('');
items['restore-main-menu'] = item('');
customClay.call({
  meta: {userData: {defaultSlots: catalog.defaults}},
  EVENTS: {AFTER_BUILD: 'built'},
  on: function(event, handler) { handler(); },
  getItemById: function(key) { return items[key]; },
  getItemByMessageKey: function(key) { return items[key]; }
});
items.MainMenuSlot1.set('5');
assert.equal(Number(items.MainMenuSlot5.get()), 1);
items.MainMenuSlot6.set('5');
assert.equal(Number(items.MainMenuSlot1.get()), 0);
items.MainMenuSlot6.set('0');
assert.equal(Number(items.MainMenuSlot5.get()), 1);
items['restore-main-menu'].click();
catalog.defaults.forEach(function(value, index) { assert.equal(Number(items['MainMenuSlot' + (index + 1)].get()), value); });
items.MainMenuSlot1.set('2');
assert.equal(Number(items.MainMenuSlot2.get()), 1); // Previous values reset with defaults.
assert.equal(catalog.options.length, 39);
console.log('Phone shortcut swap and defaults regression tests passed');
