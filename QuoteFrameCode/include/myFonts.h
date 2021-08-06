#include <Adafruit_GFX.h>
#include <U8g2_for_Adafruit_GFX.h>

// nice Fonts http://oleddisplay.squix.ch/#/home: 
// script: Kranky, Rock Salt, Schoolbell
// slab: Maiden Orange, Special Elite
// sans: Luckiest Guy, Crushed

typedef struct {
    String name;
    uint8_t line_distance;
    const uint8_t* font_big;
    const uint8_t* font_medium;
    const uint8_t* font_small;
}FontSet;
FontSet fontSetOfLines[10]; // an array of 10 fontsets, one for each line of text. each quotes can have 10 lines of text at max.
uint8_t fontSizeOfLines[10]; // an array for maximum 10 lines of text which holds the font size for each of the 10 lines, can be 0,1 or 2

#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSansBold18pt7b.h"

#include "u8g2_fonts/BebasNeue_Regular_35.h"
#include "u8g2_fonts/BebasNeue_Regular_45.h"
#include "u8g2_fonts/BebasNeue_Regular_57.h"
#include "u8g2_fonts/BebasNeue_Regular_78.h"
FontSet FontSet_BebasNeue = {"BebasNeue_Regular", 10, BebasNeue_Regular_78, BebasNeue_Regular_57, BebasNeue_Regular_45};

#include "u8g2_fonts/AlfaSlabOne_Regular_35.h"
#include "u8g2_fonts/AlfaSlabOne_Regular_45.h"
#include "u8g2_fonts/AlfaSlabOne_Regular_57.h"
FontSet FontSet_AlfaSlabOne = {"AlfaSlabOne_Regular", 0, AlfaSlabOne_Regular_57, AlfaSlabOne_Regular_45, AlfaSlabOne_Regular_35};

#include "u8g2_fonts/SpecialElite_Regular_45.h"
#include "u8g2_fonts/SpecialElite_Regular_55.h"
#include "u8g2_fonts/SpecialElite_Regular_65.h"
FontSet FontSet_SpecialElite = {"SpecialElite_Regular", 10, SpecialElite_Regular_65, SpecialElite_Regular_55, SpecialElite_Regular_45};



FontSet fontSets[3] = {FontSet_SpecialElite, FontSet_BebasNeue, FontSet_AlfaSlabOne};



FontSet GetRandomFontSet()
{
    Serial.print("myFonts::GetRandomFontSet()");

    // if analog input pin 0 is unconnected, random analog
    // noise will cause the call to randomSeed() to generate
    // different seed numbers each time the sketch runs.
    // randomSeed() will then shuffle the random function.
    uint16_t seed = analogRead(27); 
    randomSeed(seed);

    int size = sizeof fontSets / sizeof fontSets[0];
    int randomID = random(0,size);
    
    // uncomment the following line to always get a certain font, i.e. for testing purposes
    //randomID = 0;

    Serial.print(" seed:");
    Serial.print(seed);
    Serial.print(" size:");
    Serial.print(size);
    Serial.print(" randomID:");
    Serial.println(randomID);

    return fontSets[randomID];
}