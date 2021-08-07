#include <Arduino.h>

#define LOCA_MAX_LENGTH 50

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

    static char LOCASTRINGS[][2][LOCA_MAX_LENGTH];

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


char Loca::LOCASTRINGS[][2][LOCA_MAX_LENGTH] = {
    // LANG_GER strings   // LANG_ENG strings
    // 012345678901235 ,  0123456789012345    // string length guides to ensure we don't go beyond MAX_STR_LEN
    { "English"                                 , "Deutsch"                                         }, // 0
    { "Select Language"                         , "Sprache wählen"                                  },
    { "Settings"                                , "Einstellungen"                                   },
    { "Add some quotes and restart QUOTEFRAME." , "Füge Zitate hinzu und starte SPRYGGKLOPPA neu."  },
    { "Add your first quote!"                   , "Füge dein erstes Zitat hinzu."                   }, 
    { "RAYDIY's QUOTEFRAME"                     , "RAYDIY's SPRYGGKLOPPA"                           }, // 5
    { "Start config mode ..."                   , "Starte Konfigurations-Modus ..."                 },
    { "SSID"                                    , "SSID"                                            },
    { "Password"                                , "Passwort"                                        },
    { "Number of quotes: "                      , "Anzahl Zitate: "                                 }, 
    { "IP Address"                              , "IP-Adresse"                                      }, //10
    { "No quotes found."                        , "Keine Zitate gefunden."                          },
    { "\nRestart in config mode"                , "\nStarte die Konfiguration"                      }, 
    { "\nand add some quotes!"                  , "\nund füge Zitate hinzu."                        }, 
    { "Quote Interval"                          , "Zitate Intervall"                                }, 
    { "Seconds"                                 , "Sekunden"                                        }, // 15
    { "Minutes"                                 , "Minuten"                                         }, 
    { "Hours"                                   , "Stunden"                                         }, 
    { "Days"                                    , "Tage"                                            }, 
    { "Save"                                    , "Speichern"                                       }, 
    { "Edit Quote"                              , "Zitat bearbeiten"                                }, // 20
    { "Quote"                                   , "Zitat"                                           },
    { "New Quote here ..."                      , "Neues Zitat hier eingeben ..."                   },
    { "Author here ... (optional)"              , "Autor hier eingeben ... (optional)"              },
    { "Import Quotes"                           , "Zitate importieren"                              },
    { "JSON code here ..."                      , "JSON Code hier einfügen ..."                     }, //25
    { "Replace all quotes?"                     , "Alle Zitate ersetzen?"                           },
    { "Export Quotes"                           , "Zitate exportieren"                              },
    { "Quotes"                                  , "Zitate"                                          },
    { "Add New Quote"                           , "Neues Zitat hinzufügen"                          }, 
    { "Quote Saved"                             , "Zitat gespeichert"                               }, // 30
    { "QUOTEFRAME"                              , "SPRYGGKLOPPA"                                    },

    // ...add more string couples...
    // don't forget to add their string identifiers in ArduL10n.h too...
};

static uint8_t STR_LANG                 = 0;
static uint8_t STR_SELECT_LANG          = 1;
static uint8_t STR_SETTINGS             = 2;
static uint8_t STR_ADD_SOME_QUOTES      = 3;
static uint8_t STR_ADD_FIRST_QUOTE      = 4;
static uint8_t STR_RAYDIYS_QUOTEFRAME   = 5;
static uint8_t STR_START_CONFIG_MODE    = 6;
static uint8_t STR_SSID                 = 7;
static uint8_t STR_PASSWORD             = 8;
static uint8_t STR_NUMBER_OF_QUOTES     = 9;
static uint8_t STR_IP_ADDRESS           = 10;
static uint8_t STR_NO_QUOTES_FOUND_1    = 11;
static uint8_t STR_NO_QUOTES_FOUND_2    = 12;
static uint8_t STR_NO_QUOTES_FOUND_3    = 13;
static uint8_t STR_QUOTE_INTERVAL       = 14;
static uint8_t STR_SECONDS              = 15;
static uint8_t STR_MINUTES              = 16;
static uint8_t STR_HOURS                = 17;
static uint8_t STR_DAYS                 = 18;
static uint8_t STR_SAVE                 = 19;
static uint8_t STR_EDIT_QUOTE           = 20;
static uint8_t STR_QUOTE                = 21;
static uint8_t STR_NEW_QUOTE_HERE       = 22;
static uint8_t STR_NEW_AUTHOR_HERE      = 23;
static uint8_t STR_IMPORT_QUOTES        = 24;
static uint8_t STR_JSON_CODE_HERE       = 25;
static uint8_t STR_REPLACE_ALL_QUOTES   = 26;
static uint8_t STR_EXPORT_QUOTES        = 27;
static uint8_t STR_QUOTES               = 28;
static uint8_t STR_ADD_NEW_QUOTE        = 29;
static uint8_t STR_QUOTE_SAVED          = 30;
static uint8_t STR_QUOTEFRAME           = 31;

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
    Serial.print(F("Loca::setLanguage( "));
    Serial.print(language);
    Serial.println(F(" )"));
}