## Process this file with automake to produce Makefile.in
## Copyright (c) 2001-2017 Philip Kendall, Sergio Baldoví

## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##
## Author contact information:
##
## E-mail: philip-fuse@shadowmagic.org.uk

if UI_SWIG

fuse_SOURCES += $(ui_swig_files)

BUILT_SOURCES += $(ui_swig_built)

endif

CLEANFILES += $(ui_swig_built)

ui_swig_files = \
		ui/swig/swig_ui.c \
		ui/swig/ui_io.c \
                ui/swig/keysyms.c
                # ui/swig/options.c

# ui_swig_built = \
#                 ui/swig/options.c

# EXTRA_DIST += \
#               ui/swig/options.pl \
#               ui/swig/options-header.pl

# ui/swig/options.c: $(srcdir)/perl/cpp-perl.pl config.h $(srcdir)/ui/swig/options.pl $(srcdir)/ui/options.dat $(srcdir)/perl/Fuse.pm $(srcdir)/perl/Fuse/Dialog.pm
# 	@$(MKDIR_P) ui/swig
# 	$(AM_V_GEN)$(PERL) $(srcdir)/perl/cpp-perl.pl config.h $(srcdir)/ui/options.dat | $(PERL) -I$(srcdir)/perl $(srcdir)/ui/swig/options.pl - > $@.tmp && mv $@.tmp $@


# ui_swig_built = ui/swig/keysyms.c

# ui/swig/keysyms.c: $(srcdir)/keysyms.pl $(srcdir)/keysyms.dat
# 	@$(MKDIR_P) ui/swig
# 	$(AM_V_GEN)$(PERL) -I$(srcdir)/perl $(srcdir)/keysyms.pl swig $(srcdir)/keysyms.dat > $@.tmp && mv $@.tmp $@
