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


SUBDIRS = libecess-common cotovia-ecess-m1 


.PHONY : all dbg win32 win32-dbg $(SUBDIRS) clean clean-dbg cleanall cleanall-dbg
all: $(SUBDIRS)

dbg: dbg=-dbg
dbg: $(SUBDIRS)

win32: win=-win32
win32: $(SUBDIRS)

win32-dbg: win=-win32
win32-dbg: dbg=-dbg
win32-dbg: $(SUBDIRS)

help:
	@printf "Targets:\
		\n\tmake, make all: (default) Compila todos los proyectos con optimización \
		\n\tmake dbg: Compila sin optimizar y con información de depuracion \
		\n\tmake win32: Compila para windows 32 (cross-compiler)\
		\n\tmake win32-dbg: Idem pero sin optimizar y con info de depuración\
		\n\tmake clean: Limpia objetos\
		\n\tmake clean-dbg: Limpia objetos (version depuracion)\
		\n\tmake cleanall: Limpia objetos y dependencias\
		\n\tmake cleanall-dbg: Limpia objetos y dependencias (version depuracion)\
		\n\tmake help: Esta ayuda\n"

$(SUBDIRS):
	$(MAKE) -C $@ $@$(win)$(dbg)

clean:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done
cleanall:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir cleanall; done
clean-dbg:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean-dbg; done
cleanall-dbg:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir cleanall-dbg; done
distclean:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir distclean; done



