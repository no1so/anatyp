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
#include <EEPROM.h>

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

//-Battery Wake Up Circuitry
#define WAKE_PIN        23
#define WAKE_ON_TIME    400
#define WAKE_PERIOD     2000
long last_wake = 0;
int wake_state = LOW;


//-OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
//-Printer
Adafruit_Thermal printer(&Serial1, 16);

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
char  word_buffer[WRD_BUF + 1]; // do something about this
int   row_len = 0;
int   neg = 0;
int   err;

int val;

//-Setup------------------------------------------------------------------------
void setup() {
  keyboard.begin(DataPin, IRQpin);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  val = EEPROM.read(0);

  // pinMode(WAKE_PIN, OUTPUT);
  // digitalWrite(WAKE_PIN, LOW);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(50, 14);
  display.print("anatyp");
  display.display();
  delay(1000L);

  Serial1.begin(9600);
  printer.begin(200);
  // printer.setTimes(35000, 2100);
  printer.boldOn();
  printer.justify('C');

}

//-Loop-------------------------------------------------------------------------
void loop() {

  c = keyboard.read();
  newline = false;

//--Display---------------------------------------------------------------------
  if (c >= 33 && c <= 126) {
    word_buffer[word_len] = c;
    word_len++;
    disp_buffer[disp_start + disp_len] = c;
    disp_len++;
    print_buffer[print_len] = c;
    print_len++;
    row_len++;
  }

  // space and reset word buffer
  if (c == 32) {
    disp_buffer[disp_start + disp_len] = c;
    for (int i = 0; i <= word_len; i++)
      word_buffer[i] = ' ';
    word_len = 0;
    disp_len++;
    print_buffer[print_len] = c;
    print_len++;
    row_len++;
  }

  // backspace
  if (c == PS2_DELETE) {
    disp_len--;
    row_len--;
    disp_buffer[disp_start + disp_len] = ' ';
    print_len--;
    //    print_buffer[print_len] = ' ';
    if (word_len > 0)
      word_len--;
    if (row_len < 0 && row_len > -21) {
      neg++;
      row_len = 21 - neg;
    }
    if (row_len < -21)
      row_len = -1;
    if (disp_len < 0)
      disp_len = 0; // keep display length min 0
    if (print_len < 0)
      print_len = 0; // keep print length min 0

  } neg = 0;


  // word wrap
  if (row_len == 22 && word_len < 22 && word_len > 0) {
    for (int i = 0; i <= word_len; i++)
      disp_buffer[disp_start + disp_len - 1 + i] = word_buffer[i];
    for (int i = 0; i < word_len - 1; i++)
      disp_buffer[disp_start + disp_len - word_len + i] = ' ';
    disp_len += word_len - 1;
    row_len = word_len;
  }
  else if (word_len == 22)
    row_len = 1;
  else if (row_len == 22 && word_len == 0)
    row_len = 1;

  // scroll up
  if (disp_start + disp_len > HIDDEN_BUF + VIS_BUF - 5) {
    for (int i = 0; i < HIDDEN_BUF + VIS_BUF - CHR_PER_LINE; i++)
      disp_buffer[i] = disp_buffer[i + CHR_PER_LINE];
    for (int i = HIDDEN_BUF + VIS_BUF - CHR_PER_LINE; i < HIDDEN_BUF + VIS_BUF; i++)
      disp_buffer[i] = ' ';
    disp_start -= CHR_PER_LINE;
  }


//--Printing--------------------------------------------------------------------

  if (c == PS2_ESC){
    printer.feed(1);
  }

  if (c == PS2_ENTER) {
    newline = true;
    for (int i = 0; i < 22 - row_len; i++)
      disp_buffer[disp_start + disp_len + i] = ' ';
    disp_len += 21 - row_len;
    row_len = 0;
    word_len = 0;

    for (int i = 0; i < print_len; i++) {
      printer.print(print_buffer[i]);
    }

    printer.feed(1);
    for (int i = 0; i < print_len; i++)
      print_buffer[i] = ' ';
    print_len = 0;
    // printer.print("THIS IS A TEST");
    // printer.feed(1);
  }

  // print when threshold is met
  if (word_len > 21 - (print_len - word_len)) {
    for (int i = 0; i < (print_len - word_len); i++) {
      printer.print(print_buffer[i]);
    }
    printer.feed(1);
    for (int i = 0; i < word_len; i++)
      print_buffer[i] = word_buffer[i];
    for (int i = 0; i < (print_len - word_len); i++)
      print_buffer[word_len + i] = ' ';
    print_len = word_len;
  }


//--Error-Handling--------------------------------------------------------------

  if (word_len == 20) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Error:\n");
    display.println("Buffer Overload. \nRestarting...");
    display.display();
    delay(3000);
    REBOOT;
  }

  // REBOOT
  if (c == PS2_PAGEUP) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("REBOOT\n");
    display.display();
    delay(1000);
    REBOOT
  }

//--Refresh---------------------------------------------------------------------
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(disp_buffer + HIDDEN_BUF);
  display.display();

  printer.boldOn();
  printer.justify('C');

}

//-End--------------------------------------------------------------------------
