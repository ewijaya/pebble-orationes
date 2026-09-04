#include "prayers.h"

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
