#!../../bin/linux-x86_64/parker

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/parker.dbd"
parker_registerRecordDeviceDriver pdbbase

cd "${TOP}/iocBoot/${IOC}"

## motorUtil (allstop & alldone)
dbLoadRecords("$(MOTOR)/db/motorUtil.db", "P=parker:")

## 
< PC6K.cmd

iocInit

## motorUtil (allstop & alldone)
motorUtilInit("parker:")

# Create source files before uncommenting following line
#!< PC6K-postInit.cmd

# Boot complete
