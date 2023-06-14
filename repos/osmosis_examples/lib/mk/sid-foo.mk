SRC_CC  += sqlite3.c
INC_DIR += $(REP_DIR)/src/lib/sid-foo

LIBS = libc

CC_OPT   += -D__GENODE__
CC_OPT   += -DSQLITE_THREADSAFE=0

vpath %.cc $(REP_DIR)/src/lib/sid-foo

SHARED_LIB = yes

CC_CXX_WARN_STRICT_CONVERSION =