# motorParker
EPICS motor drivers for the following [Parker Hannifin](http://www.parkermotion.com/) controllers:<br>
6K Series controllers and ACR series of controllers, including the Aries

[![Build Status](https://travis-ci.org/epics-motor/motorParker.png)](https://travis-ci.org/epics-motor/motorParker)

motorParker is a submodule of [motor](https://github.com/epics-modules/motor).  When motorParker is built in the ``motor/modules`` directory, no manual configuration is needed.

motorParker can also be built outside of motor by copying it's ``EXAMPLE_RELEASE.local`` file to ``RELEASE.local`` and defining the paths to ``MOTOR`` and itself.

motorParker contains an example IOC that is built if ``CONFIG_SITE.local`` sets ``BUILD_IOCS = YES``.  The example IOC can be built outside of driver module.
