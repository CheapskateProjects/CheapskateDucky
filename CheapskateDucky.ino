/*
  CheapskateDucky

  Project inspired by USB Rubber Ducky ( http://usbrubberducky.com )

  This code reads from sd card and then reproduces file content as keyboard events.

  Used Arduino must be 32u4 based to have keyboard interface support.

  !!! To reprogram Arduino you should first remove SD card from module so that Arduino does not start acting as keyboard !!!

  Unless escape sequences are used each line of configured input file (test.txt) is
  used as input that is inserted through key presses. Escape sequences may be used
  to press special keys or key combinations.

  Sequences:
  - To simulate pressing left Crtl, left Alt and t -buttons simultaneously use sequence [128+130+116]
  - To add 1000ms delay use sequence [DEL1000]
  - To actually use [ or \ character as input escape it with backslash. \[

  SD module pinout:
  MOSI - pin 11
  MISO - pin 12
  CLK - pin 13
  CS - pin 4

  created   Nov 2016
  by CheapskateProjects

  ---------------------------
  The MIT License (MIT)

  Copyright (c) 2016 CheapskateProjects

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <SPI.h>
#include <SD.h>
#include <Keyboard.h>

// File handle
File myFile;
// File to read from SD card
String sdFileName = "test.txt";
// How long to wait before "releasing" button combination
int buttonCombiDelay = 50;

void setup()
{
  delay(1000);// Wait for everything to initialize

  // Test SD
  if (!SD.begin(4))
  {
    return;
  }

  myFile = SD.open(sdFileName);
  if (myFile)
  {
    Keyboard.begin();
    bool skipSpecial = false;
    while (myFile.available())
    {
      char nextChar = myFile.read();
      if (skipSpecial)
      {
        Keyboard.print(nextChar);
        skipSpecial = false;
      }
      else if (nextChar == '\\')
      {
        skipSpecial = true;
      }
      else if (nextChar == '[')
      {
        handleSpecialSequence();
      }
      else
      {
        Keyboard.print(nextChar);
      }
    }
    myFile.close();
  }
}

// Special '[' -sequence
void handleSpecialSequence()
{
  String seq = "";
  seq += (char)myFile.read();
  seq += (char)myFile.read();
  seq += (char)myFile.read();

  // Handle delay sequence
  if (seq.equals("DEL"))
  {
    String collect = "";
    while (((char)myFile.peek()) != ']')
    {
      collect += (char)myFile.read();
    }
    myFile.read();// Skip ]
    int del = collect.toInt();

    delay(del);
  }
  // Handle key press sequence
  else
  {
    int charCode = seq.toInt();
    char character = (char)charCode;
    Keyboard.press(character);

    char testChar = myFile.read();
    if (testChar == '+')
    {
      handleSpecialSequence();
    }
    else
    {
      delay(buttonCombiDelay);
      Keyboard.releaseAll();

      return;
    }
  }
}

void loop() {
  // nothing happens after setup
}


