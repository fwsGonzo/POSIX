#################################################
#          IncludeOS SERVICE makefile           #
#################################################

# The name of your service
SERVICE = posix_test
SERVICE_NAME = POSIX Testing Service

# Your service parts
FILES = service.cpp

# Your disk image
DISK=

# Drivers to be used with service
DRIVERS=virtionet

# Your own include-path
LOCAL_INCLUDES=

# IncludeOS location
ifndef INCLUDEOS_INSTALL
INCLUDEOS_INSTALL=$(HOME)/IncludeOS_install
endif

# Include the installed seed makefile
include $(INCLUDEOS_INSTALL)/Makeseed

# Disable crash context buffer
#CPPOPTS += -DDISABLE_CRASH_CONTEXT
