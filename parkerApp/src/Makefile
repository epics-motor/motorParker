# Makefile
TOP = ../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# The following are used for debugging messages.
#USR_CXXFLAGS += -DDEBUG

#OPT_CXXFLAGS = -g -O0

#==================================================
# Build IOC support libraries

DBD += devPC6K.dbd
DBD += ACRMotorSupport.dbd
DBD += OEMMotorSupport.dbd
DBD += ParkerSupport.dbd

# Build a library wtih only ACR support for backwards compatibility
LIBRARY_IOC += ACRMotor

ACRMotor_SRCS += ACRMotorDriver.cpp
ACRMotor_LIBS += motor
ACRMotor_LIBS += asyn
ACRMotor_LIBS += $(EPICS_BASE_IOC_LIBS)

# Build a library with all of the Parker support
LIBRARY_IOC += Parker

# Parker Compumotor driver support.
Parker_SRCS += ParkerRegister.cc
Parker_SRCS += devPC6K.cc drvPC6K.cc
# Parker ACR driver support
Parker_SRCS += ACRMotorDriver.cpp
# Parker OEM driver support
Parker_SRCS += OEMMotorDriver.cpp

Parker_LIBS += motor 
Parker_LIBS += asyn
Parker_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES

