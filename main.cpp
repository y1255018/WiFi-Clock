#include <WiFi.h>
#include <ESP32Time.h>
#include <SimpleButton.h>
#include "SevSeg.h"


#define MODE_MAX 2

// pin assign
#define RA0 34
#define RA1 35
#define RA2 32
#define RA3 33
#define RB0 25
#define RB1 26
#define RB2 27
#define RB3 14
#define RB4 12
#define RB5 15
#define RB6 2
#define RB7 4
#define RC0 16
#define RC1 17
#define RC2 5
#define RC3 18
#define RC4 19
#define RC5 21
#define RC6 22
#define RC7 23

using namespace simplebutton;
const char ssid[] = "hoge";
const char passwd[] = "moge";
int modeNum = 0;
bool cnt_state = false;
bool stopwatch_clear = false;

SevSeg sevseg; //Instantiate a seven segment controller object

// tact switch
Button *modeSW = new Button(RA0, true);
Button *upSW = new Button(RA1, true);
Button *downSW = new Button(RA2, true);
Button *adjSW = new Button(RA3, true);

void setup()
{
  // set pin mode
  //pinMode(1, OUTPUT);  // NC
  //pinMode(2, OUTPUT);  // VDD
  //pinMode(3, OUTPUT);  // GND
  //pinMode(4, OUTPUT);  // VSS
  //pinMode(5, OUTPUT);  // NC
  pinMode(RA0, INPUT);  // MODE_SW
  pinMode(RA1, INPUT);  // UP_SW
  pinMode(RA2, INPUT);  // DOWN_SW
  pinMode(RA3, INPUT);  // ADJ_SW
  pinMode(RB0, OUTPUT); // 4
  pinMode(RB1, OUTPUT); // DP
  pinMode(RB2, OUTPUT); // LED 9
  //pinMode(RB3, OUTPUT); // BZ
  //pinMode(RB4, OUTPUT); // BZ
  pinMode(RB5, OUTPUT); // 3
  pinMode(RB6, OUTPUT); // 2
  pinMode(RB7, OUTPUT); // 1
  pinMode(RC0, OUTPUT); // F
  pinMode(RC1, OUTPUT); // G
  pinMode(RC2, OUTPUT); // D
  pinMode(RC3, OUTPUT); // E
  pinMode(RC4, OUTPUT); // C
  pinMode(RC5, OUTPUT); // A
  pinMode(RC6, OUTPUT); // B
  pinMode(RC7, OUTPUT); // LED 1,2
  //pinMode(26, OUTPUT); // NC
  //pinMode(27, OUTPUT); // NC
  //pinMode(28, OUTPUT); // NC


         
  byte numDigits = 4;
  byte digitPins[] = {RB7, RB6, RB5, RB0};
  byte segmentPins[] = {RC5, RC6, RC4, RC2, RC3, RC0, RC1, RB1};
  bool resistorsOnSegments = true;     // 'false' means resistors are on digit pins
  byte hardwareConfig = N_TRANSISTORS; // See README.md for options
  bool updateWithDelays = false;       // Default 'false' is Recommended
  bool leadingZeros = false;           // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false;        // Use 'true' if your decimal point doesn't exist or isn't connected

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  //sevseg.setBrightness(10);

  Serial.begin(115200);

  log_i("connecting to %s ...", ssid);
  WiFi.begin(ssid, passwd);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    log_e("WiFi connection failed:(");
    while (1)
      delay(1000);
  }

  ESP32Time.begin(); //< adjusted the time
}

void wifi_clock()
{
  time_t t = time(NULL);
  struct tm *t_st;
  t_st = localtime(&t);
  int disp_time = t_st->tm_hour * 100 + t_st->tm_min; // display hour:min

  sevseg.setNumber(disp_time, 4);
}

void stop_watch()
{

  static unsigned long timer = millis();
  static int deciSeconds = 0;

  if (millis() - timer >= 100)
  {
    timer += 100;
    if (cnt_state)
      deciSeconds++; // 100 milliSeconds is equal to 1 deciSecond

    if (deciSeconds == 10000 || stopwatch_clear == true)
    { // Reset to 0 after counting for 1000 seconds.
      deciSeconds = 0;
      stopwatch_clear = false;
    }

    sevseg.setNumber(deciSeconds, 1);
  }
}

void loop()
{
  modeSW->update();
  adjSW->update();

  if (modeSW->clicked())
  {
    //Serial.println("modeSW clicked");
    modeNum++;
    if (modeNum >= MODE_MAX)  modeNum = 0;
  }

  // button state
  if (adjSW->clicked())
  {
    //Serial.println("adjSW clicked");
    if (cnt_state == false)       cnt_state = true;
    else                          cnt_state = false;
  }
  if (adjSW->holding())
  {
    //Serial.println("adjSW holding");
    cnt_state = false;
    stopwatch_clear = true;
  }

  // event
  switch (modeNum)
  {
  case 0:
    wifi_clock();
    break;
  case 1:
    stop_watch();
    break;
  default:
    sevseg.setNumber(0000, 4);
    break;
  }

  sevseg.refreshDisplay(); // Must run repeatedly
}