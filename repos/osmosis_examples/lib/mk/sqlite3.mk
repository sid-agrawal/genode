SRC_CC  += sqlite3.c
INC_DIR += $(REP_DIR)/src/lib/sqlite3

LIBS = libc

CC_OPT   += -D__GENODE__
CC_OPT   += -DSQLITE_THREADSAFE=0

vpath %.c $(REP_DIR)/src/lib/sqlite3

SHARED_LIB = yes

CC_CXX_WARN_STRICT_CONVERSION =
