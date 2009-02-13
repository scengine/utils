#-------------------------------------------------------------------------------
#   SCEngine - A 3D real time rendering engine written in the C language
#   Copyright (C) 2006-2009  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------------------------------------

# created: 15/05/2007
# updated: 05/02/2009

# engine version
VERSION = 0.0.9
# lib4fm version
FFM_VERSION = 0.1
# compiling debugging (parano)
STATIC_DBG = 0
# dynamic bug fix (coding bugs)
DYNAMIC_DBG = 1
# use Cg shaders
USE_CG = 0
# debug backtracer
USE_BT = 0

# gets if arch is 64bits or not. Tested and working under Linux and FreeBSD
ARCH = $(shell uname -m | grep --only-matching '64')

# GNU C Compiler
export CC ?= gcc
# AR
AR ?= ar
# C flags
export CFLAGS = -Wall -O2 -DSCE_VERSION_STRING=\"$(VERSION)\" \
                -DFFM_VERSION_STRING=\"$(FFM_VERSION)\"


# debug mode
ifeq ($(DYNAMIC_DBG), 1)
	export CFLAGS += -DSCE_DEBUG -g
endif

# using Cg shaders -> define SCE_USE_CG
ifeq ($(USE_CG), 1)
	export CFLAGS += -DSCE_USE_CG
endif

# static backtracer
ifeq ($(USE_BT), 1)
	export CFLAGS += -DSCE_USE_BACKTRACER
endif

# architecture
ifeq ($(ARCH),64)
	export CFLAGS += -fPIC
endif

# parano-mode
ifeq ($(STATIC_DBG),1)
	export CFLAGS += -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int \
	                 -Werror-implicit-function-declaration -Wmain \
	                 -Wparentheses -Wsequence-point -Wreturn-type -Wswitch \
	                 -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas\
	                 -Wfloat-equal -Wundef -Wshadow -Wpointer-arith \
	                 -Wbad-function-cast -Wwrite-strings \#-Wconversion \
	                 -Wsign-compare -Waggregate-return -Wstrict-prototypes \
	                 -Wmissing-prototypes -Wmissing-declarations \
	                 -Wmissing-noreturn -Wformat -Wmissing-format-attribute\
	                 -Wpacked -Wredundant-decls -Wnested-externs \
	                 -Winline -Wlong-long -Wunreachable-code
endif


# include directory
export INCLUDE_DIR = include
# ...
export SCE = SCE
# final variable, for installation
INCLUDE = $(INCLUDE_DIR)/$(SCE)


# libs
LDFLAGS = -lm -lGL -lGLU -lIL -lILU
ifeq ($(USE_CG),1)
	LDFLAGS += -lCg -lCgGL
endif

# static library name
STATIC_LIB = libSCE.a
# shared library name (add version at building)
SHARED_LIB = libSCE.so

STATIC_LIB_LDFLAGS = -Wl,-soname=$(SHARED_LIB)

prefix = /usr/local
DESTDIR = $(prefix)
# directory for installation of static/dynamics libraries files
INSTALL_LIBDIR = $(DESTDIR)/lib
# directory for installation of headers files
INSTALL_HDDIR = $(DESTDIR)/include/SCE


# engine's modules directories
export UTILS = utils
export CORE = core
export INTERFACE = interface

ENGINE = src
E_UTILS = $(ENGINE)/$(UTILS)
E_CORE = $(ENGINE)/$(CORE)
E_INTERFACE = $(ENGINE)/$(INTERFACE)


all: staticlib sharedlib
	
# static library (.a)
staticlib: $(ENGINE)
	$(AR) rs $(STATIC_LIB) $(E_UTILS)/*.o $(E_CORE)/*.o $(E_INTERFACE)/*.o $(ENGINE)/*.o

sharedlib: $(ENGINE)
	$(CC) -o $(SHARED_LIB).$(VERSION) -shared $(E_UTILS)/*.o $(E_CORE)/*.o $(E_INTERFACE)/*.o $(ENGINE)/*.o $(LDFLAGS) $(SHARED_LIB_LDFLAGS)

# Generate doc
doc:
	doxygen Doxyfile

# installation
install: install-createdirs
	# copying headers files
	install -m 644 $(INCLUDE)/*.h $(INSTALL_HDDIR)/
	install -m 644 $(INCLUDE)/$(UTILS)/*.h $(INSTALL_HDDIR)/$(UTILS)/
	install -m 644 $(INCLUDE)/$(CORE)/*.h $(INSTALL_HDDIR)/$(CORE)/
	install -m 644 $(INCLUDE)/$(INTERFACE)/*.h $(INSTALL_HDDIR)/$(INTERFACE)/
	
	# copying shared and static library
	install -m 644 $(SHARED_LIB).$(VERSION) $(INSTALL_LIBDIR)
	install -m 644 $(STATIC_LIB) $(INSTALL_LIBDIR)
	
	# ldconfig do that | yno : nan elle se chie dessus...
	## make link
	cd $(INSTALL_LIBDIR) && ln -sf $(SHARED_LIB).$(VERSION) $(SHARED_LIB)
	
	# update the libraries manager...
	ldconfig -n $(INSTALL_LIBDIR)

install-createdirs:
	# verifying existence of installation directories
	test -d $(INSTALL_HDDIR) || mkdir -p $(INSTALL_HDDIR)
	test -d $(INSTALL_HDDIR)/$(UTILS) || mkdir -p $(INSTALL_HDDIR)/$(UTILS)
	test -d $(INSTALL_HDDIR)/$(CORE) || mkdir -p $(INSTALL_HDDIR)/$(CORE)
	test -d $(INSTALL_HDDIR)/$(INTERFACE) || mkdir -p $(INSTALL_HDDIR)/$(INTERFACE)
	test -d $(INSTALL_LIBDIR) || mkdir -p $(INSTALL_LIBDIR)


uninstall:
	test -d !$(INSTALL_HDDIR) || rm -rf $(INSTALL_HDDIR)
	cd $(INSTALL_LIBDIR) && test -e !$(STATIC_LIB) || rm -f $(STATIC_LIB) \
	&& test -h !$(SHARED_LIB) || rm -f $(SHARED_LIB) \
	&& test -e !$(SHARED_LIB).$(VERSION) || rm -f $(SHARED_LIB).$(VERSION)
	ldconfig -n $(INSTALL_LIBDIR)

# build modules
$(ENGINE):
	cd $@ && $(MAKE)

clean:
	rm -f $(SHARED_LIB).$(VERSION)
	rm -f $(STATIC_LIB)
	cd $(ENGINE) && $(MAKE) $@

distclean: clean

.PHONY: all clean distclean staticlib sharedlib install $(ENGINE) doc
