'use strict';

var Clay = require('@rebble/clay');
var messageKeys = require('message_keys');
var config = require('./config');
var customClay = require('./custom-clay');

var clay = new Clay(config, customClay, { autoHandleEvents: false, userData: {
  defaultSlots: require('./catalog').defaults,
  settingsDefaults: require('./settings-defaults')
} });
function sendSettings(settings) {
  Pebble.sendAppMessage(settings, function() {}, function(error) {
    console.log('Could not send Orationes settings: ' + JSON.stringify(error));
  });
}
var sync = require('./settings-sync')({
  keys: messageKeys, storage: localStorage, send: sendSettings,
  // WebView timers require their global receiver. Passing them directly makes
  // settings-sync call them with its options object and throws before sending.
  schedule: function(callback, delay) { return setTimeout(callback, delay); },
  cancel: function(timer) { clearTimeout(timer); }
});

function requestWatchSettings() {
  var request = {};
  request[messageKeys.SettingsRequest] = 1;
  sendSettings(request);
}

function snapshotForClay(payload) {
  var settings = {};
  Object.keys(messageKeys).forEach(function(name) {
    if (name.indexOf('Settings') === 0) {
      return;
    }
    var numericKey = messageKeys[name];
    if (typeof payload[name] !== 'undefined') {
      settings[name] = payload[name];
    } else if (typeof payload[numericKey] !== 'undefined') {
      settings[name] = payload[numericKey];
    }
  });
  return settings;
}

Pebble.addEventListener('ready', function() {
  sync.start();
  requestWatchSettings();
});

var configurationTimer = null;
function openConfiguration(fresh) {
  if (configurationTimer === null) return;
  clearTimeout(configurationTimer);
  configurationTimer = null;
  clay.config[1].defaultValue = fresh ? sync.status()
    : 'The watch did not reply. Showing cached settings. Open Orationes on the watch, then reopen this page to refresh.';
  Pebble.openURL(clay.generateUrl());
}
Pebble.addEventListener('showConfiguration', function() {
  if (configurationTimer !== null) clearTimeout(configurationTimer);
  configurationTimer = setTimeout(function() { openConfiguration(false); }, 2000);
  requestWatchSettings();
});

Pebble.addEventListener('webviewclosed', function(event) {
  if (!event || !event.response) {
    return;
  }

  var settings;
  try {
    settings = clay.getSettings(event.response);
  } catch (error) {
    console.log('Could not parse Orationes settings: ' + error);
    return;
  }
  try {
    sync.submit(settings);
  } catch (error) {
    console.log('Could not queue Orationes settings: ' + error);
  }
});

Pebble.addEventListener('appmessage', function(event) {
  sync.receive(event.payload || {});
  var snapshot = snapshotForClay(sync.draft() || event.payload || {});
  if (Object.keys(snapshot).length > 0) {
    clay.setSettings(snapshot);
  }
  // A full watch snapshot answers the request; partial notifications do not.
  if (typeof snapshotForClay(event.payload || {}).TextSize !== 'undefined') {
    openConfiguration(true);
  }
});
