SRC_CC  += sid-foo.c
INC_DIR += $(REP_DIR)/src/lib/sid-foo

LIBS = posix

vpath %.cc $(REP_DIR)/src/lib/sid-foo

SHARED_LIB = yes
