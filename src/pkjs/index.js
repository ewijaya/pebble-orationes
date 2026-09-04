'use strict';

var Clay = require('@rebble/clay');
var messageKeys = require('message_keys');
var config = require('./config');
var customClay = require('./custom-clay');

var PENDING_SETTINGS_KEY = 'orationes-pending-settings';
var clay = new Clay(config, customClay, { autoHandleEvents: false });

function readPendingSettings() {
  try {
    return JSON.parse(localStorage.getItem(PENDING_SETTINGS_KEY));
  } catch (error) {
    console.log('Could not read pending Orationes settings: ' + error);
    return null;
  }
}

function savePendingSettings(settings) {
  localStorage.setItem(PENDING_SETTINGS_KEY, JSON.stringify(settings));
}

function clearPendingSettings() {
  localStorage.removeItem(PENDING_SETTINGS_KEY);
}

function sendSettings(settings, clearWhenSent) {
  Pebble.sendAppMessage(settings, function() {
    if (clearWhenSent) {
      clearPendingSettings();
    }
    console.log('Sent Orationes settings to Pebble');
  }, function(error) {
    console.log('Could not send Orationes settings: ' + JSON.stringify(error));
  });
}

function requestWatchSettings() {
  var request = {};
  request[messageKeys.SettingsRequest] = 1;
  sendSettings(request, false);
}

function snapshotForClay(payload) {
  var settings = {};
  Object.keys(messageKeys).forEach(function(name) {
    if (name === 'SettingsRequest') {
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
  var pending = readPendingSettings();
  if (pending) {
    sendSettings(pending, true);
  } else {
    requestWatchSettings();
  }
});

Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(event) {
  if (!event || !event.response) {
    return;
  }

  try {
    var settings = clay.getSettings(event.response);
    savePendingSettings(settings);
    sendSettings(settings, true);
  } catch (error) {
    console.log('Could not parse Orationes settings: ' + error);
  }
});

Pebble.addEventListener('appmessage', function(event) {
  var snapshot = snapshotForClay(event.payload || {});
  if (Object.keys(snapshot).length > 0) {
    clay.setSettings(snapshot);
  }
});
