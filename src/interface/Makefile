#******************************************************************************#
#
# SCEngine version 0.0.9 Alpha
#
# Copyright (C) 2009 Antony Martin
#
# Cree le
# 15/05/2007
#
# Derniere modification le
# 18/08/2008
#
#******************************************************************************#


# module name
MNAME=$(INTERFACE)
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
# geth.py src/interface/ -o '%filename%.o: %$(IINCLUDE)/:deps%' -I include/
SCEModel.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECore.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECMatrix.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECFramebuffer.h $(IINCLUDE)/SCE/core/SCECShader.h $(IINCLUDE)/SCE/core/SCECMaterial.h $(IINCLUDE)/SCE/core/SCECLight.h $(IINCLUDE)/SCE/core/SCECOcclusionQuery.h $(IINCLUDE)/SCE/core/SCECPointSprite.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/interface/SCENode.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/interface/SCEMaterial.h $(IINCLUDE)/SCE/interface/SCEMesh.h $(IINCLUDE)/SCE/interface/SCECamera.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/interface/SCELight.h $(IINCLUDE)/SCE/interface/SCETexture.h $(IINCLUDE)/SCE/interface/SCEShaders.h $(IINCLUDE)/SCE/interface/SCEFrustum.h $(IINCLUDE)/SCE/utils/SCEPlane.h
SCEOBJLoader.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/interface/SCEMesh.h $(IINCLUDE)/SCE/interface/SCEOBJLoader.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h
SCEState.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/interface/SCEState.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEList.h
SCETexture.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/utils/SCEMedia.h $(IINCLUDE)/SCE/utils/SCEResources.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/interface/SCETexture.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECFramebuffer.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/utils/SCEVector.h
SCEMaterial.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEResources.h $(IINCLUDE)/SCE/core/SCECore.h $(IINCLUDE)/SCE/core/SCECMatrix.h $(IINCLUDE)/SCE/interface/SCEMaterial.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECFramebuffer.h $(IINCLUDE)/SCE/core/SCECShader.h $(IINCLUDE)/SCE/core/SCECMaterial.h $(IINCLUDE)/SCE/core/SCECLight.h $(IINCLUDE)/SCE/core/SCECOcclusionQuery.h $(IINCLUDE)/SCE/core/SCECPointSprite.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/interface/SCELight.h $(IINCLUDE)/SCE/interface/SCETexture.h $(IINCLUDE)/SCE/interface/SCEShaders.h $(IINCLUDE)/SCE/interface/SCENode.h
SCESkybox.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEResources.h $(IINCLUDE)/SCE/interface/SCESkybox.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/interface/SCENode.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/interface/SCEMaterial.h $(IINCLUDE)/SCE/interface/SCEMesh.h $(IINCLUDE)/SCE/interface/SCECamera.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/core/SCECMaterial.h $(IINCLUDE)/SCE/core/SCECPointSprite.h $(IINCLUDE)/SCE/interface/SCELight.h $(IINCLUDE)/SCE/interface/SCETexture.h $(IINCLUDE)/SCE/interface/SCEShaders.h $(IINCLUDE)/SCE/core/SCECLight.h $(IINCLUDE)/SCE/core/SCECShader.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECFramebuffer.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/interface/SCEFrustum.h $(IINCLUDE)/SCE/utils/SCEPlane.h
SCECamera.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/core/SCECMatrix.h $(IINCLUDE)/SCE/interface/SCECamera.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/interface/SCEFrustum.h $(IINCLUDE)/SCE/interface/SCENode.h $(IINCLUDE)/SCE/utils/SCEPlane.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/utils/SCEList.h
SCEOctree.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/interface/SCEOctree.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h
SCEQuad.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEResources.h $(IINCLUDE)/SCE/core/SCECMatrix.h $(IINCLUDE)/SCE/interface/SCEMesh.h $(IINCLUDE)/SCE/interface/SCEQuad.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h
SCEBoundingBox.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCE4FMLoader.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/interface/SCEMesh.h $(IINCLUDE)/SCE/interface/lib4fm.h $(IINCLUDE)/SCE/interface/SCE4FMLoader.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h
SCENode.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECMatrix.h $(IINCLUDE)/SCE/interface/SCENode.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h
SCEBillboard.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/interface/SCEBillboard.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h
SCEMesh.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/interface/lib4fm.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/utils/SCEResources.h $(IINCLUDE)/SCE/utils/SCEMedia.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/interface/SCEMesh.h $(IINCLUDE)/SCE/interface/SCE4FMLoader.h $(IINCLUDE)/SCE/interface/SCEOBJLoader.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h
SCELight.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/interface/SCELight.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/core/SCECLight.h $(IINCLUDE)/SCE/interface/SCENode.h $(IINCLUDE)/SCE/interface/SCEShaders.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECShader.h
SCEShaders.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/utils/SCEString.h $(IINCLUDE)/SCE/utils/SCEMedia.h $(IINCLUDE)/SCE/utils/SCEResources.h $(IINCLUDE)/SCE/interface/SCEShaders.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECShader.h
SCEFrustum.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/interface/SCEFrustum.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/utils/SCEPlane.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/utils/SCEMath.h
SCEScene.o: $(IINCLUDE)/SCE/SCEMinimal.h $(IINCLUDE)/SCE/core/SCECore.h $(IINCLUDE)/SCE/interface/SCEScene.h $(IINCLUDE)/SCE/utils/SCEVector.h $(IINCLUDE)/SCE/interface/SCEBoundingBox.h $(IINCLUDE)/SCE/utils/SCEBacktracer.h $(IINCLUDE)/SCE/core/SCECTypes.h $(IINCLUDE)/SCE/utils/SCEError.h $(IINCLUDE)/SCE/utils/SCEMemory.h $(IINCLUDE)/SCE/core/SCECSupport.h $(IINCLUDE)/SCE/core/SCECMatrix.h $(IINCLUDE)/SCE/core/SCECBuffers.h $(IINCLUDE)/SCE/core/SCECTexture.h $(IINCLUDE)/SCE/core/SCECFramebuffer.h $(IINCLUDE)/SCE/core/SCECShader.h $(IINCLUDE)/SCE/core/SCECMaterial.h $(IINCLUDE)/SCE/core/SCECLight.h $(IINCLUDE)/SCE/core/SCECOcclusionQuery.h $(IINCLUDE)/SCE/core/SCECPointSprite.h $(IINCLUDE)/SCE/utils/SCEList.h $(IINCLUDE)/SCE/core/SCECImage.h $(IINCLUDE)/SCE/utils/SCERectangle.h $(IINCLUDE)/SCE/utils/SCELine.h $(IINCLUDE)/SCE/utils/SCEMath.h $(IINCLUDE)/SCE/interface/SCEState.h $(IINCLUDE)/SCE/interface/SCECamera.h $(IINCLUDE)/SCE/interface/SCENode.h $(IINCLUDE)/SCE/interface/SCEFrustum.h $(IINCLUDE)/SCE/utils/SCEMatrix.h $(IINCLUDE)/SCE/utils/SCEPlane.h $(IINCLUDE)/SCE/utils/SCEQuaternion.h $(IINCLUDE)/SCE/interface/SCEMaterial.h $(IINCLUDE)/SCE/interface/SCEMesh.h $(IINCLUDE)/SCE/interface/SCELight.h $(IINCLUDE)/SCE/interface/SCETexture.h $(IINCLUDE)/SCE/interface/SCEShaders.h
lib4fm.o: $(IINCLUDE)/SCE/interface/lib4fm.h


clean:
	rm -f *.o


.PHONY: all clean
