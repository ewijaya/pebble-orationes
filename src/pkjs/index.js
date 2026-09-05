'use strict';

var Clay = require('@rebble/clay');
var messageKeys = require('message_keys');
var config = require('./config');
var customClay = require('./custom-clay');

var clay = new Clay(config, customClay, { autoHandleEvents: false, userData: {defaultSlots: require('./catalog').defaults} });
function sendSettings(settings) {
  Pebble.sendAppMessage(settings, function() {}, function(error) {
    console.log('Could not send Orationes settings: ' + JSON.stringify(error));
  });
}
var sync = require('./settings-sync')({
  keys: messageKeys, storage: localStorage, send: sendSettings,
  schedule: setTimeout, cancel: clearTimeout
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

Pebble.addEventListener('showConfiguration', function() {
  clay.config[1].defaultValue = sync.status();
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(event) {
  if (!event || !event.response) {
    return;
  }

  try {
    var settings = clay.getSettings(event.response);
    sync.submit(settings);
  } catch (error) {
    console.log('Could not parse Orationes settings: ' + error);
  }
});

Pebble.addEventListener('appmessage', function(event) {
  sync.receive(event.payload || {});
  var snapshot = snapshotForClay(sync.draft() || event.payload || {});
  if (Object.keys(snapshot).length > 0) {
    clay.setSettings(snapshot);
  }
});
