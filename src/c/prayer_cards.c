#include "prayer_cards.h"

// Canonical user source: prayer-cards.json. Original order and wording retained.
// Biographical metadata and the postal address are not part of the reader.

static const PrayerParagraph s_card_0_paragraphs[] = {
    {.text = "O God, through the mediation of Mary our Mother, you granted your priest St. Josemaría countless graces, choosing him as a most faithful instrument to found Opus Dei, a way of sanctification in daily work and in the fulfillment of the Christian's ordinary duties. Grant that I too may learn to turn all the circumstances and events of my life into occasions of loving You and serving the Church, the Pope and all souls with joy and simplicity, lighting up the pathways of this earth with faith and love. Deign to grant me, through the intercession of St. Josemaría, the favor of ... (make your request). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = false},
};

static const PrayerTranslation s_card_0_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_0_paragraphs,
     .paragraph_count = sizeof(s_card_0_paragraphs) / sizeof(s_card_0_paragraphs[0])},
};

static const PrayerParagraph s_card_1_paragraphs[] = {
    {.text = "O God, merciful Father, you granted your bishop Blessed Alvaro the grace of being, with our Lady's help, an exemplary pastor in the service of the Church, and a most faithful son and successor of Saint Josemaría, the founder of Opus Dei. Grant that I too may respond faithfully to the demands of the Christian vocation, turning all the circumstances and events of my life into opportunities to love you and serve the kingdom of Jesus Christ. Deign to grant the canonization of Blessed Alvaro, and through his intercession grant me the favour I request … (here make your petition). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = false},
};

static const PrayerTranslation s_card_1_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_1_paragraphs,
     .paragraph_count = sizeof(s_card_1_paragraphs) / sizeof(s_card_1_paragraphs[0])},
};

static const PrayerParagraph s_card_2_paragraphs[] = {
    {.text = "O God, grant me through the intercession of Blessed Guadalupe that, like her, I may learn to carry out my ordinary work with love, and spread faith and joy to everyone around me, so that many more may come to know you and love you. Deign to grant the canonisation of Guadalupe and, through her intercession, grant me the favour I ask of you... (here make your petition). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = false},
};

static const PrayerTranslation s_card_2_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_2_paragraphs,
     .paragraph_count = sizeof(s_card_2_paragraphs) / sizeof(s_card_2_paragraphs[0])},
};

static const PrayerParagraph s_card_3_paragraphs[] = {
    {.text = "Almighty God, you granted your servant Isidoro countless graces in the fulfillment of his professional duties in the world. Grant that I too may sanctify my ordinary work and bring Christ's light to my friends and acquaintances. Deign to glorify your servant Isidoro and through his intercession grant me the favor I ask of you... (here make your request). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_3_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_3_paragraphs,
     .paragraph_count = sizeof(s_card_3_paragraphs) / sizeof(s_card_3_paragraphs[0])},
};

static const PrayerParagraph s_card_4_paragraphs[] = {
    {.text = "O God, you granted your servant Montse the grace of a serene and cheerful dedication to your will with admirable simplicity in everyday life. Grant that I may lovingly offer to you all my daily activity and convert it into Christian service for others. Deign to glorify your servant Montse and through her intercession grant me the favor I ask of you… (ask your favor here). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_4_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_4_paragraphs,
     .paragraph_count = sizeof(s_card_4_paragraphs) / sizeof(s_card_4_paragraphs[0])},
};

static const PrayerParagraph s_card_5_paragraphs[] = {
    {.text = "O Lord, you called your servant Dora to work at the tasks that the Blessed Virgin Mary carried out in her home at Nazareth. Help me find you in the ordinary events of daily life, and to fill my surroundings with the same family warmth that Dora brought to hers with her cheerful and selfless work, following the teachings of St Josemaria. Glorify your servant Dora, and grant me the favour that I ask (here make your petition). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_5_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_5_paragraphs,
     .paragraph_count = sizeof(s_card_5_paragraphs) / sizeof(s_card_5_paragraphs[0])},
};

static const PrayerParagraph s_card_6_paragraphs[] = {
    {.text = "God, you helped your servant Joseph work with generosity and simplicity. He spread the message of sanctity in secular life to many people, teaching them to find joy and peace in their daily life. Help me to seek first the kingdom of God, by sanctifying my everyday work and dedicating myself generously to the salvation of souls. Glorify your servant Joseph, and through his intercession, grant me the favor I ask of you. (Here make your request). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_6_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_6_paragraphs,
     .paragraph_count = sizeof(s_card_6_paragraphs) / sizeof(s_card_6_paragraphs[0])},
};

static const PrayerParagraph s_card_7_paragraphs[] = {
    {.text = "Lord our God, you deigned to make use of your priest servant José María to spread throughout many countries the call to sanctity in ordinary life. Help me to draw close to Christ in my daily occupations, in order to bring the joy of the Christian vocation to many other souls. Glorify your servant José María, and through his intercession grant me the favor I request. (Here make your request). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_7_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_7_paragraphs,
     .paragraph_count = sizeof(s_card_7_paragraphs) / sizeof(s_card_7_paragraphs[0])},
};

static const PrayerParagraph s_card_8_paragraphs[] = {
    {.text = "God our Father, you have entrusted the earth to mankind to build it up and so give you glory. Through your grace your servant Toni imbued his work with professional quality and a Christian spirit. Help me also to carry out my work in a way that gives you glory; and grant that my example will lead many people to discover the dignity and divine meaning of their daily activity. Deign to glorify your servant Toni and through his intercession grant me the favor I ask of You . . . Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_8_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_8_paragraphs,
     .paragraph_count = sizeof(s_card_8_paragraphs) / sizeof(s_card_8_paragraphs[0])},
};

static const PrayerParagraph s_card_9_paragraphs[] = {
    {.text = "O God, our Father, source of all good, You granted your son Ernesto abundant graces to be a doctor who faithfully serves the life that begins in You and belongs to You alone. Grant that I too may respect and foster the gift of life, and generously fulfill my daily duties, out of love for Jesus Christ and my fellow men and women. Deign to glorify your servant Ernesto, and through his intercession grant me the favor I ask of You. (Here make your request). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_9_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_9_paragraphs,
     .paragraph_count = sizeof(s_card_9_paragraphs) / sizeof(s_card_9_paragraphs[0])},
};

static const PrayerParagraph s_card_10_paragraphs[] = {
    {.text = "Lord, You showed your daughter Encarnita the attractiveness of holiness lived in the middle of world, and You gave her the grace to faithfully follow that path in Opus Dei: grant me, like her, to discover your love each day and to transmit it to everyone around me. Deign to glorify your daughter Encarnita and through her intercession grant me the favor I request. (Here make your request). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_10_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_10_paragraphs,
     .paragraph_count = sizeof(s_card_10_paragraphs) / sizeof(s_card_10_paragraphs[0])},
};

static const PrayerParagraph s_card_11_paragraphs[] = {
    {.text = "God our Father, who poured out your grace on your servants Paquita and Tomás, so that they could live their marriage and professional and social obligations with a Christian spirit, send us the strength of your Love in order to spread in the world the marvelous reality of fidelity and holiness in marriage. Deign to glorify your servants and grant me through their intercession the favor that I ask. (Ask for the favor here.) Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_11_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_11_paragraphs,
     .paragraph_count = sizeof(s_card_11_paragraphs) / sizeof(s_card_11_paragraphs[0])},
};

static const PrayerParagraph s_card_12_paragraphs[] = {
    {.text = "O God, merciful Father, you granted your Servants Laurita and Eduardo an abundant grace to practise the Christian virtues in carrying out their family and professional duties. Grant that like them I too may learn to become an instrument of peace and joy in the world. Deign to glorify your Servants and, through their intercession, grant me the favour I ask of you… (here make your petition). Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father, Hail Mary, Glory be to the Father.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_12_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_12_paragraphs,
     .paragraph_count = sizeof(s_card_12_paragraphs) / sizeof(s_card_12_paragraphs[0])},
};

static const PrayerParagraph s_card_13_paragraphs[] = {
    {.text = "Heavenly Father, you gave your son Pedro a deep and cheerful faith, love for the Cross, and zeal to bring his friends closer to Christ. Help me also to offer my work, joys and sufferings for the good of the Church and for the salvation of all. Grant through Pedro's prayers what I now ask (name the intention) and that I too may grow each day in faith and love for Jesus, by making Him, with our Lady's help, the centre of my life and love. Amen.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "Our Father. Hail Mary. Glory Be.",
     .style = PRAYER_PARAGRAPH_PRIMARY, .space_after = true},
    {.text = "In conformity with the decrees of Pope Urban VIII, we declare that there is no intention of anticipating in any way the judgment of the Church, and that this prayer is not intended for public use.",
     .style = PRAYER_PARAGRAPH_NOTE, .space_after = false},
};

static const PrayerTranslation s_card_13_translations[] = {
    {.language = PRAYER_LANGUAGE_ENGLISH,
     .paragraphs = s_card_13_paragraphs,
     .paragraph_count = sizeof(s_card_13_paragraphs) / sizeof(s_card_13_paragraphs[0])},
};

const Prayer prayer_cards[PRAYER_CARD_COUNT] = {
    {.name = "St. Josemaría Escrivá",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_0_translations,
     .translation_count = 1},
    {.name = "Bl. Álvaro del Portillo",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_1_translations,
     .translation_count = 1},
    {.name = "Bl. Guadalupe Ortiz de Landázuri",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_2_translations,
     .translation_count = 1},
    {.name = "Ven. Isidoro Zorzano",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_3_translations,
     .translation_count = 1},
    {.name = "Ven. Montse Grases",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_4_translations,
     .translation_count = 1},
    {.name = "Dora del Hoyo",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_5_translations,
     .translation_count = 1},
    {.name = "Fr. Joseph Múzquiz",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_6_translations,
     .translation_count = 1},
    {.name = "Fr. José María Hernández Garnica",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_7_translations,
     .translation_count = 1},
    {.name = "Toni Zweifel",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_8_translations,
     .translation_count = 1},
    {.name = "Ernesto Cofiño",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_9_translations,
     .translation_count = 1},
    {.name = "Encarnita Ortega",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_10_translations,
     .translation_count = 1},
    {.name = "Tomás & Paquita Alvira",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_11_translations,
     .translation_count = 1},
    {.name = "Laura & Eduardo Ortiz de Landázuri",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_12_translations,
     .translation_count = 1},
    {.name = "Pedro Ballester",
     .destination = PRAYER_DESTINATION_TEXT,
     .default_language = PRAYER_LANGUAGE_ENGLISH,
     .translations = s_card_13_translations,
     .translation_count = 1},
};
