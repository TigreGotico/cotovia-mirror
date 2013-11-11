#
#Cotovia: Text-to-speech system for Galician and Spanish languages.

#Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
# and the Center 'Ramón Piñeiro' for Research in Humanities.
#http://webs.uvigo.es/gtm_voz
#http://www.cirp.es

#Copyright: 1996-2012  Multimedia Technologies Group, University of Vigo, Spain
#           1996-2012 Centro Ramon Pineiro para a Investigación en Humanidades,
#                     Xunta de Galicia, Santiago de Compostela, Spain

#License: GPL-3.0+
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# .
# This package is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# .
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
# .
# On Debian systems, the complete text of the GNU General
# Public License version 3 can be found in /usr/share/common-licenses/GPL-3.

#______________________________________________________________________________


vpath %.hpp include
#VERSION:=$(shell  LC_ALL=C;cat ../COPYRIGHT_and_LICENSE.txt >include/version.hpp;printf "const char *cotovia_version=\"0.5.git~%s\";\nconst char *cotovia_date=\"%s\";\nconst char *cotovia_compilation=\"Compiled %s using %s\";" "`git log --abbrev-commit |head -n1 |cut -d' ' -f2 |tr -d '\n'`" "`git log | sed -n '3p'`" "`date| tr -d '\n'`" "`gcc --version| head -n1 |tr -d '\n'`" >> include/version.hpp)
VERSION:=$(shell  LC_ALL=C;cat ../COPYRIGHT_and_LICENSE.txt >include/version.hpp;printf "const char *cotovia_version=\"0.5\";\nconst char *cotovia_date=\"%s\";\nconst char *cotovia_compilation=\"Compiled %s using %s\";" "June 2012" "`date| tr -d '\n'`" "`gcc --version| head -n1 |tr -d '\n'`" >> include/version.hpp)
DEBUG=$(findstring -dbg,$(MAKECMDGOALS))
CROSS_COMPILE = $(findstring -win32,$(MAKECMDGOALS))

CFLAGS = $(DEFS) $(INCLUDE)
LFLAGS = --nounistd
crosscompiler=i586-mingw32msvc-
#crosscompiler=i686-w64-mingw32-
#crosscompiler=x86_64-w64-mingw32-
#PROFILE = yes
#USE_STLPORT = yes
STATIC_LIB = yes

ifeq ($(MAKECMDGOALS),)
TARGET=cotovia
else
TARGET=$(MAKECMDGOALS)
endif
EXEC=cotovia

ifeq ($(CROSS_COMPILE),-win32)
HOSTTYPE:=i686 
#HOSTTYPE := $(shell uname -m)
CC=$(crosscompiler)gcc
CXX=$(crosscompiler)g++
AR=$(crosscompiler)ar
RANLIB=$(crosscompiler)ranlib
#LDFLAGS += -m32
WIN32 = yes
else
HOSTTYPE := $(shell uname -m)
AR=ar
RANLIB=ranlib
CC=gcc
CXX=g++
endif

ifeq ($(HOSTTYPE),x86_64)
arch = x86_64
bindir = ../../bin64
libdir = ../../lib64
DEFS += -D_x86_64
else
arch = i686
bindir = ../../bin
libdir = ../../lib
endif

ifdef USE_STLPORT
CFLAGS  += -pthread
LDLIBS  += -lstlport
INCLUDE += -I/usr/include/stlport
endif

ifdef PROFILE #para utilizar el gprof
CFLAGS += -pg
CFLAGS += --coverage
#CFLAGS += -fbranch-probabilities
LDFLAGS += -pg
LDFLAGS += --coverage
#LDFLAGS += -fbranch-probabilities
endif

ifeq ($(DEBUG),-dbg) #para depurar
DEFS += -D_DEBUG
CFLAGS += -g
CFLAGS += -O0
#LDLIBS += -lm
dbg =-dbg
dist = debug
else #optimización
dist = release
CFLAGS += -O3 -fpermissive
#CFLAGS += -funroll-all-loops
ifeq ($(HOSTTYPE),x86_64)
#CFLAGS += -march=core2
CFLAGS += -march=nocona
#CFLAGS += -march=opteron
else
CFLAGS += -march=i686
#CFLAGS += -m32
#LDFLAGS += -m32
endif
CFLAGS += -ffast-math# -ggdb3
#CFLAGS +=  -finline-limit=8192
#CFLAGS += -fprofile-arcs -ftest-coverage
#LDFLAGS += -fprofile-arcs -ftest-coverage
#CFLAGS += -fbranch-probabilities
#LDFLAGS += -fbranch-probabilities
endif

#warnings
#CFLAGS += -Wall
#CFLAGS += -Wextra
#CFLAGS += -Winline
#CFLAGS += -Wno-write-strings
#CFLAGS += -pedantic

INCLUDE = -Iinclude -I../util/include/utf8cpp
#LDFLAGS =
#LDFLAGS += -Wl,--unresolved-symbols=ignore-in-object-files
#LDFLAGS += -static
LDLIBS += -lm
LDLIBS += -lstdc++
LEX = flex
OBJECTS = $(SOURCES:%.cpp=$(arch)/$(dist)/$(TARGET)/%.o)
SOURCES := $(wildcard *.cpp)
YACC = bison

ifeq ($(CROSS_COMPILE),-win32)
win = -win32
INCLUDE += -I../include -I../include/utf8cpp
LDLIBS += -liconv -lpcreposix -lpcre -lwinmm
LDFLAGS += -L../../lib/win32 -mwin32 -mconsole
DLLWIN32FLAGS += -Wl,--output-def,$(libdir)/$@$(win)$(dbg).def,--out-implib,$(libdir)/$@$(win)$(dbg).lib
CFLAGS += -mwin32 -mconsole -fpermissive
DEFS = -D_WIN32
dlext =.dll
execext = .exe
else
LDLIBS += -lasound
dlext = .so
endif

cotovia-hts$(win)$(dbg): DEFS += -D_MODO_NORMAL -D_SOLO_PAUSAS_ORTOGRAFICAS
cotovia$(win)$(dbg): DEFS += -D_MODO_NORMAL
cotovia$(win)$(dbg) cotovia-hts$(win)$(dbg): dirs $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(bindir)/$@$(execext)

ifndef WIN32
libcotovia$(win)$(dbg):CFLAGS += -fpic
endif
libcotovia$(win)$(dbg):DEFS += -D_DLL -D_MODO_NORMAL
libcotovia$(win)$(dbg):dirs $(OBJECTS)
ifdef STATIC_LIB
	$(AR) rcus $(libdir)/$@.a $(OBJECTS)
	$(RANLIB) $(libdir)/$@.a
else
	$(CC) -shared -Wl,-soname,$@$(dlext) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(libdir)/$@$(dlext) $(DLLWIN32FLAGS)
endif


libcotovia-is$(dbg):CFLAGS += -fPIC
libcotovia-is$(dbg):DEFS += -D_DLL -D_SCRIPT -D_SOLO_PAUSAS_ORTOGRAFICAS
libcotovia-ds$(dbg):CFLAGS += -fPIC
libcotovia-ds$(dbg):DEFS += -D_DLL -D_PARA_ELISA -D_SOLO_PAUSAS_ORTOGRAFICAS
libcotovia-ga$(dbg):CFLAGS += -fPIC
libcotovia-ga$(dbg):DEFS += -D_DLL -D_GRUPOS_ACENTUALES -D_SOLO_PAUSAS_ORTOGRAFICAS
libcotovia-is$(dbg) libcotovia-ds$(dbg) libcotovia-ga$(dbg):dirs $(OBJECTS)
ifdef STATIC_LIB
	$(AR) cru $(libdir)/$@$(win).a $(OBJECTS)
	$(RANLIB) $(libdir)/$@$(win).a
else
	$(CC) -shared -Wl,-soname,$@$(win)$(dlext) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(libdir)/$@$(win)$(dlext) $(DLLWIN32FLAGS)
endif

cotovia_genera_corpus_prosodico$(win)$(dbg): DEFS += -D_CORPUS_PROSODICO -D_SOLO_PAUSAS_ORTOGRAFICAS
cotovia_genera_corpus$(win)$(dbg): DEFS += -D_GENERA_CORPUS
cotovia_estadistica$(win)$(dbg): DEFS += -D_ESTADISTICA -D_MODO_NORMAL
cotovia_script_duracion$(win)$(dbg): DEFS += -D_SCRIPT_DURACION -D_SOLO_PAUSAS_ORTOGRAFICAS
cotovia_duracion_silabas$(win)$(dbg): DEFS += -D_PARA_ELISA -D_SOLO_PAUSAS_ORTOGRAFICAS
cotovia_energia$(win)$(dbg): DEFS += -D_CALCULA_DATOS_MODELO_ENERGIA -D_CALCULA_DISTANCIAS_CEPSTRALES -D_SOLO_PAUSAS_ORTOGRAFICAS
cotovia_energia_RN$(win)$(dbg): DEFS += -D_ESTIMA_ENERGIA_RED_NEURONAL -D_CALCULA_DATOS_MODELO_ENERGIA -D_SOLO_PAUSAS_ORTOGRAFICAS
cotovia_grupos_acentuales$(win)$(dbg): DEFS += -D_GRUPOS_ACENTUALES -D_SOLO_PAUSAS_ORTOGRAFICAS
cotovia_informacion_semifonemas$(win)$(dbg): DEFS += -D_SCRIPT -D_SOLO_PAUSAS_ORTOGRAFICAS
cotovia_genera_corpus_prosodico$(win)$(dbg) cotovia_genera_corpus$(win)$(dbg) cotovia_estadistica$(win)$(dbg) cotovia_script_duracion$(win)$(dbg) cotovia_duracion_silabas$(win)$(dbg) cotovia_energia$(win)$(dbg) cotovia_energia_RN$(win)$(dbg) cotovia_grupos_acentuales$(win)$(dbg) cotovia_informacion_semifonemas$(win)$(dbg):dirs $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(bindir)/$@$(execext)

variantes.tab.cpp variantes.tab.hpp:variantes.ypp
	$(YACC) --defines=include/variantes.tab.hpp variantes.ypp

lex.yy.cpp:leevariantes.lex variantes.tab.hpp
	$(LEX) $(LFLAGS) -olex.yy.cpp leevariantes.lex

dep/$(dist)/$(TARGET)/%.d:%.cpp lex.yy.cpp
	@mkdir -p dep/$(dist)/$(TARGET)
	@$(CC) -MM $(INCLUDE) $< | sed "s/\($*\)\.o[ :]*/$(arch)\/$(dist)\/$(TARGET)\/\1.o dep\/$(dist)\/$(TARGET)\/$< : /g" > $@

$(arch)/$(dist)/$(TARGET)/%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all$(dbg):
	@make cotovia_estadistica$(win)$(dbg);make cotovia_script_duracion$(win)$(dbg);make cotovia_duracion_silabas$(win)$(dbg);make cotovia_grupos_acentuales$(win)$(dbg);make cotovia_informacion_semifonemas$(win)$(dbg);make cotovia_energia$(win)$(dbg);make cotovia_energia_RN$(win)$(dbg);make cotovia$(win)$(dbg)

help:
	@printf "\nTargets:\n\
      \n\tmake, make $(EXEC) : (default) Compila $(EXEC) con optimización \
      \n\tmake $(EXEC)-dbg : Compila sin optimizar y con información de depuracion \
      \n\tmake $(EXEC)-win32 : Compila para windows 32 (cross-compiler)\
      \n\tmake $(EXEC)-win32-dbg : Idem pero sin optimizar y con info de depuración\
			\n\tmake dirs : Crea directorios para objetos, dependencias y binarios\
      \n\tmake clean : Limpia objetos\
			\n\tmake cleandep : Limpia dependencias\
      \n\tmake cleanall : Limpia objetos y dependencias\
      \n\tmake help : Esta ayuda\n"



.PHONY: dirs
dirs:
	@mkdir -p dep/$(dist)/$(TARGET)
	@mkdir -p $(arch)/$(dist)/$(TARGET)
	@mkdir -p $(bindir)
	@mkdir -p $(libdir)

.PHONY: cleandep$(dbg)
cleandep$(dbg):
	rm -rf dep/$(dist)/*

.PHONY: clean$(dbg)
clean$(dbg):
	rm -rf $(arch)/$(dist)/*

.PHONY: cleanall$(dbg)
cleanall$(dbg): cleandep$(dbg) clean$(dbg)

.PHONY: distclean
distclean:
	rm -rf dep $(arch)

ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
ifneq ($(findstring help,$(MAKECMDGOALS)),help)
include $(addprefix dep/$(dist)/$(TARGET)/, $(SOURCES:.cpp=.d))
endif
endif

