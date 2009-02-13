#******************************************************************************#
#
# SCEngine version 0.0.3 Alpha
#
# Copyright (C) 2007 Antony Martin
#
# Cree le
# 15/05/2007
#
# Derniere modification le
# 18/08/2008
#
#******************************************************************************#


# include directory
# for compilation
IINCLUDE=../$(INCLUDE_DIR)
# 
INCLUDE=../$(INCLUDE_DIR)/$(SCE)


# minimal depend (not used here)
export MINIMAL=SCEMinimal.h


SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)


all: $(UTILS) $(CORE) $(INTERFACE) $(OBJS)

%.o: %.c $(INCLUDE)/%.h Makefile
	$(CC) -o $@ -c $< -I$(IINCLUDE) $(CFLAGS)


# add depends (all)
SCEngine.o: $(INCLUDE)/$(UTILS)/*.h $(INCLUDE)/$(CORE)/*.h $(INCLUDE)/$(INTERFACE)/*.h


# build modules
$(INTERFACE): $(UTILS) $(CORE)
	cd $@ && $(MAKE)

$(CORE): $(UTILS)
	cd $@ && $(MAKE)

$(UTILS):
	cd $@ && $(MAKE)


clean:
	rm -f *.o
	cd $(INTERFACE) && $(MAKE) $@
	cd $(CORE) && $(MAKE) $@
	cd $(UTILS) && $(MAKE) $@


.PHONY: all clean $(INTERFACE) $(CORE) $(UTILS)
