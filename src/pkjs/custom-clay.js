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
    var defaultSlots = [1, 2, 3, 4, 5, 0, 0];
    var appearance = clayConfig.getItemByMessageKey('Appearance');
    var accentColor = clayConfig.getItemByMessageKey('AccentColor');
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

    function updateAccentPreview() {
      var palette = accentPalettes[accentColor.get()][appearance.get()];
      accentPreview.set(
        '<span style="display:block;padding:10px 12px;border:2px solid #777;' +
        'border-radius:4px;background:' + palette.color + ';color:' +
        palette.text + ';font-weight:bold;text-align:center">' +
        palette.name + ' accent · ' + palette.color + '</span>'
      );
    }

    slotKeys.forEach(function(messageKey, changedIndex) {
      clayConfig.getItemByMessageKey(messageKey).on('change', function() {
        if (restoringDefaults) {
          return;
        }
        var changedValue = this.get();
        if (changedValue === 0) {
          return;
        }
        for (var index = 0; index < slotKeys.length; index += 1) {
          if (index !== changedIndex &&
              clayConfig.getItemByMessageKey(slotKeys[index]).get() ===
                  changedValue) {
            window.alert('That entry is already assigned to another slot.');
            this.set(0);
            return;
          }
        }
      });
    });

    restoreButton.on('click', function() {
      restoringDefaults = true;
      slotKeys.forEach(function(messageKey, index) {
        clayConfig.getItemByMessageKey(messageKey).set(defaultSlots[index]);
      });
      restoringDefaults = false;
    });

    appearance.on('change', updateAccentPreview);
    accentColor.on('change', updateAccentPreview);
    updateAccentPreview();
  });
};
