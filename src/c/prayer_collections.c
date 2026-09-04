#include "prayer_collections.h"

#include <stddef.h>

// Canonical wording selected from the user's Hop prayer catalog.
static const char s_mental_prayer_english[] =
    "Before\n\n"
    "My Lord and my God, I firmly believe that you are here, that you see "
    "me, that you hear me. I adore you with profound reverence; I beg your "
    "pardon for my sins and the grace to spend this time of prayer "
    "fruitfully. My immaculate Mother, St. Joseph my father and lord, my "
    "guardian angel, intercede for me.\n\n"
    "After\n\n"
    "I thank you, my God, for the good resolutions, affections and "
    "inspirations that you have communicated to me in this meditation. I "
    "beg your help in performing them. My immaculate Mother, St. Joseph my "
    "father and lord, my guardian angel, intercede for me.";

static const char s_visit_and_communion_english[] =
    "Recite three times:\n\n"
    "Our Father . . .\n"
    "Hail Mary . . .\n"
    "Glory be . . .\n\n"
    "Spiritual Communion\n\n"
    "I wish, my Lord, to receive you with the purity, humility and devotion "
    "with which your most holy Mother received you, with the spirit and "
    "fervor of the saints.";

static const char s_before_work_english[] =
    "Direct, we beg you, O Lord, our actions by your holy inspirations, and "
    "carry them on by your gracious assistance, that every prayer and work "
    "of ours may begin always with you, and through you be happily ended. "
    "Amen.";

static const char s_night_examination_english[] =
    "Make a brief examination of conscience before going to rest at night. "
    "Two or three minutes are enough.\n\n"
    "• Place yourself in the presence of God recognizing his strength and "
    "your weakness. Tell him: “Lord, if you will, you can make me clean.”\n\n"
    "• Ask your guardian angel for light to acknowledge your defects and "
    "virtues: What have I done wrong? What have I done right? What could I "
    "have done better?\n\n"
    "• Examine your conscience with sincerity:\n\n"
    "- Did I often consider that God is my Father? Did I offer him my work? "
    "Did I make good use of my time? Did I pray slowly and with attention?\n\n"
    "- Did I try to make life pleasant for other people? Did I criticize "
    "anyone? Was I forgiving? Did I pray and offer some sacrifices for the "
    "Church, the Pope and for all those around me?\n\n"
    "- Did I allow myself to be carried away by sensuality? By pride?\n\n"
    "• Sorrow. Make an act of contrition asking our Lord’s pardon.\n\n"
    "• Resolution. Make a specific resolution for tomorrow:\n\n"
    "- Staying away from certain temptations.\n\n"
    "- Avoiding some specific faults.\n\n"
    "- Exerting special effort to practice some virtue.\n\n"
    "- Taking advantage of occasions for improvement.\n\n"
    "• Pray three Hail Marys to the Virgin Mary asking for the virtue of "
    "purity for yourself and your loved ones.";

static const char s_confession_examination_english[] =
    "• When was my last good Confession? Did I receive Communion or other "
    "sacraments in the state of mortal sin? Did I intentionally fail to "
    "confess some mortal sin in my previous Confession?\n\n"
    "• Did I seriously doubt my faith or put myself in danger of losing my "
    "faith through readings hostile to Catholic teachings or involvement "
    "in non-Catholic sects? Did I engage in superstitious practices: "
    "palm-reading, fortune telling, etc.?\n\n"
    "• Did I take the name of God in vain? Did I curse, or take a false "
    "oath? Did I use improper language?\n\n"
    "• Did I miss Mass on Sundays or holy days of obligation through my own "
    "fault, without any serious reason? Did I keep fast and abstinence on "
    "the prescribed days?\n\n"
    "• Did I disobey my parents and lawful superiors in important matters?\n\n"
    "• Did I hate or quarrel with anyone, or desire revenge? Did I refuse "
    "to forgive? Did I hurt or cause to kill someone? Did I get drunk? Did "
    "I take illicit drugs? Did I consent to, recommend, advise or actively "
    "take part in an abortion?\n\n"
    "• Did I willfully look at indecent pictures or watch immoral movies? "
    "Did I read immoral books or magazines? Did I engage in impure jokes or "
    "conversations? Did I willfully entertain impure thoughts or feelings? "
    "Did I commit impure acts, alone or with others? Did I take "
    "contraceptive or abortifacient pills or use other artificial means in "
    "order to prevent conception?\n\n"
    "• Did I steal or damage to another’s property? How much? Have I made "
    "reparation for the damages done? Have I been honest in my business "
    "relations?\n\n"
    "• Did I tell lies? Did I sin by calumny, or detraction telling the "
    "unknown grave faults of others without necessity, even if they are "
    "true? Did I judge others rashly in serious matters? Have I tried to "
    "make restitution for the bad reputation I caused?\n\n"
    "If you remember other serious sins besides those indicated here, "
    "mention them in your Confession.";

static const char s_act_of_contrition_english[] =
    "O my God, I am heartily sorry for having offended you and I detest all "
    "my sins, because I dread the loss of heaven and the pains of hell; but "
    "most of all because they offend you, my God, who are all good and "
    "deserving of all my love. I firmly resolve, with the help of your "
    "grace, to confess my sins, to do penance, and to amend my life. Amen.";

static const char s_before_confession_english[] =
    "Be truly sorry for your sins.\n\n"
    "“The essential act of Penance, on the part of the penitent, is "
    "contrition, a clear and decisive rejection of the sin committed, "
    "together with a resolution not to commit it again, out of the love one "
    "has for God and which is reborn with repentance. Understood in this "
    "way, contrition is therefore, the beginning and the heart of "
    "conversion, of that evangelical metánoia that brings the person back "
    "to God like the Prodigal Son returning to his father, and which has in "
    "the sacrament of Penance its visible sign and which perfects "
    "attrition.”*\n\n"
    "The resolution to avoid committing these sins in the future "
    "(amendment) is a sure sign that your sorrow is genuine and authentic.\n\n"
    "This does not mean that a promise never to fall again into sin is "
    "necessary. A resolution to try to avoid the near occasions of sin "
    "suffices for true repentance. God’s grace in cooperation with the "
    "intention to rectify your life will give you the strength to resist "
    "and overcome temptation in the future.\n\n"
    "Act of Contrition\n\n"
    "O my God, I am heartily sorry for having offended you and I detest all "
    "my sins, because I dread the loss of heaven and the pains of hell; but "
    "most of all because they offend you, my God, who are all good and "
    "deserving of all my love. I firmly resolve with the help of your "
    "grace, to confess my sins, to do penance, and to amend my life. Amen.\n\n"
    "or any other like: I confess . . .\n\n"
    "You may now go to Confession";

static const char s_after_confession_english[] =
    "· Give thanks to God for forgiving you again.\n\n"
    "· If you recall some serious sin you forgot to tell, rest assured that "
    "it has been forgiven with the others, but be sure to confess it in "
    "your next Confession.";

#define ENGLISH_TRANSLATIONS(name)                                    \
  static const PrayerTranslation s_##name##_translations[] = {        \
      {                                                               \
          .language = PRAYER_LANGUAGE_ENGLISH,                         \
          .text = s_##name##_english,                                  \
      },                                                              \
  }

ENGLISH_TRANSLATIONS(mental_prayer);
ENGLISH_TRANSLATIONS(visit_and_communion);
ENGLISH_TRANSLATIONS(before_work);
ENGLISH_TRANSLATIONS(night_examination);
ENGLISH_TRANSLATIONS(confession_examination);
ENGLISH_TRANSLATIONS(act_of_contrition);
ENGLISH_TRANSLATIONS(before_confession);
ENGLISH_TRANSLATIONS(after_confession);

#define ENGLISH_PRAYER(label, key)                                    \
  {                                                                   \
    .name = label, .destination = PRAYER_DESTINATION_TEXT,             \
    .default_language = PRAYER_LANGUAGE_ENGLISH,                       \
    .translations = s_##key##_translations,                            \
    .translation_count =                                              \
        (uint8_t)(sizeof(s_##key##_translations) /                     \
                  sizeof(s_##key##_translations[0])),                  \
  }

static const Prayer s_daily_prayers[] = {
    ENGLISH_PRAYER("Mental Prayer", mental_prayer),
    ENGLISH_PRAYER("Visit & Communion", visit_and_communion),
    ENGLISH_PRAYER("Before Work", before_work),
    ENGLISH_PRAYER("Night Examination", night_examination),
};

static const Prayer s_confession_prayers[] = {
    ENGLISH_PRAYER("Examination", confession_examination),
    ENGLISH_PRAYER("Act of Contrition", act_of_contrition),
    ENGLISH_PRAYER("Before Confession", before_confession),
    ENGLISH_PRAYER("After Confession", after_confession),
};

static const PrayerCollection s_collections[] = {
    {
        .name = "More Prayers",
        .prayers = s_daily_prayers,
        .prayer_count =
            (uint8_t)(sizeof(s_daily_prayers) / sizeof(s_daily_prayers[0])),
    },
    {
        .name = "Confession",
        .prayers = s_confession_prayers,
        .prayer_count = (uint8_t)(sizeof(s_confession_prayers) /
                                  sizeof(s_confession_prayers[0])),
    },
};

const PrayerCollection *prayer_collections_get(
    PrayerCollectionId collection_id) {
  return (uint32_t)collection_id < PRAYER_COLLECTION_COUNT
             ? &s_collections[collection_id]
             : NULL;
}
