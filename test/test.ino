// anatyp source code
// Author: Will Hovik | willhovik@gmail.com
//

//-Libraries--------------------------------------------------------------------
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_Thermal.h>
#include <PS2Keyboard.h>
#include <SPI.h>
#include <Wire.h>

//-Definitions------------------------------------------------------------------
#define LED_PIN         13

#define SCREEN_WIDTH    128 // OLED width (pixels)
#define SCREEN_HEIGHT   32 // OLED height (pixels)
#define OLED_MOSI       7
#define OLED_CLK        6
#define OLED_DC         5
#define OLED_CS         3
#define OLED_RESET      4

#define CHR_PER_LINE    21
#define HIDDEN_BUF      21
#define VIS_BUF         84
#define WRD_BUF         10  // do something about this
#define REBOOT (_reboot_Teensyduino_());


//-OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
//-Printer
Adafruit_Thermal printer(&Serial1);

//-Keyboard
const int DataPin = 14; // orange wire
const int IRQpin =  15; // green wire
PS2Keyboard keyboard;

//-Variables--------------------------------------------------------------------
char  c;
bool  newline;
char  disp_buffer[HIDDEN_BUF + VIS_BUF + 1];
int   disp_start = HIDDEN_BUF;
int   disp_len = 0;
char  print_buffer[33];
int   print_len = 0;
int   word_len = 0;
char  word_buffer[30]; // do something about this
int   row_len = 0;
int   neg = 0;
int   err = 0;


//-Setup------------------------------------------------------------------------
void setup() {



  keyboard.begin(DataPin, IRQpin);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(50, 14);
  display.print("anatyp");
  display.display();
  delay(3000L);

  Serial1.begin(9600);
  printer.begin(255);
  printer.boldOn();
  printer.setDefault();
  // printer.justify('C');
  // printer.setSize('L');
  // printer.print("ANATYP");
  // printer.feed(3);



}

//-Loop-------------------------------------------------------------------------
void loop() {

  c = keyboard.read();
  // newline = false;

//--Display---------------------------------------------------------------------
  if (c >= 33 && c <= 126) {
    word_buffer[word_len] = c;
    word_len++;
  }



  // space and reset word buffer
  if (c == 32) {
    printer.print(F(word_buffer));
    printer.feed(1);
    for (int i = 0; i <= word_len; i++)
      word_buffer[i] = ' ';
    word_len = 0;

  }




//--Printing--------------------------------------------------------------------



//--Refresh---------------------------------------------------------------------
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(word_buffer);
  display.display();


  // printer.boldOn();
  // printer.justify('C');

}

//-End--------------------------------------------------------------------------
