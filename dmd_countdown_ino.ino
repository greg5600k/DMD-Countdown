/*--------------------------------------------------------------------------------------
Header
DMD Countdown
Counts down to a time and 
V1.0 2013 08 01 Greg Ryan
--------------------------------------------------------------------------------------*/


// Includes
#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>        //
#include <TimerOne.h>   //
#include "SystemFont5x7.h"
#include "Arial_black_16.h"
#include "Arial14.h"





// Time library
#include <Time.h>  
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 



//DMD init
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
#define BITS_PER_PIXEL 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN, BITS_PER_PIXEL);


int DisplayFormat = 4;

time_t TargetTime = tmConvert_t(2013,9,7,18,0,0); // September 14


// Setup
void setup(void)
{
  // DMD Init
  Timer1.initialize( 5000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  dmd.clearScreen( false );   

  // Setup for time message on serial port
  Serial.begin(9600);
  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");

}

// Loop Here
void loop(void)
{

  
// Set the clock  
    if(Serial.available() ) 
  {
    processSyncMessage();
  }
  if(timeStatus()!= timeNotSet)   
  {
 //   digitalWrite(13,timeStatus() == timeSet); // on if synced, off if needs refresh  // Dont use, pin 13 used by DMD
  }




// Get the time and how long is left  
time_t CurrentTime = now();
unsigned long TimeLeft = TargetTime - CurrentTime;


if (TargetTime < CurrentTime){
  TimeLeft = 0;
}


// How Many Days Left
int DaysLeft = TimeLeft/86400;
int HoursLeft = (TimeLeft/60/60) % 24;
int MinutesLeft = (TimeLeft/60) % 60;
int SecondsLeft = TimeLeft % 60;

// Create String types of the integers, so we can format them as strings (and convert to char later for DMD)
char DaysChar [3];
itoa(DaysLeft,DaysChar,10); 
String DaysString = String(DaysChar);

char HourChar [3];
itoa(HoursLeft,HourChar,10); 
String HourString = String(HourChar);

char MinuteChar [3];
itoa(MinutesLeft,MinuteChar,10); 
String MinuteString = String(MinuteChar);

char SecondChar [3];
itoa(SecondsLeft,SecondChar,10); 
String SecondString = String(SecondChar);




// Format The time
// Add leading Zeros
if (DaysLeft < 100){  
  DaysString = "0" + DaysString;
}
if (DaysLeft < 10){  
  DaysString = "0" + DaysString;
}

if (HoursLeft < 10){
HourString = "0" + HourString;
}

if (MinutesLeft < 10){
MinuteString = "0" + MinuteString;
}

if (SecondsLeft < 10){
SecondString = "0" + SecondString;
}

// Convert strings to char arrays as required by DMD
int sLength ;
sLength = DaysString.length();
DaysString.toCharArray(DaysChar, sLength+1);

sLength = HourString.length();
HourString.toCharArray(HourChar,sLength +1);

sLength = MinuteString.length();
MinuteString.toCharArray(MinuteChar, sLength+1);

sLength = SecondString.length();
SecondString.toCharArray(SecondChar, sLength);


// Calculate how many dots to display on each line for the given number of seconds left

int DotsDisplayed = SecondsLeft + 1;






/*


Serial.print("Remaining Time: "); 
Serial.println(TimeLeft); 

Serial.print("intDays: "); 
Serial.println(DaysLeft); 

Serial.print("intHours: "); 
Serial.println(HoursLeft); 

Serial.print("intMinutes: "); 
Serial.println(MinutesLeft); 

Serial.print("intSeconds: "); 
Serial.println(SecondsLeft); 


Serial.println("    "); 

Serial.print("char Days: "); 
Serial.println(DaysChar); 

Serial.print("char Hours: "); 
Serial.println(HourChar); 

Serial.print("char Minutes: "); 
Serial.println(MinuteChar); 


Serial.println("    "); 


  
//   Serial.print("Current Time: "); 
//     Serial.println(CurrentTime); 
//   Serial.print("Target Time: "); 
//     Serial.println(TargetTime); 



// Serial.print("DotsRow1: "); 
//     Serial.println(DotsRow1); 



// Serial.print("DotsRow2: "); 
//     Serial.println(DotsRow2); 



// Serial.print("DotsRow3: "); 
//     Serial.println(DotsRow3); 



 Serial.print("         ");
  Serial.print(HourString);
  Serial.print(":");
  Serial.print(MinuteString);
  Serial.print(":");

 
  Serial.print("         ");
  
    Serial.print(SecondString);
  Serial.print("   ");
  
  
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 

*/

// Display Stuff

//----------------------------------------------------------------------------------------------------------------------------------
// Format 1

if (DisplayFormat == 1 ) {



int DotsRow1;
int DotsRow2;
int DotsRow3;  


if (DotsDisplayed == 60 ) {
DotsRow1 = 20;
DotsRow2 = 20;  
DotsRow3 = 20;
}

if (DotsDisplayed >= 0 ) {
DotsRow1 = 0;
DotsRow2 = 0;  
DotsRow3 = (DotsDisplayed % 20);
}

if (DotsDisplayed >= 20 ) {
DotsRow1 = 0;
DotsRow2 = (DotsDisplayed % 20);
DotsRow3 = 20;  
}

if (DotsDisplayed >= 40 ) {
DotsRow1 = (DotsDisplayed % 20);
DotsRow2 = 20;
DotsRow3 = 20;   
}



// Print The Numbers
// Days 
dmd.selectFont( System5x7);
dmd.drawString(1,2,DaysChar,3,1,0); 
// Hours       
dmd.selectFont( System5x7);
dmd.drawString(21,0,HourChar,2,1,0); // Hours
// Minutes
dmd.drawString(21,8,MinuteChar,2,1,0); // Minutes


// 5 Second markers
//dmd.writePixel( 0, 15, 1); // Looks a bit silly...
dmd.writePixel( 4, 15, 1);
dmd.writePixel( 9, 15, 1);
dmd.writePixel( 14, 15, 1);
dmd.writePixel( 19, 15, 1);

// Clear each pixel of the lit block
dmd.writePixel( DotsRow1, 12, 0); // If DotsRow1=20 then the pixel being turned off is to the right of the block
dmd.writePixel( DotsRow2, 13, 0);
dmd.writePixel( DotsRow3, 14, 0);



// When time left = 0 or 60 seconds, all dots are lit
if (DotsDisplayed == 60){
dmd.drawLine( 0, 12, 19, 12, 1);
dmd.drawLine( 0, 13, 19, 13, 1);
dmd.drawLine( 0, 14, 19, 14, 1);
}
else{
}


}
//----------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------------
// Format 2

if (DisplayFormat == 2 ) {




// Print The Numbers
// Days 
dmd.selectFont( Arial_14);
dmd.drawString(0,0,DaysChar,3,1,0); 
// Hours       
dmd.selectFont( System5x7);
dmd.drawString(21,0,HourChar,2,1,0); // Hours
// Minutes
dmd.drawString(21,8,MinuteChar,2,1,0); // Minutes



}
//----------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------------
// Format 2

if (DisplayFormat == 3 ) {




// Print The Numbers
// Days 
dmd.selectFont( Arial_14);
dmd.drawString(0,7,DaysChar,3,1,0); 
// Hours       
dmd.selectFont( System5x7);
dmd.drawString(21,0,HourChar,2,1,0); // Hours
// Minutes
dmd.drawString(21,8,MinuteChar,2,1,0); // Minutes



}
//----------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------------
// Format 1

if (DisplayFormat == 4 ) {



int DotsRow1;
int DotsRow2;
int DotsRow3;  
int DotsRow4; 

if (DotsDisplayed == 60 ) {
DotsRow1 = 15;
DotsRow2 = 15;  
DotsRow3 = 15;
DotsRow4 = 15;
}

if (DotsDisplayed >= 0 ) {
DotsRow1 = 0;
DotsRow2 = 0;  
DotsRow3 = 0;
DotsRow4 = (DotsDisplayed % 15);
}

if (DotsDisplayed >= 15 ) {
DotsRow1 = 0;
DotsRow2 = 15;
DotsRow3 = (DotsDisplayed % 15);  
DotsRow4 = 15;

}

if (DotsDisplayed >= 30 ) {
DotsRow1 = 0;
DotsRow2 = (DotsDisplayed % 15);
DotsRow3 = 15;   
DotsRow4 = 15;

}

if (DotsDisplayed >= 45 ) {
DotsRow1 = (DotsDisplayed % 15);
DotsRow2 = 15;
DotsRow3 = 15;   
DotsRow4 = 15;

}


// Print The Numbers
// Days 
dmd.selectFont( System5x7);
dmd.drawString(1,2,DaysChar,3,1,0); 
// Hours       
dmd.selectFont( System5x7);
dmd.drawString(21,0,HourChar,2,1,0); // Hours
// Minutes
dmd.drawString(21,9,MinuteChar,2,1,0); // Minutes

int Xoffset = 2;
// 5 Second markers
//dmd.writePixel( 0, 15, 1); // Looks a bit silly...
//dmd.writePixel( 4+Xoffset, 15, 1);
///dmd.writePixel( 9+Xoffset, 15, 1);
//dmd.writePixel( 14+Xoffset, 15, 1);


// Clear each pixel of the lit block
dmd.writePixel( DotsRow1+Xoffset, 11, 0); // If DotsRow1=20 then the pixel being turned off is to the right of the block
dmd.writePixel( DotsRow2+Xoffset, 12, 0);
dmd.writePixel( DotsRow3+Xoffset, 13, 0);
dmd.writePixel( DotsRow4+Xoffset, 14, 0);


// When time left = 0 or 60 seconds, all dots are lit
if (DotsDisplayed == 60){
dmd.drawLine( 0+Xoffset, 11, 14+Xoffset, 11, 1);
dmd.drawLine( 0+Xoffset, 12, 14+Xoffset, 12, 1);
dmd.drawLine( 0+Xoffset, 13, 14+Xoffset, 13, 1);
dmd.drawLine( 0+Xoffset, 14, 14+Xoffset, 14, 1);
}
else{
}


}
//----------------------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------------
// Format 5

if (DisplayFormat == 5 ) {




// Print The Numbers
// Days 
dmd.selectFont( System5x7);
dmd.drawString(0,0,DaysChar,3,1,0); 
// Hours       
dmd.selectFont( System5x7);
dmd.drawString(21,0,HourChar,2,1,0); // Hours
// Minutes
dmd.drawString(21,8,MinuteChar,2,1,0); // Minutes
dmd.drawString(0,8,SecondChar,3,1,0); // Minutes


}
//----------------------------------------------------------------------------------------------------------------------------------




// End of script
 delay( 100 );
}




// Functions Below



void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of a header and ten ascii digits
    char c = Serial.read() ; 
    Serial.print(c);  
    if( c == TIME_HEADER ) {       
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){   
        c = Serial.read();          
        if( c >= '0' && c <= '9'){   
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }   
      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
    }  
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}






time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet);         //convert to time_t
}
 








/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}

