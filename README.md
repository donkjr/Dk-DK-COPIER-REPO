# **DKDK Slide Copier Controller**

 Overview
 This code controls a modified slide projector used to copy slides to a phones camera.
 An arduino NANO is used as the microprocessor
 The arduino interfaces with a series of relays and mimics the slides remote control pad.
 Various relays interface with the remote control socket on the projector.
 One relay interfaces with the capture button on the BT selfie control to command the phone to take a picture
 The nano also interfaces with a control planel which has switches and a I2C OLED display.

## **OPERATORS MANUAL**

### **DISPLAY FORMAT**
The OLED display provides information about the state the controller is executing, the slide # currently in process, the status of the operation and instructions to the operator
  - There are 4 lines of 10 characters each.
  - The first line displays the controllerscurrent MODE
  - The second line displays the current Slide Counters value
  - The third line displays status depending on the state of the controller
  - The forth line displays requested actions based on the state of the controller    

### **Setting up the slide copier system**
  - Turn on the projector by rotating the Lamp knob clockwise, the fan will come on.
  - Turn on the controller, the lamp will blink as the controller initializes then stay on 
  - The shutter will connect to the phone just before the lamp blinks
  - Mount and align the phone by manually inserting the target slide into the projector
  - Open the phones camera app and verify that the shutter works by pressing the shutter release
  - Note: the shutter will shut down after xx seconds of inactivity. 
  - To reconnect the shutter press and hold the shutter button for 3 sec to reconnect.
  - The shutter will blink every 5-6 seconds while connected
  - Insure that the slide tray is properly in the gear track and the arm is fully inserted flush against the front of the tray
  - Do not put the arm at position 1 but rather just in front of the tray

### **Initialization** 
  - The OLED display will show the name and release
  - The BT shutter will initialize and connect to the phone if BT enabled (LED on shutte blinks blue & red)
  - The projectors led lamp will blink at then end of the initialization

### **Inserting a Long Tray**
  - Insert the tray with the slide # facing up and toward the back of the projector
  - Postion the tray so the slide arm is in postion in front of the tray not adjacent to the 1st slide
  - The slide arm should be in front of the tray not inserted into the tray
  - Push the slide arm forward into the projector without a slide, this is the home postion
  - This position allows the slide arm to retract and then insert the 1st slide into the projector. 

## **Inserting a Round Tray**

### **Wait Mode:**  
The controller will be in the wait mode when not in another mode. Exit the Wait mode by pressing the Run button. The controller will always return to the wait mode when the run button is pushed after completing activity in any other state

### **Auto Mode:**  
- Before pressing Run you can set a slide # by using the UP/DWM buttons. The default will start at 1 unless another # is entered.
- Pressing the Run button while the Auto/Man sw is in the AUTO postion will cause the controller to enter the Auto capture mode.
- The Auto mode will cycle through an entire tray (based on Postion of 36/100 sw) without stopping.
- The Auto mode can be exited to the Wait state by pushing and holding the Run button while in the Auto state. 
- The controller will start with slide 0 and capture photos until the end of the tray which is either 36 or 100 based on the 36/100 switch. 

**Manual Mode:**   
- Pressing the Run button while the Auto/Man sw is in the MAN postion will cause the controller to enter the MAN capture mode.
- The contoller will execute one capture cycle for every press of the RUN button.
- Switching the AUTO/MAN sw to AUTO while in the MAN mode will exit the Man mode.

**CONTROL PANEL**

_ _Run button:_ _      The run button is used to exit the wait mode, start the Auto cycle. Single cycle in the MAN mode and return to the WAIT state . 

_ _UP/DWN:_ _           The up down switch increases or decreases the slide counter value when in the slide setting mode

_ _AUTO/MAN switch:_ _  Selects either the automatic capture mode (AUTO) or the single cycle mode (MAN)

_ _36/100 switch:_ _    Sets the tray size value. 36 is for linear trays and 100 is for circular trays

_ _Debug SW:_ _  Located at the left back of the controller this slide switch sets the TEST mode. When the debug sw is on pushing the Run PB will cause the controller to enter the Testing state.

_ _Up/Dwn SW_ _ Used to enter the starting slide number in the Auto or Manual mode

_ _Lamp Ovride_ _ Turn on the lamp not matter what state the controller is in. Will turn off the lamp if the controller did not turn it on otherwise only the controller can turn it off. Usefull for setting up the copier in cases where the controller has not turned on the lamp.

**Sensors:** 

Tray Present Switch:    Located on the projectors front surface above the lamp switch this sensor senses that either a round or long tray is placed in the projector. This switch is checked before any state (except debug) is executed

Slide Home Switch:   Located on the projectors front surface to the left of the Forward/Reverse switch this sensor senses when the slide arm is fully inserted (home postion) into the projector. This sensor is used to detect jams by checking this switch after the arm is commanded to insert a slide into the projector. If the required time for the slide arm to insert the slide into the projector has expired and the arm is not detected at home an error (Tray Jam) is posted. 

**ERRORS:**

TRAY MISSING: the controller detected that a slide tray is not installed 
TRAY JAM: Indicates that something inhibited the slide arm from inserting the slide into the projector within the expected time.

