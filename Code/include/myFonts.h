#include <Adafruit_GFX.h>

// nice Fonts http://oleddisplay.squix.ch/#/home: 
// script: Kranky, Rock Salt, Schoolbell
// slab: Maiden Orange, Special Elite
// sans: Luckiest Guy, Crushed

typedef struct {
    GFXfont font_big;
    GFXfont font_medium;
    GFXfont font_small;
}FontSet;
FontSet fontSetOfLines[10];
uint8_t fontSizeOfLines[10];

#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSansBold18pt7b.h"

#include "myFonts/Crushed_Regular_40.h"
#include "myFonts/Crushed_Regular_50.h"
#include "myFonts/Crushed_Regular_65.h"
FontSet FontSet_Crushed = {Crushed_Regular_65, Crushed_Regular_50, Crushed_Regular_40};


#include "myFonts/Kranky_Regular_40.h"
#include "myFonts/Kranky_Regular_50.h"
#include "myFonts/Kranky_Regular_65.h"
FontSet FontSet_Kranky = {Kranky_Regular_65, Kranky_Regular_50, Kranky_Regular_40};

#include "myFonts/Luckiest_Guy_Regular_40.h"
#include "myFonts/Luckiest_Guy_Regular_50.h"
#include "myFonts/Luckiest_Guy_Regular_65.h"
FontSet FontSet_Luckiest_Guy = {Luckiest_Guy_Regular_65, Luckiest_Guy_Regular_50, Luckiest_Guy_Regular_40};

#include "myFonts/Maiden_Orange_Regular_40.h"
#include "myFonts/Maiden_Orange_Regular_50.h"
#include "myFonts/Maiden_Orange_Regular_65.h"
FontSet FontSet_Maiden_Orange = {Maiden_Orange_Regular_65, Maiden_Orange_Regular_50, Maiden_Orange_Regular_40};

#include "myFonts/Permanent_Marker_Regular_45.h"
#include "myFonts/Permanent_Marker_Regular_52.h"
#include "myFonts/Permanent_Marker_Regular_64.h"
FontSet FontSet_PermanentMarker = {Permanent_Marker_Regular_64, Permanent_Marker_Regular_52, Permanent_Marker_Regular_45};

#include "myFonts/Special_Elite_Regular_40.h"
#include "myFonts/Special_Elite_Regular_50.h"
#include "myFonts/Special_Elite_Regular_65.h"
FontSet FontSet_Special_Elite = {Special_Elite_Regular_65, Special_Elite_Regular_50, Special_Elite_Regular_40};


FontSet fontSets[6] = {FontSet_Crushed,FontSet_Kranky,FontSet_Luckiest_Guy,FontSet_Maiden_Orange,FontSet_PermanentMarker,FontSet_Special_Elite};



FontSet GetRandomFontSet()
{
    int size = sizeof fontSets / sizeof fontSets[0];
    int randomID = random(0,size-1);
    return fontSets[randomID];
}