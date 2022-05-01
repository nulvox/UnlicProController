#include "LUFAConfig.h"
#include <LUFA.h>
#include "Joystick.h"
#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>

#define MILLIDEBOUNCE 1 //Debounce time in milliseconds
//#define pinOBLED 21  //Onboard LED pin


bool buttonHomeBefore;
bool buttonShiftBefore;
byte buttonStatus[15];

/*
  0x4000,
  0x8000,
#define CAPTURE_MASK_ON 0x2000
#define R3_MASK_ON 0x800
#define L3_MASK_ON 0x400
 */
#define DPAD_UP_MASK_ON 0x00
#define DPAD_UPRIGHT_MASK_ON 0x01
#define DPAD_RIGHT_MASK_ON 0x02
#define DPAD_DOWNRIGHT_MASK_ON 0x03
#define DPAD_DOWN_MASK_ON 0x04
#define DPAD_DOWNLEFT_MASK_ON 0x05
#define DPAD_LEFT_MASK_ON 0x06
#define DPAD_UPLEFT_MASK_ON 0x07
#define DPAD_NOTHING_MASK_ON 0x08
#define A_MASK_ON 0x04
#define B_MASK_ON 0x02
#define X_MASK_ON 0x08
#define Y_MASK_ON 0x01
#define LB_MASK_ON 0x10
#define RB_MASK_ON 0x20
#define ZL_MASK_ON 0x40
#define ZR_MASK_ON 0x80
#define START_MASK_ON 0x200
#define SELECT_MASK_ON 0x100
#define HOME_MASK_ON 0x1000

#define BUTTONUP 9
#define BUTTONDOWN 8
#define BUTTONLEFT 7
#define BUTTONRIGHT 6
#define BUTTONA 3
#define BUTTONB 1 //TX0
#define BUTTONX 18 //A0
#define BUTTONY 15
#define BUTTONLB 19 //A1
#define BUTTONRB 5
#define BUTTONLT 20 // A2
#define BUTTONRT 0 // RX1
#define BUTTONSTART 10
#define BUTTONSELECT 14
#define BUTTONHOME 16
#define BUTTONSHIFT 2
#define BLUELIGHT 4
#define REDLIGHT 21 //A3

Bounce joystickUP = Bounce();
Bounce joystickDOWN = Bounce();
Bounce joystickLEFT = Bounce();
Bounce joystickRIGHT = Bounce();
Bounce buttonA = Bounce();
Bounce buttonB = Bounce();
Bounce buttonX = Bounce();
Bounce buttonY = Bounce();
Bounce buttonLB = Bounce();
Bounce buttonRB = Bounce();
Bounce buttonLT = Bounce();
Bounce buttonRT = Bounce();
Bounce buttonSTART = Bounce();
Bounce buttonSELECT = Bounce();
Bounce buttonHOME = Bounce();
Bounce buttonSHIFT = Bounce();

// Mode select code
typedef enum {
  ANALOG_MODE,
  DIGITAL,
  SMASH
} State_t;
State_t state = DIGITAL;

// Swap input modes with Shift+Home
void checkModeChange(){
   if (buttonStatus[BUTTONHOME] && buttonStatus[BUTTONSHIFT]){
    if(buttonHomeBefore == 0 && buttonShiftBefore == 0){
        switch(state)
        {
           case DIGITAL:
              state=ANALOG_MODE;
              digitalWrite(REDLIGHT, HIGH);
           break;

           case ANALOG_MODE:
              state=SMASH;
              digitalWrite(REDLIGHT, HIGH);
              digitalWrite(BLUELIGHT, HIGH);
           break;

           case SMASH:
              state=DIGITAL;
              digitalWrite(BLUELIGHT, HIGH);
           break;
        }
        buttonHomeBefore = 1;
        buttonShiftBefore = 1;
    }
  }
  else {buttonShiftBefore = 0;buttonHomeBefore = 0;}
}


void setupPins(){
    joystickUP.attach(BUTTONUP,INPUT_PULLUP);
    joystickDOWN.attach(BUTTONDOWN,INPUT_PULLUP);
    joystickLEFT.attach(BUTTONLEFT,INPUT_PULLUP);
    joystickRIGHT.attach(BUTTONRIGHT,INPUT_PULLUP);
    buttonA.attach(BUTTONA,INPUT_PULLUP);
    buttonB.attach(BUTTONB,INPUT_PULLUP);;
    buttonX.attach(BUTTONX,INPUT_PULLUP);
    buttonY.attach(BUTTONY,INPUT_PULLUP);
    buttonLB.attach(BUTTONLB,INPUT_PULLUP);
    buttonRB.attach(BUTTONRB,INPUT_PULLUP);
    buttonLT.attach(BUTTONLT,INPUT_PULLUP);
    buttonRT.attach(BUTTONRT,INPUT_PULLUP);
    buttonSTART.attach(BUTTONSTART,INPUT_PULLUP);
    buttonSELECT.attach(BUTTONSELECT,INPUT_PULLUP);
    buttonHOME.attach(BUTTONHOME,INPUT_PULLUP);
    buttonSHIFT.attach(BUTTONSHIFT,INPUT_PULLUP);

    joystickUP.interval(MILLIDEBOUNCE);
    joystickDOWN.interval(MILLIDEBOUNCE);
    joystickLEFT.interval(MILLIDEBOUNCE);
    joystickRIGHT.interval(MILLIDEBOUNCE);
    buttonA.interval(MILLIDEBOUNCE);
    buttonB.interval(MILLIDEBOUNCE);
    buttonX.interval(MILLIDEBOUNCE);
    buttonY.interval(MILLIDEBOUNCE);
    buttonLB.interval(MILLIDEBOUNCE);
    buttonRB.interval(MILLIDEBOUNCE);
    buttonLT.interval(MILLIDEBOUNCE);
    buttonRT.interval(MILLIDEBOUNCE);
    buttonSTART.interval(MILLIDEBOUNCE);
    buttonSELECT.interval(MILLIDEBOUNCE);
    buttonHOME.interval(MILLIDEBOUNCE);
    buttonSHIFT.interval(MILLIDEBOUNCE);
    
    pinMode(REDLIGHT, OUTPUT);  
    pinMode(BLUELIGHT, OUTPUT);  
    //Set the LED to low to make sure it is off
    digitalWrite(BLUELIGHT, HIGH);
    digitalWrite(REDLIGHT, HIGH);
}

void setup() {
  buttonStartBefore = false;
  buttonSelectBefore = false;
  setupPins();
  SetupHardware();
  GlobalInterruptEnable();
}


void loop() {
    buttonRead();
    checkModeChange();
    processButtons();
    HID_Task();
    // We also need to run the main USB management task.
    USB_USBTask();
}

void buttonRead()
{
  if (joystickUP.update()) {buttonStatus[BUTTONUP] = joystickUP.fell();}
  if (joystickDOWN.update()) {buttonStatus[BUTTONDOWN] = joystickDOWN.fell();}
  if (joystickLEFT.update()) {buttonStatus[BUTTONLEFT] = joystickLEFT.fell();}
  if (joystickRIGHT.update()) {buttonStatus[BUTTONRIGHT] = joystickRIGHT.fell();}
  if (buttonA.update()) {buttonStatus[BUTTONA] = buttonA.fell();}
  if (buttonB.update()) {buttonStatus[BUTTONB] = buttonB.fell();}
  if (buttonX.update()) {buttonStatus[BUTTONX] = buttonX.fell();}
  if (buttonY.update()) {buttonStatus[BUTTONY] = buttonY.fell();}
  if (buttonLB.update()) {buttonStatus[BUTTONLB] = buttonLB.fell();}
  if (buttonRB.update()) {buttonStatus[BUTTONRB] = buttonRB.fell();}
  if (buttonLT.update()) {buttonStatus[BUTTONLT] = buttonLT.fell();}
  if (buttonRT.update()) {buttonStatus[BUTTONRT] = buttonRT.fell();}
  if (buttonSTART.update()) {buttonStatus[BUTTONSTART] = buttonSTART.fell();}
  if (buttonSELECT.update()) {buttonStatus[BUTTONSELECT] = buttonSELECT.fell();}
  if (buttonHOME.update()) {buttonStatus[BUTTONHOME] = buttonHOME.fell();}
}

// // SOCD Dpad with fewer tests
// void processDPAD(){
//     ReportData.LX = 128;
//     ReportData.LY = 128;
//     ReportData.RX = 128;
//     ReportData.RY = 128;
//     if ((!buttonStatus[BUTTONSHIFT])) { // with shift pressed, do SOCD
//       if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.HAT = DPAD_UPRIGHT_MASK_ON;}
//       else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.HAT = DPAD_DOWNRIGHT_MASK_ON;} 
//       else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.HAT = DPAD_DOWNLEFT_MASK_ON;}
//       else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.HAT = DPAD_UPLEFT_MASK_ON;}
//       else if (buttonStatus[BUTTONUP]) {ReportData.HAT = DPAD_UP_MASK_ON;}
//       else if (buttonStatus[BUTTONDOWN]) {ReportData.HAT = DPAD_DOWN_MASK_ON;}
//       else if (buttonStatus[BUTTONLEFT]) {ReportData.HAT = DPAD_LEFT_MASK_ON;}
//       else if (buttonStatus[BUTTONRIGHT]) {ReportData.HAT = DPAD_RIGHT_MASK_ON;}
//       else{ReportData.HAT = DPAD_NOTHING_MASK_ON;}
//     } else { // without shift pressed, just input some regular inputs
//     // left or right will be negated if present.
//     // without left or right, down will become up and up will become nothing
//       if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.HAT = DPAD_UP_MASK_ON;}
//       else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.HAT = DPAD_DOWN_MASK_ON;} 
//       else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.HAT = DPAD_DOWN_MASK_ON;}
//       else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.HAT = DPAD_UP_MASK_ON;}
//       else if (buttonStatus[BUTTONDOWN]) {ReportData.HAT = DPAD_UP_MASK_ON;}
//       else if (buttonStatus[BUTTONLEFT]) {ReportData.HAT = DPAD_LEFT_MASK_ON;}
//       else if (buttonStatus[BUTTONRIGHT]) {ReportData.HAT = DPAD_RIGHT_MASK_ON;}
//       else{ReportData.HAT = DPAD_NOTHING_MASK_ON;}      
//     }
// }

// SOCD Dpad with fewer tests
void processDPAD(){
    ReportData.LX = 128;
    ReportData.LY = 128;
    ReportData.RX = 128;
    ReportData.RY = 128;
    if ((!buttonStatus[BUTTONSHIFT])) { // without shift pressed, just input some regular inputs
      if ((buttonStatus[BUTTONUP]) {
        if (buttonStatus[BUTTONLEFT]) {ReportData.HAT = DPAD_UPLEFT_MASK_ON;}
        else if (buttonStatus[BUTTONRIGHT]) {ReportData.HAT = DPAD_UPRIGHT_MASK_ON;}
        else {ReportData.HAT = DPAD_UP_MASK_ON;}
      } else if (buttonStatus[BUTTONDOWN]){
        if (buttonStatus[BUTTONLEFT]) {ReportData.HAT = DPAD_DOWNLEFT_MASK_ON;}
        else if (buttonStatus[BUTTONRIGHT]) {ReportData.HAT = DPAD_DOWNRIGHT_MASK_ON;}
        else {ReportData.HAT = DPAD_DOWN_MASK_ON;}        
      }
      else if (buttonStatus[BUTTONLEFT]) {ReportData.HAT = DPAD_LEFT_MASK_ON;}
      else if (buttonStatus[BUTTONRIGHT]) {ReportData.HAT = DPAD_RIGHT_MASK_ON;}
      else{ReportData.HAT = DPAD_NOTHING_MASK_ON;}
    } else { // with shift pressed, do SOCD
    // left or right will be negated if present.
    // without left or right, down will become up and up will become nothing
      if ((buttonStatus[BUTTONUP]) {
        if (buttonStatus[BUTTONRIGHT])){ReportData.HAT = DPAD_UP_MASK_ON;}
        else if (buttonStatus[BUTTONLEFT]) {ReportData.HAT = DPAD_UP_MASK_ON;} 
        else{ReportData.HAT = DPAD_NOTHING_MASK_ON;}
      } else if ((buttonStatus[BUTTONDOWN]) {
        if (buttonStatus[BUTTONRIGHT])) {ReportData.HAT = DPAD_DOWN_MASK_ON;}
        else if (buttonStatus[BUTTONLEFT])) {ReportData.HAT = DPAD_DOWN_MASK_ON;}
        else {ReportData.HAT = DPAD_UP_MASK_ON;}
      } else {ReportData.HAT = DPAD_NOTHING_MASK_ON;}      
    }
}

// Gives access to the left or right stick controlled by the shift key
void processLANALOG(){
    ReportData.HAT = DPAD_NOTHING_MASK_ON;
    ReportData.RX = 128;
    ReportData.RY = 128;

    if ((buttonStatus[BUTTONSHIFT])) { //Holding shift key, we use the right stick
      ReportData.RX = 128;ReportData.LY = 128;
      if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.RY = 0;ReportData.LX = 255;}
      else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.RY = 255;ReportData.LX = 255;}
      else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.RY = 255;ReportData.LX = 0;}
      else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.RY = 0;ReportData.LX = 0;}
      else if (buttonStatus[BUTTONUP]) {ReportData.RY = 0;ReportData.LX = 128;}
      else if (buttonStatus[BUTTONDOWN]) {ReportData.RY = 255;ReportData.LX = 128;}
      else if (buttonStatus[BUTTONLEFT]) {ReportData.LX = 0;ReportData.RY = 128;}
      else if (buttonStatus[BUTTONRIGHT]) {ReportData.LX = 255;ReportData.RY = 128;}
      else {ReportData.LX = 128;ReportData.RY = 128;}
    } else { //Not holding Shift key, we use the Left stick
      ReportData.LX = 128;ReportData.RY = 128;
      if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.RY = 0;ReportData.RX = 255;}
      else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.LY = 255;ReportData.RX = 255;}
      else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.LY = 255;ReportData.RX = 0;}
      else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.LY = 0;ReportData.RX = 0;}
      else if (buttonStatus[BUTTONUP]) {ReportData.LY = 0;ReportData.RX = 128;}
      else if (buttonStatus[BUTTONDOWN]) {ReportData.LY = 255;ReportData.RX = 128;}
      else if (buttonStatus[BUTTONLEFT]) {ReportData.RX = 0;ReportData.LY = 128;}
      else if (buttonStatus[BUTTONRIGHT]) {ReportData.RX = 255;ReportData.LY = 128;}
      else {ReportData.RX = 128;ReportData.LY = 128;}
    }
}

// Gives access to only pressing the joystick half way by using the shift key
void processLANALOGSmash(){
    ReportData.HAT = DPAD_NOTHING_MASK_ON;
    ReportData.RX = 128;
    ReportData.RY = 128;
    
    if ((buttonStatus[BUTTONSHIFT])) { // Holding shift, the Lstick is 50% depressed
      if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.LY = 64;ReportData.LX = 192;}
      else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.LY = 192;ReportData.LX = 192;}
      else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.LY = 192;ReportData.LX = 64;}  
      else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.LY = 64;ReportData.LX = 64;}
      else if (buttonStatus[BUTTONUP]) {ReportData.LY = 64;ReportData.LX = 128;}
      else if (buttonStatus[BUTTONDOWN]) {ReportData.LY = 192;ReportData.LX = 128;}
      else if (buttonStatus[BUTTONLEFT]) {ReportData.LX = 64;ReportData.LY = 128;}
      else if (buttonStatus[BUTTONRIGHT]) {ReportData.LX = 192;ReportData.LY = 128;}
      else{ReportData.LX = 128;ReportData.LY = 128;}
    } else { // Without holding shift, we slam the stick all the way down
      if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.LY = 0;ReportData.LX = 255;}
      else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.LY = 255;ReportData.LX = 255;}
      else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.LY = 255;ReportData.LX = 0;}
      else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.LY = 0;ReportData.LX = 0;}
      else if (buttonStatus[BUTTONUP]) {ReportData.LY = 0;ReportData.LX = 128;}
      else if (buttonStatus[BUTTONDOWN]) {ReportData.LY = 255;ReportData.LX = 128;}
      else if (buttonStatus[BUTTONLEFT]) {ReportData.LX = 0;ReportData.LY = 128;}
      else if (buttonStatus[BUTTONRIGHT]) {ReportData.LX = 255;ReportData.LY = 128;}
      else{ReportData.LX = 128;ReportData.LY = 128;}
    }
}

// Good Ol' analog processing. Shift key just feels good to press right now.
// I may add some strange SOCD stuff after reviewing EVO rules
void processRANALOG(){
    ReportData.HAT = 0x08;
    ReportData.LX = 128;
    ReportData.LY = 128;
    
    if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.RY = 0;ReportData.RX = 255;}
    else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.RY = 0;ReportData.RX = 0;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.RY = 255;ReportData.RX = 255;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.RY = 255;ReportData.RX = 0;}
    else if (buttonStatus[BUTTONUP]) {ReportData.RY = 0;ReportData.RX = 128;}
    else if (buttonStatus[BUTTONDOWN]) {ReportData.RY = 255;ReportData.RX = 128;}
    else if (buttonStatus[BUTTONLEFT]) {ReportData.RX = 0;ReportData.RY = 128;}
    else if (buttonStatus[BUTTONRIGHT]) {ReportData.RX = 255;ReportData.RY = 128;}
    else {ReportData.RX = 128;ReportData.RY = 128;}
    
}
void processButtons(){
  //state gets set with checkModeChange
  switch (state)
  {
    case DIGITAL:
        processDPAD();
        buttonProcessing();
    break;

    case ANALOG_MODE:   
       if(buttonStatus[BUTTONLT]){processRANALOG();}
       else{processLANALOG();}
       buttonProcessing();
    break;

    case SMASH:
       buttonProcessingSmash();
    break;
  }
}
void buttonProcessing(){
  if (buttonStatus[BUTTONA]) {ReportData.Button |= A_MASK_ON;}
  if (buttonStatus[BUTTONB]) {ReportData.Button |= B_MASK_ON;}
  if (buttonStatus[BUTTONX]) {ReportData.Button |= X_MASK_ON;}
  if (buttonStatus[BUTTONY]) {ReportData.Button |= Y_MASK_ON;}
  if (buttonStatus[BUTTONLB]) {ReportData.Button |= LB_MASK_ON;}
  if (buttonStatus[BUTTONRB]) {ReportData.Button |= RB_MASK_ON;}
  if (buttonStatus[BUTTONLT]) {ReportData.Button |= ZL_MASK_ON;}
  if (buttonStatus[BUTTONRT]) {ReportData.Button |= ZR_MASK_ON;}
  if (buttonStatus[BUTTONSTART]){ReportData.Button |= START_MASK_ON;}
  if (buttonStatus[BUTTONSELECT]){ReportData.Button |= SELECT_MASK_ON;}
  if (buttonStatus[BUTTONHOME]){ReportData.Button |= HOME_MASK_ON;}
}
void buttonProcessingSmash(){
  if (buttonStatus[BUTTONA]) {ReportData.Button |= X_MASK_ON;}
  if (buttonStatus[BUTTONB]) {}
  if (buttonStatus[BUTTONX]) {ReportData.Button |= A_MASK_ON;}
  if (buttonStatus[BUTTONY]) {ReportData.Button |= B_MASK_ON;}
  if (buttonStatus[BUTTONLB]) {ReportData.Button |= LB_MASK_ON;}
  if (buttonStatus[BUTTONRB]) {ReportData.Button |= ZR_MASK_ON;}
  if (buttonStatus[BUTTONLT]) {ReportData.Button |= ZL_MASK_ON;}
  if (buttonStatus[BUTTONRT]) {ReportData.Button |= RB_MASK_ON;}
  if (buttonStatus[BUTTONSTART]){ReportData.Button |= START_MASK_ON;}
  if (buttonStatus[BUTTONSELECT]){ReportData.Button |= SELECT_MASK_ON;}
  if (buttonStatus[BUTTONHOME]){ReportData.Button |= HOME_MASK_ON;}
}
