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

drillwriter_SOURCES = drill.h d_gtk.h testbench/tb.hpp testbench/util.hpp count-con.c \
		      drawfield.c drill.c \
		      entry-con.c file-ops.c main.c media.c \
		      perf-con.c set-controls.c \
		      testbench/testbench.cpp testbench/util.cpp

## Define an independent executable script for inclusion in the distribution

## archive. It will not be installed on an end user's system, however.
drillwriter_LDFLAGS = $(GTK_LIBS) -lm
drillwriter_CFLAGS = $(GTK_CFLAGS) 

dist_noinst_SCRIPTS = autogen.sh
