
#include <Zumo32U4.h>

Zumo32U4LCD lcd;
Zumo32U4Motors motors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4ButtonA buttonA;

const uint8_t blockedThreshold = 6;
const uint8_t unBlockedThreshold = 4;

// The maximum and minimum speed to drive the motors. 400 is full speed.
const uint16_t speedMin = 100;
const uint16_t speedMax = 400;
const uint16_t speedIncrement = speedMax / 6;

// Current speeds for forward, and turns
uint16_t fwdSpeed = speedMax;
uint16_t turnSpeed = speedMax / 2;

// Whether the robot is currently blocked
bool isBlocked = false;

// Turn direction
#define STRAIGHT 0
#define LEFT 1
#define RIGHT -1
int8_t turnDirection = STRAIGHT;

void setup() {
//  proxSensors.initFrontSensor();
  proxSensors.initThreeSensors();

  // Wait for the user to press A before driving the motors.
  lcd.clear();
  lcd.print(F("Press A"));
  buttonA.waitForButton();
  lcd.clear();
}

void turnRight()
{
  motors.setSpeeds(turnSpeed, -turnSpeed);
}

void turnLeft()
{
  motors.setSpeeds(-turnSpeed, turnSpeed);
}

void driveForward()
{
  motors.setSpeeds(fwdSpeed, fwdSpeed); 
}

void stop()
{
  motors.setSpeeds(0, 0);
}


void loop() {
/*
 * Set fwd speed based on front proximity
 * if front blocked
 *  choose which side is has less proximity
 *  turn that direction until front is no longer blocked
 */

  proxSensors.read();
  uint8_t frontProxCount = max(proxSensors.countsFrontWithLeftLeds(),proxSensors.countsFrontWithRightLeds());
  uint8_t leftProxCount = proxSensors.countsLeftWithLeftLeds();
  uint8_t rightProxCount = proxSensors.countsRightWithRightLeds();

  // Not blocked state
  if (!isBlocked)
  {
    // If now blocked, stop driving forward
    if (fwdSpeed > 0 & frontProxCount >= 6)
    {
      stop();
      isBlocked = true;
    }
  
    // Front is not blocked, set fwd drive speed
    else
    {
      fwdSpeed = speedMax - (speedIncrement * frontProxCount);
      driveForward();
      turnDirection = STRAIGHT;
    }    
  }

  // Blocked state
  else {

    if (turnDirection == STRAIGHT)
    {    
      // Determine which direction is less blocked
      if (leftProxCount >= rightProxCount)
      {
        turnDirection = RIGHT;
      }
      else
      {
        turnDirection = LEFT;  
      }
    }

    // In a turn
    else
    {
      // Continue turn
      if (frontProxCount > unBlockedThreshold)
      {
        if (turnDirection == RIGHT)
        {
          turnRight();
        }
        else
        {
          turnLeft();
        }
      }

      // Front is no longer blocked
      else
      {
        stop();
        turnDirection = STRAIGHT;
        isBlocked = false;
      }      
    }

  } // end blocked state


  // Display current state
  lcd.clear();
  lcd.gotoXY(0, 0);
  if (isBlocked)
  {
    lcd.print(F("Blocked"));
  }

  lcd.gotoXY(0,1);
  if (turnDirection == STRAIGHT)
  {
    lcd.print(F("Straight"));
  }
  else if (turnDirection == LEFT)
  {
    lcd.print(F("<-- Left"));
  }
  else if (turnDirection == RIGHT)
  {
    lcd.print(F("Right ->"));
  }
  else
  {
    lcd.print(F("Unknown"));
  }

} // end loop()


