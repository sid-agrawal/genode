SRC_CC  += sid-foo.c
INC_DIR += $(REP_DIR)/src/lib/sid-foo

LIBS = libc

vpath %.cc $(REP_DIR)/src/lib/sid-foo

SHARED_LIB = no
