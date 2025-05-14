/*
FILENAME... OEMMotorDriver.cpp
USAGE...    Motor driver support for the Parker OEM series of controllers.

Guilherme Rodrigues de Lima
May 27, 2025

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iocsh.h>
#include <epicsThread.h>

#include <asynOctetSyncIO.h>

#include "asynMotorController.h"
#include "asynMotorAxis.h"

#include <epicsExport.h>
#include "OEMMotorDriver.h"

static const char *driverName = "OEMMotorDriver";

/** Creates a new OEMController object.
  * \param[in] portName          The name of the asyn port that will be created for this driver
  * \param[in] OEMPortName       The name of the drvAsynIPPPort that was created previously to connect to the OEM controller 
  * \param[in] numAxes           The number of axes that this controller supports 
  * \param[in] movingPollPeriod  The time between polls when any axis is moving 
  * \param[in] idlePollPeriod    The time between polls when no axis is moving 
  */
OEMController::OEMController(const char *portName, const char *OEMPortName, int numAxes, 
                             double movingPollPeriod, double idlePollPeriod)
  :  asynMotorController(portName, numAxes, 1, 
                         asynUInt32DigitalMask, 
                         asynUInt32DigitalMask,
                         ASYN_CANBLOCK | ASYN_MULTIDEVICE, 
                         1, // autoconnect
                         0, 0)  // Default priority and stack size
{
  int axis;
  asynStatus status;
  static const char *functionName = "OEMController";

  // Create controller-specific parameters
  createParam(OEMSelectSwitchString,        asynParamInt32,         &OEMSelectSwitch_);
  createParam(OEMSwitchDetectedString,      asynParamInt32,         &OEMSwitchDetected_);

  /* Connect to OEM controller */
  status = pasynOctetSyncIO->connect(OEMPortName, 0, &pasynUserController_, NULL);
  pasynOctetSyncIO->setInputEos(pasynUserController_, "\r", 1);
  pasynOctetSyncIO->setOutputEos(pasynUserController_, "\r", 1);

  if (status) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, 
      "%s:%s: cannot connect to OEM series controller\n",
      driverName, functionName);
  }

  // Create the axis objects
  for (axis=0; axis<numAxes; axis++) {
    new OEMAxis(this, axis);
  }

  startPoller(movingPollPeriod, idlePollPeriod, 2);
}


/** Creates a new OEMController object.
  * Configuration command, called directly or from iocsh
  * \param[in] portName          The name of the asyn port that will be created for this driver
  * \param[in] OEMPortName       The name of the drvAsynIPPPort that was created previously to connect to the OEM controller 
  * \param[in] numAxes           The number of axes that this controller supports 
  * \param[in] movingPollPeriod  The time in ms between polls when any axis is moving
  * \param[in] idlePollPeriod    The time in ms between polls when no axis is moving 
  */
extern "C" int OEMCreateController(const char *portName, const char *OEMPortName, int numAxes, 
                                   int movingPollPeriod, int idlePollPeriod)
{
  new OEMController(portName, OEMPortName, numAxes, movingPollPeriod/1000., idlePollPeriod/1000.);
  return(asynSuccess);
}

/** Reports on status of the driver
  * \param[in] fp The file pointer on which report information will be written
  * \param[in] level The level of report detail desired
  *
  * If details > 0 then information is printed about each axis.
  * After printing controller-specific information calls asynMotorController::report()
  */
void OEMController::report(FILE *fp, int level)
{
  fprintf(fp, "OEM motor driver %s, numAxes=%d, moving poll period=%f, idle poll period=%f\n", 
    this->portName, numAxes_, movingPollPeriod_, idlePollPeriod_);

  // Call the base class method
  asynMotorController::report(fp, level);
}

/** Returns a pointer to an OEMMotorAxis object.
  * Returns NULL if the axis number encoded in pasynUser is invalid.
  * \param[in] pasynUser asynUser structure that encodes the axis index number. */
OEMAxis* OEMController::getAxis(asynUser *pasynUser)
{
  return static_cast<OEMAxis*>(asynMotorController::getAxis(pasynUser));
}

/** Returns a pointer to an OEMMotorAxis object.
  * Returns NULL if the axis number encoded in pasynUser is invalid.
  * \param[in] axisNo Axis index number. */
OEMAxis* OEMController::getAxis(int axisNo)
{
  return static_cast<OEMAxis*>(asynMotorController::getAxis(axisNo));
}

/** Called when asyn clients call pasynInt32->write().
  * Extracts the function and axis number from pasynUser.
  * Sets the value in the parameter library.
  * For all other functions it calls asynMotorController::writeInt32.
  * Calls any registered callbacks for this pasynUser->reason and address.  
  * \param[in] pasynUser asynUser structure that encodes the reason and address.
  * \param[in] value     Value to write. */
asynStatus OEMController::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int function = pasynUser->reason;
  asynStatus status = asynSuccess;
  OEMAxis *pAxis = getAxis(pasynUser);
  static const char *functionName = "writeInt32";
  
  /* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
   * status at the end, but that's OK */
  status = setIntegerParam(pAxis->axisNo_, function, value);
  
  /* Call base class method */
  status = asynMotorController::writeInt32(pasynUser, value);
  
  /* Do callbacks so higher layers see any changes */
  callParamCallbacks(pAxis->axisNo_);
  if (status) 
    asynPrint(pasynUser, ASYN_TRACE_ERROR, 
        "%s:%s: error, status=%d function=%d, value=%d\n", 
        driverName, functionName, status, function, value);
  else    
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
        "%s:%s: function=%d, value=%d\n", 
        driverName, functionName, function, value);
  return status;
}

/** Called when asyn clients call pasynFloat64->write().
  * Extracts the function and axis number from pasynUser.
  * Sets the value in the parameter library.
  * If the function is OEMJerk_ it sets the jerk value in the controller.
  * Calls any registered callbacks for this pasynUser->reason and address.  
  * For all other functions it calls asynMotorController::writeFloat64.
  * \param[in] pasynUser asynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus OEMController::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
  int function = pasynUser->reason;
  asynStatus status = asynSuccess;
  OEMAxis *pAxis = getAxis(pasynUser);
  static const char *functionName = "writeFloat64";
  
  /* Set the parameter and readback in the parameter library. */
  status = setDoubleParam(pAxis->axisNo_, function, value);
  
  if(function == motorRecResolution_) {
    int resolution = 0;
    resolution = static_cast<int>(ceil(pow(value,-1)));
    sprintf(outString_, "%iMR%i", pAxis->address_, resolution);
    status = writeReadController();
    pAxis->pulsesPerUnit_ = (double)value;
  }

  /* Call base class method */
  status = asynMotorController::writeFloat64(pasynUser, value);
  
  /* Do callbacks so higher layers see any changes */
  callParamCallbacks(pAxis->axisNo_);
  if (status) 
    asynPrint(pasynUser, ASYN_TRACE_ERROR, 
        "%s:%s: error, status=%d function=%d, value=%f\n", 
        driverName, functionName, status, function, value);
  else    
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
        "%s:%s: function=%d, value=%f\n", 
        driverName, functionName, function, value);
  return status;
}

/** Creates a new OEMAxis object.
  * \param[in] pC Pointer to the OEMController to which this axis belongs. 
  * \param[in] axisNo Index number of this axis, range 0 to pC->numAxes_-1.
  * 
  * Initializes register numbers, etc.
  */
OEMAxis::OEMAxis(OEMController *pC, int axisNo)
  : asynMotorAxis(pC, axisNo),
    pC_(pC)
{
  asynStatus status = asynSuccess;
  static const char *functionName = "OEMAxis";

  address_ = axisNo + 1;

  sprintf(pC_->outString_, "%iE", address_);
  status = pC_->writeReadController();

  sprintf(pC_->outString_, "%iSSA0", address_);
  status = pC_->writeReadController();

  sprintf(pC_->outString_, "%iMPA", address_);
  status = pC_->writeReadController();

  pC_->getDoubleParam(axisNo_, pC_->motorRecResolution_, &pulsesPerUnit_); //MR command to reading does not work!

  if (status) {
    asynPrint(pC_->pasynUserSelf, ASYN_TRACE_ERROR, 
      "%s:%s: error to create axis %i for address %i\n",
      driverName, functionName, axisNo, address_);
  }
}

/** Reports on status of the driver
  * \param[in] fp The file pointer on which report information will be written
  * \param[in] level The level of report detail desired
  *
  * If details > 0 then information is printed about each axis.
  * After printing controller-specific information calls asynMotorController::report()
  */
void OEMAxis::report(FILE *fp, int level)
{
  // Call the base class method
  asynMotorAxis::report(fp, level);
}

asynStatus OEMAxis::move(double position, int relative, double minVelocity, double maxVelocity, double acceleration)
{
  asynStatus status = asynSuccess;
  static const char *functionName = "moveAxis";

  sprintf(pC_->outString_, "%iMN", address_);
  status = pC_->writeReadController();

  sprintf(pC_->outString_, "%iA%.2f", address_, maxVelocity/acceleration);
  status = pC_->writeReadController();

  sprintf(pC_->outString_, "%iV%.2f", address_, maxVelocity*pulsesPerUnit_);
  status = pC_->writeReadController();

  sprintf(pC_->outString_, "%iD%.0f", address_, position);
  status = pC_->writeReadController();

  sprintf(pC_->outString_, "%iG", address_);
  status = pC_->writeReadController();

  return status;
}

asynStatus OEMAxis::home(double minVelocity, double maxVelocity, double acceleration, int forwards)
{
  asynStatus status = asynSuccess;
  static const char *functionName = "homeAxis";
  
  sprintf(pC_->outString_, "%iMC", address_);
  status = pC_->writeReadController();

  sprintf(pC_->outString_, "%iV%.2f", address_, maxVelocity*pulsesPerUnit_);
  status = pC_->writeReadController();

  if(forwards) {
    sprintf(pC_->outString_, "%iH-", address_);
  }
  else {
    sprintf(pC_->outString_, "%iH+", address_);
  }
  status = pC_->writeReadController();

  sprintf(pC_->outString_, "%iG", address_);
  status = pC_->writeReadController();

  return status;
}

asynStatus OEMAxis::stop(double acceleration )
{
  asynStatus status = asynSuccess;
  static const char *functionName = "stopAxis";
  
  sprintf(pC_->outString_, "%iS", address_);
  status = pC_->writeReadController();

  return status;
}

asynStatus OEMAxis::setClosedLoop(bool closedLoop)
{
  asynStatus status = asynSuccess;
  static const char *functionName = "setClosedLoopAxis";

  sprintf(pC_->outString_, "%iFSC%i", address_, (int)closedLoop);
  status = pC_->writeReadController();
  
  return status;
}

/** Polls the axis.
  * This function reads the controller position, encoder position, the limit status, the moving status, 
  * and the drive power-on status.  It does not current detect following error, etc. but this could be
  * added.
  * It calls setIntegerParam() and setDoubleParam() for each item that it polls,
  * and then calls callParamCallbacks() at the end.
  * \param[out] moving A flag that is set indicating that the axis is moving (1) or done (0). */
asynStatus OEMAxis::poll(bool *moving)
{ 
  asynStatus comStatus = asynSuccess;
  int selectSwitch, switchDetector;
  int done;

  // Read the current flags
  sprintf(pC_->outString_, "%iR", address_);
  comStatus = pC_->writeReadController();
  if (comStatus) goto skip;
  comStatus = pC_->readController();
  done = (strcmp(pC_->inString_, "*B") == 0 || strcmp(pC_->inString_, "*C") == 0)?false:true;
  setIntegerParam(pC_->motorStatusDone_, done);
  *moving = !done;

  // Read the current  or step encoder position
  if(done) {
    sprintf(pC_->outString_, "%iPR", address_);
    comStatus = pC_->writeReadController();
    if (comStatus) goto skip;
    comStatus = pC_->readController();
    if (!comStatus) { //The response to this command will be reported after the move is completed
      encoderPosition = (double)strtod(pC_->inString_+1, NULL);      
      setDoubleParam(pC_->motorEncoderPosition_, encoderPosition);
      setDoubleParam(pC_->motorPosition_, encoderPosition);
    }
  }
  else {
    sprintf(pC_->outString_, "%iW3", address_);
    comStatus = pC_->writeReadController();
    if (comStatus) goto skip;
    comStatus = pC_->readController();
    encoderStep = (double)stol(pC_->inString_+1, nullptr, 16);
    if(strstr(pC_->inString_, "*F") != nullptr) {
      encoderStep -= 4294967296;
    }
    setDoubleParam(pC_->motorEncoderPosition_, (encoderPosition+encoderStep));
    setDoubleParam(pC_->motorPosition_, (encoderPosition+encoderStep));
  }

  // Read the current status
  sprintf(pC_->outString_, "%iIS", address_);
  comStatus = pC_->writeReadController();
  if (comStatus) goto skip;
  comStatus = pC_->readController();

  //Read limit switch (CW and CCW)
  sprintf(pC_->outString_, "%iRA", address_);
  comStatus = pC_->writeReadController();
  if (comStatus) goto skip;
  comStatus = pC_->readController();
  if (strcmp(pC_->inString_, currentLimitSwitch_[0].c_str()) == 0) {
    setIntegerParam(pC_->OEMSwitchDetected_, 1);
  }
  else if (strcmp(pC_->inString_, currentLimitSwitch_[1].c_str()) == 0) {
    setIntegerParam(pC_->OEMSwitchDetected_, 2);
  }
  else {
    setIntegerParam(pC_->OEMSwitchDetected_, 0);
  }
  pC_->getIntegerParam(axisNo_, pC_->OEMSelectSwitch_, &selectSwitch);
  if (strcmp(pC_->inString_, currentLimitSwitch_[selectSwitch].c_str()) == 0 && encoderPosition != 0) {
    sprintf(pC_->outString_, "%iPZ", address_);
    comStatus = pC_->writeReadController();
  }

  skip:
  setIntegerParam(pC_->motorStatusProblem_, comStatus ? 1:0);
  callParamCallbacks();

  return comStatus ? asynError : asynSuccess;
}

/** Code for iocsh registration */
static const iocshArg OEMCreateControllerArg0 = {"Port name", iocshArgString};
static const iocshArg OEMCreateControllerArg1 = {"OEM port name", iocshArgString};
static const iocshArg OEMCreateControllerArg2 = {"Number of axes", iocshArgInt};
static const iocshArg OEMCreateControllerArg3 = {"Moving poll period (ms)", iocshArgInt};
static const iocshArg OEMCreateControllerArg4 = {"Idle poll period (ms)", iocshArgInt};
static const iocshArg * const OEMCreateControllerArgs[] = {&OEMCreateControllerArg0,
                                                           &OEMCreateControllerArg1,
                                                           &OEMCreateControllerArg2,
                                                           &OEMCreateControllerArg3,
                                                           &OEMCreateControllerArg4};
static const iocshFuncDef OEMCreateControllerDef = {"OEMCreateController", 5, OEMCreateControllerArgs};
static void OEMCreateContollerCallFunc(const iocshArgBuf *args)
{
  OEMCreateController(args[0].sval, args[1].sval, args[2].ival, args[3].ival, args[4].ival);
}

static void OEMMotorRegister(void)
{
  iocshRegister(&OEMCreateControllerDef, OEMCreateContollerCallFunc);
}

extern "C" {
epicsExportRegistrar(OEMMotorRegister);
}
