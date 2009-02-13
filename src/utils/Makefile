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


# module name
MNAME=$(UTILS)
# include directory
# for compilation
IINCLUDE=../../$(INCLUDE_DIR)
# 
INCLUDE=../../$(INCLUDE_DIR)/$(SCE)


SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)


all: $(OBJS)


# general
%.o: %.c $(INCLUDE)/$(MNAME)/%.h Makefile
	$(CC) -o $@ -c $< -I$(IINCLUDE) $(CFLAGS)
	

# additionnal sources dependencies
# generated automatically by geth.py
# geth.py src/utils/ -o '%filename%.o: %$(IINCLUDE)/:deps%' -I include/
SCEInert.o: $(IINCLUDE)/SCE/utils/SCEInert.h
SCEQuaternion.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCELine.o: $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCEList.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h
SCETime.o: $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/utils/SCETime.h $(IINCLUDE)/SCE/core/SCECTypes.h
SCEString.o: $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/core/SCECTypes.h
SCEResources.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/utils/SCEMedia.h $(IINCLUDE)/SCE/utils/SCEResources.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h
SCEBacktracer.o: $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h
SCEMedia.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/utils/SCEMedia.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h
SCEMath.o: $(IINCLUDE)/SCE/utils/SCEMath.h
SCEPlane.o: $(IINCLUDE)/SCE/utils/SCEPlane.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCERectangle.o: $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCEError.o: $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/utils/SCETime.h $(IINCLUDE)/SCE/utils/SCEError.h
SCEMemory.o: $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/core/SCECTypes.h
SCEVector.o: $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEVector.h
SCEMatrix.o: $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h

clean:
	rm -f *.o


.PHONY: all clean
