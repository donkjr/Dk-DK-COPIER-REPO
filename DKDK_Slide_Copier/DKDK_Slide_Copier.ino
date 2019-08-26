/**************************************************************************
 DKDK Slide Copier Controller

 This code controls a modified slide projector used to copy slides to a phones camera.
 An arduino NANO is used as the uprocessor
 The arduino interfaces with a series of relays and repolaces the slides remote control pad.
 Various relays interface with the remote control socket on the projector.
 On relay interfaces with the capture button on the BT selfie control to command the phone to take a picture
 The nano also interfaces with a control planel and I2C OLED display. 

 
 The display portion of this code was modified from an Adafruit example for their Monochrome OLEDs based on SSD1306 drivers
 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Source Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.

Version Control
6/19  V0.0 uncompiled version [donkjr]
6/19  V1.0 compiled version
7/3   V1.1 Added tray present sensor
7/6   V1.2 Reformated display messages. Cleared compliler bug by updating GFX library. Rearrained I/O for easier wireing.
7/7/  V1.3 Wiring complete testing started. 
7/9   Changed declarations from String to char* to solve intermittent display problem.
      Use the Serial.println(F(" ")); macro to conserve on ram which is needed for display. 
7/10  Implemented Auto/manual Capture mode detection in main look
      Tested tray size switch and associated cyle
      Changed display message order 
      Tweaked the initialization messaging format for the serial port
      Added structure for Capture() function so there is used by both Auto & Manual capture process
7/12  Completed the capturePhoto() function and pointed all states to that common function
      The serial port now tracks the full cycle with messages.
      Completed testing all relays 
      Added LED lamp control 
      Added single cycle function
      Tested tray present sensor
      Added tray missing error state
      Developed Test mode
        -Added display test 
        -Added read input test
7/14  Completed input ports test
      Added relay test 
 Known bugs-----
  Fixed  7/14:  Test state: When the tray type switch is turned off the console displays "OFF" to the left        

----------RELEASE V2.0 includes all changes from 6/19-7/14 ............................
 Missing Functions----
  7/14 BT Shutter: the remote shutter connects to the BT the first time the shutter button is activated.
                   Thereafter it acts as a remote shutter. 
                   The shutter may have to be activated once during STARTUP to initialize (connect to BT)
                   Add a shutter relay pulse to startup
                   What to do about shut down? 
 7/16 SLIDEHOME    Added a slide home sensor to the defines and the startup.
 7/19 SLIDEHOME    Removed the debug sw test*. Replace with armhome sw test.
                   * the debug switch must be aserted to enter the test mode, 
                   * therefore is is unnecassary to test it in the test mode
                   Added slideArmHome sensor and supporting code
                   Jams are now detected.

----- RELEASE V2.1 includes all the above. All changes from 6/19- 7/19
                   :) tested 5 slide auto cycles 2 times.                   

  7/20 Testing:
                  Changed long slide setting to 36 and tested full trays.
                  Changed the lamp to be on all the time so that seting up camera is easier with a slide in the projector
                  For the slide count to be accurate the first slide must be started with the tray in position 1 and the arm pulled fully out. 
  7/23 Testing: 
                  Replace turning on lamp at start of auto as comming from debog cam leave it off.
                  Changed the circular slide setting to 100
                  Tested round 100 slide tray
  7/24 BT Shutter
                  The Mooni shutter must be first paired with the phone (push and hold the sutter release button for 3 seconds
                  To connect to the Mooni (once paired) the shutter release must be pushed for 3 sec (approx)
                  Once connected to the Mooni the phones camera app must be opened 
                  Subsequent assertions of the shutter release will take pictures with the app
                  The app takes about 3 seconds to take the picture.
                  The code was modified to automate the above setup and activation                  
  7/25 RLS 2.2    
                  Changes are now trackable. Built code in GIT Repository Dk-DK-REPO
                  Fixed Bugs.
                    Turn everything off when entering the test mode
                    Contamination in the display just after the version #. Missing clear.Display()
                    If the tray is not sensed you cannot enter the test state. This is proper operation for all other states.
                    You should be able to enter the Test state irrespective of sensor conditions. 
                    After all the test state tests the sensors!      
 7/25 Playing with N++
					        Editing in N++
					        Again editing
 7/27 V2.3 Branch
                Added set starting slide # code 
 Known Bugs-----
  Open   7/14:    
                  The shutter activates as the relay turns off therefore the shutter wait time can be shortened
                  If the controller is on and the shutter is already connected when it initializes it takes a picture.
                  Turn off the BT by aserting a long shutter release, or leave this alone? 
                  
 **************************************************************************/

// ----------------------- INCLUDED LIBRARIES -------------------
#include <Wire.h>             //For I2C serial communications
#include <Adafruit_GFX.h>     // For the display graphics
#include <Adafruit_SSD1306.h> // For the display controller

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define VERSION 2.3          // version # for displaying

// ----- Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)-------
#define OLED_RESET     -1  // Reset pin # (or -1 if sharing Arduino reset pin). [Was set to 4 which is being used by tray sise sw]
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// The display SDA is on A4
// The display SCL is on A5

// ---------------------DEFINED CONSTANTS -------------------------
// The 128x64 pixel display can display 10 x 5 pixel characters that are 2x the normal height.
// There are 4 lines of 2x size characters


// Character and line Display Constants
//2X character dimensions
#define CHARWIDTH2X   12    // pixel width of a 2x char
#define CHARHEIGHT2X  16    // pixel height of a 2x char
#define LINELENGTH2X  10    // # of char in a line
//Constants that define the row pixel postion where lines start on the display
#define LINE1START    0     
#define LINE2START    LINE1START + CHARHEIGHT2X
#define LINE3START    LINE2START + CHARHEIGHT2X
#define LINE4START    LINE3START + CHARHEIGHT2X
//Column constants
#define UPDATECOLSTART 72 //column number position for update area

//Define the Output Pins; assign a hardware pin # to a constant
#define LEDLAMPRELAY 6  // turns on the led lamp
#define SLIDEADVANCERELAY 7  // energized to activate a slide cycle which
                              // moves the tray extracts a slide, 
                              // returns the slide to the tray, 
                              // advances the tray and then pushes in a new slide.
#define SHUTTERRELAY 8       // activates the capture button on the BT selfie controller

//Define INPUT Pins; assign a hardware pin # to a constant
#define RUNPB 9               // a Pushbutton that starts the copy process.
#define CNTUPMOMSW 11      // momentary switch that counts slide starting count up
#define CNTDWNMOMSW 10     // momentary switch that counts slide starting count down
#define TRAYPRESENTSW A0   // indicates that a slide tray is mounted
#define AUTOMODESW 12     // mode switch. When ON the controller copies all the sides in a tray and then stops
                              // When mode switch is off the controller is in single cycle mode and copies one slide then stops.
                              // When mode is not AUTO the RUNPB initates one copy cycle per push.
#define TRAYTYPESW 4          // Tray type switch. ON = 100 slide circle, OFF = 36 slide linear. This is so we know how many slides in each tray
#define TRAYPRESENTSW A0      // Optical sensor that indicates a slide tray is present
#define SLIDEARMHOMESW   A1    // Optical sensor that indicates a slide arm if fully inserted in the projector unit
#define DEBUGSW 5             // Jumper to put the controller in debug mode.

//Label state machine states, assign a constant name to a state

#define WAIT  0         // stop state
#define AUTOCYCLE  1    // slide exchange state
#define MANCYCLE 2
#define INSERTTRAY 3    // Prompt to insert tray
#define SETSLIDENO 4       // Prompt to remove replace tray
#define TEST 5         // Test mode
 // Other constants
#define LONGTRAYSIZE 36     // the long tray size = 36
#define ROUNDTRAYSIZE 100   // the round tray = 100


// DISPLAY STRING CONSTANTS; predefine strings to use as display messages
// These are string pointers (char*) that point to the start of strings
char* titleMsg ="DK^2 SlideCopier";// missing space is on purpose for formatting
char* versionMsg = "V2.3";
char* readyMsg ="RDY!";

//Status line messages
char* insertTrayMsg = "Insrt Tray"; 
char* addTrayMsg = "Add Tray";
char* captureMsg = "Capturing";
char* insertingMsg ="Inserting";
char* cyclingMsg = "Cycling";
char* pushRunMsg = "Push Run";
char* doneMsg = "Tray Done";
char* trayJamMsg ="Tray Jam";
char* runstopsMsg = "Run 2 Stop";
char* blanksMsg = "          "; // blank space
char* toStartMsg = "To start";
char* testingMsg = "To Test";
char* readInputMsg ="Readinputs";
char* onMsg = "ON";
char* offMsg ="OFF";
char* testRelayMsg = "Relay:";
char* lampRelayMsg = "Lamp";
char* shutterRelayMsg = "Shutter";
char* cycleRelayMsg = "Cycle";
char* setSlideNoMsg = "Chg slide#";

//Mode field messages
char* runModeMsg = "RUN";
char* stopModeMsg = "STOP";
char* waitModeMsg = "Wait";
char* autoCycleModeMsg = "Auto";
char* manCycleModeMSG = "Man";
char* testModeMsg = "TEST";
char* errorModeMsg = "EROR";

// these are string pointer variables used to dynamically update the display
char* modeDisplay; 			// writing string pointer to here updates Mode area of display
char* statusDisplayLine1; 	//writing string pointer to here updates STATUS area of display
char* statusDisplayLine2; 	// writing string pointer to here updates ERROR area of display

// pointers used by the test state to display 
char* displayLine1;
char* displayLine2;
char* displayLine3;
char* displayLine4;

//DEFINE VARIABLES; create variables for various control elements

//slide counting 
int slideCounter =0;
int slideTraySize = 0;

// delays for slide insertion cycle
int slideArmCyclePulse = 1000;	// time the cycle relay is ON
int slideArmReturnDelay = 1400;
int slideSettleDelay = 1000;

//delays for BT shutter 
int shutterPulse = 1000;      	// time the shutter release needs to be on to take a picture
int slideUploadDelay =1000;   	// time for the camera to be ready to take another photo
int shutterConnect = 3000; 		// time the sutter release is pushed to connect to phone 

//Misc variables
int vertCharSize = 0;
int horCharSize = 0;
int state = 0;
int debounceDelay1 = 500 ; // delay for switch demounce in (ms)
int trayJam = true; // indicates the tray is jamed

// ------------------------- STARTUP ----------------------
// THIS SECTION RUNS 1 TIME AT RESET or POR
void setup() 
{ 
  Serial.begin(57600);
  //Turn these outputs off
  digitalWrite(SLIDEADVANCERELAY,HIGH);
  digitalWrite(SHUTTERRELAY,HIGH);
  digitalWrite (LEDLAMPRELAY, HIGH); // turn off LED lamp
  Serial.println (versionMsg); // opening message
  Serial.println(F(".......Initializing Controller......."));
  //Turn off the BT shutter??
  //Serial.println(startMsg);
   Serial.println(F("Initializing OLED..."));
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
    { // Address 0x3D for 128x64, the default on UCTRONICS display is 0x3C
      Serial.println(F("SSD1306 allocation failed")); 
      // Load strings into NonVol using serial.println(F("  ")) to enable enough dynamic memory for display
      for(;;); // Don't proceed, loop forever
    }
  delay (2000);     // let the display initialize
  //SET UP I/O--------------
  //OUTPUTS
  pinMode(LEDLAMPRELAY,OUTPUT);
  pinMode(SLIDEADVANCERELAY,OUTPUT);
  pinMode(SHUTTERRELAY,OUTPUT);
  //INPUTS
  pinMode(RUNPB, INPUT_PULLUP);
  pinMode(CNTUPMOMSW, INPUT_PULLUP);
  pinMode(CNTDWNMOMSW, INPUT_PULLUP);
  pinMode(TRAYPRESENTSW, INPUT_PULLUP);
  pinMode(TRAYTYPESW, INPUT_PULLUP);
  pinMode(SLIDEARMHOMESW, INPUT_PULLUP);
  pinMode(AUTOMODESW, INPUT_PULLUP);
  pinMode(DEBUGSW, INPUT_PULLUP);
  //DISPLAY OPENING MESSAGES
  display.clearDisplay(); // stops the adafruit logo from comming up
  Serial.println (F("OLED initialized succesfully")); 
  write2xCharString(0,0,titleMsg);//write title on top of display
  write2xCharString(0,33,versionMsg);
  display.display(); // write it to display
  // end of intial messages
  delay (4000);
  
  //Show the display buffer on the screen. You MUST call display() after
  Serial.println(F("Formatting OLED Display Area"));
  //Set up the main OLED display format
  display.clearDisplay();             // Cleared bug 
  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(F("MODE: ")); 
  display.setCursor(0,16);
  display.print(F("Slide#: ")); 
  display.setCursor(0,32);
  display.display();  //write it to the screen
  
  int state = WAIT; // initialize state 
  // Initialize the shutter
  // Once paired the BT shutter can be activated by pushing and holding the shutter release 
  // The led on the Mooni should blink
  
  Serial.println(F("Initializing Mooni BT Shutter"));
  digitalWrite(SHUTTERRELAY,LOW);   // activate the shutter release to connect to phone
  delay(shutterConnect);            // wait for the shutter to connect to the phone
  digitalWrite(SHUTTERRELAY, HIGH); // release the shutter, phone should be connected
  
  Serial.println(F(".......Initialization Complete ......."));
  digitalWrite(LEDLAMPRELAY, LOW); //Turn on the LED lamp
  delay (1000); //blink the led lamp;
  digitalWrite(LEDLAMPRELAY, HIGH); //Turn off the LED lamp
  delay (1000); //blink the led lamp
  digitalWrite(LEDLAMPRELAY, LOW); //Turn on the LED lamp
  
}


// ---------------- Start MAINLOOP ----------------
void loop() 
{//Slide copier control
  //if !TRAYINSERTEDSW state = INSERTTRAY
  
  Serial.println("Loop Start...");
  
  //Check and set the slide tray size base on 36/100 switch
  //Serial.println(digitalRead(TRAYTYPESW));
  
  if(!digitalRead(TRAYTYPESW))
    {
      slideTraySize = LONGTRAYSIZE;
    }
    else if (digitalRead(TRAYTYPESW))
    {
      slideTraySize = ROUNDTRAYSIZE; 
    }
  Serial.print(F("Tray Size = "));
  Serial.println(slideTraySize);
  Serial.print(F("Tray Present Sensor = "));
  Serial.println(digitalRead(TRAYPRESENTSW));

// ----------------------- STATE MACHINE---------------------------------
  switch(state)
    {
    // WAIT STATE: resting and launching state for all other states
    case WAIT:
             Serial.print(F("State= "));
             Serial.println(waitModeMsg);
             modeDisplay = waitModeMsg; // display the WAIT mode
             statusDisplayLine1 = toStartMsg;
             statusDisplayLine2 = pushRunMsg; //display the "Push run" Message
             updateDisplay();
             Serial.println(pushRunMsg);
             while (digitalRead(RUNPB))
              { //stay here waiting to exit while the run button is not pushed 
                //Serial.println(F("Waiting for run butn"));
				  if (!digitalRead(AUTOMODESW))
					{ //if in automode set that state
					  //Serial.println(F("Automode Set"));
					  state = AUTOCYCLE; // if the Auto/Manual switch is in Auto go to automatic cycle
					}
				  else if (digitalRead(AUTOMODESW))  
				  //else if (AUTOMODESW)// WRONG!
					{//if not in autmode check for manual mode and set that state
					 state = MANCYCLE; // if the Auto/manual switch is Manual set the manual state
					}
              }  //Exit here when run button pushed 
              delay(debounceDelay1); // wait for switch to stop bouncing
              if(digitalRead(TRAYPRESENTSW))
				{//if tray is not inserted go to tray error state
					state = INSERTTRAY;
				}
              if(!digitalRead (DEBUGSW))
				{//if the debug switch is on go to test state
				state=TEST;
				}  
              
              Serial.print(F("Going to mode: "));
              Serial.println(state); 
    break;
    
    // AUTOCYCLE STATE: cycles the slide in an out of the tray from begining to end of the tray
    // Holding the RUN PB exits this state
    case AUTOCYCLE:
       // in this state an entire slide tray is copied to the destination of the phone 
        Serial.print(F("STATE = "));
        Serial.println(autoCycleModeMsg);
        modeDisplay = autoCycleModeMsg; 
        statusDisplayLine1 = pushRunMsg;
        statusDisplayLine2 = toStartMsg; // display Run Message
        updateDisplay(); // updates the above values to the display
        trayJam = false; // reset tray jams
        slideCounter = 1; // initialize the slide counter
		while (digitalRead(RUNPB))
        {
        //Wait for Run Button pushed to start capturing photos
		//Meanwhile allow the starting slide # to be changed
        setSlideNo(); 
		}
        delay(debounceDelay1); //wait for RUN switch to stop bounceing       
        digitalWrite(LEDLAMPRELAY, LOW); //Turn on the LED lamp        
        Serial.println(F("LED lamp ON"));
        Serial.println(captureMsg);
        statusDisplayLine1 = captureMsg; //
        statusDisplayLine2 = runstopsMsg; // display Run = Stop Message
        updateDisplay();
        for (int i=slideCounter; i<=slideTraySize && trayJam == false; i++)
         {// cycle through all the slides without stopping except for a jam
            slideCounter = i; // 
            updateDisplay();
            Serial.print(F("Slide# = "));
            Serial.println (slideCounter); 
            if (capturePhoto())
              { // run the exchange slide and capture cycle 
                // if there is a jam capturePhoto() will return a 1, then set jam
                Serial.println(F("Tray JAM.."));
                trayJam = true;
                break;
              }
            if (!digitalRead(RUNPB))
              {
                //if run button held get out of here
                break; //   
              }
          }//end of cycle
          // cycle one more time without taking photo to eject the tray
          digitalWrite(SLIDEADVANCERELAY,LOW); // turn the slide arms relay on
          delay (slideArmCyclePulse);//wait the slide motors pulse time
          digitalWrite(SLIDEADVANCERELAY,HIGH);// turn the slide arms relay off, we only need a pulse
          // now we are done
          Serial.println(doneMsg);
          Serial.println(pushRunMsg);
          //slideCounter = 0; //reset the slide counter
          statusDisplayLine1 = doneMsg; //display Done message
          statusDisplayLine2 = pushRunMsg; // display push Run button message 
          if(trayJam)
          {
            statusDisplayLine1 = trayJamMsg;
          }
          updateDisplay(); 
          //digitalWrite(LEDLAMPRELAY, HIGH); // Turn OFF the LED lamp        
          //Serial.println(F("LED lamp OFF"));
          while (digitalRead(RUNPB)== HIGH)
            {
            //wait for run button to be pushed to exit to wait state
            }
          delay(debounceDelay1); // wait for switch to stop bouncing
          // clean up and leave
          state = WAIT; // go to wait state    
          slideCounter = 0; //reset the slide counter
          
    break;
    
    //MANCYCLE STATE: cycles the slide in and out of the tray once for each press of the RUN PB until the end of tray.
    case MANCYCLE:

        Serial.print(F("STATE = "));
        Serial.println(manCycleModeMSG);
        //.....................
        modeDisplay = manCycleModeMSG; 
        statusDisplayLine1 = pushRunMsg;
        statusDisplayLine2 = toStartMsg; // display Run Message
        updateDisplay();
        while (digitalRead(RUNPB))
        {
        //Wait for Run Button pushed to capturing photo
        }
        delay(debounceDelay1); //wait for RUN switch to stop bounceing       
        digitalWrite(LEDLAMPRELAY, LOW); //Turn on the LED lamp        
        Serial.println(F("LED lamp ON"));
        Serial.println(captureMsg);
        statusDisplayLine1 = captureMsg; //
        statusDisplayLine2 = runstopsMsg; // display Run = Stop Message
        updateDisplay();
        // ................
        while(digitalRead(AUTOMODESW))
        {// if the mode is manual (automode is off) keep enabling single cycling
            statusDisplayLine1 = pushRunMsg;
            statusDisplayLine2 = toStartMsg; // display Run Message
            updateDisplay();
            Serial.print(F("Slide# = "));
            Serial.println (slideCounter); 
            capturePhoto(); // run the exchange slide and capture cycle
            slideCounter = slideCounter+1; //inc the slide counter 
            while (digitalRead(RUNPB)== HIGH)
                {
                //wait for run button to be pushed to cycle again
                }
            delay(debounceDelay1); // wait for switch to stop bouncing
        }  
        // the Auto/Man sw was changed to auto, end single cycle mode
        state = WAIT;            
        Serial.println(doneMsg);
        Serial.println(pushRunMsg);
        statusDisplayLine2 = pushRunMsg; // display push Run button message
        statusDisplayLine1 = doneMsg; //display Done message
        updateDisplay();                 
        //digitalWrite(LEDLAMPRELAY, HIGH); // Turn OFF the LED lamp [removed as bug]       
        Serial.println(F("LED lamp OFF"));              
        while (digitalRead(RUNPB)== HIGH)
          {
          //wait for run button to be pushed to exit to wait state
          }
        delay(debounceDelay1); // wait for switch to stop bouncing
        
    break;
    // Insert the tray error processing state
    case INSERTTRAY: 
         Serial.println(F("ERROR: Insert a Tray"));
         modeDisplay = errorModeMsg;//display error mode 
         statusDisplayLine1 = insertTrayMsg;
         statusDisplayLine2 = pushRunMsg;
         updateDisplay();
         while (digitalRead(RUNPB)== HIGH)
              {
              //wait for run button to be pushed to exit to wait state
              }
            delay(debounceDelay1); // wait for switch to stop bouncing
         state = WAIT;
        break;
        //NOT YET IMPLEMENTED
        case SETSLIDENO:
           // display "Remove/relace tray"
          // not implemented
          state = WAIT;
    break;
    // Test state runs simple tests on the hardware
    case TEST:
        Serial.println(F("TEST MODE"));
        modeDisplay = testModeMsg; // display the TEST mode
        statusDisplayLine1 = testingMsg; // display testing
        statusDisplayLine2 = pushRunMsg; //display the "Push run" Messag
        Serial.print(statusDisplayLine1);
        Serial.print("\t");
        Serial.println(statusDisplayLine2);
        updateDisplay();
        //Serial.println(F("Push RUN to continue"));
        while (digitalRead(RUNPB))
          {
           // wait for run pb to continue       
          }
        // initialize for testing
        //BUG FIX: Turn these outputs off Bug 
        digitalWrite(SLIDEADVANCERELAY,HIGH);
        digitalWrite(SHUTTERRELAY,HIGH);
        digitalWrite (LEDLAMPRELAY, HIGH); // turn off LED lamp
        Serial.println(F("Testing OLED Display ...."));
        testdrawline();
        testdrawchar(); 
        modeDisplay = testModeMsg; // display the TEST mode
        statusDisplayLine1 = readInputMsg; // inputs testing
        statusDisplayLine2 = runstopsMsg; //display the "Run 2 stop" Message
        updateDisplay();
        Serial.println(F("OLED Testing Complete"));
        // ----------- Input port tests -------------------       
        Serial.println(F("Read 4 Main Input Ports ..."));
        Serial.println(F("Logic: 0 = ON, 1 = OFF"));
        Serial.println(F("Note: The Run button & Diag switch works if you got here!"));
        while (digitalRead(RUNPB))
          {// read and display the state of each port. Probably a more efficient way!     
          Serial.println(F("    TrayType   Auto  TrayIn   Arm Home"));  // uneven tabbing needs this format    
                         
          //read & display the state of the TRAYTYPESW 
          if(!digitalRead(TRAYTYPESW))
            {
            Serial.print("\t");        
            Serial.print(onMsg);    // display ON on the console
            Serial.print("\t");
            displayLine1 = onMsg;   // display ON on the OLED
            }
          else
          {
            Serial.print("\t");
            Serial.print(offMsg);   //display OFF on the console
            Serial.print("\t");
            displayLine1 = offMsg;  //display OFF on the OLED
          }
          //read & display the state of AUTOMODESW 
          if(!digitalRead(AUTOMODESW))
          {
            Serial.print(onMsg);
            Serial.print("\t");
            displayLine2 = onMsg;
          }
          else
          {
            Serial.print(offMsg);
            Serial.print("\t");
            displayLine2 = offMsg;
          }
          //read & display the state of TRAYPRESENTSW 
         if(!digitalRead(TRAYPRESENTSW))
          {
            Serial.print(onMsg);
            Serial.print("\t");
            displayLine3 = onMsg;
          }
          else
          {
            Serial.print(offMsg);
            Serial.print("\t");
            displayLine3 = offMsg;
          }
          //read & display the state of Arm Home Switch
          if(!digitalRead(SLIDEARMHOMESW))
          {
            Serial.print(onMsg);
            Serial.println("\t");
            displayLine4 = onMsg;
          }
          else
          {
            Serial.print(offMsg);
            Serial.println("\t");
            displayLine4 = offMsg;
          }
           displayInputPorts(); // display input ports on the OLED
        }//Run PB was pushed
        delay(debounceDelay1); // wait for switch to stop bouncing
        // ............. RELAYS TEST .................
        Serial.println(F("Test Relays ....."));
        //Serial.println(F("Test Lamp Relay"));
        display.clearDisplay();
        display.setTextSize(2);             // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print(F("Relay Test")); 
        //
        relayTest(LEDLAMPRELAY,"LAMP");       //test the lamp relay
        relayTest(SHUTTERRELAY,"SHUTTER");    //test the shutter relay
        relayTest(SLIDEADVANCERELAY, "CYCLE");//test the cycle relay 
        state = WAIT;
    break;
    
    default :
    // IF you got here there is an error
    Serial.println(F("Fatal state machine error"));
    break;
  }//------------------------- End state machine
} //END MAIN LOOP

//---------------------------------------- FUNCTIONS -------------------------------

/* setSlideNo
Allows the operator to set the starting slide # before starting Auto Capture 
Parameters:
Input: none
Return: the starting slide #
*/

int setSlideNo()
	{

	if (!digitalRead(CNTUPMOMSW))
		{
		++slideCounter;
		delay(debounceDelay1);
		}
	else if(!digitalRead(CNTDWNMOMSW))
		{
		--slideCounter;
		delay(debounceDelay1);
		}

	// Keep the slidecount within limits
	if (slideCounter > slideTraySize)
		{
		slideCounter = slideTraySize;	
		}
	if (slideCounter < 1)
		{
		slideCounter = 1;	
		}
	
	// update the display values mainly the slide counter
	modeDisplay = autoCycleModeMsg; 
	statusDisplayLine1 = pushRunMsg;
	statusDisplayLine2 = toStartMsg; // display Run Message
	updateDisplay(); // updates the above values to the display
	return(slideCounter);
	}

/* Relay testing
 * Function prompts to turn on and then prompts to turn off a particular relay
 * Input parameters: 
 * char* relayMsg: passed name of the relay name
 * int portNum: passed assigned I/O port number for the relay
 */

void relayTest(int portNum, char* relayMsg)
{
        //set up display
        display.clearDisplay();
        display.setTextSize(2);             // Draw 2X-scale text
        display.setTextColor(WHITE);
        // write title on the top line
        display.setCursor(0,0);
        display.print(F("Relay Test")); 
        Serial.print (F("Testing: "));
        Serial.println (relayMsg); 
        // write the relay name to the second line
        display.setCursor(0,16);
        display.print(relayMsg);
        //Serial.println(relayMsg);
        // push run message on the 4th line
        display.setCursor(0,48);
        display.print(pushRunMsg);
        Serial.println(pushRunMsg);
        //display the above
        display.display();         
        delay(debounceDelay1); // wait for switch to stop bouncing
        while (digitalRead(RUNPB))
          {
           // wait here for ok to turn relay on                         
          }
        delay(debounceDelay1); // wait for switch to stop bouncing
        //turn relay ON as pointed to by portNum
        digitalWrite(portNum,LOW);    
        //display
        display.clearDisplay();
        // write title on the top line
        display.setCursor(0,0);
        display.print(F("Relay Test")); 
        // write the relay name to the second line
        display.setCursor(0,16);
        display.print(relayMsg);
        // push run message on the 4th line
        display.setCursor(0,48);
        display.print(pushRunMsg);
        //display relay on msg in 3rd line
        display.setCursor(0,32);
        display.print(F("Relay ON"));
        Serial.println(F("Relay ON"));
        display.display();
        while (digitalRead(RUNPB))
          {// wait for run to turn relay off
                                    
          }                       
        delay(debounceDelay1); // wait for switch to stop bouncing
        // turn relay off
        digitalWrite(portNum,HIGH); 
        //rewrite display state
        display.clearDisplay();
        // write title on the top line
        display.setCursor(0,0);
        display.print(F("Relay Test")); 
        // write the relay name to the second line
        display.setCursor(0,16);
        display.print(relayMsg);
        // push run message on the 4th line
        display.setCursor(0,48);
        display.print(pushRunMsg);
        //display relay off in 3rd line
        display.setCursor(0,32);
        display.print(F("          ")); // clear line by writing spaces
        display.setCursor(0,32);
        display.print(F("Relay OFF"));
        Serial.println(F("Relay OFF"));
        display.display(); 
        while (digitalRead(RUNPB))
          {// wait for run to exit to testing
                                    
          } 
        delay(debounceDelay1); // wait for switch to stop bouncing
}



/* Wait for RUN pushbutton
 *  Common routine to wait for Run button to be pushed.
 *  Input parameters: none
 *  Return parameters :none
*/

void waitForRun()
{
  statusDisplayLine1 = pushRunMsg; //display the "Push run" Message
  updateDisplay();
  while (digitalRead(RUNPB))
          {
          Serial.println(F("Push RUN to continue"));
          }
  delay(debounceDelay1); // wait for switch to stop bouncing
}


/* Capture photo
The  capture function is used by both AUTO and MANUAL capture modes to process the capture of ONE photo
This fuction performs the process needed to exchange a slide and capture the picture on the phone
Input Parameters :none
Return parameters: returns 1 if there was an jam

*/

int capturePhoto()
{
   
     // capture one slide and exit
     // the slide exchange mechanism consist of the 
     // slide tray
     // slide arm
     // slide arm motor
     // The slide arms resting postion is inside the projector interposing the optical path with a slide in the arm 
     // In the rest postion the motor is disconnected from its supply. 
     // A pulse on the remotes "CYCLE" switch causes the motor to start which
     // connects the motor to its supply for the remainder of the cycle.
     // A full projector cycle consits of:
        // slide arm stoped with slide arm and slide fully insered in the machine
          // note: when the slide is at rest the arm can be retracted and a tray and slide loaded by pulling/pushing the arm
        // motor is pulsed and its movement closes the arm motors contacts
        // the slide arm extends out of the projector clearing the tray, putting the slide back in the tray
        // the slide arm cams the tray forward to the next slide position
        // the slide arm is pulled into the projector and stops when the slide is in the optical path
        // the slide arms motor contacts are opened
        // the slide arm is at rest waiting for a cycle pulse from the remote
     //The photo is captured using a mobile phones camera Bluetooth connected to a "selfie" module (manufacture is mooni).
        // The shutter relay closes the contact on the button in the selfie module (hacked)
        // After the shutter cycle the function waits a delay to allow the phone to recycle for another picture.
     // This function creates the above cycle
        
        // ...... CYCLE THE SLIDE ARM.....
        
        // pulse the slide arm motors relay
        digitalWrite(SLIDEADVANCERELAY,LOW); // turn the slide arms relay on
        //Serial.println(F("Start Slide Arm Cycle"));
        delay (slideArmCyclePulse);//wait the slide motors pulse time
        digitalWrite(SLIDEADVANCERELAY,HIGH);// turn the slide arms relay off, we only need a pulse
        //Serial.println(F("Pulse OFF"));
        Serial.println(F("Slide Arm cycling"));
        delay (slideArmReturnDelay); //wait until the slide arm extends and then retracts
        // check for a jam
        //added jam detection
        if(digitalRead(SLIDEARMHOMESW))
        {// if the sensor is not interupted (low) there must have been a jam so return with error
          
          Serial.println(F("Slide Jam ...."));
          return(1);// 1 means jam was detected
        }
    
        Serial.println(F("Slide at rest"));
        
        // ...... ENERGIZE THE SHUTTER RELAY TO TAKE A PHOTO .....
        Serial.println(F("Snapping a photo"));
        digitalWrite(SHUTTERRELAY,LOW); // turn the shutter relay on
        //Serial.println(F("Shutter on"));
        delay (shutterPulse);//we just need a pulse to take the photo
        digitalWrite(SHUTTERRELAY,HIGH);// turn the shutter relay off, we only need a pulse      
        Serial.println(F("Photo Snapped, uploading ...."));
        delay (slideUploadDelay); //wait until the photo has time to upload
        Serial.println(F("Photo uploaded")); 
       
}
// DISPLAY FUNCTIONS
//----------------------- Update OLED display -----
// Updates the OLED display by writing:
//Mode
//Slide # 
//Display line 1 & 2
//From the associated variables
//Parameters: none

  void updateDisplay()
  {
    display.clearDisplay();
    display.setTextSize(2);             // Draw 2X-scale text
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(F("Mode: ")); 
    display.setCursor(0,16);
    display.print(F("Slide: ")); 
    display.setCursor(0,32);
    //display.display();
    //display the mode
    display.setCursor(UPDATECOLSTART,LINE1START);
    display.print (modeDisplay);  // assign integer to a variable
    //display the slide counter 
    display.setCursor(72,16);
    display.print (slideCounter);
    //display the fist status line
    display.setCursor(0, 32);
    display.print (statusDisplayLine1);
    //display the second status line
    display.setCursor(0,48);
    display.print (statusDisplayLine2);
    display.display(); 
  }

  
// Display the values of the inputs

void displayInputPorts()
{
 
    display.clearDisplay();
    display.setTextSize(2);             // Draw 2X-scale text
    display.setTextColor(WHITE);
    //display the input ports titles
    display.setCursor(0,0);
    display.print(F("36/100:")); 
    display.setCursor(0,16);
    display.print(F("Auto:")); 
    display.setCursor(0,32);
    display.print(F("Tray In:"));
    display.setCursor(0,48);
    display.print(F("Home Sw:"));
    //display the values of the input ports
    display.setCursor(92,0);
    display.print(displayLine1);
    display.setCursor(92,16);
    display.print(displayLine2); 
    display.setCursor(92,32);
    display.print(displayLine3);
    display.setCursor(92,48);   
    display.print(displayLine4);
    display.display(); 
}

// Print the display values to the console
void printDisplayValues()
{
  Serial.println(F("Display variables ....."));
  Serial.print(F("Mode ="));
  Serial.println(modeDisplay);
  Serial.print(F("CNTR ="));
  Serial.println(slideCounter);
  Serial.print(F("Line1 ="));
  Serial.println (statusDisplayLine1);
  Serial.print(F("Line2 ="));
  Serial.println(statusDisplayLine2);
}

// ---------- WRITE A 2X CHARACTER TO THE DISPLAY
//parameters: 
//  stingcontent: a char string to be written
//  cursorX, cursorY: the starting cursor postion in pixels
void write2xCharString(int cursorX,int cursorY, String stringContent)
{
  
  //10-2x characters fit horizontally
  // a line is 16 pixels high and characters are 12 pixels wide  
  //Serial.print(cursorX);
  //Serial.print(",");
  //Serial.println(cursorY);
 
 // Display on the oled 
  display.setTextColor(WHITE);        // Draw white text
  display.setTextSize(2);             // Draw 2X-scale text
  display.setCursor(cursorX,cursorY);// cursor postion is in pixels
  display.println(stringContent); 
  display.display();
}
// ---------- SCROLL TEXT RIGHT
//parameters: 
//  text: a char string to be written
//  cursorX, cursorY: the starting cursor postion in pixels


void scrolltextright(int cursorX,int cursorY, String text) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE,WHITE);
  display.setCursor(cursorX,cursorY);
  display.println(text);
  display.display();      // Show initial text
  delay(100);
  // Scroll right
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  //delay(2000);
  
}

void fillUpdateArea(void) 
{
     //display.clearDisplay();
    // The INVERSE color is used so rectangles alternate white/black
    Serial.println("Filling");
    display.fillRect(72,0,56,16, BLACK); // fillRect(xstart, ystart, width, height)
    display.display(); // Update screen with each newly-drawn rectangle
    //delay(2000);
}



// TEST FUNCTIONS

void testdrawline() {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for(i=0; i<display.width(); i+=4) 
  {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display.height(); i+=4) 
  {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.width(); i+=4) 
  {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) 
  {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=display.width()-1; i>=0; i-=4) 
  {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) 
  {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.height(); i+=4) 
  {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
    delay(1);
  }
  for(i=0; i<display.width(); i+=4) 
  {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE);
    display.display();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}

void testdrawrect(void) 
{
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) 
  {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testfillrect(void) 
{
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=3) 
  {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testdrawcircle(void) 
{
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) 
  {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillcircle(void) 
{
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) 
  {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}

void testdrawroundrect(void) 
{
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) 
  {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillroundrect(void) 
{
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) 
  {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawtriangle(void) 
{
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) 
  {
    display.drawTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfilltriangle(void) 
{
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) 
  {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawchar(void) 
{
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) 
  {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
}

void testdrawstyles(void) 
{
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.print(F("0x")); 
  display.println(0xDEADBEEF, HEX);
  display.display();
  delay(2000);
}
// scrolltext
void scrolltextright(String text) 
{
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0,32);
  display.println(text);
  display.display();      // Show initial text
  delay(100);
  // Scroll right
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  
}


//-------
void testscrolltext(void) 
{
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  //display.println(F("JEDU V1.0"));
  display.println(F("Start.."));
  display.display();      // Show initial text
  delay(100);
  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

/*
void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2
*/

/*
void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) 
{
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) { // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }

    display.display(); // Show the display buffer on the screen
    delay(200);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
*/
