/*
FILENAME...   OEMMotorDriver.h
USAGE...      Motor driver support for the Parker OEM series of controllers.

Guilherme Rodrigues de Lima
May 27, 2025

*/

// Standard includes
#include <vector>
#include <string>
#include <cmath>

#include "asynMotorController.h"
#include "asynMotorAxis.h"

using namespace std;

/** drvInfo strings for extra parameters that the ACR controller supports */
#define         OEMSelectSwitchString         "OEM_SELECT_SWITCH"
#define         OEMSwitchDetectedString       "OEM_SWITCH_DETEC"

class epicsShareClass OEMAxis : public asynMotorAxis
{
public:
  /* These are the methods we override from the base class */
  OEMAxis(class OEMController *pC, int axis);
  void report(FILE *fp, int level);
  asynStatus move(double position, int relative, double minVelocity, double maxVelocity, double acceleration);
  asynStatus home(double minVelocity, double maxVelocity, double acceleration, int forwards);
  asynStatus stop(double acceleration);
  asynStatus poll(bool *moving);
  asynStatus setClosedLoop(bool closedLoop);

private:
  OEMController *pC_;      /**< Pointer to the asynMotorController to which this axis belongs.
                                *   Abbreviated because it is used very frequently */
  int address_;
  int posCounterZero_;
  double pulsesPerUnit_;
  double encoderPosition, encoderStep;
  vector<string> currentLimitSwitch_ = {"*J", "*E"};
  
friend class OEMController;
};

class epicsShareClass OEMController : public asynMotorController {
public:
  OEMController(const char *portName, const char *OEMPortName, int numAxes, double movingPollPeriod, double idlePollPeriod);
  
  /* These are the methods that we override from asynMotorDriver */
  asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
  void report(FILE *fp, int level);
  OEMAxis* getAxis(asynUser *pasynUser);
  OEMAxis* getAxis(int axisNo);
  
protected:

private:
  int OEMSelectSwitch_;
  int OEMSwitchDetected_;
  
friend class OEMAxis;
};
