'use strict';

function mainMenuOptions() {
  return [
    { label: 'Empty', value: '0' },
    { label: 'Preces', value: '1' },
    { label: 'Holy Rosary', value: '2' },
    { label: 'Regina Caeli', value: '3' },
    { label: 'Angelus', value: '4' },
    { label: 'Memorare', value: '5' },
    { label: 'Litany of Loreto', value: '6' },
    { label: 'More Prayers', value: '7' },
    { label: 'Mental Prayer', value: '8' },
    { label: 'Visit & Communion', value: '9' },
    { label: 'Before Work', value: '10' },
    { label: 'Night Examination', value: '11' },
    { label: 'Confession', value: '12' },
    { label: 'Examination', value: '13' },
    { label: 'Act of Contrition', value: '14' },
    { label: 'Before Confession', value: '15' },
    { label: 'After Confession', value: '16' },
    { label: 'Psalm 50 (51)', value: '17' },
    { label: 'Acceptance of Death', value: '18' },
    { label: 'Prayer for Vocations', value: '19' },
    { label: 'Blessed Be Your Purity', value: '20' },
    { label: 'Canticle of the Three Children', value: '21' },
    { label: 'Psalm 2', value: '22' },
    { label: 'Aspirations', value: '23' },
    { label: 'Prayer Cards', value: '24' },
    { label: "Card: St. Josemaría Escrivá", value: '25' },
    { label: "Card: Bl. Álvaro del Portillo", value: '26' },
    { label: "Card: Bl. Guadalupe Ortiz de Landázuri", value: '27' },
    { label: "Card: Ven. Isidoro Zorzano", value: '28' },
    { label: "Card: Ven. Montse Grases", value: '29' },
    { label: "Card: Dora del Hoyo", value: '30' },
    { label: "Card: Fr. Joseph Múzquiz", value: '31' },
    { label: "Card: Fr. José María Hernández Garnica", value: '32' },
    { label: "Card: Toni Zweifel", value: '33' },
    { label: "Card: Ernesto Cofiño", value: '34' },
    { label: "Card: Encarnita Ortega", value: '35' },
    { label: "Card: Tomás & Paquita Alvira", value: '36' },
    { label: "Card: Laura & Eduardo Ortiz de Landázuri", value: '37' },
    { label: "Card: Pedro Ballester", value: '38' }
  ];
}

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
      mainMenuSlot(1, '1'),
      mainMenuSlot(2, '2'),
      mainMenuSlot(3, '3'),
      mainMenuSlot(4, '4'),
      mainMenuSlot(5, '5'),
      mainMenuSlot(6, '0'),
      mainMenuSlot(7, '0'),
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
