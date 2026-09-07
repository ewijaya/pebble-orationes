#pragma once
#include "main_menu_catalog.h"
#include "prayers.h"

// Display-only segmentation. Source byte anchors survive layout/font changes.
typedef struct {
  PrayerParagraph *paragraphs;
  uint32_t *anchors;
  uint16_t count;
  char *copy;
} PrayerDocument;
bool prayer_document_build(PrayerDocument *document, MainMenuEntryId entry,
                           const PrayerTranslation *translation);
void prayer_document_free(PrayerDocument *document);
bool prayer_document_section(MainMenuEntryId entry, const PrayerParagraph *paragraphs,
                             uint16_t count, uint16_t index);
