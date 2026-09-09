'use strict';
var assert = require('assert');
var catalog = require('../src/pkjs/catalog');
var customClay = require('../src/pkjs/custom-clay');
var defaults = require('../src/pkjs/settings-defaults');
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
items.RememberPlace = item(false);
items.ContinueFirst = item(true);
items.CompactMenus = item(true);
items.NoonReminderEnabled = item(true);
items.NoonReminderDuration = item(2);
items['restore-defaults'] = item('');
customClay.call({
  meta: {userData: {defaultSlots: catalog.defaults, settingsDefaults: defaults}},
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
items['restore-defaults'].click();
catalog.defaults.forEach(function(value, index) { assert.equal(Number(items['MainMenuSlot' + (index + 1)].get()), value); });
items.MainMenuSlot1.set('2');
assert.equal(Number(items.MainMenuSlot2.get()), 1); // Previous values reset with defaults.
assert.equal(catalog.options.length, 41);
assert.deepStrictEqual(catalog.options.slice(-2), [
  {label: 'Come, Holy Spirit', value: '39'},
  {label: 'Litany of Humility', value: '40'}
]);
items.MainMenuSlot6.set('39');
items.MainMenuSlot7.set('40');
items.MainMenuSlot6.set('40');
assert.equal(Number(items.MainMenuSlot7.get()), 39);
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
items.Appearance.set(0); items.AccentColor.set(2); items.NavigationHighlight.set(3);
items.RememberPlace.set(false); items.ContinueFirst.set(true); items.CompactMenus.set(true);
items.NoonReminderEnabled.set(true); items.NoonReminderDuration.set(2);
items['restore-defaults'].click();
assert.equal(Number(items.Appearance.get()), 1);
assert.equal(Number(items.AccentColor.get()), 0);
assert.equal(Number(items.NavigationHighlight.get()), 5);
Object.keys(defaults).forEach(function(key) { assert.strictEqual(items[key].get(), defaults[key]); });
assert.deepStrictEqual(catalog.defaults.map(function(_, i) { return Number(items['MainMenuSlot'+(i+1)].get()); }), catalog.defaults);
var preview = items['accent-preview'].get();
['#00AAAA', '#AAFF00', 'background:#000000;color:#FFFFFF'].forEach(function(color) { assert(preview.includes(color)); });
items['restore-defaults'].click();
assert.equal(Number(items.Appearance.get()), 1);
assert.equal(Number(items.NavigationHighlight.get()), 5);
console.log('Clay single reset restores every setting, shortcuts and preview');
