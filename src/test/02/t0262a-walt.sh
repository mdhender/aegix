#!/bin/sh
#
#	aegis - project change supervisor
#	Copyright (C) 2007, 2008 Walter Franzini
#	Copyright (C) 2008 Peter Miller
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 3 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program; if not, see
#	<http://www.gnu.org/licenses/>.
#

unset AEGIS_PROJECT
unset AEGIS_CHANGE
unset AEGIS_PATH
unset AEGIS
umask 022

LINES=24
export LINES
COLS=80
export COLS

USER=${USER:-${LOGNAME:-`whoami`}}

work=${AEGIS_TMP:-/tmp}/$$
PAGER=cat
export PAGER
AEGIS_FLAGS="delete_file_preference = no_keep; \
	lock_wait_preference = always; \
	diff_preference = automatic_merge; \
	pager_preference = never; \
	persevere_preference = all; \
	log_file_preference = never; \
	default_development_directory = \"$work\";"
export AEGIS_FLAGS
AEGIS_THROTTLE=-1
export AEGIS_THROTTLE

# This tells aeintegratq that it is being used by a test.
AEGIS_TEST_DIR=$work
export AEGIS_TEST_DIR

here=`pwd`
if test $? -ne 0 ; then exit 2; fi

parch=
test "$1" != "" && parch="$1/"
bin="$here/${parch}bin"

if test "$EXEC_SEARCH_PATH" != ""
then
    tpath=
    hold="$IFS"
    IFS=":$IFS"
    for tpath2 in $EXEC_SEARCH_PATH
    do
	tpath=${tpath}${tpath2}/${parch}bin:
    done
    IFS="$hold"
    PATH=${tpath}${PATH}
else
    PATH=${bin}:${PATH}
fi
export PATH

AEGIS_DATADIR=$here/lib
export AEGIS_DATADIR

#
# set the path, so that the aegis command that aepatch/aedist invokes
# is from the same test set as the aepatch/aedist command itself.
#
PATH=${bin}:$PATH
export PATH

pass()
{
	set +x
	echo PASSED 1>&2
	cd $here
	find $work -type d -user $USER -exec chmod u+w {} \;
	rm -rf $work
	exit 0
}
fail()
{
	set +x
	echo "FAILED test of the aecp -delta functionality ($activity)" 1>&2
	cd $here
	find $work -type d -user $USER -exec chmod u+w {} \;
	rm -rf $work
	exit 1
}
no_result()
{
	set +x
	echo "NO RESULT when testing the aecp -delta " \
	    "functionality ($activity)" 1>&2
	cd $here
	find $work -type d -user $USER -exec chmod u+w {} \;
	rm -rf $work
	exit 2
}
trap \"no_result\" 1 2 3 15

check_it()
{
	sed	-e "s|$work|...|g" \
		-e 's|= [0-9][0-9]*; /.*|= TIME;|' \
		-e "s/\"$USER\"/\"USER\"/g" \
		-e 's/19[0-9][0-9]/YYYY/' \
		-e 's/20[0-9][0-9]/YYYY/' \
		-e 's/node = ".*"/node = "NODE"/' \
		-e 's/crypto = ".*"/crypto = "GUNK"/' \
                -e 's/uuid = ".*"/uuid = "UUID"/' \
		< $2 > $work/sed.out
	if test $? -ne 0; then no_result; fi
	diff -U100 $1 $work/sed.out
	if test $? -ne 0; then fail; fi
}

activity="create test directory 131"
mkdir $work $work/lib
if test $? -ne 0 ; then no_result; fi
chmod 777 $work/lib
if test $? -ne 0 ; then no_result; fi
cd $work
if test $? -ne 0 ; then no_result; fi
workchan=$work/change-dir

#
# use the built-in error messages
#
AEGIS_MESSAGE_LIBRARY=$work/no-such-dir
export AEGIS_MESSAGE_LIBRARY
unset LANG
unset LANGUAGE

#
# If the C++ compiler is called something other than "c++", as
# discovered by the configure script, create a shell script called
# "c++" which invokes the correct C++ compiler.  Make sure the current
# directory is in the path, so that it will be invoked.
#
if test "$CXX" != "c++"
then
	cat >> $work/c++ << fubar
#!/bin/sh
exec ${CXX-g++} \$*
fubar
	if test $? -ne 0 ; then no_result; fi
	chmod a+rx $work/c++
	if test $? -ne 0 ; then no_result; fi
	PATH=${work}:${PATH}
	export PATH
fi

AEGIS_PATH=$work/lib
export AEGIS_PATH

#
# test the aecp -delta functionality
#
activity="new project 173"
$bin/aegis -npr test -version - -v -dir $work/proj.dir \
	-lib $AEGIS_PATH > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

AEGIS_PROJECT=test
export AEGIS_PROJECT

activity="project attributes 181"
cat > paf << fubar
developer_may_review = true;
developer_may_integrate = true;
reviewer_may_integrate = true;
default_test_exemption = true;
develop_end_action = goto_awaiting_integration;
default_development_directory = "$work";
fubar
if test $? -ne 0 ; then no_result; fi
$bin/aegis -pa -f paf -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi;

activity="staff 194"
$bin/aegis -nd $USER -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi
$bin/aegis -nrv $USER -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi
$bin/aegis -ni $USER -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="create the branch 202"
$bin/aegis -nbr -p $AEGIS_PROJECT 1 -v > log 2>&1
if test $? -ne 0; then cat log; no_result; fi

AEGIS_PROJECT=test.1
export AEGIS_PROJECT

activity="create the branch 209"
$bin/aegis -nbr -p $AEGIS_PROJECT 1 -v > log 2>&1
if test $? -ne 0; then cat log; no_result; fi

AEGIS_PROJECT=test.1.1
export AEGIS_PROJECT

#
# delta 1
#
# the following files are created:
#	aegis.conf
#
activity="create the 1st change 222"
cat > caf <<EOF
brief_description = "zero";
cause = internal_enhancement;
test_baseline_exempt = true;
EOF
if test $? -ne 0 ; then no_result; fi
$bin/aegis -nc -f caf -v -p $AEGIS_PROJECT > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="develop begin 232"
workchan=$work/change-dir-10
$bin/aegis -db 10 -dir $workchan -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="new file 237"
$bin/aegis -nf $workchan/aegis.conf -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

cat > $workchan/aegis.conf << 'fubar'
build_command = "date > derived1 && date > derived2";

history_get_command = "aesvt -check-out -edit ${quote $edit} "
    "-history ${quote $history} -f ${quote $output}";
history_put_command = "aesvt -check-in -history ${quote $history} "
    "-f ${quote $input}";
history_create_command = "aesvt -check-in -history ${quote $history} "
    "-f ${quote $input}";
history_query_command = "aesvt -query -history ${quote $history}";
history_content_limitation = binary_capable;

diff_command = "set +e; diff $orig $i > $out; test $$? -le 1";
diff3_command = "(diff3 -e $mr $orig $i | sed -e '/^w$$/d' -e '/^q$$/d'; \
	echo '1,$$p' ) | ed - $mr > $out";
link_integration_directory = true;
fubar
if test $? -ne 0 ; then no_result; fi

activity="new file 260"
$bin/aegis -nf $workchan/pippo -v > log 2>&1
if test $? -ne 0; then cat log; no_result; fi

cat > $workchan/pippo <<EOF
Pippo is the italian name of Goofy the friend of Mickey Mouse
(a.k.a Topolino).
In Italy every programmer has written some code with a variables named
pippo.
EOF
if test $? -ne 0; then no_result; fi

activity="build 272"
$bin/aegis -b -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="diff 276"
$bin/aegis -diff -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="develop end 280"
$bin/aegis -de -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="integrate begin 284"
$bin/aegis -ib -c 10 -v > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

activity="integrate 288"
$bin/aefinish -p $AEGIS_PROJECT -c 10 -v > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

$bin/aegis -de -p test.1 1 -v > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

$bin/aegis -ib -p test.1 1 -v > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

$bin/aefinish -p test.1 1 -v > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

$bin/aegis -nbr -p test.1 2 -v > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

AEGIS_PROJECT=test.1.2
export AEGIS_PROJECT

#
# delta 2
#
# * pippo is renamed to pluto
#
activity="the 2nd change 312"
cat > caf <<EOF
brief_description = "zero";
cause = internal_enhancement;
test_baseline_exempt = true;
EOF
if test $? -ne 0 ; then no_result; fi
$bin/aegis -nc 11 -f caf -v -p $AEGIS_PROJECT > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="develop begin 322"
workchan=$work/change-dir-11
$bin/aegis -db 11 -dir $workchan -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="rename a file 327"
$bin/aegis -mv $workchan/pippo $workchan/pluto -v > log 2>&1
if test $? -ne 0; then cat log; no_result; fi

cat >> $workchan/pluto <<EOF
Pluto is also a common variable name in simple programs.
EOF
if test $? -ne 0; then no_result; fi

activity="build 336"
$bin/aegis -b -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="diff 340"
$bin/aegis -diff -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="develop end 344"
$bin/aegis -de -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="integrate begin 348"
$bin/aegis -ib -c 11 -v > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

activity="integrate 352"
$bin/aefinish -p $AEGIS_PROJECT -c 11 > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

#
# delta 3
#
# * modify pluto
#
activity="the 3rd change 361"
cat > caf <<EOF
brief_description = "zero";
cause = internal_enhancement;
test_baseline_exempt = true;
EOF
if test $? -ne 0 ; then no_result; fi
$bin/aegis -nc 12 -f caf -v -p $AEGIS_PROJECT > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="develop begin 371"
workchan=$work/change-dir-12
$bin/aegis -db 12 -dir $workchan -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="modify pluto 376"
$bin/aegis -cp $workchan/pluto -v > log 2>&1
if test $? -ne 0; then no_result; fi

cat >> $workchan/pluto <<EOF
Some random line added.
EOF
if test $? -ne 0; then no_result; fi

activity="build 385"
$bin/aegis -b -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="diff 389"
$bin/aegis -diff -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="develop end 393"
$bin/aegis -de -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="integrate begin 397"
$bin/aegis -ib -c 12 -v > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

activity="integrate 401"
$bin/aefinish -p $AEGIS_PROJECT > log 2>&1
if test $? -ne 0 ; then cat log ; no_result; fi

#
# * delete pluto
#
activity="new change 408"
cat > caf <<EOF
brief_description = "zero";
cause = internal_enhancement;
test_baseline_exempt = true;
EOF
if test $? -ne 0 ; then no_result; fi
$bin/aegis -nc 13 -f caf -v -p $AEGIS_PROJECT > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

activity="develop begin 418"
workchan=$work/change-dir-13
$bin/aegis -db 13 -dir $workchan -v > log 2>&1
if test $? -ne 0 ; then cat log; no_result; fi

$bin/aegis -rm $workchan/pluto -v -nl > log 2>&1
if test $? -ne 0; then cat log; no_result; fi

activity="develop end 426"
$bin/aefinish 13 -v > log 2>&1
if test $? -ne 0; then cat log; no_result; fi

activity="integrate begin 430"
$bin/aegis -ib 13 -v > log 2>&1
if test $? -ne 0; then cat log; no_result; fi

activity="diff the change 434"
$bin/aegis -diff 13 -v > log 2>&1
if test $? -ne 0; then cat log; fail; fi

#
# Only definite negatives are possible.
# The functionality exercised by this test appears to work,
# no other guarantees are made.
#
pass
