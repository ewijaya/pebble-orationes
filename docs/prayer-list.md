# Orationes prayer directory

Includes Latin Preces, prayers before and after mental prayer, aspirations for everyday life, and prayers for offering your work to God.

This directory describes the library in **v0.6.0**. It lists what is available rather than reproducing the prayer texts. [Install Orationes](https://apps.repebble.com/9882f741750c43eb8309777e) · [Back to README](../README.md)

## Choose what appears on your watch

Open **Settings → Prayer Shortcuts** on the watch, or Orationes **Settings** in the Pebble mobile app. Choose and arrange up to seven entries: individual prayers, Prayer Cards, or collections. Choose **Empty** to leave a slot unused. Settings stays available.

The default shortcuts are **Preces, Holy Rosary, Regina Caeli, Angelus, and Memorare**, in that order. Everything else is optional; adding a shortcut does not download anything. All prayer content is already on the watch.

## Ready on your watch

| Default entry | Language | What opens |
| --- | --- | --- |
| Preces | Latin | Complete scrollable text, from *Sérviam!* through *Pax. / In ætérnum.*, with versicles, responses, and rubrics. |
| Holy Rosary | English | Today's Mysteries, All Mysteries, and Litany of Loreto. A mystery reference, not a bead counter or a full guided Rosary. |
| Regina Caeli | English | Versicles, responses, and concluding prayer. |
| Angelus | English | Versicles, responses, and concluding prayer; each Hail Mary is abbreviated as `Hail Mary ...`. |
| Memorare | English | Complete prayer beginning “Remember, O most gracious Virgin Mary”. |

## Throughout the day

These English entries can be opened through the **More Prayers** collection or added as individual shortcuts.

| Entry | Included content |
| --- | --- |
| Mental Prayer | Prayers before and after mental prayer, together on one scrollable screen. |
| Visit & Communion | A visit to the Blessed Sacrament using abbreviated Our Father, Hail Mary, and Glory be references, followed by the Spiritual Communion prayer. |
| Before Work | Prayer asking God's guidance and assistance in our actions and work. |
| Night Examination | A brief examination of conscience, sorrow, and a resolution for tomorrow, with a closing instruction to pray three Hail Marys. |

## Marian prayers and Rosary

**Holy Rosary → Today's Mysteries** selects the set using the watch's local weekday. **All Mysteries** lets you choose any set. Each contains five mystery names:

| Set | Days | Mysteries, in order |
| --- | --- | --- |
| Joyful | Monday / Saturday | Annunciation; Visitation; Nativity; Presentation; Finding in the Temple. |
| Sorrowful | Tuesday / Friday | Agony in the Garden; Scourging at the Pillar; Crowning with Thorns; Carrying of the Cross; Crucifixion. |
| Glorious | Wednesday / Sunday | Resurrection; Ascension; Descent of Holy Spirit; Assumption of Mary; Coronation of Mary. |
| Luminous | Thursday | Baptism in the Jordan; Wedding at Cana; Proclamation of Kingdom; Transfiguration; Institution of Eucharist. |

Today's Mysteries uses “Proclamation of the Kingdom” and “Institution of the Eucharist” for those two Luminous labels.

- **Litany of Loreto** — English; open through Holy Rosary or its own shortcut. Includes Mother of mercy, Mother of hope, and Solace of migrants. “Pray for us” appears once for the Marian invocations; Lamb of God responses and the final versicle/response remain explicit. Other Intentions follow for the Church and Nation, the bishop, and souls in purgatory, using abbreviated prayer references.
- **Blessed Be Your Purity** — English; choose **Blessed Purity** in Prayer Shortcuts.
- Angelus, Regina Caeli, and Memorare are listed among the default entries above.

## Confession

These English entries are available through the **Confession** collection or as individual shortcuts. The collection includes preparation and guidance as well as prayer text.

| Entry | Included content |
| --- | --- |
| Examination | Questions for an examination of conscience before Confession. |
| Act of Contrition | Complete prayer beginning “O my God, I am heartily sorry”. |
| Before Confession | Guidance on contrition and amendment, followed by an Act of Contrition. |
| After Confession | Brief guidance on thanksgiving and sins remembered after Confession. |

## Psalms, aspirations, and other prayers

All are available as individual shortcuts. Except for the bilingual material in Aspirations, these entries are in English.

| Prayer | Shortcut label | Included content |
| --- | --- | --- |
| Psalm 50 (51) | Psalm 50 (51) | The penitential psalm beginning “Have mercy on me, God, in your kindness”. |
| Psalm 2 | Psalm 2 | Psalm text with an antiphon, versicles, responses, and concluding prayer; Glory be remains abbreviated. |
| Canticle of the Three Children | Three Children | Canticle, Psalm 150, antiphon, responses, and concluding prayers. Familiar prayer references remain abbreviated. |
| Acceptance of Death | Acceptance | Short prayer of acceptance beginning “O Lord, my God”. |
| Prayer for Vocations | Vocations | Prayer beginning “Lord Jesus Christ, Shepherd of souls”. |
| Aspirations | Aspirations | One continuous collection of 92 short prayers: 71 Latin/English pairs and 21 English-only entries. Latin is italicized; concise Scripture references appear in parentheses where supplied. Individual aspirations are not separate shortcuts. |

## Prayer Cards

All 14 cards are in English. Add **Prayer Cards** for the collection, or choose one person's card as a direct shortcut. On the watch's entry picker, individual cards have a **Card:** prefix.

1. St. Josemaría Escrivá
2. Bl. Álvaro del Portillo
3. Bl. Guadalupe Ortiz de Landázuri
4. Ven. Isidoro Zorzano
5. Ven. Montse Grases
6. Dora del Hoyo
7. Fr. Joseph Múzquiz
8. Fr. José María Hernández Garnica
9. Toni Zweifel
10. Ernesto Cofiño
11. Encarnita Ortega
12. Tomás & Paquita Alvira
13. Laura & Eduardo Ortiz de Landázuri
14. Pedro Ballester

Each card includes its intercessory prayer and abbreviated Our Father, Hail Mary, and Glory be references. The 11 cards from Isidoro Zorzano through Pedro Ballester also retain their supplied private-use declarations in small print. Those qualifications remain part of the cards; their inclusion here does not imply approval for public devotion. Biographies and postal contact details are not included in the watch reader.

## About this directory

Entries and labels reflect the bundled [prayer catalog](../src/c/main_menu_catalog.c), [prayers](../src/c/prayers.c), [collections](../src/c/prayer_collections.c), [Rosary data](../src/c/rosary_data.c), [Litany](../src/c/litany.c), [Aspirations](../src/c/aspirations.c), and [Prayer Cards](../src/c/prayer_cards.c). Languages are fixed per entry; there is no language-selection menu.

Orationes is an independent personal project. It is not an official application of, or endorsed by, Opus Dei, the Catholic Church, Core Devices, or Pebble.
