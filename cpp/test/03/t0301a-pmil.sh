#!/bin/sh
#
# aegis - project change supervisor.
# Copyright (C) 2012 Peter Miller
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.
#

TEST_SUBJECT="aemakegen, vs nlsdir and i18n"

# load up standard prelude and test functions
. test_funcs

AEGIS_PROJECT=projname
export AEGIS_PROJECT

activity="new project 28"
aegis -npr ${AEGIS_PROJECT} -version - -v -dir $work/proj.dir \
    -lib $AEGIS_PATH > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="project attributes 33"
cat > paf << fubar
developer_may_review = true;
developer_may_integrate = true;
reviewer_may_integrate = true;
default_test_exemption = true;
develop_end_action = goto_awaiting_integration;
default_development_directory = "$work";
fubar
if test $? -ne 0 ; then no_result; fi
aegis -pa -f paf -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="staff 46"
aegis -nd $USER -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi
aegis -nrv $USER -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi
aegis -ni $USER -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="new change 54"
cat > caf << 'fubar'
brief_description = "one";
cause = internal_enhancement;
test_baseline_exempt = true;
fubar
if test $? -ne 0 ; then no_result; fi
aegis -nc -f caf -v -p $AEGIS_PROJECT > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="develop begin 64"
aegis -db 10 -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="new file 68"
aegis -nf \
        $work/${AEGIS_PROJECT}.C010/aegis.conf \
        $work/${AEGIS_PROJECT}.C010/configure.ac \
        $work/${AEGIS_PROJECT}.C010/example/main.cc \
        $work/${AEGIS_PROJECT}.C010/lib/en/LC_MESSAGES/example.po \
        $work/${AEGIS_PROJECT}.C010/lib/en/LC_MESSAGES/common.po \
        $work/${AEGIS_PROJECT}.C010/lib/fr/LC_MESSAGES/example.po \
        -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

cat > $work/${AEGIS_PROJECT}.C010/configure.ac << 'fubar'
AC_CHECK_PROGS(MSGFMT, gmsgfmt msgfmt)
AC_SUBST(NLSDIR)
fubar
if test $? -ne 0 ; then no_result; fi

cat > $work/${AEGIS_PROJECT}.C010/aegis.conf << 'fubar'
build_command = "exit 0";

history_get_command = "aesvt -check-out -edit ${quote $edit} "
    "-history ${quote $history} -f ${quote $output}";
history_put_command = "aesvt -check-in -history ${quote $history} "
    "-f ${quote $input}";
history_query_command = "aesvt -query -history ${quote $history}";
history_content_limitation = binary_capable;

diff_command = "set +e; diff $orig $i > $out; test $$? -le 1";
diff3_command = "(diff3 -e $mr $orig $i | sed -e '/^w$$/d' -e '/^q$$/d'; \
    echo '1,$$p' ) | ed - $mr > $out";
link_integration_directory = true;

test_command = "$sh $filename $arch";

project_specific =
[
    {
        name = "aemakegen:debian:maintainer";
        value = "maintainer@example.com";
    },
    {
        name = "aemakegen:libtool";
        value = "true";
    },
];
fubar
if test $? -ne 0 ; then no_result; fi

TAB=`awk 'BEGIN{printf("%c", 9)}' /dev/null`

# ----------  makefile  ----------------------------------------------------

activity="aemakegen 118"
aemakegen -c 10 --target=makefile > test.out
if test $? -ne 0 ; then fail; fi

activity="check makefile 122"
sed "s|{TAB}|${TAB}|g" > ok << 'fubar'
#
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
#    W   W    A    RRRR   N   N   III  N   N  III  N   N   GGG
#    W   W   A A   R   R  NN  N    I   NN  N   I   NN  N  G   G
#    W   W  A   A  RRRR   N N N    I   N N N   I   N N N  G
#    W W W  AAAAA  R R    N  NN    I   N  NN   I   N  NN  G  GG
#    W W W  A   A  R  R   N   N    I   N   N   I   N   N  G   G
#     W W   A   A  R   R  N   N   III  N   N  III  N   N   GGG
#
# Warning: DO NOT send patches which fix this file. IT IS NOT the original
# source file. This file is GENERATED from the Aegis repository file manifest.
# If you find a bug in this file, it could well be an Aegis bug.
#
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#

#
# @configure_input@
#
# The configure script generates 2 files:
# 1. This Makefile
# 2. /config.h
# If you change this Makefile, you may also need to change these files. To see
# what is configured by the configure script, search for @ in the Makefile.in
# file.
#
# If you wish to reconfigure the installations directories it is RECOMMENDED
# that you re-run the configure script.
#
# Use "./configure --help" for a list of options.
#

#
# directory containing the source
#
srcdir = @srcdir@
VPATH = @srcdir@

#
# the name of the install program to use
#
INSTALL = @INSTALL@
INSTALL_PROGRAM = @INSTALL_PROGRAM@
INSTALL_DATA = @INSTALL_DATA@
INSTALL_DIR = @INSTALL@ -m 0755 -d

#
# The name of the C++ compiler to use.
#
CXX = @CXX@

#
# The C++ compiler flags to use.
#
CXXFLAGS = @CXXFLAGS@

#
# The C++ preprocessor flags to use.
#
CPPFLAGS = @CPPFLAGS@

#
# The linker flags to use
#
LDFLAGS = @LDFLAGS@

#
# prefix for installation path
#
prefix = @prefix@
exec_prefix = @exec_prefix@

#
# Where to put the executables.
#
# On a network, this would only be shared between machines of identical
# cpu-hw-os flavour. It can be read-only.
#
# The $(DESTDIR) is for packaging.
#
bindir = $(DESTDIR)@bindir@

# define this to silence ./configure warning
datarootdir = $(DESTDIR)@datarootdir@

#
# ${NLSDIR} is for locale files
#
# This is where the binary error message translation files (.mo) are to be
# installed.
#
# The $(DESTDIR) is for packaging.
#
NLSDIR = $(DESTDIR)@NLSDIR@

#
# The program to be used to translate source error message translation .po files
# into binary error message translation .mo files. It is usually called "msgfmt"
# or "gmsgfmt".
#
MSGFMT = @MSGFMT@

#
# The program to be used to join, combine and merge source error message
# translation .po files. It is usually called "msgcat" or "gmsgcat".
#
MSGCAT = @MSGCAT@

#
# The name of the GNU Libtool command.
#
LIBTOOL = @LIBTOOL@

#
# extra libraries required for your system
#
LIBS = @LIBS@

#
# shell to use to run tests and commands
#
SH = @SH@

# ---------------------------------------------------------
# You should not need to change anything below this line.

#
# The default target
#
the-default-target: all

example/main.lo example/main.o: example/main.cc
{TAB}$(LIBTOOL) --mode=compile --tag=CXX $(CXX) $(CPPFLAGS) $(CXXFLAGS) -I. \
{TAB}{TAB}-c example/main.cc -o example/main.lo

lib/en/LC_MESSAGES/example.mo: lib/en/LC_MESSAGES/common.po \
{TAB}{TAB}lib/en/LC_MESSAGES/example.po
{TAB}$(MSGFMT) -o $@ lib/en/LC_MESSAGES/example.po \
{TAB}{TAB}lib/en/LC_MESSAGES/common.po

$(NLSDIR)/en/LC_MESSAGES/example.mo: .mkdir.__NLSDIR__en_LC_MESSAGES \
{TAB}{TAB}lib/en/LC_MESSAGES/example.mo
{TAB}$(INSTALL_DATA) lib/en/LC_MESSAGES/example.mo $@

lib/fr/LC_MESSAGES/example.mo: lib/fr/LC_MESSAGES/example.po
{TAB}$(MSGFMT) -o $@ lib/fr/LC_MESSAGES/example.po

$(NLSDIR)/fr/LC_MESSAGES/example.mo: .mkdir.__NLSDIR__fr_LC_MESSAGES \
{TAB}{TAB}lib/fr/LC_MESSAGES/example.mo
{TAB}$(INSTALL_DATA) lib/fr/LC_MESSAGES/example.mo $@

.mkdir.__NLSDIR_:
{TAB}-$(INSTALL_DIR) $(NLSDIR)
{TAB}@-test -d $(NLSDIR) && touch $@
{TAB}@sleep 1

.mkdir.__NLSDIR__en: .mkdir.__NLSDIR_
{TAB}-$(INSTALL_DIR) $(NLSDIR)/en
{TAB}@-test -d $(NLSDIR)/en && touch $@
{TAB}@sleep 1

.mkdir.__NLSDIR__en_LC_MESSAGES: .mkdir.__NLSDIR__en
{TAB}-$(INSTALL_DIR) $(NLSDIR)/en/LC_MESSAGES
{TAB}@-test -d $(NLSDIR)/en/LC_MESSAGES && touch $@
{TAB}@sleep 1

.mkdir.__NLSDIR__fr: .mkdir.__NLSDIR_
{TAB}-$(INSTALL_DIR) $(NLSDIR)/fr
{TAB}@-test -d $(NLSDIR)/fr && touch $@
{TAB}@sleep 1

.mkdir.__NLSDIR__fr_LC_MESSAGES: .mkdir.__NLSDIR__fr
{TAB}-$(INSTALL_DIR) $(NLSDIR)/fr/LC_MESSAGES
{TAB}@-test -d $(NLSDIR)/fr/LC_MESSAGES && touch $@
{TAB}@sleep 1

.mkdir.__bindir_:
{TAB}-$(INSTALL_DIR) $(bindir)
{TAB}@-test -d $(bindir) && touch $@
{TAB}@sleep 1

#
# The example program.
#
example_obj = example/main.lo

bin/example: $(example_obj) .bin
{TAB}$(LIBTOOL) --mode=link --tag=CXX $(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ \
{TAB}{TAB}$(example_obj) $(LDFLAGS) $(LIBS)

$(bindir)/example: .mkdir.__bindir_ bin/example
{TAB}$(LIBTOOL) --mode=install $(INSTALL_PROGRAM) bin/example $@

all: all-bin all-i18n

all-bin: bin/example

all-i18n: lib/en/LC_MESSAGES/example.mo lib/fr/LC_MESSAGES/example.mo

.bin:
{TAB}-mkdir bin
{TAB}-chmod 0755 bin
{TAB}@-test -d bin && touch $@
{TAB}@sleep 1

check: sure

sure:
{TAB}@echo Passed All Tests

clean: clean-bin clean-i18n clean-misc clean-obj

clean-bin:
{TAB}rm -f bin/example

clean-i18n:
{TAB}rm -f lib/en/LC_MESSAGES/example.mo lib/fr/LC_MESSAGES/example.mo

clean-misc:
{TAB}rm -f .bin .mkdir.__NLSDIR_ .mkdir.__NLSDIR__en
{TAB}rm -f .mkdir.__NLSDIR__en_LC_MESSAGES .mkdir.__NLSDIR__fr
{TAB}rm -f .mkdir.__NLSDIR__fr_LC_MESSAGES .mkdir.__bindir_ core

clean-obj:
{TAB}rm -f example/main.lo example/main.o

distclean: clean distclean-directories distclean-files

distclean-files:
{TAB}rm -f Makefile config.cache config.log config.status

distclean-directories:
{TAB}rm -rf bin example/.libs

install: install-bin install-i18n

install-bin: $(bindir)/example

install-i18n: $(NLSDIR)/en/LC_MESSAGES/example.mo \
{TAB}{TAB}$(NLSDIR)/fr/LC_MESSAGES/example.mo

uninstall:
{TAB}rm -f $(NLSDIR)/en/LC_MESSAGES/example.mo
{TAB}rm -f $(NLSDIR)/fr/LC_MESSAGES/example.mo $(bindir)/example

.PHONY: all all-bin all-i18n check clean clean-bin clean-i18n distclean \
{TAB}{TAB}distclean-directories distclean-files install install-bin \
{TAB}{TAB}install-i18n sure the-default-target

# vim: set ts=8 sw=8 noet :
fubar
if test $? -ne 0 ; then no_result; fi

diff -u ok test.out
if test $? -ne 0 ; then fail; fi

#
# Only definite negatives are possible.
# The functionality exercised by this test appears to work,
# no other guarantees are made.
#
pass
# vim: set ts=8 sw=4 et :
