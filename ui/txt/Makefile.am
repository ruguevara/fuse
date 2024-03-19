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

if UI_TXT

fuse_SOURCES += $(ui_txt_files)

BUILT_SOURCES += $(ui_txt_files)

endif

CLEANFILES += $(ui_txt_files)

ui_txt_files = \
		ui/txt/txt_ui.c \
		ui/txt/ui_io.c \
                ui/txt/keysyms.c
                # ui/txt/options.c

# ui_txt_files = \
#                 ui/txt/options.c

# EXTRA_DIST += \
#               ui/txt/options.pl \
#               ui/txt/options-header.pl

# ui/txt/options.c: $(srcdir)/perl/cpp-perl.pl config.h $(srcdir)/ui/txt/options.pl $(srcdir)/ui/options.dat $(srcdir)/perl/Fuse.pm $(srcdir)/perl/Fuse/Dialog.pm
# 	@$(MKDIR_P) ui/txt
# 	$(AM_V_GEN)$(PERL) $(srcdir)/perl/cpp-perl.pl config.h $(srcdir)/ui/options.dat | $(PERL) -I$(srcdir)/perl $(srcdir)/ui/txt/options.pl - > $@.tmp && mv $@.tmp $@


# ui_txt_files = ui/txt/keysyms.c

# ui/txt/keysyms.c: $(srcdir)/keysyms.pl $(srcdir)/keysyms.dat
# 	@$(MKDIR_P) ui/txt
# 	$(AM_V_GEN)$(PERL) -I$(srcdir)/perl $(srcdir)/keysyms.pl txt $(srcdir)/keysyms.dat > $@.tmp && mv $@.tmp $@
