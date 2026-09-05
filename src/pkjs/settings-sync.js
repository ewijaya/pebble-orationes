'use strict';

// The transport callback confirms delivery only. Keep the transaction until the
// watch acknowledges a successful durable save. Unacknowledged work survives JS
// restarts; only the current transaction may be acknowledged or retried.
module.exports = function(options) {
  var key = 'orationes-pending-settings';
  var pending = null;
  var timer = null;
  var attempts = 0;
  function persist() {
    if (pending) options.storage.setItem(key, JSON.stringify(pending));
    else options.storage.removeItem(key);
  }
  function stop() {
    if (timer !== null) options.cancel(timer);
    timer = null;
  }
  function transmit() {
    stop();
    if (!pending || pending.blocked || attempts >= 5) return;
    var message = {};
    Object.keys(pending.settings).forEach(function(k) { message[k] = pending.settings[k]; });
    message[options.keys.SettingsTransaction] = pending.id;
    attempts += 1;
    timer = options.schedule(transmit, Math.min(30000, 1000 * Math.pow(2, attempts)));
    options.send(message);
  }
  function submit(settings) {
    var last = Number(options.storage.getItem('orationes-settings-sequence')) || 0;
    var id = (last + 1) % 2147483647 || 1;
    options.storage.setItem('orationes-settings-sequence', String(id));
    pending = {id: id, settings: settings, blocked: false};
    attempts = 0;
    persist();
    transmit();
  }
  return {
    start: function() {
      try { pending = JSON.parse(options.storage.getItem(key)); } catch (error) { pending = null; }
      if (pending && !pending.settings) { submit(pending); return; } // Upgrade old queue.
      attempts = 0;
      transmit();
    },
    submit: submit,
    receive: function(payload) {
      var id = payload.SettingsAck;
      if (typeof id === 'undefined') id = payload[options.keys.SettingsAck];
      var status = payload.SettingsStatus;
      if (typeof status === 'undefined') status = payload[options.keys.SettingsStatus];
      if (!pending || Number(id) !== pending.id || typeof status === 'undefined') return;
      stop();
      if (Number(status) === 0) pending = null;
      else pending.blocked = true;
      persist();
    },
    draft: function() { return pending ? pending.settings : null; },
    status: function() {
      if (!pending) return 'Settings are saved on your watch.';
      return pending.blocked
        ? 'The watch could not save your changes. Review the settings and save again.'
        : 'Changes are waiting for confirmation from your watch. Open Orationes on the watch to sync.';
    }
  };
};
