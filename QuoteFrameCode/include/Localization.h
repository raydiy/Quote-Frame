#include <Arduino.h>



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS DECLARATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class Loca 
{
    public:
    static uint8_t LANG_EN;
    static uint8_t LANG_DE;

    static uint8_t DEFAULT_LANG;
    static uint8_t CURRENT_LANG;

    static char LOCASTRINGS[][2][32];

    static char* S(uint language, uint8_t string_id);
    static char* S(uint8_t string_id);
    static uint8_t getLanguage();
    static void setLanguage(uint8_t language);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE THE LOCA STRINGS HERE -- START


char Loca::LOCASTRINGS[][2][32] = {
    // LANG_GER strings   // LANG_ENG strings
    // 012345678901235 ,  0123456789012345    // string length guides to ensure we don't go beyond MAX_STR_LEN
    { "English"             , "Deutsch"         },
    { "Select Language"     , "Sprache wählen"  },
    { "Wait"                , "Warten"          },
    // ...add more string couples...
    // don't forget to add their string identifiers in ArduL10n.h too...
};

static uint8_t STR_LANG         = 0;
static uint8_t STR_SELECT_LANG  = 1;
static uint8_t STR_WAIT         = 2;


// UPDATE THE LOCA STRINGS HERE -- END
//////////////////////////////////////////////////////////////////////////////////////////////////////////



uint8_t Loca::LANG_EN = 0;
uint8_t Loca::LANG_DE = 1;
uint8_t Loca::DEFAULT_LANG = LANG_EN;
uint8_t Loca::CURRENT_LANG = DEFAULT_LANG;



char* Loca::S(uint language, uint8_t string_id)
{
    return LOCASTRINGS[string_id][language];
}

char* Loca::S(uint8_t string_id)
{
    return LOCASTRINGS[string_id][CURRENT_LANG];
}

uint8_t Loca::getLanguage ()
{
    return CURRENT_LANG;
}

void Loca::setLanguage (uint8_t language)
{
    CURRENT_LANG = language;
}