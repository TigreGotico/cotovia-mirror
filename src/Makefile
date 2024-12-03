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


BIN=$(DESTDIR)/usr/bin
ifeq ($(shell uname -m),x86_64)
bindir=../bin64
else
bindir=../bin
endif
cotovia-exec:
	$(MAKE) -C cotovia
	$(MAKE) -C cotovia libcotovia
	$(MAKE) -C proyectos/libecess-common/ libecess-common
	$(MAKE) -C proyectos/cotovia-ecess-m1

install: cotovia-exec
	install -d $(BIN)
	install -m755  $(bindir)/cotovia $(BIN)
	install -m755  $(bindir)/cotovia-ecess-m1 $(BIN)

debian-packages:
	debuild -i\.git -I.git -us -uc -b
	make -C ../data/lang
	make -C ../data/iago
	make -C ../data/sabela
	make -C ../data/sabela-large
cleanall:
	$(MAKE) -C cotovia cleanall
	$(MAKE) -C proyectos cleanall
	$(MAKE) -C cotovia cleanall-dbg
	$(MAKE) -C proyectos cleanall-dbg

distclean:
	$(MAKE) -C cotovia distclean
	$(MAKE) -C proyectos distclean

