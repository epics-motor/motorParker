# serial 1 is a RS232 link to a Parker Gemini 6K Controller
drvAsynSerialPortConfigure("serial1", "/dev/ttyUSB0", 0, 0, 0)
asynOctetSetInputEos("serial1",0,">")
asynOctetSetOutputEos("serial1",0,"\r")
asynOctetConnect("serial1", "serial1")

# serial 3 is ETHERNET link to the Parker Gemini 6K Controller
drvAsynIPPortConfigure("serial3", "192.168.10.29:5002", 0, 0, 0)
asynOctetConnect("serial3", "serial3")
asynOctetSetInputEos("serial3",0,">")
asynOctetSetOutputEos("serial3",0,"\r")

dbLoadTemplate("PC6K.substitutions")

# Parker/Compumotor - Gemini 6K driver setup parameters:
#     (1) maximum number of controllers in system
#     (2) motor task polling rate (min=1Hz, max=60Hz)
PC6KSetup(2, 60)

# Parker/Compumotor - Gemini 6K driver configuration parameters:
#     (1) controller being configured
#     (2) asyn port name (string)
PC6KConfig(0, "serial1")
PC6KConfig(1, "serial3")
