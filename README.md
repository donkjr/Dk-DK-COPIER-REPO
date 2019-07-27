# **DKDK SLIDE COPIER OPERATORS MANUAL**

 **Overview**
 This code controls a modified slide projector used to copy slides to a mobile phones camera.
 An Aarduino NANO is used as the microprocessor.
 The Arduino interfaces with a series of relays and mimics the slides remote control pad.
 Various relays interface with the Nano and subsequently with the remote control socket on the projector.
 One relay interfaces with the capture button on an external BT selfie radio to remotely command the phone to take a picture.
 The Nano also interfaces with an operator control panel which has operator switches and a I2C OLED display.

### **DISPLAY FORMAT**
The OLED display provides information about the state the controller is executing, the slide # currently in process, the status of the operation and instructions to the operator:
  - There are 4 lines of 10 characters each.
  - The first line displays the controllers current MODE
  - The second line displays the current number that of the slide being copied
  - The third line displays status depending on the state of the controller
  - The fourth line displays requested actions based on the state of the controller    

### **BT SELFIE SHUTTER**
The controller communicates with the phone via a MOONI BT connected Selfie Radio (BTSR). The selfie radio is battery operated and provides a button on its face called "shutter release". The shutter release interfaces with the phones camera application to "snap" a picture.
The controller is wired into the BTSR and takes a picture by simulating the press of the shutter release button (SRB).
The BTSR must initially be paired with the phone. From then on the BTSR will connect without pairing
The BTSR will exhibit these characteristics when working properly:
- The led embeded in its face will blink every 5-6 seconds when connected 
- It will shut down after xx seconds of inactivity.
- It will shut down if the phone is disconencted from BT
- It will automatically connect to the phone (if its previously paired) when the SRB is pushed
- If the BTRS gets disconnected, press and hold the SRB for 3 sec to reconnect.

### **SETTING UP THE SYSTEM**
  - With the power plugged in, turn on the projector by rotating the Lamp knob clockwise, the fan will come on.
  - Insure that the 12 VDC plug on the front of the controller in inserted and AC power is applied to power supply
  - Turn on the controllers inline DC power switch and it will initialize:
  			- The OLED will display the name and version of the controllers micorcode
     - The OLED will display the initial screen and enter the WAIT mode
     - The lamp will blink at the end of the controllers initialialization and then stay on 
     - The shutter will connect to the phone just before the lamp blinks
  - Mount and align the phone by:
     - Manually insert the target slide into the projector using the slide arm
     - Insert the phone in its mounting postion
     - Open the phones camera app and verify that the shutter works by pressing the BT shutter release. 
     - Zoom in the camera until the picture nearly fills the viewfinder. Leave a small black outline around the target
     - Take a test picture of the target slide.
  - Insure that the slide trays gear is properly in the track and the arm is fully inserted while flush against the front of the tray
  - Do not put the arm at position 1 but rather just in front of the tray fully into the home position.

### **SYSTEM INTITIALIZATION** 
  - The OLED display will show the name and release
  - The BTSR will initialize and connect to the phone if BT isenabled (LED on BTSR blinks blue & red)
  - The projectors led lamp will blink at then end of the initialization

## INSERTING TRAYS
### **Inserting a Long Tray**
  - Insert the tray with the slide # facing up and toward the back of the projector
  - Postion the tray so the slide arm is fully home in postion in front of the tray. Do not postion adjacent to the 1st slide
  - The slide arm should be in front of the tray not inserted through the tray
  - Push the slide arm forward into the projector without a slide, this is the home postion
  - This position allows the slide arm to retract and then insert the 1st slide into the projector.
  - If the slide arm and tray is not started in the proper position the slide count will be off by one

### **Inserting a Round Tray**
TBD

## **CONTROLLER MODES**

### **Wait Mode:**  
The controller will always be in the wait mode when not in another mode. Exit the Wait mode by pressing the Run button. The controller will always return to the wait mode when the run button is pushed after completing an activity in any other state

### **Auto Mode:**  
#### **Setting the starting slide #**
- Before pressing Run you can set an initial slide # by using the UP/DWM buttons. The UP/DWN buttons will increment/decrement the slide #. The cycle will start at 1 unless another # is entered. 
- Pushing and holding the UP/DWN switch will auto increment the slide #.
- The slide # will be constrained between 1 and the max slide tray size as set by the 36/100 switch.
- If you want to start on a slide that is other than the first slide:
  - Position the tray such that the slide prior to the slide you want to start is line with the slide arm
  - Push the slide arm and associated slide into the home position. 
  - Set the slide # by incrementing decrementing using the UP/DWN switch
  - Press run, the slide copier will cycle through all the remaining slides until the end of tray
- If you want to start at slide #1 position the tray as described above (INSERTING TRAYS) press the run without incrementing the slide #. The default slide setting is #1.

#### **Starting the auto cycle**
- Pressing the Run button while the Auto/Man switch is in the AUTO postion will cause the controller to enter the Auto (capture) mode.
- The Auto mode will cycle through all of a trays slides starting at the initial slide # and continuing to the end of the tray (based on the 36/100 switches postion).
- The Auto mode can be exited to the Wait state by pushing and holding the Run button while in the Auto state. 
- The controller will start with the intial slide # and capture photos until the end of the tray which is either 36 or 100 based on the 36/100 switch. 

### **Manual Mode:**   
- Pressing the Run button while the Auto/Man switch is in the MAN postion will cause the controller to enter the MAN capture mode.
- The contoller will execute one capture cycle for every press of the RUN button.
- in this mode it is the operators responsibility to postion the slide tray as needed to access the desired slide(s).
- Switching the AUTO/MAN sw to AUTO while in the MAN mode will exit the Man mode.

## **CONTROL PANEL DESCRIPTIONS**

_ _Run button:_ _      The run button is used to exit the wait mode, start the Auto cycle. Single cycle in the MAN mode and return to the WAIT state . 

_ _UP/DWN:_ _           The up down switch increases or decreases the slide counter value when in the slide setting mode

_ _AUTO/MAN switch:_ _  Selects either the automatic capture mode (AUTO) or the single cycle mode (MAN)

_ _36/100 switch:_ _    Sets the tray size value. 36 is for linear trays and 100 is for circular trays

_ _Debug SW:_ _  Located at the left back of the controller this slide switch sets the TEST mode. When the debug sw is on pushing the Run PB will cause the controller to enter the Testing state.

_ _Up/Dwn SW_ _ Used to enter the starting slide number in the Auto or Manual mode

_ _Lamp Ovride_ _ Turn on the lamp not matter what state the controller is in. Will turn off the lamp if the controller did not turn it on otherwise only the controller can turn it off. Usefull for setting up the copier in cases where the controller has not turned on the lamp.

## **Sensors** 

Tray Present Switch:    Located on the projectors front surface above the lamp switch this sensor senses that either a round or long tray is placed in the projector. This switch is checked before any state (except debug) is executed

Slide Home Switch:   Located on the projectors front surface to the left of the Forward/Reverse switch this sensor senses when the slide arm is fully inserted (home postion) into the projector. This sensor is used to detect jams by checking this switch after the arm is commanded to insert a slide into the projector. If the required time for the slide arm to insert the slide into the projector has expired and the arm is not detected at home an error (Tray Jam) is posted. 

## **ERRORS**

TRAY MISSING: the controller detected that a slide tray is not installed. Install a tray. 
TRAY JAM: Indicates that something inhibited the slide arm from inserting the slide into the projector within the expected time. Carefully pull the slide arm out of the projector pulling the slide back into the tray. Remove the tray, check the slide in the tray and reload the tray starting at the appropiate position.

## **TROUBLESHOOTING**
If the camera does not take pictures:
- Check that the camera app is open
- Check that the phones BT is enabled and paired with the BTSR
If the controllers slide # and the postion of the tray are out of sync
- Check that you started the slide tray properly. See instructions above
- If you set the starting slide # to other than 1 insure that you are starting the auto cycle with the arm inserted 1 slide sooner than the # you selected.




