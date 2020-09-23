
#include <SD.h>
#include <SPI.h>
#include <stdlib.h>

File batFile;
float batVar;
char batNum[100];
int  i = 0;

const int chipSelect = BUILTIN_SDCARD;

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect.
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }

  if (SD.exists("bat.txt")){
      SD.remove("bat.txt");
      batFile = SD.open("bat.txt", FILE_WRITE);
  }
  else {
      batFile = SD.open("bat.txt", FILE_WRITE);
  }

  // if the file opened okay, write to it:
  if (batFile) {
    Serial.print("Writing to bat.txt...");
    batFile.println("100.123456");
	// close the file:
    batFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening bat.txt");
  }

  // re-open the file for reading:
  batFile = SD.open("bat.txt");
  if (batFile) {
    // read from the file until there's nothing else in it:
    while (batFile.available()) {

        char c = batFile.read();

        if (c == '\n' || c == '\r')
        {
            i = 0;
            batVar = atof(batNum);
        }
        else
        {
            batNum[i++] = c;
        }
    }
    // close the file:
    batFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    }
    // close the file:
    batFile.close();

  Serial.print("batVar = ");
  Serial.println(batVar,16);
}

void loop()
{
	// nothing happens after setup
}
