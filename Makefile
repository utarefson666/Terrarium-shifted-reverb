  
# Project Name
TARGET = Terrarium-shifted-reverb

# Sources
CPP_SOURCES = Terrarium-shifted-reverb.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy
DAISYSP_DIR = ../../DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile