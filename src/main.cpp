// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// North1               motor         1               
// South2               motor         2               
// West3L               motor         3               
// East4R               motor         4               
// LaserL               distance      10              
// LaserR               distance      11              
// BorderDetector       line          B               
// StopButton           bumper        C               
// ---- END VEXCODE CONFIGURED DEVICES ----
#include<vex.h>
#include<cmath>
#include<iostream>

using namespace vex;
// ---- Initialization ---- //
void spinMotors();
void updateVelocity(int n, bool useIn);
void eBrake();
void unbrake();
float LaserAvg();
void checkLine();
void checkObject();
bool laserInRange(int range);
void updateConsole();
void degMove(int dir, float in, float deg, int v);
void screenColor(color c);
void updateDirection(int r);
void laserDistanceOut();
void checkButton();
void checkContrArrows();
int controllerThread();

int Brain_precision = 0, Console_precision = 0, Controller1_precision = 0, range, count = 1, chargeDT;
float northV, southV, eastV, westV, stickRotate, stickForward, stickSideways, autoTurnSpeed, autoFollowSpeed, autoFocus, charge, distLMem, distRMem, distAvgMem;
bool init = true, useController, autonomous = false, objLeft, braking, atLine = false, nearObject = false, seeObject, brakeMem, autoMem, lineMem, detectMem, objMem, leftMem, inRange;

// ---- FUNCTIONS ---- //
void spinMotors() {
  // Spin wheels to preset velocity
  North1.spin(forward);
  South2.spin(forward);
  West3L.spin(forward);
  East4R.spin(forward);
}
void updateVelocity(int n, bool useIn) {
  //Calculate correct wheel speeds
  northV = (stickForward + stickSideways) + stickRotate;
  eastV = (stickForward - stickSideways) - stickRotate;
  westV = (stickForward - stickSideways) + stickRotate;
  southV = (stickForward + stickSideways) - stickRotate;
  //Set velocity
  if (!useIn) { //!useIn = use controller input
    North1.setVelocity(northV, percent);
    South2.setVelocity(southV, percent);
    West3L.setVelocity(westV, percent);
    East4R.setVelocity(eastV, percent);

  }
  else if (useIn && n != 0) {
    North1.setVelocity(n, percent);
    South2.setVelocity(n, percent);
    West3L.setVelocity(n, percent);
    East4R.setVelocity(n, percent);
  }
  else if (useIn && n == 0) {
    North1.stop();
    South2.stop();
    West3L.stop();
    East4R.stop();
  }
}
void eBrake() { //
  updateVelocity(0, true); //Halt
  North1.setStopping(hold);
  South2.setStopping(hold);
  West3L.setStopping(hold);
  East4R.setStopping(hold);
}
void unbrake() {
  North1.setStopping(coast);
  South2.setStopping(coast);
  West3L.setStopping(coast);
  East4R.setStopping(coast);
}
float laserAvg() {
  //Avg both laser inputs
  return (LaserL.objectDistance(mm) + LaserR.objectDistance(mm)) / 2;
}
void checkLine() {
  if (BorderDetector.value(percent) <= 10) {
    atLine = true;
  }
  else {
    atLine = false;
  }
}
void checkObject() {
  if (laserAvg() > 1 && laserAvg() < 150) {
    nearObject = true;
  }
  else {
    nearObject = false;
  }
}
bool laserInRange(int range) {
  if ((LaserL.objectDistance(mm) <= range && LaserL.objectDistance(mm) > 1) || (LaserR.objectDistance(mm) <= range && LaserR.objectDistance(mm) > 1)) {
    return true;
  }
  else {
    return false;
  }
}
void updateConsole() {
  if (init || braking != brakeMem || autonomous != autoMem || atLine != lineMem || laserInRange(range) != detectMem || nearObject != objMem || objLeft != leftMem) {
    std::cout << "OUTPUT" <<std::endl;
    Brain.Screen.clearScreen(); //Clear Text
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.setFillColor(black);
    //Braking
    if (braking) { 
      Brain.Screen.print("Braking: True\n");
    }
    else {
      Brain.Screen.print("Braking: False\n");
    }
    Brain.Screen.newLine(); //Return
    //Autonomous
    if (autonomous) {
      Brain.Screen.print("Autonomous: True\n");
    }
    else {
      Brain.Screen.print("Autonomous: False\n");
    }
    Brain.Screen.newLine(); //Return
    //At Line
    if (atLine) {
      Brain.Screen.print("At Line: True\n");
    }
    else {
      Brain.Screen.print("At Line: False\n");
    }
    Brain.Screen.newLine(); //Return
    //Object detected
    if (laserInRange(range)) {
      Brain.Screen.print("Detects Object: True\n");
    }
    else {
      Brain.Screen.print("Detects Object: False\n");
    }
    Brain.Screen.newLine(); //Return
    //Close to object
    if (nearObject) {
      Brain.Screen.print("Near Object: True\n");
    }
    else {
      Brain.Screen.print("Near Object: False\n");
    }
    Brain.Screen.newLine(); //Return
    //Object is Left
    if (objLeft) {
      Brain.Screen.print("Object is Left\n");
    }
    else {
      Brain.Screen.print("Object is Right\n");
    }

    init = false;
    brakeMem = braking;
    autoMem = autonomous;
    lineMem = atLine;
    detectMem = laserInRange(range);
    objMem = nearObject;
    leftMem = objLeft;
  }
}
void degMove(int dir, float in, float deg, int v) {
  updateVelocity(v, true);
  switch(dir) {
    case 1:
      float wheelDeg = in / 0.05;
      North1.spinFor(wheelDeg, degrees, false);
      South2.spinFor(wheelDeg, degrees, false);
      West3L.spinFor(wheelDeg, degrees, false);
      East4R.spinFor(wheelDeg, degrees, true);
      break;
    case 2:
      float wheelDeg = in / -0.05;
      North1.spinFor(wheelDeg, degrees, false);
      South2.spinFor(wheelDeg, degrees, false);
      West3L.spinFor(wheelDeg, degrees, false);
      East4R.spinFor(wheelDeg, degrees, true);
      break;
    case 3:
      float wheelDeg = in / 0.05;
      North1.spinFor(wheelDeg * -1, degrees, false);
      South2.spinFor(wheelDeg * -1, degrees, false);
      West3L.spinFor(wheelDeg, degrees, false);
      East4R.spinFor(wheelDeg, degrees, true);
      break;
    case 4;
      float wheelDeg = in / 0.05;
      North1.spinFor(wheelDeg, degrees, false);
      South2.spinFor(wheelDeg, degrees, false);
      West3L.spinFor(wheelDeg * -1, degrees, false);
      East4R.spinFor(wheelDeg * -1, degrees, true);
      break;
    case 5:
      float wheelDeg = deg / 0.279;
      North1.spinFor(wheelDeg, degrees, false);
      South2.spinFor(wheelDeg * -1, degrees, false);
      East4R.spinFor(wheelDeg * -1, degrees, false);
      West3L.spinFor(wheelDeg, degrees, true);
      break;
    case 6:
      float wheelDeg = deg / 0.279;
      North1.spinFor(wheelDeg * -1, degrees, false);
      South2.spinFor(wheelDeg, degrees, false);
      East4R.spinFor(wheelDeg, degrees, false);
      West3L.spinFor(wheelDeg * -1, degrees, true);
      break;
  }
}
void screenColor(color c) {
  Brain.Screen.setFillColor(c);
  Brain.Screen.drawRectangle(270, 0, 210, 272);
  updateConsole();
}
void updateDirection(int r) {
  if (LaserL.objectDistance(mm) <= r && (LaserR.objectDistance(mm) > LaserL.objectDistance(mm) + 20)) { //Only LaserL sees object
    //objLeft = true;
    objLeft = false;
    stickRotate = autoFocus * -1; //Rotate left
  }
  else if (LaserR.objectDistance(mm) <= r && (LaserL.objectDistance(mm) > LaserR.objectDistance(mm) + 20)) { //Only LaserR sees object
    //objLeft = false;
    objLeft = true;
    stickRotate = autoFocus; //Rotate right
  }
  else {
    stickRotate = 0;
  }
  updateVelocity(1, false);
  spinMotors();
}
void laserDistanceOut() {
  if (LaserL.objectDistance(mm) != distLMem) {
    Controller1.Screen.clearLine(1);
    Controller1_precision = 1;
    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print("LaserL: ");
    Controller1.Screen.print(LaserL.objectDistance(mm));
    
    distLMem = LaserL.objectDistance(mm);
  }
  if (LaserR.objectDistance(mm) != distRMem) {
    Controller1.Screen.clearLine(2);
    Controller1_precision = 1;
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print("LaserR: ");
    Controller1.Screen.print(LaserR.objectDistance(mm));
    
    distRMem = LaserR.objectDistance(mm);
  }
  if (laserAvg() != distAvgMem) {
    Controller1.Screen.clearLine(3);
    Controller1_precision = 1;
    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print("Laser Avg: ");
    Controller1.Screen.print(laserAvg());

    distAvgMem = laserAvg();
  }
}
void checkButton() {
  if (StopButton.pressing()) { //Emergency stop button
      braking = true;
      autonomous = false;
      //Controller1.rumble(rumbleShort);
  }
}
void checkContrArrows() {
  if (Controller1.ButtonUp.pressing()) {
      degMove(1, 10, 0, 50);
  }
  if (Controller1.ButtonDown.pressing()) {
      degMove(2, 10, 0, 50);
  }
  if (Controller1.ButtonLeft.pressing()) {
      degMove(3, 10, 0, 50);
  }
  if (Controller1.ButtonRight.pressing()) {
      degMove(4, 10, 0, 50);
  }
}
int controllerThread() {
  while (true) {
    laserDistanceOut();
    this_thread::sleep_for( 25 );
  }
  return 0;
}
// ===========================================================================================================

// ---- MAIN DRIVE CONTROL ---- //
int omniControl() {
  //Autonomous values
  autoFollowSpeed = 25.0; //Object track/approach speed
  autoTurnSpeed = 20.0;//-//Looking for object spin speed
  autoFocus = 20; //------//Correct to center strength
  charge = 60; //---------//nearObject charge velocity
  range = 850; //---------//Range to start tracking object (mm)
  chargeDT = 500; //------//Charge timer before backing up (cycles)

  while (true) { //Run Forever
    checkLine(); //Check if over boundary
    checkObject(); //Check if near object
    checkButton(); //Check emerg button

    if (!braking) { //Not braking
      unbrake(); //Release wheels
      if (autonomous) { //If bot is in autonomous mode
        if (!atLine && !nearObject) { //Not over line AND not near object
          screenColor(orange); //Set cortex color to ORANGE
          if (laserInRange(range)) { //Object obstructs either laser
            seeObject = true;
            screenColor(yellow);
            updateDirection(range);
            stickForward = autoFollowSpeed; //Track object at autoFollowSpeed velocity
          }
          else { //If object not in range
            screenColor(orange); //Set cortex color to ORANGE
            seeObject = false;
            
            stickForward = 0.0;

            if (objLeft) {
              stickRotate = autoTurnSpeed * -1.0; //Rotate left based on last known object direction
            }

            else {
              stickRotate = autoTurnSpeed; //Rotate right based on last known object direction
            }
          }

          updateVelocity(1, false);
          spinMotors();
        }
        else if (atLine) { //If robot over line
          screenColor(white); //Set cortex color to WHITE
          degMove(5, 0, 180, 50); //Spin 180deg with a velocity of 50
          degMove(1, 10, 0, 40); //Move forward 7in with a velocity of 40
          //autonomous = false;
        }
        if (nearObject && count <= chargeDT) {
          
          updateDirection(range);
          
          if (nearObject && charge > 1) {  //If near object AND charge velocity set
            if (BorderDetector.value(percent) <= 10) { //Check if at arena border or robot picked up
              screenColor(white); //Set cortex color to WHITE
              degMove(2, 10, 0, 50); //Go back 10 in
              count = 0;
            }
            else { //Charge sequence
              count++;
              screenColor(purple); //Set cortex color to PURPLE
              updateVelocity(charge, true); //Set velocity to charge velocity
              spinMotors(); //Update motor spin
            }
          }
        }
        else if (nearObject && count > chargeDT) {
          degMove(2, 20, 0, 50); //Go back 10 in
          count = 0;
        }
        else if (nearObject && charge == 0) { //If near object AND no charge velocity set
          updateVelocity(0, true); //Stop
          braking = true;
        }
      }
      else { //Use controller input   
        screenColor(green); //Set cortex color to GREEN
        checkContrArrows();
        stickRotate = Controller1.Axis1.position();
        stickForward = Controller1.Axis3.position();
        stickSideways = Controller1.Axis4.position();
        
        updateVelocity(1, false); //Update velocity by stick input
        spinMotors();
      }
    }
    else if (braking) {
      screenColor(red); //Set cortex color to RED
      eBrake();
    }
    wait(5, msec);
  }
  return 0;
}

// ---- EVENT TRIGGERS ---- //
void onEvent_ButtonL1Pressed() { //L1 Pressed
  //Set braking
  braking = true;
}
void onEvent_ButtonL1Released() { //L1 Released
  braking = false;
  unbrake();
}
void onEvent_ButtonR1Pressed() { //R1 Pressed
  //Toggle autonomous mode
  autonomous = !autonomous;
}

// ---- INT MAIN ---- // ---- REGISTER EVENT HANDLERS ---- //
int main() {
  vexcodeInit();

  thread t(controllerThread);
  
  Controller1.ButtonL1.pressed(onEvent_ButtonL1Pressed);
  Controller1.ButtonL1.released(onEvent_ButtonL1Released);
  Controller1.ButtonR1.pressed(onEvent_ButtonR1Pressed);

  wait(15, msec);

  screenColor(green);
  omniControl();
}