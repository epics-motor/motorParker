# motorParker Releases

## __R1-1-1 (2023-04-12)__
R1-1-1 is a release based on the master branch.

### Changes since R1-1

#### New features
* None

#### Modifications to existing features
* None

#### Bug fixes
* None

#### Continuous integration
* Added ci-scripts (v3.0.1)
* Configured to use Github Actions for CI

## __R1-1 (2020-05-12)__
R1-1 is a release based on the master branch.  

### Changes since R1-0

#### New features
* Commit [7a17a95](https://github.com/epics-motor/motorParker/commit/7a17a9529b46b3ae2d518d099d57c5a684c55fb2): User displays can now be autoconverted at build time

#### Modifications to existing features
* Commit [a6e5e6a](https://github.com/epics-motor/motorParker/commit/a6e5e6a1df089a3929f48645a614c82773422dc2): ``CONFIG_SITE`` now includes ``$(SUPPORT)/configure/CONFIG_SITE``, if it exists
* Commit [a80f80d](https://github.com/epics-motor/motorParker/commit/a80f80dd655180a6e59c80be823fefd825c2e87f): ``SUPPORT`` is now defined in ``RELEASE``, which is needed for autoconvert
* Commit [c8d208f](https://github.com/epics-motor/motorParker/commit/c8d208fe2d0d6c633fe3b3ff3804a231c92c3082): User displays have been autoconverted using the latest converter

#### Bug fixes
* Commit [03ef9ca](https://github.com/epics-motor/motorParker/commit/03ef9ca874ad61747e7efafe08527e378608f216): Include ``$(MOTOR)/modules/RELEASE.$(EPICS_HOST_ARCH).local`` instead of ``$(MOTOR)/configure/RELEASE``
* Pull request [#1](https://github.com/epics-motor/motorParker/pull/1): Eliminated compiler warnings

## __R1-0 (2019-04-18)__
R1-0 is a release based on the master branch.  

### Changes since motor-6-11

motorParker is now a standalone module, as well as a submodule of [motor](https://github.com/epics-modules/motor)

#### New features
* motorParker can be built outside of the motor directory
* motorParker has a dedicated example IOC that can be built outside of motorParker

#### Modifications to existing features
* None

#### Bug fixes
* None
