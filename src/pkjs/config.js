'use strict';

var catalog = require('./catalog');
function mainMenuOptions() { return catalog.options; }

function mainMenuSlot(slotNumber, defaultValue) {
  return {
    type: 'select',
    messageKey: 'MainMenuSlot' + slotNumber,
    label: 'Slot ' + slotNumber,
    defaultValue: defaultValue,
    serializeValueAs: 'integer',
    options: mainMenuOptions()
  };
}

module.exports = [
  {
    type: 'heading',
    defaultValue: 'Orationes',
    size: 1
  },
  {
    type: 'text',
    defaultValue: 'Settings are stored on your Pebble and work fully offline.'
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Reading'
      },
      {
        type: 'select',
        messageKey: 'TextSize',
        label: 'Text size',
        defaultValue: '0',
        serializeValueAs: 'integer',
        options: [
          { label: 'Large', value: '0' },
          { label: 'Extra Large', value: '1' }
        ]
      },
      {type: 'toggle', messageKey: 'RememberPlace', label: 'Remember reading place', defaultValue: true},
      {
        type: 'select',
        messageKey: 'Appearance',
        label: 'Appearance',
        defaultValue: '0',
        serializeValueAs: 'integer',
        options: [
          { label: 'Light', value: '0' },
          { label: 'Dark', value: '1' }
        ]
      },
      {
        type: 'select',
        messageKey: 'AccentColor',
        label: 'Accent color',
        defaultValue: '0',
        serializeValueAs: 'integer',
        options: [
          { label: 'Ocean', value: '0' },
          { label: 'Burgundy', value: '1' },
          { label: 'Forest', value: '2' },
          { label: 'Monochrome', value: '3' }
        ]
      },
      {
        type: 'text',
        id: 'accent-preview',
        defaultValue: ''
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Prayer Shortcuts'
      },
      {
        type: 'text',
        defaultValue: 'Choose up to seven shortcuts. Collections open a submenu; individual prayers open directly. Select Prayer Cards to browse all cards, or Card: followed by a name to open one directly. Empty slots are omitted. Settings is always available.'
      },
      mainMenuSlot(1, String(catalog.defaults[0])),
      mainMenuSlot(2, String(catalog.defaults[1])),
      mainMenuSlot(3, String(catalog.defaults[2])),
      mainMenuSlot(4, String(catalog.defaults[3])),
      mainMenuSlot(5, String(catalog.defaults[4])),
      mainMenuSlot(6, String(catalog.defaults[5])),
      mainMenuSlot(7, String(catalog.defaults[6])),
      {
        type: 'button',
        id: 'restore-main-menu',
        defaultValue: 'Restore Main Menu Defaults'
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Noon Prayer'
      },
      {
        type: 'toggle',
        messageKey: 'NoonReminderEnabled',
        label: 'Noon reminder',
        defaultValue: false
      },
      {
        type: 'select',
        messageKey: 'NoonReminderDuration',
        label: 'Reminder duration',
        defaultValue: '1',
        serializeValueAs: 'integer',
        options: [
          { label: '5 seconds', value: '0' },
          { label: '10 seconds', value: '1' },
          { label: '30 seconds', value: '2' }
        ]
      }
    ]
  },
  {
    type: 'submit',
    defaultValue: 'Save Settings'
  }
];
