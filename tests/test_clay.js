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
items.TextSize = item(0);
items.NavigationHighlight = item(0);
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

[0,1].forEach(function(appearance) {
  items.Appearance.set(appearance);
  [0,1,2,3].forEach(function(accent) {
    items.AccentColor.set(accent);
    [0,1,2,3,4,5].forEach(function(nav) {
      items.NavigationHighlight.set(nav);
      var preview=items['accent-preview'].get();
      assert(preview.indexOf('Selected prayer')!==-1);
      assert(preview.indexOf('undefined')===-1);
      assert.equal(items.AccentColor.get(),accent);
    });
  });
});
items.TextSize.set(1);assert(items['accent-preview'].get().indexOf('font-size:34px')!==-1);
