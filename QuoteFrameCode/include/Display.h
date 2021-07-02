#include <Arduino.h>
#include "Frames.h"
#include "myFonts.h"
#include "Frames/QuoteFrameQRCode.h"
#include "Frames/arrow.h"

#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h> // including both doesn't hurt
#include <GxEPD2_3C.h> // including both doesn't hu

// ESP32 DEVKIT C (WROOM 32) PINS
static const uint8_t EPD_BUSY         = 4;  // to EPD BUSY
static const uint8_t EPD_CS           = 5;  // (VSPI) to EPD CS
static const uint8_t EPD_RST          = 16;  // to EPD RST
static const uint8_t EPD_DC           = 17;  // to EPD DC
static const uint8_t EPD_SCK          = 18; // (VSPI) to EPD CLK
static const uint8_t EPD_MISO         = 19; // (VSPI) Master-In Slave-Out not used, as no data from display
static const uint8_t EPD_MOSI         = 23; // (VSPI) to EPD DIN


// ESP32 ePulse (WROVER B) PINS
/*
static const uint8_t EPD_BUSY         = 4;  // to EPD BUSY
static const uint8_t EPD_CS           = 5;  // (VSPI)to EPD CS
static const uint8_t EPD_RST          = 21; // to EPD RST
static const uint8_t EPD_DC           = 22; // to EPD DC
static const uint8_t EPD_SCK          = 18; // (VSPI) to EPD CLK
static const uint8_t EPD_MISO         = 19; // (VSPI) Master-In Slave-Out not used, as no data from display
static const uint8_t EPD_MOSI         = 23; // (VSPI) to EPD DIN
*/

#if defined(ESP32)
// copy constructor for your e-paper from GxEPD2_Example.ino, and for AVR needed #defines
#define MAX_DISPLAY_BUFFER_SIZE 800 // 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
//GxEPD2_3C<GxEPD2_583c, GxEPD2_583c::HEIGHT> display(GxEPD2_583c(/*CS=5*/ EPD_CS, /*DC=*/ EPD_DC, /*RST=*/ EPD_RST, /*BUSY=*/ EPD_BUSY));
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(/*CS=5*/ EPD_CS, /*DC=*/ EPD_DC, /*RST=*/ EPD_RST, /*BUSY=*/ EPD_BUSY)); // GDEW075T7 800x480
//GxEPD2_BW<GxEPD2_750, GxEPD2_750::HEIGHT> display(GxEPD2_750(/*CS=5*/ 5, /*DC=*/ 2, /*RST=*/ 0, /*BUSY=*/ 4));
//GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(/*CS=5*/ 5, /*DC=*/ 2, /*RST=*/ 0, /*BUSY=*/ 4)); // GDEW075T7 800x480
#endif


// the following values depends on the size of the ePaper display and 
// the frame and passepartout you use and how have mopunted the display.
// so you need to finetune the values until the TestRactangle() fits exactly.
// maximal visible area rectangle test hidden by the frame
// we have an effective area from 615x400 pixels
// with a top left corner at 65,35 x,y position
/*
static const int CANVAS_LEFT = 58;
static const int CANVAS_TOP = 45;
static const int CANVAS_WIDTH = 680;
static const int CANVAS_HEIGHT = 437;
static const int CANVAS_MARGIN = 60; // minimal margin between quote and left and right side of canvas

static const float LINE_DISTANCE = 20; 
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS DECLARATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class Display
{
    public:
    static int CANVAS_LEFT;
    static int CANVAS_TOP;
    static int CANVAS_WIDTH;
    static int CANVAS_HEIGHT;
    static int CANVAS_MARGIN;
    static float LINE_DISTANCE; 
    static void Init();
    static void Draw();
    static void SetCanvasSize(int x, int y, int w, int h, int m);
    static void SetLineDistance(int f);
    static void DrawCenteredText(String text, int y);
    static void ShowMessage(String message);

    private:
    static int MapValue(int a1, int a2, int b1, int b2, int aVal);
    static void DrawFrame();
    static void CreateStringArrayFromText(String text);
    static void ExchangeGermanUnmlaute(String &text);
    static void DrawLinesOfTextCentered(quote q);
    static int LinesOfTextCount();
    static uint16_t LinesOfTextLineHeight(int i);
    static uint16_t LinesOfTextLineWidth(int i);
    static int LinesOfTextTotalHeight();
    static int LinesOfTextTotalWidth();
    static bool StringContainsChar(String &text, char needle);
    static void LineBreakTextAtChar(String &text, char needle);
    static void LineBreakTextAfterChar(uint8_t index, char needle);
    static int StringCountChar(String &text, char needle);
    static int StringCharIsLast(String &text, char needle);
    static void SetTextLineFontSet(FontSet fontSet, uint8_t i);
    static void SetTextLineFontSet(FontSet fontSet);
    static void SetTextLine(String text, uint8_t i);
    static void SetTextLine(String text);
    static bool DoesQuoteFitIntoWidth();
    static bool DoesQuoteFitIntoHeight();
    static void BreakLineOfText(uint8_t index);
    static void InsertLineOfText(uint8_t index, String text);
    static void LineBreakTextAtPosition(uint8_t index, int position);
    static void LineBreakAtMiddleSpace(uint8_t index);
    static void SetBiggestFont();
    static void SetDisplayFontFromLineOfText(uint8_t index);
    static void DrawTestRectangle();
    static void DebugPrintLinesOfText();
    static void DrawQRCode();
    static void DrawConfigInfos();
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////


int Display::CANVAS_LEFT = 0;
int Display::CANVAS_TOP = 0;
int Display::CANVAS_WIDTH = display.width();
int Display::CANVAS_HEIGHT = display.height();
int Display::CANVAS_MARGIN = 60;
float Display::LINE_DISTANCE = 20;


/**************************************************
* Init() 
* initialises the ePaper display 
*/
void Display::Init()
{
    Serial.println("Display::InitialiseDisplay()");

    display.init(115200,true, 2);
    SPI.end();
    SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);

    display.setRotation( SCREEN_ROTATION );
    display.setFullWindow();
    display.setTextWrap(false);
}


/**************************************************
* SetCanvasSize(int x, int y, int w, int h, int m) 
*/
void Display::SetCanvasSize(int x, int y, int w, int h, int m)
{
    Display::CANVAS_LEFT = x;
    Display::CANVAS_TOP = y;
    Display::CANVAS_WIDTH = w;
    Display::CANVAS_HEIGHT = h;
    Display::CANVAS_MARGIN = m;
}


/**************************************************
* SetLineDistance(int f)
*/
void Display::SetLineDistance(int f)
{
    Display::LINE_DISTANCE = f;
}


/**************************************************
* Draw()
* Draws the quote and the background images
*/
void Display::Draw()
{
    Serial.println("Display::Draw()");

    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);

        //Display::DrawTestRectangle();

        // if we are in config mode, draw qr code of the WiFi credentials
        if (MODE==MODE_CONFIG)
        {
            Display::DrawQRCode();
            Display::DrawConfigInfos();

        // normal quote frame mode
        }else{
            Display::DrawFrame();
            quote q = Quotes::GetRandomQuote();
            Display::CreateStringArrayFromText(q.txt);
            Display::SetBiggestFont();
            Display::DrawLinesOfTextCentered(q);
        }
    }
    while (display.nextPage());
}


/**************************************************
* DrawFrame()
* Draws some random frames in the corners of the display
*/
void Display::DrawFrame()
{
    //display.drawInvertedBitmap( CANVAS_LEFT, CANVAS_TOP, image_data_frame1tl, 171,171, GxEPD_BLACK );
    //display.drawInvertedBitmap( CANVAS_LEFT + CANVAS_WIDTH - 171, CANVAS_TOP + CANVAS_HEIGHT - 171, image_data_frame1br, 171,171, GxEPD_BLACK );

    int margin = 10;

    int cntFrames = sizeof FRAMES / sizeof FRAMES[0];
    int frameIndex = random(cntFrames);

    int w = FRAMES[frameIndex].width;
    int h = FRAMES[frameIndex].height;

    // draw top left frame

    uint8_t* frameImage = FRAMES[frameIndex].topLeftFrame;
    int x = Display::CANVAS_LEFT + margin;
    int y = Display::CANVAS_TOP + margin;
    display.drawInvertedBitmap( x, y, frameImage, w,h, GxEPD_BLACK );

    // draw bottom right frame

    frameImage = FRAMES[frameIndex].bottomRightFrame;
    x = Display::CANVAS_LEFT + Display::CANVAS_WIDTH - w - margin;
    y = Display::Display::CANVAS_TOP + Display::CANVAS_HEIGHT - h - margin;
    display.drawInvertedBitmap( x, y, frameImage, w,h, GxEPD_BLACK );
}


/**************************************************
* DrawQRCode()
*/
void Display::DrawQRCode()
{
    // draw qr code graphic
    int x = Display::CANVAS_LEFT + 40;
    int y = Display::CANVAS_TOP + 100;
    display.drawInvertedBitmap( x, y, image_data_QuoteFrameQRCode, 237,237, GxEPD_BLACK );

    // draw arrow image
    int cx = Display::CANVAS_LEFT + Display::CANVAS_WIDTH/2 - 35;
    int cy = Display::CANVAS_TOP + Display::CANVAS_HEIGHT/2 - 14 + 130;
    display.drawInvertedBitmap( cx, cy, image_data_arrow, 70,28, GxEPD_BLACK );
}


/**************************************************
* DrawConfigInfos()
*/
void Display::DrawConfigInfos()
{
    display.setTextColor(GxEPD_BLACK);
    
    // draw headline
    display.setFont(&Maiden_Orange_Regular_50);
    Display::DrawCenteredText("RAYDIY's QuoteFrame", Display::CANVAS_TOP + 60);

    // draw centered line
    int x = Display::CANVAS_LEFT + Display::CANVAS_WIDTH/2;
    int y = Display::CANVAS_TOP + 100;
    display.drawLine(x,y,x,y+237,GxEPD_BLACK);

    x += Display::CANVAS_LEFT + 5;
    
    // draw SSID
    y += 30;
    display.setFont(&FreeSans12pt7b);
    display.setCursor(x,y);
    display.print("SSID:");

    y += 50;
    display.setFont(&Maiden_Orange_Regular_50);
    display.setCursor(x,y);
    display.print(String(AP_SSID));

    // draw PASSWORD
    y += 50;
    display.setFont(&FreeSans12pt7b);
    display.setCursor(x,y);
    display.print("PASSWORD:");

    y += 50;
    display.setFont(&Maiden_Orange_Regular_50);
    display.setCursor(x,y);
    display.print(String(AP_PASSWORD));

    // draw IP
    y += 50;
    display.setFont(&FreeSans12pt7b);
    display.setCursor(x,y);
    display.print("IP: 192.168.4.1");


    // print number of quotes at the bottom of screen
    uint16_t count = Quotes::GetNumberOfQuotes();
    String quoteStats = "Number of quotes: " + String(count);
    
    display.setFont(&FreeSans12pt7b);

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds(quoteStats, 0, 0, &tbx, &tby, &tbw, &tbh); 

    // center bounding box by transposition of origin:
    x = Display::CANVAS_LEFT + Display::CANVAS_WIDTH/2 - tbw/2;
    y = Display::CANVAS_TOP + Display::CANVAS_HEIGHT - 30;
    display.setCursor(x,y);
    display.print(quoteStats);
}


/**************************************************
* DrawCenteredText(String text, int y)
*/
void Display::DrawCenteredText(String text, int y)
{
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh); 

    int x = Display::CANVAS_LEFT + Display::CANVAS_WIDTH/2 - tbw/2;
    display.setCursor(x,y);
    display.print(text);
}


/**************************************************
* MapValue(int a1, int a2, int b1, int b2, int aVal)
* map the value aVal from range a1,a2 to range b1,b2
*/
int Display::MapValue(int a1, int a2, int b1, int b2, int aVal)
{
    return b1 + (aVal-a1)*(b2-b1)/(a2-a1);
}


/**************************************************
* CreateStringArrayFromText(String text)
* fills the global linesOfText Array with the text lines of the quote string
*/
void Display::CreateStringArrayFromText(String text)
{
    //Serial.println("CreateStringArrayFromText()");

    // reset the lines of text array by filling all with an empty string
    Display::SetTextLine("");

    // Exchange all german Umlaute in the text string
    Display::ExchangeGermanUnmlaute(text);

    // get a random fonty style but always with the smallest size version
    FontSet randomFontSet = GetRandomFontSet();
    Display::SetTextLineFontSet(randomFontSet);

    // Handle manual or automatic line breaks
    if ( Display::StringContainsChar(text, '\n') )
    {
        //Serial.println("Linebreaks manually set by user.");
        Display::LineBreakTextAtChar(text, '\n');
    }

    // if no linebreaks exists, we check other chars to break for
    else
    {
        //Serial.println("No linebreaks found -> Starting auto line break!");

        // first copy the text into first line of array
        Display::SetTextLine(text, 0);

        // if a line does not fit with its current font into the margined width we break it into two lines

        for (size_t fitCheck = 0; fitCheck < 20; fitCheck++)
        {
            //Serial.println("");
            //Serial.print("====== Check quote does fit try# ");
            //Serial.println(fitCheck);

            if (Display::DoesQuoteFitIntoWidth() == false)
            {
                // get the first line that does not fit into width
                int maxWidth = CANVAS_WIDTH - CANVAS_MARGIN*2;
                int count = Display::LinesOfTextCount();
                for (size_t i = 0; i < count; i++)
                {
                    if ( LinesOfTextLineWidth(i) > maxWidth )
                    {
                        //Break LineOfText
                        Display::BreakLineOfText(i);
                        break;
                    }
                }
            }
            else
            {
                //Serial.println("Quotes does fit :)");
                break;
            }

            //Display::DebugPrintLinesOfText();
        }
        
    }
}


/**************************************************
* BreakLineOfText(uint8_t index)
*/
void Display::BreakLineOfText(uint8_t index)
{
    //Serial.println("BreakLineOfText()");

    if ( Display::StringContainsChar(linesOfText[index], ',') )
    {
        //Serial.println("Break at ,");
        Display::LineBreakTextAfterChar(index, ',');
    }

    else if ( Display::StringContainsChar(linesOfText[index], '.') && Display::StringCharIsLast(linesOfText[index], '.') == false )
    {
        //Serial.println("Break at .");
        Display::LineBreakTextAfterChar(index, '.');
    }

    else if ( Display::StringContainsChar(linesOfText[index], '-') )
    {
        //Serial.println("Break at -");
        Display::LineBreakTextAfterChar(index, '-');
    }

    // if all other methods above are not gripping then try by spaces
    else
    {
        //Serial.println("Break at MIDDLE SPACE");
        Display::LineBreakAtMiddleSpace(index);
    }
}


/**************************************************
* DoesQuoteFitIntoWidth()
*/
bool Display::DoesQuoteFitIntoWidth()
{
    int totalQuoteWidth = Display::LinesOfTextTotalWidth();
    int marginedCanvasWidth = CANVAS_WIDTH - CANVAS_MARGIN*2;

    if (totalQuoteWidth > marginedCanvasWidth)
    {
        return false;
    }

    return true;
}


/**************************************************
* DoesQuoteFitIntoHeight()
*/
bool Display::DoesQuoteFitIntoHeight()
{
    int totalQuoteHeight = Display::LinesOfTextTotalHeight();
    int marginedCanvasHeight = CANVAS_HEIGHT - CANVAS_MARGIN*2;

    if (totalQuoteHeight > marginedCanvasHeight)
    {
        return false;
    }

    return true;
}


/**************************************************
* SetTextLine(String text, int index=-1)
* Sets the lineOfText
*/
void Display::SetTextLine(String text, uint8_t index)
{
    linesOfText[index] = text;
}


/**************************************************
* SetTextLine(String text, int index=-1)
* Sets all of the lineOfText array to text
*/
void Display::SetTextLine(String text)
{
    int size = sizeof linesOfText / sizeof linesOfText[0];
    for (size_t i = 0; i < size; i++)
    {
        linesOfText[i] = text;
    }
}

/**************************************************
* SetTextLineFontSet(FontSet fontSet, int index=-1)
* Sets the lineOfText to a certain font
*/
void Display::SetTextLineFontSet(FontSet fontSet, uint8_t i)
{
    fontSetOfLines[i] = fontSet;
    fontSizeOfLines[i] = 0;
}

/**************************************************
* SetTextLineFontSet(FontSet fontSet, int index=-1)
* Sets the lineOfText to a certain font
*/
void Display::SetTextLineFontSet(FontSet fontSet)
{
    int size = sizeof fontSetOfLines / sizeof fontSetOfLines[0];
    for (size_t i = 0; i < size; i++)
    {
        fontSetOfLines[i] = fontSet;
    }
}

/**************************************************
* LineBreakTextAtChar(String &text, char needle)
* Breaks the line of text AT a certain char
*/
void Display::LineBreakTextAtChar(String &text, char needle)
{
    Serial.println("LineBreakTextAtChar()");

    // iterate the string and break into pieces at needle linebreaks
    int cnt = 0;
    int lastIndex = 0;
    int length = text.length();

    for(int i=0; i<length; i++)
    {
        // if we are at the end of the text
        // copy the rest from the text into the last line
        if (i == length-1)
        {
            String result = "";
            result = text.substring(lastIndex);
            result.trim();
            linesOfText[cnt] = result;
            cnt++;
        }
        if (text.charAt(i) == needle)
        {
            String result = "";
            result = text.substring(lastIndex,i);
            result.trim();
            linesOfText[cnt] = result;

            lastIndex = i;
            cnt++;
        }
    }

    // debug output the results
    for (size_t i = 0; i < cnt; i++)
    {
        Serial.print("i:");
        Serial.print(i);
        Serial.print(" ");
        Serial.print(linesOfText[i]);
        Serial.println("");
    }
}


/**************************************************
* LineBreakTextAfterChar(uint8_t index, char needle)
* Breaks the line of text AFTER a certain char
*/
void Display::LineBreakTextAfterChar(uint8_t index, char needle)
{
    //Serial.println("LineBreakTextAfterChar()");

    uint8_t count = sizeof linesOfText / sizeof linesOfText[0];

    if (index >= count-1)
    {
        Serial.println("WARNING: cannot break last line of text - ABORT");
        return;
    }

    // move all follow up lines of text one array
    // by inserting a blank line of text 
    Display::InsertLineOfText(index+1, "");
    

    // iterate the string and break into pieces after the needle
    //int cnt = 0;
    //int lastIndex = 0;
    String text = linesOfText[index];
    int length = text.length()-1;

    // use length-1 since we do not need to check the very last character
    for(int i=0; i<length-1; i++)
    {
        // if we are at the end of the text
        // copy the rest from the text into the last line
        /*
        if (i == length-2)
        {
            String result = "";
            result = text.substring(lastIndex);
            result.trim();
            linesOfText[cnt] = result;
            cnt++;
        }
        */
        if (text.charAt(i) == needle)
        {
            String result1 = text.substring(0,i+1);
            String result2 = text.substring(i+1);
            result1.trim();
            result2.trim();
            linesOfText[index] = result1;
            linesOfText[index+1] = result2;

            //Serial.print("BROKE: ");
            //Serial.println(text);
            //Serial.println("INTO: ");
            //Serial.print("1: ");
            //Serial.println(result1);
            //Serial.print("2: ");
            //Serial.println(result2);
        }
    }
}


/**************************************************
* LineBreakAtMiddleSpace(uint8_t index)
*/
void Display::LineBreakAtMiddleSpace(uint8_t index)
{
    int cntSpaces = Display::StringCountChar(linesOfText[index], ' ');
    if ( cntSpaces > 0 )
    {
        int q = 1 + ((cntSpaces - 1) / 2);

        // now q position of space character and break after it
        int length = linesOfText[index].length()-1;
        int cnt = 0;
        for(int i=0; i<length; i++)
        {
            if (linesOfText[index].charAt(i) == ' ')
            {
                cnt++;
            }

            if ( cnt == q )
            {
                Display::LineBreakTextAtPosition(index, i);
                break;
            }
        }
    }
}


/**************************************************
* LineBreakTextAtPosition(uint8_t index, int position)
*/
void Display::LineBreakTextAtPosition(uint8_t index, int position)
{
    //Serial.println("LineBreakTextAtPosition()");

    uint8_t count = sizeof linesOfText / sizeof linesOfText[0];

    if (index >= count-1)
    {
        Serial.println("WARNING: cannot break last line of text - ABORT");
        return;
    }

    // move all follow up lines of text one array
    // by inserting a blank line of text 
    Display::InsertLineOfText(index+1, "");

    String text = linesOfText[index];

    String result1 = text.substring(0,position+1);
    String result2 = text.substring(position+1);
    result1.trim();
    result2.trim();
    linesOfText[index] = result1;
    linesOfText[index+1] = result2;
}


/**************************************************
* InsertLineOfText(uint_8 index, String text)
*/
void Display::InsertLineOfText(uint8_t index, String text)
{
    int count = LinesOfTextCount();
    
    for (size_t i = count; i > index; i--)
    {
        linesOfText[i] = linesOfText[i-1]; 
    }

    linesOfText[index] = text;
}


/**************************************************
* StringContainsChar(String &text, char* needle)
*/
bool Display::StringContainsChar(String &text, char needle)
{
    int maxIndex = text.length();
    int cnt = 0;

    for(int i=0; i<maxIndex; i++)
    {
        if (text.charAt(i) == needle)
        {
            cnt++;
        }
    }

    if (cnt >= 2)
    {
        return true;
    }

    if (cnt == 1)
    {
        if ( Display::StringCharIsLast(text, needle) == false )
        {
            return true;
        }
    }

    return false;
}


/**************************************************
* StringCountChar(String &text, char* needle)
*/
int Display::StringCountChar(String &text, char needle)
{
    int result = 0;
    int maxIndex = text.length();

    for(int i=0; i<maxIndex; i++)
    {
        if (text.charAt(i) == needle)
        {
            result++;
        }
    }

    return result;
}


/**************************************************
* StringCharIsLast(String &text, char* needle)
*/
int Display::StringCharIsLast(String &text, char needle)
{
    int maxIndex = text.length();

    if (text.charAt(maxIndex-1) == needle)
    {
        return true;
    }

    return false;
}


/**************************************************
* ExchangeGermanUnmlaute(String text)
* exchchanges special german umlaute und retuns a new string
*/
void Display::ExchangeGermanUnmlaute(String &text)
{
    text.replace("ä", "ae");
    text.replace("ö", "oe");
    text.replace("ü", "ue");
    text.replace("Ä", "Ae");
    text.replace("Ö", "Oe");
    text.replace("Ü", "Ue");
    text.replace("ß", "ss");
}


/**************************************************
* DrawLinesOfTextCentered(Quote q)
*/
void Display::DrawLinesOfTextCentered(quote q)
{
    //Serial.println("DrawLinesOfTextCentered()");

    int cntLinesOfText = Display::LinesOfTextCount();

    int quoteHeight = LinesOfTextTotalHeight();
    //int quoteWidth = LinesOfTextTotalWidth();
    
    //int cntQuotes = sizeof QUOTES / sizeof QUOTES[0];
    //DrawDebugText(50, 60, "quotes: " + String(cntQuotes));
    //DrawDebugText(50, 90, "cntLinesOfText: " + String(cntLinesOfText));
    //DrawDebugText(50, 120, "quoteHeight: " + String(quoteHeight));
    //DrawDebugText(50, 60, "firstLineHeight: " + String(firstLineHeight));

    int cx = CANVAS_LEFT + CANVAS_WIDTH/2;
    int cy = CANVAS_TOP + CANVAS_HEIGHT/2;

    // draw bounding boxes of whole quote
    //display.drawRect( cx - quoteWidth/2, cy - quoteHeight/2, quoteWidth, quoteHeight, GxEPD_BLACK );
    //display.drawRect( cx - quoteWidth/2, cy - quoteHeight/2, quoteWidth, firstLineHeight, GxEPD_BLACK );

    int firstY = cy - quoteHeight/2;
    int lastY = firstY;

    // boundary box window
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    
    for (size_t i = 0; i < cntLinesOfText; i++)
    {
        String text = linesOfText[i];

        // set the current font for that line of text
        Display::SetDisplayFontFromLineOfText(i);

        //Serial.print("txt:");
        //Serial.println(text);
        
        // it works for origin 0, 0, fortunately (negative tby!)
        display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh); 
        lastY += tbh;

        // center bounding box by transposition of origin:
        int x = cx - tbw/2;
        int y = lastY;

        // draw bounding box around each line of text
        //display.drawRect( x, y, tbw, -tbh, GxEPD_BLACK );

        display.setCursor(x,y);
        display.setTextColor(GxEPD_BLACK);
        display.print(text);

        lastY += LINE_DISTANCE;
    }

    // print author of quote if set
    if ( q.author != "" )
    {
        int quoteLowerBorder = cy + quoteHeight/2;
        int spaceBeetweenQuoteAndLowerBorder = (CANVAS_TOP + CANVAS_HEIGHT) - quoteLowerBorder;

        //Serial.println("spaceBeetweenQuoteAndLowerBorder");
        //Serial.println(spaceBeetweenQuoteAndLowerBorder);

        display.setFont(&FreeSans12pt7b);
        String text = q.author;
        ExchangeGermanUnmlaute(text);

        // boundary box window
        int16_t tbx, tby;
        uint16_t tbw, tbh;
        
        // it works for origin 0, 0, fortunately (negative tby!)
        display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh); 

        // center bounding box by transposition of origin:
        int x = Display::CANVAS_LEFT + Display::CANVAS_MARGIN;
        int y = Display::CANVAS_TOP + Display::CANVAS_HEIGHT - tbh;

        if ( spaceBeetweenQuoteAndLowerBorder >= 90 )
        {
            x = cx - tbw/2;
            y = quoteLowerBorder + spaceBeetweenQuoteAndLowerBorder/2;
        }

        display.setCursor(x,y);
        display.setTextColor(GxEPD_BLACK);
        display.print(text);
    }
}


/**************************************************
* LinesOfTextCount()
* counts how many items in the linesOfText array do have real text and are not empty
*/
int Display::LinesOfTextCount()
{
    int result = 0;
    int max = sizeof linesOfText / sizeof linesOfText[0];

    for (size_t i = 0; i < max; i++)
    {
        if ( linesOfText[i] != "" )
        {
            result++;
        }
    }

    return result;
}


/**************************************************
* LinesOfTextLineHeight()
* returns the height of a single line of text in the linesOfText array
* i: the index in the linesOfText array to get the height from
* remember this width is based on the font currently set with display.setFont()
*/
uint16_t Display::LinesOfTextLineHeight(int i)
{
    if (linesOfText[i] == "")
    {
        return 0;
    }

    // set the current font for that line of text
    Display::SetDisplayFontFromLineOfText(i);

    // boundary box window
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    
    // it works for origin 0, 0, fortunately (negative tby!)
    display.getTextBounds(linesOfText[i], 0, 0, &tbx, &tby, &tbw, &tbh);

    return tbh;
}


/**************************************************
* LinesOfTextLineWidth()
* returns the width of a single line of text in the linesOfText array
* i: the index in the linesOfText array to get the height from
* remember this width is based on the font currently set with display.setFont()
*/
uint16_t Display::LinesOfTextLineWidth(int i)
{
    if (linesOfText[i] == "")
    {
        return 0;
    }

    // set the current font for that line of text
    Display::SetDisplayFontFromLineOfText(i);

    // boundary box window
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    
    // it works for origin 0, 0, fortunately (negative tby!)
    display.getTextBounds(linesOfText[i], 0, 0, &tbx, &tby, &tbw, &tbh);

    return tbw;
}


/**************************************************
* LinesOfTextTotalHeight()
* returns the height of all lines of text in the linesOfText array
* remember this width is based on the font currently set with display.setFont()
*/
int Display::LinesOfTextTotalHeight()
{
    int result = 0;
    int lastLineHeight = 0;
    int max = Display::LinesOfTextCount();

    for (size_t i = 0; i < max; i++)
    {
        lastLineHeight = LinesOfTextLineHeight(i);
        result += lastLineHeight + LINE_DISTANCE;
    }

    result -= lastLineHeight + LINE_DISTANCE;
    result += lastLineHeight;

    return result;
}


/**************************************************
* LinesOfTextTotalWidth()
* returns the width of the widest line of text in the linesOfText array
* remember this width is based on the font currently set with display.setFont()
*/
int Display::LinesOfTextTotalWidth()
{
    int result = 0;
    int max = sizeof linesOfText / sizeof linesOfText[0];

    for (size_t i = 0; i < max; i++)
    {
        if ( linesOfText[i] != "" )
        {
            int width = LinesOfTextLineWidth(i);
            if ( result < width )
            {
                result = width;
            }
        }
    }

    return result;
}


/**************************************************
* SetBiggestFont()
* check each line of the quote if setting a bigger font still fits into width and height
* if yes, set the bigger font
*/
void Display::SetBiggestFont()
{
    //Serial.println("SetBiggestFont()");

    int count = LinesOfTextCount();

    // iterate each lien of text
    for (size_t i = 0; i < count; i++)
    {
        if ( fontSizeOfLines[i] == 0 )
        {
            fontSizeOfLines[i] = 1;
            if ( Display::DoesQuoteFitIntoWidth() == false || Display::DoesQuoteFitIntoHeight() == false )
            {
                fontSizeOfLines[i] = 0;
            }
        }

        if ( fontSizeOfLines[i] == 1 )
        {
            fontSizeOfLines[i] = 2;
            if ( Display::DoesQuoteFitIntoWidth() == false || Display::DoesQuoteFitIntoHeight() == false )
            {
                fontSizeOfLines[i] = 1;
            }
        }
    }
    
}


/**************************************************
* SetDisplayFontFromLineOfText(uint8_t index)
*/
void Display::SetDisplayFontFromLineOfText(uint8_t index)
{
    if ( fontSizeOfLines[index] == 0 )
    {
        display.setFont(&fontSetOfLines[index].font_small);
    }
    else if ( fontSizeOfLines[index] == 1 )
    {
        display.setFont(&fontSetOfLines[index].font_medium);
    }
    else
    {
        display.setFont(&fontSetOfLines[index].font_big);
    }
}


/**************************************************
* DrawTestRectangle()
* Draws a test rectangle to check the position of the visible area of the ePaper display.
* Cause of the frame and the passepartout not all of the ePaper display is visible.
* With this rectangle you can check the visible area.
* To tune the visible area change the values of CANVAS_LEFT, CANVAS_TOP, CANVAS_WIDTH, CANVAS_HEIGHT
* at the top of the script
*/
void Display::DrawTestRectangle()
{
    int x = MapValue(0, display.width(), Display::CANVAS_LEFT, Display::CANVAS_WIDTH, 0);
    int y = MapValue(0, display.height(), Display::CANVAS_TOP, Display::CANVAS_HEIGHT, 0);
    int w = MapValue(0, display.width(), Display::CANVAS_LEFT, Display::CANVAS_WIDTH, display.width());
    int h = MapValue(0, display.height(), Display::CANVAS_TOP, Display::CANVAS_HEIGHT, display.height());

    // draw outer border, most maximal visible screen
    display.drawRect( x,y,w,h, GxEPD_BLACK );

    // top left filled rectangle
    display.fillRect(x,y,60,60, GxEPD_BLACK);
    // top right filled rectangle
    display.fillRect(x+w-60,y,60,60, GxEPD_BLACK);
    // bottom left filled rectangle
    display.fillRect(x,y+h-60,60,60, GxEPD_BLACK);
    // bottom right filled rectangle
    display.fillRect(x+w-60,y+h-60,60,60, GxEPD_BLACK);
    // centered filled rectangle
    display.fillRect(x+w/2-30,y+h/2-30,60,60, GxEPD_BLACK);

    // draw crosshair in the center
    display.drawLine(x+w/2, y, x+w/2, y+h, GxEPD_BLACK);
    display.drawLine(x, y+h/2, x+w, y+h/2, GxEPD_BLACK);

    // draw border with margin, smaller rectangle
    x += Display::CANVAS_MARGIN;
    y += Display::CANVAS_MARGIN;
    w -= Display::CANVAS_MARGIN *2;
    h -= Display::CANVAS_MARGIN *2;
    display.drawRect( x,y,w,h, GxEPD_BLACK );
}


/**************************************************
* DrawTestRectangle()
*/
void Display::DebugPrintLinesOfText()
{
    Serial.println("");
    Serial.println("###### LINES OF TEXT #####");

    int count = Display::LinesOfTextCount();
    for (size_t i = 0; i < count; i++)
    {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(linesOfText[i]);
    }
}


/**************************************************
* ShowMessage()
* clears the screen and displays a centered message string
*/
void Display::ShowMessage(String message)
{
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);
        display.setFont(&Maiden_Orange_Regular_50);

        int16_t tbx, tby;
        uint16_t tbw, tbh;

        // it works for origin 0, 0, fortunately (negative tby!)
        display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh); 
        int cy = Display::CANVAS_TOP + Display::CANVAS_HEIGHT/2 + tbh/2;

        Display::DrawCenteredText(message, cy);
    }
    while (display.nextPage());
}