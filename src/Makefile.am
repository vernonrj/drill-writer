## Drillwriter program

## Additional flags to pass to aclocal when it is invoked automatically at

## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment

## to provide a way for the user to supply additional arguments.

ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

## Define an executable target "drillwriter", which will be installed into the

## directory named by the predefined variable $(bindir).

bin_PROGRAMS = drillwriter

## Define the list of source files for the "drillwriter" target. The file extension

## .cc is recognized by Automake, and causes it to produce rules which invoke

## the C compiler to produce an object file (.o) from each source file. The

## header files (.h) do not result in object files by themselves, but will be

## included in distribution archives of the project.

drillwriter_SOURCES = drillwriter.h gtk-drill/drillwriter_gtk.h bsd-queue.h dr_select.h dr_stepfield.h \
		      dr_forms.h dr_fieldrel.h dr_form_parent.h \
		      selection/dr_stepfield.c selection/dr_select.c \
		      coords.c relations/dr_fieldrel.c file-ops.c \
		      drillwriter.c perf-con.c forms/dr_forms.c forms/dr_form_parent.c group.c\
		      set-controls.c tempo.c undo.c \
		      gtk-drill/file-ops-gtk.c gtk-drill/coords-gtk.c\
		      gtk-drill/main-gtk.c gtk-drill/perf-con-gtk.c \
		      gtk-drill/mouse_gtk.c gtk-drill/dr-zoom-gtk.c \
		      gtk-drill/dr-drill-class.h gtk-drill/dr-drill-class.c \
		      gtk-drill/dr-setbar.h gtk-drill/dr-setbar.c \
		      gtk-drill/dr-media.h gtk-drill/dr-media.c \
		      gtk-drill/dr-perfbar.h gtk-drill/dr-perfbar.c \
		      gtk-drill/dr-sidebar.h gtk-drill/dr-sidebar.c \
		      gtk-drill/dr-sidebar-sets.h gtk-drill/dr-sidebar-sets.c \
		      gtk-drill/dr-sidebar-perfs.h gtk-drill/dr-sidebar-perfs.c \
		      gtk-drill/dr-sidebar-groups.h gtk-drill/dr-sidebar-groups.c \
		      gtk-drill/dr-group-cell.h gtk-drill/dr-group-cell.c 
##testbench/tb.hpp testbench/util.hpp \
##testbench/testbench.cpp testbench/util.cpp

## Define an independent executable script for inclusion in the distribution

## archive. It will not be installed on an end user's system, however.
drillwriter_LDFLAGS = $(GTK_LIBS) -lm -Wall
drillwriter_CFLAGS = $(GTK_CFLAGS) -Wall

dist_noinst_SCRIPTS = autogen.sh

EXTRA_DIST = menu-ui.xml
