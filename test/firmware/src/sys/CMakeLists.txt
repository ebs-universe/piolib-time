
FIND_PACKAGE("prbs" REQUIRED)
FIND_PACKAGE("crypto" REQUIRED)
FIND_PACKAGE("time" REQUIRED)
FIND_PACKAGE("ucdm" REQUIRED)
FIND_PACKAGE("ds" REQUIRED)

ADD_PLATFORM_LIBRARY(ucdm-if STATIC "ucdm" ucdm_if.c)
ADD_PLATFORM_LIBRARY(time-if STATIC "hal-uc-timer time" time_if.c)
ADD_PLATFORM_LIBRARY(rand STATIC "hal-uc-entropy prbs crypto" rand.c)

ADD_SUBDIRECTORY(modbus/src)

