#include "Adafruit_mfGFX.h"
#include "Adafruit_ILI9341.h"
#include "SD.h"


#define SD_CS A6
#define TFT_CS A2
#define TFT_RST A0
#define TFT_DC A1
#define MOTOR D0
#define BUFFPIXEL 20

//TFT variables
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

//DC Motor variables
int milisecsMovingMotor = 1000;

void bmpDraw(char *filename, uint8_t x, uint16_t y);
void showTextInScreen(String textToPrint, int x, int y);
uint16_t read16(File &f);
uint32_t read32(File &f);


// This routine runs only once upon reset
void setup()
{
   //setup TFT
   tft.begin();

   //rotation to horizontal
   tft.setRotation(1);

   //test TFT
   testFillScreen();

   if (!SD.begin(SD_CS)) {
     showTextInScreen("SD failed!",10,10);
     return;
   } else {
     showTextInScreen("SD started",10,10);
   }

   //setup DC Motor
   pinMode(MOTOR, OUTPUT);

   //setup StandBy image at TFT
   setScreenToStandBy();

   //Register our Spark function here
   Spark.function("twitter", tftShowTwitter);

   //Register our Spark function here
   Spark.function("candy", moveMotorCandy);
}

// This routine loops forever
void loop()
{
   // Nothing to do here
}

void testFillScreen() {
  tft.fillScreen(ILI9341_RED);
  tft.fillScreen(ILI9341_YELLOW);
  tft.fillScreen(ILI9341_GREEN);
  tft.fillScreen(ILI9341_BLACK);
}

void setScreenToStandBy(){
  bmpDraw("btdev.bmp", 0, 0);
}

void showTextInScreen(String textToPrint, int x, int y)
{
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(x, y);
  tft.setTextSize(3);
  tft.println();

  tft.println(textToPrint);
}

// This function gets called whenever there is a matching API request
// the command string format is l<led number>,<state>
// for example: l1,HIGH or l1,LOW
//              l2,HIGH or l2,LOW

int tftShowTwitter(String command)
{
  //Adjustment to present correctly the text on the screen
  String textToPrint = " Sending tweet to \n"
                       " "+ command;

  //Printing the Twitter screen
  bmpDraw("twitter.bmp", 0, 0);
  showTextInScreen(textToPrint,10,140);
  delay(4000);
}

int moveMotorCandy(String command)
{
  //Adjustment to present correctly the text on the screen
  String textToPrint = " Enjoy! \n"
                       " Tr.ID: "+ command;

  //Printing the MotorMovement screen
  bmpDraw("motor.bmp", 0, 0);
  showTextInScreen (textToPrint,10,160);
  delay(1000);

  digitalWrite(MOTOR, 1);   // Turn ON the motor
  delay(milisecsMovingMotor);
  digitalWrite(MOTOR, 0);   // Turn OFF the motor

  delay(3000);
  setScreenToStandBy();
}


void bmpDraw(char *filename, uint8_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
