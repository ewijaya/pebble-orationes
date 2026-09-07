#include "prayer_document.h"
#include <stdlib.h>
#include <string.h>

static bool begins(const char *text, const char *prefix) {
  return strncmp(text, prefix, strlen(prefix)) == 0;
}
void prayer_document_free(PrayerDocument *document) {
  free(document->paragraphs); free(document->anchors); free(document->copy);
  *document = (PrayerDocument){0};
}
bool prayer_document_build(PrayerDocument *document, MainMenuEntryId entry,
                           const PrayerTranslation *translation) {
  *document = (PrayerDocument){0};
  if (!translation->text || !strchr(translation->text, '\n')) return false;
  const char *source = translation->text;
  const size_t length = strlen(source);
  unsigned capacity = 5;
  for (size_t i = 0; i < length; ++i) if (source[i] == '\n') ++capacity;
  if (capacity > UINT16_MAX) return false;
  document->copy = malloc(length + 1);
  document->paragraphs = calloc(capacity, sizeof(PrayerParagraph));
  document->anchors = calloc(capacity, sizeof(uint32_t));
  if (!document->copy || !document->paragraphs || !document->anchors) {
    prayer_document_free(document); return false;
  }
  memcpy(document->copy, source, length + 1);
  if (entry == MAIN_MENU_ENTRY_LITANY_OF_HUMILITY && translation->paragraph_count) {
    document->paragraphs[0] = translation->paragraphs[0];
    document->anchors[0] = UINT32_MAX;
    document->count = 1;
  }
  size_t start = 0;
  unsigned line = 0;
  while (start < length) {
    if (source[start] == '\n') {
      if (document->count) document->paragraphs[document->count - 1].space_after = true;
      ++start; continue;
    }
    const char *newline = strchr(source + start, '\n');
    size_t end = newline ? (size_t)(newline - source) : length;
    // Separate the three existing inline responses, never repeat or rewrite them.
    if (entry == MAIN_MENU_ENTRY_LITANY_OF_HUMILITY) {
      const char *responses[] = {" Hear me", " Deliver me Jesus", " Jesus grant me the grace to desire it"};
      for (unsigned r = 0; r < 3; ++r) {
        const char *response = strstr(source + start, responses[r]);
        if (response && (size_t)(response - source) < end) end = response - source;
      }
    }
    if (end == start) { ++start; continue; }
    const uint16_t index = document->count++;
    PrayerParagraph *paragraph = &document->paragraphs[index];
    paragraph->text = document->copy + start;
    document->anchors[index] = start;
    document->copy[end] = '\0';
    paragraph->style = begins(paragraph->text, "R. ") ||
        (entry == MAIN_MENU_ENTRY_LITANY_OF_LORETO &&
         (!strcmp(paragraph->text, "Have mercy on us.") || !strcmp(paragraph->text, "Pray for us"))) ||
        (entry == MAIN_MENU_ENTRY_LITANY_OF_HUMILITY &&
         (begins(paragraph->text, "Hear me") || begins(paragraph->text, "Deliver me Jesus") ||
          begins(paragraph->text, "Jesus grant me"))) ? PRAYER_PARAGRAPH_RESPONSE : PRAYER_PARAGRAPH_PRIMARY;
    if (entry == MAIN_MENU_ENTRY_COME_HOLY_SPIRIT && ++line % 3 == 0)
      paragraph->space_after = true;
    start = end + 1;
  }
  return true;
}
bool prayer_document_section(MainMenuEntryId entry, const PrayerParagraph *paragraphs,
                             uint16_t count, uint16_t index) {
  if (index >= count || paragraphs[index].style == PRAYER_PARAGRAPH_NOTE) return false;
  const char *text = paragraphs[index].text;
  if (!index || paragraphs[index - 1].style == PRAYER_PARAGRAPH_NOTE) return true;
  if (entry == MAIN_MENU_ENTRY_ASPIRATIONS) {
    // Ten-aspiration groups keep the jump menu short; Latin/English stay together.
    unsigned group = 0;
    for (uint16_t i = 0; i < index; ++i) if (paragraphs[i].space_after) ++group;
    return paragraphs[index - 1].space_after && group % 10 == 0;
  }
  if (entry == MAIN_MENU_ENTRY_LITANY_OF_HUMILITY)
    return begins(text, "From the desire of being esteemed") ||
           begins(text, "From the fear of being humiliated") ||
           begins(text, "That others may be loved");
  if (entry == MAIN_MENU_ENTRY_LITANY_OF_LORETO)
    return !strcmp(text, "God the Father of heaven.") || !strcmp(text, "Holy Mary.") ||
           !strcmp(text, "Queen of angels.") || begins(text, "V. Lamb of God who") ||
           !strcmp(text, "Other Intentions");
  if (entry == MAIN_MENU_ENTRY_PRECES)
    return begins(text, "V. Ad ") || begins(text, "V. Orémus ") ||
           !strcmp(text, "Orémus.") || !strcmp(text, "Omnes dicunt:") ||
           !strcmp(text, "V. Sancte Míchaël.") || begins(text, "Cum adsit ");
  return paragraphs[index - 1].space_after && paragraphs[index].style == PRAYER_PARAGRAPH_PRIMARY;
}
