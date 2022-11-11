#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
int db=0;

char auth[] = "q38BfdwaJsO8UU7oFuqQKWQrYl_13ZnV";
char ssid[] = "Galaxy A22162E";
char pass[] = "ayancasabianca";



void setup() 
{
  // Initialize serial and wait for port to open:
   Serial.begin(115200);                                    //Serial comms for debugging
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);              //OLED display start
   display.display();                                     //show buffer
   display.clearDisplay();                                //clear buffer
   display.setTextSize(1);                                //Set text size to 1 (1-6)
   display.setTextColor(WHITE);                           //Set text color to WHITE (no choice lol)
   display.setCursor(20,20);                                //cursor to upper left corner
   display.println("Decibel Meter");               //write title
   display.display();                                     //show title
   delay(2000); 
     // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
}

void loop() 
{
    sample = analogRead(A0);  //reading the sensor on A0
  Blynk.virtualWrite(V5,sample); //sending to Blynk

   unsigned long startMillis= millis();                   // Start of sample window
   float peakToPeak = 0;                                  // peak-to-peak level
   unsigned int signalMax = 0;                            //minimum value
   unsigned int signalMin = 1024;                         //maximum value
                                                           // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(A0);                             //get reading from microphone
      if (sample < 1024)                                  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;                           // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;                           // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;                    // max - min = peak-peak amplitude
   db = map(peakToPeak,20,900,49.5,90);             //calibrate for deciBels
   display.setCursor(0,0);                                //cursor to upper left
   display.setTextSize(2);                                //set text size to 2
   display.print(db);                                     //write calibrated deciBels
   display.print(" dB");                                  //write units
 
 
    for(int x =5;x<114;x=x+6){                            //draw scale
      display.drawLine(x, 32, x, 27, WHITE);
    }
   display.drawRoundRect(0, 32, 120, 20, 6, WHITE);       //draw outline of bar graph
   int r = map(db,0,120,1,120);                           //set bar graph for width of screen
   display.fillRoundRect(1, 33, r, 18, 6, WHITE);         //draw bar graph with a width of r
   display.display();                                     //show all that we just wrote & drew
   display.clearDisplay();
   delay(150);
   Blynk.run();
}
