TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install) into <top>/db
# databases, templates, substitutions like this
#DB += xxx.db

ifdef MOTOR_PARKER
### this module was built outside motor
DB_INSTALLS += $(MOTOR_PARKER)/db/ACRAuxBi.template
DB_INSTALLS += $(MOTOR_PARKER)/db/ACRAuxBoRBV.template
DB_INSTALLS += $(MOTOR_PARKER)/db/ACRAuxBo.template
DB_INSTALLS += $(MOTOR_PARKER)/db/ACRAuxLi.template
DB_INSTALLS += $(MOTOR_PARKER)/db/ACRAuxRead.template
DB_INSTALLS += $(MOTOR_PARKER)/db/ACRAux.template
DB_INSTALLS += $(MOTOR_PARKER)/db/OEMLimitSwitch.template
else
### this module was built inside motor/modules
DB_INSTALLS += $(MOTOR)/db/ACRAuxBi.template
DB_INSTALLS += $(MOTOR)/db/ACRAuxBoRBV.template
DB_INSTALLS += $(MOTOR)/db/ACRAuxBo.template
DB_INSTALLS += $(MOTOR)/db/ACRAuxLi.template
DB_INSTALLS += $(MOTOR)/db/ACRAuxRead.template
DB_INSTALLS += $(MOTOR)/db/ACRAux.template
DB_INSTALLS += $(MOTOR)/db/OEMLimitSwitch.template
endif

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_template = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

