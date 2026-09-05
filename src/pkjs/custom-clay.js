'use strict';

module.exports = function() {
  var clayConfig = this;

  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var restoreButton = clayConfig.getItemById('restore-main-menu');
    var restoringDefaults = false;
    var slotKeys = [
      'MainMenuSlot1',
      'MainMenuSlot2',
      'MainMenuSlot3',
      'MainMenuSlot4',
      'MainMenuSlot5',
      'MainMenuSlot6',
      'MainMenuSlot7'
    ];
    var defaultSlots = clayConfig.meta.userData.defaultSlots;
    var appearance = clayConfig.getItemByMessageKey('Appearance');
    var accentColor = clayConfig.getItemByMessageKey('AccentColor');
    var navigation = clayConfig.getItemByMessageKey('NavigationHighlight');
    var textSize = clayConfig.getItemByMessageKey('TextSize');
    var accentPreview = clayConfig.getItemById('accent-preview');
    var accentPalettes = {
      0: {
        0: { name: 'Ocean', color: '#0055AA', text: '#FFFFFF' },
        1: { name: 'Ocean', color: '#00AAAA', text: '#000000' }
      },
      1: {
        0: { name: 'Burgundy', color: '#550000', text: '#FFFFFF' },
        1: { name: 'Burgundy', color: '#AA0000', text: '#FFFFFF' }
      },
      2: {
        0: { name: 'Forest', color: '#005500', text: '#FFFFFF' },
        1: { name: 'Forest', color: '#55AA55', text: '#000000' }
      },
      3: {
        0: { name: 'Monochrome', color: '#000000', text: '#FFFFFF' },
        1: { name: 'Monochrome', color: '#FFFFFF', text: '#000000' }
      }
    };

    var navigationPalettes = [null,
      {color: '#FFAA00', text: '#000000'}, {color: '#FF5500', text: '#000000'},
      {color: '#AA00FF', text: '#FFFFFF'}, {color: '#FF00FF', text: '#000000'},
      {color: '#AAFF00', text: '#000000'}];
    function updateAccentPreview() {
      var dark = Number(appearance.get()) === 1;
      var palette = accentPalettes[accentColor.get()][dark ? 1 : 0];
      var nav = navigationPalettes[Number(navigation.get())] ||
        {color: dark ? '#FFFFFF' : '#000000', text: dark ? '#000000' : '#FFFFFF'};
      var background = dark ? '#000000' : '#FFFFFF';
      var foreground = dark ? '#FFFFFF' : '#000000';
      accentPreview.set('<div style="border:2px solid #777;background:' + background + ';color:' + foreground + '">' +
        '<div style="padding:8px;background:' + palette.color + ';color:' + palette.text + ';font-size:22px;font-weight:bold">Title preview</div>' +
        '<div style="padding:12px;font-weight:bold;font-size:' + (Number(textSize.get()) ? 34 : 28) + 'px">Make time for prayer.</div>' +
        '<div style="padding:10px;background:' + nav.color + ';color:' + nav.text + ';font-size:24px;font-weight:bold">Selected prayer</div></div>');
    }

    var priorValues = slotKeys.map(function(k) { return Number(clayConfig.getItemByMessageKey(k).get()); });
    slotKeys.forEach(function(messageKey, changedIndex) {
      clayConfig.getItemByMessageKey(messageKey).on('change', function() {
        if (restoringDefaults) {
          return;
        }
        var changedValue = Number(this.get());
        var priorValue = priorValues[changedIndex];
        restoringDefaults = true;
        if (changedValue !== 0) {
          for (var index = 0; index < slotKeys.length; index += 1) {
            if (index !== changedIndex && Number(clayConfig.getItemByMessageKey(slotKeys[index]).get()) === changedValue) {
              clayConfig.getItemByMessageKey(slotKeys[index]).set(priorValue);
              priorValues[index] = priorValue;
              break;
            }
          }
        }
        priorValues[changedIndex] = changedValue;
        restoringDefaults = false;
      });
    });

    restoreButton.on('click', function() {
      restoringDefaults = true;
      slotKeys.forEach(function(messageKey, index) {
        clayConfig.getItemByMessageKey(messageKey).set(defaultSlots[index]);
      });
      priorValues = defaultSlots.slice();
      restoringDefaults = false;
    });

    navigation.on('change', updateAccentPreview);
    textSize.on('change', updateAccentPreview);
    appearance.on('change', updateAccentPreview);
    accentColor.on('change', updateAccentPreview);
    updateAccentPreview();
  });
};
