#include "aspirations.h"

// Canonical source: user-provided aspirations.json (92 entries, in file order).
// Only formatting is added: paragraph separation and parentheses around citations.
// Internal id/latin/source metadata is not displayed.
static const PrayerParagraph s_aspirations_paragraphs[] = {
    {
        .text = "Sweet heart of Jesus, be my love!",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Sweet heart of Mary, be my salvation!",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Domine, fac cum servo tuo secundum magnam misericordiam tuam.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Lord, deal with your servant according to your great mercy.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Sancte Pater omnipotens, aeterne et misericors Deus: Beata Maria intercedente, gratias tibi ago pro universis beneficiis tuis, etiam ignotis.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Holy Father almighty, eternal and merciful God, by the intercession of the Blessed Virgin Mary I thank you for all your benefits, even those unknown to me.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Cor Iesu sacratissimum et misericors, dona nobis pacem.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Most sacred and merciful heart of Jesus, grant us peace.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Cor Mariae dulcissimum, iter para tutum.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Sweetest heart of Mary, provide a safe path.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Benedicamus Patrem et Filium cum Sancto Spiritu.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Let us bless the Father and the Son with the Holy Spirit.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Domine, ut videam!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Lord, that I might see!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Mk 10:51)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Domine, ut sit!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Lord, let it be!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Domina, ut videam!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Lady, that I might see!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Domina, ut sit!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Lady, let it be!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Domine, tu omnia nosti, tu scis quia amo te!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Lord, you know all things, you know that I love you!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Jn 21:17)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "I am yours, I was born for you; what is it, Jesus, that you want of me?",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Jesus, I love you.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Glory to the Father, glory to the Son, glory to the Holy Spirit! Glory to holy Mary, and also to St. Joseph! Thanks be to the angels who attend you!",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Lord, I abandon myself to you, I trust in you, I rest in you.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "I believe in God the Father, I believe in God the Son, I believe in God the Holy Spirit. I hope in God the Father, I hope in God the Son, I hope in God the Holy Spirit. I love God the Father, I love God the Son, I love God the Holy Spirit.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Iesu, Iesu, esto mihi semper Iesus.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Jesus, Jesus, always be Jesus to me.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Tu es sacerdos in aeternum.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "You are a priest forever.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Ps 110:4)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Quod bonum est oculis eius, faciat.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Whatever is good in his eyes, let him do it.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(1 Sm 3:18)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Monstra te esse matrem!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Show yourself to be a mother!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Mother, my Mother!",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Sancta Maria, refugium nostrum et virtus!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Holy Mary, our refuge and our strength!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Holy Mary, hold back the day!",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Sancta Maria, filios tuos adiuva; filias tuas adiuva.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Holy Mary, help your sons, and help your daughters.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Sancta Maria, spes nostra, sedes sapientiae, ora pro nobis.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Holy Mary, our hope, seat of wisdom, pray for us.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Sancta Maria, spes nostra, ancilla Domini, filias tuas adiuva.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Holy Mary, our hope, handmaid of the Lord, help your daughters.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Sancta Maria, regina Operis Dei, filios tuos adiuva.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Holy Mary, queen of Opus Dei, help your children.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Sancta Maria, stella orientis, filios tuos adiuva.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Holy Mary, star of the East, help your children.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Dominus tecum.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "The Lord be with you.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(2 Thes 3:16)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Sancti angeli custodes nostri, defendite nos.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Our holy guardian angels, defend us.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "St. Joseph, our father and lord, bless all the children of the holy Church of God.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Adeamus cum fiducia ad thronum gloriae, ut misericordiam Dei consequamur.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Let us go with confidence to the throne of glory, to attain the mercy of God.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(see Heb 4:16)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Hail, Mary most pure, conceived without sin.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Sub tuum praesidium confugimus . . .",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "We fly to your protection . . .",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Nostras deprecationes ne despicias.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Despise not our petitions.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Blessed be the Mother who brought you into the world.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = false,
    },
    {
        .text = "(Lk 11:27)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Cor Mariae perdolentis, miserere nobis; miserere mei.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Sorrowful heart of Mary, have mercy on us; have mercy on me.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Beata Mater et intacta Virgo, intercede pro nobis.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Blessed Mother and inviolate Virgin, intercede for us.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Omnia in bonum!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "All for the good!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Semper ut iumentum!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Always like a beast of burden!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Ut iumentum factus sum apud te.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "I have become like a beast of burden before you.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Et ego semper tecum; tenuisti manum dexteram meam, et in voluntate tua deduxisti me, et cum gloria suscepisti me.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Lord, I want to stand before you like a donkey; but you have taken me by the bridle and have brought me along, and will receive me into your glory.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Fiat, adimpleatur, laudetur, et in aeternum superexaltetur iustissima atque amabilissima voluntas Dei super omnia. Amen. Amen!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "May the most righteous and most lovable will of God be done, accomplished, praised, and eternally exalted above all things. Amen. Amen!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "I believe more than if I saw you with my own eyes, more than if I heard you with my own ears, more than if I touched you with my own hands.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Ut in gratiarum semper actione maneamus!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "May we always remain in an act of thanksgiving!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Montes, sicut cera, fluxerunt a facie Domini.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "The mountains dissolved like wax before the face of the Lord.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Ps 97:5)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Qui tribulant me, inimici mei, ipsi infirmati sunt et ceciderunt.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Those who trouble me -- my enemies -- themselves have grown weak and have fallen.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Ps 27:2)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Servi inutiles sumus: quod debuimus facere fecimus.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "We are useless servants: we have only done what we ought to have done.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Lk 17:10)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Oportet semper orare, et non deficere.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "It is well always to pray, without ceasing.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Lk 18:1)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Ure igne Sancti Spiritus!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Burn with the fire of the Holy Spirit!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Veni, Sancte Spiritus, reple tuorum corda fidelium, et tui amoris in eis ignem accende.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Come, Holy Spirit, fill the hearts of your faithful, and enkindle in them the fire of your love.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Oportet illum crescere, me autem minui.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "He must increase, I must decrease.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Jn 3:30)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Nunc coepi!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "I begin now!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "St. Michael the Archangel, defend us in battle; be our protection against the wickedness and snares of the devil. May God rebuke him, we humbly pray, and do you, O Prince of the Heavenly Host, by the power of God, thrust into hell Satan and all the other evil spirits who prowl about the world, seeking the ruin of souls. Amen.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Oremus pro Beatissimo Papa nostro N. . . . Dominus conservet eum, et vivificet eum, et beatum faciat eum in terra, et non tradat eum in animam inimicorum eius.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Let us pray for our Most Holy Father N. . . . May the Lord preserve him, and give him life, and make him blessed on this earth, and not hand him over to the will of his enemies.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "My God! Let me hate sin and be united to you, may I embrace your holy cross, so that I in my turn may fulfill your most lovable will . . . , stripped of all earthly affection, seeking nothing but your glory . . . , generously, not keeping anything for myself, offering myself with you as a perfect holocaust.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Come, Holy Spirit! Enlighten my understanding so that I may know your commands; strengthen my heart against the ambushes of the enemy; inflame my will. . . . I have heard your voice, and I don't want to harden my heart by resisting, by saying 'later . . . tomorrow.' Nunc coepi! [I begin now!] Now! Lest there be no 'tomorrow' for me! O Spirit of truth and wisdom, Spirit of understanding and counsel, Spirit of joy and peace, I want what you want, I want it because and as and when you want it. . . .",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Serviam!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "I will serve!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Out of love I offer you all my thoughts, all my words, and all my deeds of this day, O Lord, together with my whole life.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Sancta Maria, stella maris, filios tuos adiuva!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Holy Mary, star of the sea, help your children!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Fiat, adimpleatur.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Let it be done, let it be accomplished.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Deo gratias!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Thanks be to God!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Deo omnis gloria!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "All glory to God!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "My Lord and my God.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Adauge nobis fidem, spem, et caritatem.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Increase our faith, hope, and charity.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Holy Father, through the Immaculate Heart of Mary I offer to you Jesus, your beloved Son, and in him, through him, and with him I offer myself for all his intentions and in the name of all creatures.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Lord, grant purity and gaudium cum pace [joy with peace] to me and to all.",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Welcome to the altar!",
        .style = PRAYER_PARAGRAPH_PRIMARY,
        .space_after = true,
    },
    {
        .text = "Cor contritum et humiliatum, Deus, non despicies.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "A contrite and humble heart, O God, you will not spurn.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Contritum et humiliatum valde!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Very contrite and humble!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Ite ad Ioseph.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Go to Joseph.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Gn 41:55)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "videre Petrum",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "to see Peter",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Abba, Pater!",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Father!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Gal 4:6)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Apud Deum omnia possibilia sunt.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "With God all things are possible.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Mt 19:26)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Cor mundum crea in me, Deus.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "A clean heart create for me, O God.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Ps 51:10)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Credo, sed adiuva incredulitatem meam.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "I do believe; help my unbelief.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Mk 9:24)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Diligentibus Deum omnia cooperantur in bonum.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "For those who love God all things work together unto good.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Rom 8:28)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Domine, quid me vis facere?",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Lord, what do you want me to do?",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Acts 9:6)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Ecce ego, quia vocasti me.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Here I am, for you did call me.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(1 Sm 3:6)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Filius hominis non venit ut ministraretur ei, sed ut ministraret.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "The Son of Man came not to be served, but to serve.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Mk 10:45)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Iesu, fili David, miserere mei peccatoris.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Jesus, Son of David, have mercy on me, a sinner!",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Mk 10:47)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "In te, Domine, speravi; non confundar in aeternum.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "In you, O Lord, I take refuge: let me never be put to shame.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Ps 31:1)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Mater pulchrae dilectionis, filios tuos adiuva.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Mother of fair love, help your children.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Meus cibus est ut faciam voluntatem eius qui misit me, ut perficiam opus eius.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "My food is to do the will of him who sent me, to accomplish his work.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Jn 4:34)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Non voluntas mea, sed tua fiat.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Not as I will, but as you will.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Mt 26:39)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Omnia possum in eo qui me confortat.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "I can do all things in him who strengthens me.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Phil 4:13)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Pauper servus et humilis.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "A poor and lowly servant am I.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Quia tu es, Deus, fortitudo mea.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "For you, O God, are my strength.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Ps 43:2)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Regina apostolorum, ora pro nobis.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Queen of apostles, pray for us.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = true,
    },
    {
        .text = "Sine me nihil potestis facere.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Without me you can do nothing.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Jn 15:5)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
    {
        .text = "Tantum dic verbo.",
        .style = PRAYER_PARAGRAPH_LATIN,
        .space_after = false,
    },
    {
        .text = "Only say the word.",
        .style = PRAYER_PARAGRAPH_TRANSLATION,
        .space_after = false,
    },
    {
        .text = "(Mt 8:8)",
        .style = PRAYER_PARAGRAPH_REFERENCE,
        .space_after = true,
    },
};

const PrayerTranslation aspirations_translations[1] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .paragraphs = s_aspirations_paragraphs,
        .paragraph_count = sizeof(s_aspirations_paragraphs) /
                           sizeof(s_aspirations_paragraphs[0]),
    },
};
