#################################################
#          IncludeOS SERVICE makefile           #
#################################################

# The name of your service
SERVICE = posix_test
SERVICE_NAME = POSIX Testing Service

# Your service parts
FILES = service.o socket_test.o
				#sqlite_test.o sqlite/sqlite3.o

# Your disk image
DISK=

# Drivers to be used with service
DRIVERS=virtionet

LOCAL_DEFS=-D_HAVE_SQLITE_CONFIG_H
LOCAL_INCLUDES=

# IncludeOS location
ifndef INCLUDEOS_INSTALL
INCLUDEOS_INSTALL=$(HOME)/IncludeOS_install
endif

# Include the installed seed makefile
include $(INCLUDEOS_INSTALL)/Makeseed

# Disable crash context buffer
#CPPOPTS += -DDISABLE_CRASH_CONTEXT
