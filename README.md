# Dk-DK-COPIER-REPO

**DKDK Slide Copier Controller**

 Overview
 This code controls a modified slide projector used to copy slides to a phones camera.
 An arduino NANO is used as the microprocessor
 The arduino interfaces with a series of relays and mimics the slides remote control pad.
 Various relays interface with the remote control socket on the projector.
 One relay interfaces with the capture button on the BT selfie control to command the phone to take a picture
 The nano also interfaces with a control planel which has switches and a I2C OLED display.

**OPERATORS MANUAL**

**DISPLAY**
  *Display format for 128 x 64 pixel display.
  *There are 4 lines of 10 characters each.
  *The first line displays the controllers MODE
  *The second line displays the Slide Counter value
  *The third line displays status depending on the state of the controller
  *The forth line displays requested actions based on the state of the controller    

**Setting up the slide copier system**
  - Turn on the projector by rotating the Lamp knob clockwise, the fan will come on.
  - Turn on the controller, the lamp will blink as the controller initializes then stay on 
  - The shutter will connect to the phone just before the lamp blinks
  - Mount and align the phone by manually inserting the target slide into the projector
  - Open the phones camera app and verify that the shutter works by pressing the shutter release
  - Note: the shutter will shut down after xx seconds of inactivity. 
  - To reconnect the shutter press and hold the shutter button for 3 sec to reconnect.
  - The shutter will blink every 5-6 seconds while connected
  - Insure that the slide tray is properly in the gear track and the arm is fully in flush against the front of the tray
  - Do not put the arm at position 1 but rather just in front of the tray
**Initialization** 
            The OLED display will show the name and release
            The BT shutter will initialize (LED on shutte blinks blue & red)
            The projectors led lamp will blink at then end of initialization
Wait Mode:  the controller will be in the wait mode when not in another mode. Exit the Wait mode by pressing the Run buttion,
            the controler will return to the wait mode when the run button is pushed after completing any other state
Auto Mode:  pressing the Run button while the Auto/Man sw it in the AUTO postion will enter the Auto capture mode.
            The Auto mode will cycle through an entire tray (based on [ostion of 36/100 sw) without stopping.
            The Auto mode can be exited by pushing the Run button while in the Run state will cause the controller to exit to the WAIT state 
            The controller will start with slide 0 and capture photos until the end of the tray which is either 36 or 100. 
MAN Mode:   Pressing the Run button while the Auto/Man sw is in the MAN postion will enter the MAN capture mode.
            The contoller will execute one capture cycle for every press of the RUN button.
            Switching the AUTO/MAN sw to AUTO while in the MAN mode will exit the Man mode.

CONTROL PANEL
Run button:       The run button is used to exit the wait mode, start the Auto cycle. Single cycle in the MAN mode and return to the WAIT state . 
UP/DWN:           The up down switch increases or decreases the slide counter value when in the slide setting mode
AUTO/MAN switch:  Select the automatic capture mode (AUTO) or the single cycle mode (MAN)
36/100 switch:    Sets the tray size value. 36 is for linear trays and 100 is for circular trays

Debug SW: sets the TEST mode. When the debug sw is on pushing the Run PB will cause the controller to enter the Testing state.

Sensors: 
TRAYPRESENTSW:    Indicates that a round or long tray is placed in the projector.
                  This switch is checked before any state (except debug) is executed
SLIDEARMHOMESW:   Indicates that the slide arm is fully inserted into the projector. 
                  This sensor is used to detect jams by checking this switch after the required time for the slide arm to cycle

ERRORS:
TRAY MISSING: the controller detected that a slide tray is not installed 
TRAY JAM: Indicates that something inhibited the slide arm from inserting the slide into the projector within the expected time. 
