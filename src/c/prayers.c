#include "prayers.h"
#include "aspirations.h"

#include <stddef.h>

static const char s_angelus_english[] =
    "V. The Angel of the Lord declared unto Mary.\n"
    "R. And she conceived of the Holy Spirit.\n\n"
    "Hail Mary ...\n\n"
    "V. Behold the handmaid of the Lord.\n"
    "R. Be it done to me according to thy word.\n\n"
    "Hail Mary ...\n\n"
    "V. The Word was made flesh.\n"
    "R. And dwelt among us.\n\n"
    "Hail Mary ...\n\n"
    "V. Pray for us, O Holy Mother of God.\n"
    "R. That we may be made worthy of the promises of Christ.\n\n"
    "Let us pray.\n\n"
    "Pour forth, we beseech you, O Lord,\n"
    "your grace into our hearts,\n"
    "that we, to whom the Incarnation of Christ your Son\n"
    "was made known by the message of an angel,\n"
    "may by his passion and cross\n"
    "be brought to the glory of his resurrection.\n"
    "Through Christ our Lord.\n"
    "Amen.";

static const PrayerTranslation s_angelus_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_angelus_english,
    },
};

static const char s_memorare_english[] =
    "Remember, O most gracious Virgin Mary, that never was it known that "
    "anyone who fled to your protection, implored your help or sought your "
    "intercession, was left unaided. Inspired by this confidence, I fly "
    "unto you, O Virgin of virgins, my Mother. To you I come, before you I "
    "stand, sinful and sorrowful. O Mother of the Word incarnate, despise "
    "not my petitions, but, in your mercy, hear and answer me. Amen.";

static const PrayerTranslation s_memorare_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_memorare_english,
    },
};

static const char s_psalm_50_english[] =
    "Have mercy on me, God, in your kindness.\n"
    "In your compassion blot out my offense.\n"
    "O wash me more and more from my guilt\n"
    "and cleanse me more from sin.\n\n"
    "My offenses truly I know them;\n"
    "my sin is always before me.\n"
    "Against you, you alone, have I sinned;\n"
    "what is evil in your sight I have done.\n\n"
    "That you may be justified when you give sentence\n"
    "and be without reproach when you judge.\n"
    "O see, in guilt I was born,\n"
    "a sinner was I conceived.\n\n"
    "Indeed you love truth in the heart;\n"
    "then in the secret of my heart teach me wisdom.\n"
    "O purify me, then I shall be clean;\n"
    "O wash me, I shall be whiter than snow.\n\n"
    "Make me hear rejoicing and gladness,\n"
    "that the bones you have crushed may revive.\n"
    "From my sins turn away your face\n"
    "and blot out all my guilt.\n\n"
    "A pure heart create for me, O God,\n"
    "put a steadfast spirit within me.\n"
    "Do not cast me away from your presence,\n"
    "nor deprive me of your Holy Spirit.\n\n"
    "Give me again the joy of your help;\n"
    "with a spirit of fervor sustain me,\n"
    "that I may teach transgressors your ways\n"
    "and sinners may return to you.\n\n"
    "O rescue me, God, my helper,\n"
    "and my tongue shall ring out your goodness.\n"
    "O Lord, open my lips\n"
    "and my mouth shall declare your praise.\n\n"
    "For in sacrifice you take no delight,\n"
    "burnt offering from me\n"
    "you would refuse,\n"
    "my sacrifice, a contrite spirit.\n"
    "A humbled, contrite heart\n"
    "you will not spurn.\n\n"
    "In your goodness, show favor to Zion:\n"
    "rebuild the walls of Jerusalem.\n"
    "Then you will be pleased with lawful sacrifice,\n"
    "(burnt offerings wholly consumed),\n"
    "then you will be offered young bulls on your altar.";

static const PrayerTranslation s_psalm_50_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_psalm_50_english,
    },
};

static const char s_acceptance_of_death_english[] =
    "O Lord, my God, from this moment on I accept with a good will, as "
    "something coming from your hand, whatever kind of death you want to "
    "send me, with all its anguish, pain and sorrow.";

static const PrayerTranslation s_acceptance_of_death_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_acceptance_of_death_english,
    },
};

static const char s_prayer_for_vocations_english[] =
    "Lord Jesus Christ, Shepherd of souls, who called the apostles to be "
    "fishers of men, raise up new apostles in your holy Church. Teach them "
    "that to serve you is to reign: to possess you is to possess all "
    "things. Kindle in the young hearts of our people the fire of zeal for "
    "souls. Make them eager to spread your Kingdom upon earth. Grant them "
    "courage to follow you, who are the Way, the Truth and the Life; who "
    "lives and reigns for ever and ever. Amen.";

static const PrayerTranslation s_prayer_for_vocations_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_prayer_for_vocations_english,
    },
};

static const char s_blessed_be_your_purity_english[] =
    "Blessed be your purity,\n"
    "May it be blessed for ever,\n"
    "For no less than God takes delight,\n"
    "In such exalted beauty.\n"
    "To you, heavenly Princess,\n"
    "Holy Virgin Mary,\n"
    "I offer on this day,\n"
    "My whole heart, life and soul.\n"
    "Look upon me with compassion,\n"
    "Do not leave me, my Mother.";

static const PrayerTranslation s_blessed_be_your_purity_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_blessed_be_your_purity_english,
    },
};

static const char s_canticle_three_children_english[] =
    "Ant. Let us sing the hymn of the three children, which these holy ones "
    "sang of old in the fiery furnace, giving praise to the Lord. (P. T. "
    "Alleluia).\n\n"
    "1. Bless the Lord, all you works of the Lord;\n"
    "praise and exalt him above all forever.\n\n"
    "2. Heavens, bless the Lord;\n"
    "angels of the Lord, bless the Lord.\n\n"
    "3. All you waters that are above the heavens,\n"
    "bless the Lord,\n"
    "let all the powers bless the Lord.\n\n"
    "4. Sun and moon bless the Lord;\n"
    "stars of heaven bless the Lord.\n\n"
    "5. Every shower and dew, bless the Lord;\n"
    "all you winds, bless the Lord.\n\n"
    "6. Fire and heat, bless the Lord;\n"
    "cold and heat, bless the Lord.\n\n"
    "7. Dews and hoar frosts, bless the Lord;\n"
    "frost and cold, bless the Lord.\n\n"
    "8. Ice and snow, bless the Lord;\n"
    "nights and days, bless the Lord.\n\n"
    "9. Light and darkness, bless the Lord;\n"
    "lightning and clouds, bless the Lord.\n\n"
    "10. Let the earth bless the Lord;\n"
    "let it praise and exalt him above all forever.\n\n"
    "11. Mountains and hills bless the Lord;\n"
    "everything growing from the earth, bless the Lord.\n\n"
    "12. Seas and rivers, bless the Lord;\n"
    "fountains, bless the Lord.\n\n"
    "13. Whales and all that move in the waters,\n"
    "bless the Lord;\n"
    "all you fowls of the air, bless the Lord.\n\n"
    "14. All you beasts and cattle bless the Lord;\n"
    "sons of men bless the Lord.\n\n"
    "15. Israel bless the Lord;\n"
    "praise and exalt him above all forever.\n\n"
    "16. Priests of the Lord, bless the Lord;\n"
    "servants of the Lord, bless the Lord.\n\n"
    "17. Spirits and souls of the just, bless the Lord;\n"
    "holy men of humble heart, bless the Lord.\n\n"
    "18. Ananias, Azarias and Misael, bless the Lord; praise and exalt him "
    "above all for ever.\n\n"
    "19. Let us bless the Father and the Son,\n"
    "with the Holy Spirit;\n"
    "let us praise and exalt him above all forever.\n\n"
    "20. Blessed are you, Lord, in the firmament of heaven;\n"
    "and worthy of praise, and glorious\n"
    "above all forever.\n\n"
    "Neither Glory be . . . nor Amen is said.\n\n"
    "Psalm 150\n\n"
    "1. Praise the Lord in his holy place,\n"
    "praise him in his mighty heavens.\n\n"
    "2. Praise him for his powerful deeds,\n"
    "praise his surpassing greatness.\n\n"
    "3. O praise him with sound of trumpet,\n"
    "praise him with lute and harp.\n\n"
    "4. Praise him with timbrel and dance,\n"
    "praise him with strings and pipes.\n\n"
    "5. O praise him with resounding cymbals,\n"
    "praise him with clashing of cymbals.\n"
    "Let everything that lives and that breathes\n"
    "give praise to the Lord.\n\n"
    "Glory be . . .\n\n"
    "Ant. Let us sing the hymn of the three children, which these holy ones "
    "sang of old in the fiery furnace, giving praise to the Lord. (P. T. "
    "Alleluia).\n\n"
    "Lord, have mercy. Christ, have mercy. Lord, have mercy. Our Father . . "
    ".\n\n"
    "V. And lead us not into temptation.\n"
    "R. But deliver us from evil.\n\n"
    "V. Let all your works praise you, Lord.\n"
    "R. And let your Saints bless you.\n\n"
    "V. Your saints shall rejoice in glory.\n"
    "R. They shall rejoice in their resting place.\n\n"
    "V. Not unto us, Lord, not unto us.\n"
    "R. But unto your name give glory.\n\n"
    "V. O Lord, hear my prayer.\n"
    "R. And let my cry come unto you.\n\n"
    "Priests add:\n\n"
    "V. The Lord be with you.\n"
    "R. And also with you.\n\n"
    "Let us pray.\n\n"
    "God, who did allay the flames of fire for three children, grant in your "
    "mercy that the flame of vice may not consume us your servants.\n\n"
    "Direct, we beseech you, Lord, our actions by your inspirations, and "
    "further them by your assistance: that every word and work of ours may "
    "begin always from you and by you be likewise ended.\n\n"
    "Quench in us, we beseech you, Lord, the flame of vice even as you did "
    "enable blessed Lawrence to overcome his fire of sufferings. Through "
    "Christ our Lord. R. Amen.";

static const PrayerTranslation s_canticle_three_children_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_canticle_three_children_english,
    },
};

static const char s_psalm_2_english[] =
    "Ant. His kingdom is a kingdom of all ages, and all kings shall serve "
    "and obey him. (P. T. Alleluia).\n\n"
    "1. Why this tumult among nations,\n"
    "among peoples this useless murmuring?\n\n"
    "2. They arise, the kings of the earth,\n"
    "princes plot against the Lord\n"
    "and his Anointed.\n\n"
    "3. “Come, let us break their fetters,\n"
    "come let us cast off their yoke.”\n\n"
    "4. He who sits in the heavens laughs;\n"
    "the Lord is laughing them to scorn.\n\n"
    "5. Then he will speak in his anger,\n"
    "his rage will strike them with terror.\n\n"
    "6. “It is I who have set up my king on Zion,\n"
    "my holy mountain.”\n\n"
    "7. I will announce the decree of the Lord:\n"
    "The Lord said to me: “You are my Son.\n"
    "It is I who have begotten you this day.\n\n"
    "8. Ask and I shall bequeath you\n"
    "the nations,\n"
    "put the ends of the earth in your possession.\n\n"
    "9. With a rod of iron you will break them,\n"
    "shatter them like a potter's jar.”\n\n"
    "10. Now, O kings, understand,\n"
    "take warning, rulers of the earth;\n\n"
    "11. Serve the Lord\n"
    "with awe and trembling,\n\n"
    "12. Pay him your homage, lest he be angry and you perish, for suddenly "
    "his anger will blaze.\n"
    "Blessed are they who put their trust in God.\n\n"
    "13. Glory be . . .\n\n"
    "Ant. His kingdom is a kingdom of all ages, and all kings shall serve "
    "and obey him. (P. T. Alleluia).\n\n"
    "V. O Lord, hear my prayer.\n"
    "R. And let my cry come unto you.\n\n"
    "Priests add:\n\n"
    "V. The Lord be with you.\n"
    "R. And also with you.\n\n"
    "Let us pray.\n"
    "Almighty and eternal God, you have renewed all creation in your beloved "
    "Son, the king of the whole universe. May all the people of the earth, "
    "now torn apart by the wound of sin, become subject to the gentle rule "
    "of your only-begotten Son: Who lives and reigns with you and the Holy "
    "Spirit, one God, for ever and ever.\n\n"
    "R. Amen.";

static const PrayerTranslation s_psalm_2_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_psalm_2_english,
    },
};

static const char s_regina_caeli_english[] =
    "V. Queen of heaven, rejoice, alleluia.\n"
    "R. For he whom you did merit to bear, alleluia.\n\n"
    "V. Has risen, as he said, alleluia.\n"
    "R. Pray for us to God, alleluia.\n\n"
    "V. Rejoice and be glad, O Virgin Mary, alleluia.\n"
    "R. For the Lord has truly risen, alleluia.\n\n"
    "Let us pray.\n\n"
    "O God, who gave joy to the world through the resurrection\n"
    "of your Son, our Lord Jesus Christ, grant, we beseech you,\n"
    "that through the intercession of the Virgin Mary, his Mother,\n"
    "we may obtain the joys of everlasting life.\n"
    "Through the same Christ our Lord.\n\n"
    "R. Amen.";

static const PrayerTranslation s_regina_caeli_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_regina_caeli_english,
    },
};

static const char s_preces_latin[] =
    "Sérviam!\n\n"
    "V. Ad Trinitátem Beatíssimam.\n"
    "R. Grátias tibi, Deus, grátias tibi: vera et una Trínitas, una et "
    "summa Déitas, sancta et una Únitas.\n\n"
    "V. Ad Iesum Christum Regem.\n"
    "R. Dóminus Iudex noster; Dóminus Légifer noster; Dóminus Rex "
    "noster. Ipse salvábit nos.\n\n"
    "V. Christe, Fili Dei vivi, miserére nobis.\n"
    "R. Christe, Fili Dei vivi, miserére nobis.\n\n"
    "V. Exsúrge, Christe, ádiuva nos.\n"
    "R. Et líbera nos propter nomen tuum.\n\n"
    "V. Dóminus illuminátio mea et salus mea: quem timébo?\n"
    "R. Si consístant advérsum me castra, non timébit cor meum; si "
    "exsúrgat advérsum me prœlium, in hoc ego sperábo.\n\n"
    "V. Ad Beátam Vírginem Maríam Mediatrícem.\n"
    "R. Recordáre, Virgo Mater Dei, dum stéteris in conspéctu Dómini, "
    "ut loquáris pro nobis bona.\n\n"
    "V. Ad Sanctum Ioseph Sponsum Beátæ Maríæ Vírginis.\n"
    "R. Fecit te Deus quasi Patrem Regis, et dóminum univérsæ domus "
    "eius: ora pro nobis.\n\n"
    "V. Ad Ángelos Custódes.\n"
    "R. Sancti Ángeli Custódes nostri, deféndite nos in prœlio ut non "
    "pereámus in treméndo iudício.\n\n"
    "V. Ad Sanctum Iosephmaríam Conditórem nostrum.\n"
    "R. Intercéde pro fíliis tuis ut, fidéles spirítui Óperis Dei, "
    "labórem sanctificémus et ánimas Christo lucrifácere quærámus.\n\n"
    "V. Orémus pro Beatíssimo Papa nostro N.\n"
    "R. Dóminus consérvet eum, et vivíficet eum, et beátum fáciat eum "
    "in terra, et non tradat eum in ánimam inimicórum eius.\n\n"
    "V. Orémus et pro Antístite huius diœcésis.\n"
    "R. Stet et pascat in fortitúdine tua, Dómine, in sublimitáte "
    "nóminis tui.\n\n"
    "V. Orémus pro unitáte apostolátus.\n"
    "R. Ut omnes unum sint, sicut tu Pater in me et ego in te: ut sint "
    "unum, sicut et nos unum sumus.\n\n"
    "V. Omne regnum divísum contra se, desolábitur.\n"
    "R. Et omnis cívitas vel domus divísa contra se non stabit.\n\n"
    "V. Orémus pro benefactóribus nostris.\n"
    "R. Retribúere dignáre, Dómine, ómnibus nobis bona faciéntibus "
    "propter nomen tuum, vitam ætérnam. Amen.\n\n"
    "V. Orémus pro Patre.\n"
    "R. Misericórdia Dómini ab ætérno et usque in ætérnum super eum: "
    "custódit enim Dóminus omnes diligéntes se.\n\n"
    "V. Orémus et pro frátribus nostris Óperis Dei, vivis atque "
    "defúnctis.\n"
    "R. Salvos fac servos tuos, Deus meus, sperántes in te.\n\n"
    "V. Mitte eis, Dómine, auxílium de sancto.\n"
    "R. Et de Sion tuére eos.\n\n"
    "V. Réquiem ætérnam dona eis, Dómine.\n"
    "R. Et lux perpétua lúceat eis.\n\n"
    "V. Requiéscant in pace.\n"
    "R. Amen.\n\n"
    "V. Dómine, exáudi oratiónem meam.\n"
    "R. Et clamor meus ad te véniat.\n\n"
    "Sacerdos, si Preces moderatur, exsurgit et addit Dóminus vobíscum, "
    "stans etiam dum recitat orationem.\n\n"
    "V. Dóminus vobíscum.\n"
    "R. Et cum spíritu tuo.\n\n"
    "Orémus.\n\n"
    "Deus, cui próprium est miseréri semper et párcere: súscipe "
    "deprecatiónem nostram. Ure igne Sancti Spíritus renes nostros et "
    "cor nostrum, Dómine: ut tibi casto córpore serviámus, et mundo "
    "corde placeámus.\n\n"
    "Actiónes nostras, quǽsumus Dómine, aspirándo prǽveni et adiuvándo "
    "proséquere: ut cuncta nostra orátio et operátio a te semper "
    "incípiat, et per te cœpta finiátur. Per Christum Dóminum nostrum.\n\n"
    "R. Amen.\n\n"
    "Omnes dicunt:\n\n"
    "Gáudium cum pace, emendatiónem vitæ, spátium veræ pœniténtiæ, "
    "grátiam et consolatiónem Sancti Spíritus atque in Ópere Dei "
    "perseverántiam, tríbuat nobis Omnípotens et Miséricors Dóminus.\n\n"
    "V. Sancte Míchaël.\n"
    "R. Ora pro nobis.\n\n"
    "V. Sancte Gábriel.\n"
    "R. Ora pro nobis.\n\n"
    "V. Sancte Ráphaël.\n"
    "R. Ora pro nobis.\n\n"
    "V. Sancte Petre.\n"
    "R. Ora pro nobis.\n\n"
    "V. Sancte Paule.\n"
    "R. Ora pro nobis.\n\n"
    "V. Sancte Ioánnes.\n"
    "R. Ora pro nobis.\n\n"
    "Cum adsit aliquis Sacerdos, dignior ait:\n\n"
    "Iube, Domne, benedícere.\n\n"
    "Sacerdos benedicit:\n\n"
    "Dóminus sit in córdibus vestris, et in lábiis vestris, in nómine "
    "Patris ✠ et Fílii et Spíritus Sancti.\n\n"
    "R. Amen.\n\n"
    "V. Pax.\n"
    "R. In ætérnum.";

static const PrayerTranslation s_preces_translations[] = {
    {
        .language = PRAYER_LANGUAGE_LATIN,
        .text = s_preces_latin,
    },
};

static const Prayer s_prayers[] = {
    {
        .name = "Preces",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_LATIN,
        .translations = s_preces_translations,
        .translation_count =
            (uint8_t)(sizeof(s_preces_translations) /
                      sizeof(s_preces_translations[0])),
    },
    {
        .name = "Holy Rosary",
        .destination = PRAYER_DESTINATION_ROSARY,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = NULL,
        .translation_count = 0,
    },
    {
        .name = "Regina Caeli",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = s_regina_caeli_translations,
        .translation_count =
            (uint8_t)(sizeof(s_regina_caeli_translations) /
                      sizeof(s_regina_caeli_translations[0])),
    },
    {
        .name = "Angelus",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = s_angelus_translations,
        .translation_count =
            (uint8_t)(sizeof(s_angelus_translations) /
                      sizeof(s_angelus_translations[0])),
    },
    {
        .name = "Memorare",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = s_memorare_translations,
        .translation_count =
            (uint8_t)(sizeof(s_memorare_translations) /
                      sizeof(s_memorare_translations[0])),
    },
    {
        .name = "Psalm 50 (51)",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = s_psalm_50_translations,
        .translation_count =
            (uint8_t)(sizeof(s_psalm_50_translations) /
                      sizeof(s_psalm_50_translations[0])),
    },
    {
        .name = "Acceptance of Death",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = s_acceptance_of_death_translations,
        .translation_count =
            (uint8_t)(sizeof(s_acceptance_of_death_translations) /
                      sizeof(s_acceptance_of_death_translations[0])),
    },
    {
        .name = "Prayer for Vocations",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = s_prayer_for_vocations_translations,
        .translation_count =
            (uint8_t)(sizeof(s_prayer_for_vocations_translations) /
                      sizeof(s_prayer_for_vocations_translations[0])),
    },
    {
        .name = "Blessed Be Your Purity",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = s_blessed_be_your_purity_translations,
        .translation_count =
            (uint8_t)(sizeof(s_blessed_be_your_purity_translations) /
                      sizeof(s_blessed_be_your_purity_translations[0])),
    },
    {
        .name = "Canticle of the Three Children",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = s_canticle_three_children_translations,
        .translation_count =
            (uint8_t)(sizeof(s_canticle_three_children_translations) /
                      sizeof(s_canticle_three_children_translations[0])),
    },
    {
        .name = "Psalm 2",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = s_psalm_2_translations,
        .translation_count =
            (uint8_t)(sizeof(s_psalm_2_translations) /
                      sizeof(s_psalm_2_translations[0])),
    },
    {
        .name = "Aspirations",
        .destination = PRAYER_DESTINATION_TEXT,
        .default_language = PRAYER_LANGUAGE_ENGLISH,
        .translations = aspirations_translations,
        .translation_count = 1,
    },
};

uint16_t prayers_count(void) {
  return (uint16_t)(sizeof(s_prayers) / sizeof(s_prayers[0]));
}

const Prayer *prayers_get(uint16_t index) {
  if (index >= prayers_count()) {
    return NULL;
  }

  return &s_prayers[index];
}

const Prayer *prayers_get_by_id(PrayerId prayer_id) {
  return (uint32_t)prayer_id < PRAYER_ID_COUNT
             ? prayers_get((uint16_t)prayer_id)
             : NULL;
}

const PrayerTranslation *prayer_get_translation(const Prayer *prayer,
                                                PrayerLanguage language) {
  if (!prayer) {
    return NULL;
  }

  for (uint8_t index = 0; index < prayer->translation_count; ++index) {
    const PrayerTranslation *translation = &prayer->translations[index];
    if (translation->language == language) {
      return translation;
    }
  }

  return NULL;
}
