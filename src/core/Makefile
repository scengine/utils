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
MNAME=$(CORE)
# include directory
# for compilation
IINCLUDE=../../$(INCLUDE_DIR)
# 
INCLUDE=../../$(INCLUDE_DIR)/$(SCE)

# general (minimal) depend
MINDEP=$(INCLUDE)/$(MINIMAL) $(INCLUDE)/$(CORE)/SCECTypes.h $(INCLUDE)/$(UTILS)/SCEMemory.h $(INCLUDE)/$(UTILS)/SCEError.h

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)


all: $(OBJS)


# general
%.o: %.c $(INCLUDE)/$(MNAME)/%.h $(MINDEP) Makefile
	$(CC) -o $@ -c $< -I$(IINCLUDE) $(CFLAGS)


# additionnal dependencies
# generated automatically by geth.py
# geth.py src/core/ -o '%filename%.o: %$(IINCLUDE)/:deps%' -I include/
SCECBuffers.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h
SCECore.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECore.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECMatrix.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECFramebuffer.h $(IINCLUDE)/SCE/core/SCECShader.h $(IINCLUDE)/SCE/core/SCECMaterial.h $(IINCLUDE)/SCE/core/SCECLight.h $(IINCLUDE)/SCE/core/SCECOcclusionQuery.h $(IINCLUDE)/SCE/core/SCECPointSprite.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCECFramebuffer.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECFramebuffer.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCECImage.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/utils/SCEMedia.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEList.h
SCECLight.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECore.h $(IINCLUDE)/SCE/core/SCECLight.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECMatrix.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECFramebuffer.h $(IINCLUDE)/SCE/core/SCECShader.h $(IINCLUDE)/SCE/core/SCECMaterial.h $(IINCLUDE)/SCE/core/SCECOcclusionQuery.h $(IINCLUDE)/SCE/core/SCECPointSprite.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCECMaterial.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECMaterial.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h
SCECMatrix.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECMatrix.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h
SCECOcclusionQuery.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECOcclusionQuery.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h
SCECPointSprite.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECPointSprite.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCECShader.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECShader.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h
SCECSupport.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h
SCECTexture.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEResources.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h
SCECTypes.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h


clean:
	rm -f *.o


.PHONY: all clean
