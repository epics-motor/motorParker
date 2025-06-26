TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# The following are used for debugging messages.
#!USR_CXXFLAGS += -DDEBUG

#=============================
# Build the IOC application

PROD_IOC = parker
# parker.dbd will be created and installed
DBD += parker.dbd

# parker.dbd will be made up from these files:
parker_DBD += base.dbd

# Include dbd files from all support applications:
parker_DBD += asyn.dbd
parker_DBD += drvAsynSerialPort.dbd
parker_DBD += drvAsynIPPort.dbd
parker_DBD += motorSupport.dbd
parker_DBD += ParkerSupport.dbd

# Add all the support libraries needed by this IOC
parker_LIBS += Parker
parker_LIBS += motor
parker_LIBS += asyn

# parker_registerRecordDeviceDriver.cpp derives from parker.dbd
parker_SRCS += parker_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
parker_SRCS_DEFAULT += parkerMain.cpp
parker_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#parker_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
parker_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

