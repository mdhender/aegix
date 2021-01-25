#!/bin/sh
#
# aegis - project change supervisor.
# Copyright (C) 2008, 2011, 2012 Peter Miller
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

TEST_SUBJECT="fmtgen, time"

# load up standard prelude and test functions
. test_funcs

#
# test the functionality
#
activity="fmtgen, time, incl"
cat > test.def << 'fubar'
fred = time;
fubar
test $? -eq 0 || no_result

cat > test.fmtgen.h.ok << 'fubar'
//
// This file is generated by fmtgen.
//

#ifndef TEST_FMTGEN_H
#define TEST_FMTGEN_H
#include <common/ac/time.h>
#include <libaegis/introspector.h>
#include <libaegis/output.h>

#ifndef test_DEF
#define test_DEF

/**
* The test class is used to represent ''test'' formatted Aegis meta-data.
* See aetest(5) for more information.
*/
class test
{
public:
    /**
    * The pointer typedef is used to have a pointer type which may be used
    * by clients of this API, allowing the pointer implementation to be
    * changed without extensive code changes throughout the rest of the
    * code base.
    */
    typedef aegis_shared_ptr<test> pointer;

    /**
    * The destructor.
    */
    virtual ~test();

private:
    /**
    * The default constructor. It is private on purpose, use the #create
    * class method instead.
    */
    test();

public:
    /**
    * The create class method is used to create new dynamically allocated
    * instances of this class.
    */
    static pointer create();

private:
    /**
    * The copy constructor. It is private on purpose, use the #create class
    * method instead.
    *
    * @param rhs
    *     The right hand side of the initialization.
    */
    test(const test &rhs);

public:
    /**
    * The create class method is used to create new dynamically allocated
    * instances of this class.
    *
    * @param rhs
    *     The right hand side of the initialization.
    */
    static pointer create(const test &rhs);

    /**
    * The assignment operator.
    *
    * @param rhs
    *     The right hand side of the assignment.
    */
    test &operator=(const test &rhs);

private:
    /**
    * The fred instance variable is used to remember the time fred.
    */
    time_t fred;

public:
    /**
    * The fred_set method is used to set the #fred instance variable, and
    * remember that it has been set.
    *
    * @param n
    *     The new value.
    */
    void fred_set(time_t n);

    /**
    * The fred_get method is used to obtain the value of the #fred instance
    * variable, or 0 if it has not been set.
    */
    time_t fred_get() const;

    /**
    * The fred_is_set method is used to determine whether or not the #fred
    * instance variable is set.
    */
    bool fred_is_set() const;

    /**
    * The fred_clear method is used to clear the value of the #fred
    * instance variable, and remember that it is no longer set.
    */
    void fred_clear();

    /**
    * The fred_introspector_factory method is used to obtain an
    * introspector for the #fred instance variable.
    */
    introspector::pointer fred_introspector_factory();

public:
    /**
    * The introspector_factory method is used to create an introspector for
    * this class and instance, usually used with the meta-data parser when
    * reading the contents of a meta-data file.
    */
    introspector::pointer introspector_factory();

    /**
    * The write method is used to write this structure's contents to the
    * given output.
    *
    * @param fp
    *     Where to write the output.
    */
    void write(const output::pointer &fp) const;

    /**
    * The write_xml method is used to write this structure's contents to
    * the given output as XML.
    *
    * @param fp
    *     Where to write the output.
    */
    void write_xml(const output::pointer &fp) const;

    /**
    * The trace_print method is used by the trace_test macro to print an
    * object of this type.
    *
    * @param name
    *     The name of the variable being printed.
    */
    void trace_print(const char *name) const;

    /**
    * The create_from_file class method is used to read the given file to
    * create a new dynamically allocated instance of this class.
    *
    * @param filename
    *     The name of the file to be read.
    * @note
    *     If any errors are encountered, this method will not return. All
    *     errors will print a fatal error message, and exit with an exit
    *     status of 1.
    */
    static pointer create_from_file(const nstring &filename);

    /**
    * The write_file method is used to write the data to the named file,
    * via the #write method.
    *
    * @param filename
    *     The name of the file to be written.
    * @param compress
    *     true if data should be compressed, false if not.
    * @note
    *     If any errors are encountered, this method will not return. All
    *     errors will print a fatal error message, and exit with an exit
    *     status of 1.
    */
    void write_file(const nstring &filename, bool compress) const;

    /**
    * The report_init class method is used to initialize the report
    * generator with the names and values of enumerations used by this
    * meta-data.
    */
    static void report_init();
};

/**
* The test_trace macro is used to conditionally print a trace of a test
* value, if the DEBUG macro is defined.
*/
#include <common/debug.h>
#ifndef test_trace
#ifdef DEBUG
#define test_trace(x) ((void)(trace_pretest_ && (trace_where_, (x).trace_print(trace_stringize(x)), 0)))
#else
#define test_trace(x)
#endif
#endif

#endif // test_DEF

#endif // TEST_FMTGEN_H
fubar
test $? -eq 0 || no_result

fmtgen -ii test.fmtgen.h test.def
test $? -eq 0 || fail

diff test.fmtgen.h.ok test.fmtgen.h
test $? -eq 0 || fail

activity="fmtgen, time, code"
cat > test.fmtgen.cc.ok << 'fubar'
//
// This file is generated by fmtgen.
//
#include <test.fmtgen.h>
#include <common/ac/assert.h>
#include <common/trace.h>

test::~test()
{
    trace(("%s\n", __PRETTY_FUNCTION__));
}

test::test() :
    fred(0)
{
    trace(("%s\n", __PRETTY_FUNCTION__));
}

test::pointer
test::create()
{
    trace(("%s\n", __PRETTY_FUNCTION__));
    return pointer(new test());
}

test::test(const test &rhs) :
    fred(rhs.fred)
{
    trace(("%s\n", __PRETTY_FUNCTION__));
}

test::pointer
test::create(const test &rhs)
{
    trace(("%s\n", __PRETTY_FUNCTION__));
    return pointer(new test(rhs));
}

test &
test::operator=(const test &rhs)
{
    trace(("%s\n", __PRETTY_FUNCTION__));
    if (this != &rhs)
    {
        fred = rhs.fred;
    }
    return *this;
}

time_t
test::fred_get()
    const
{
    return fred;
}

bool
test::fred_is_set()
    const
{
    return (fred != 0);
}

void
test::fred_set(time_t x)
{
    fred = x;
}

void
test::fred_clear()
{
    fred = 0;
}
#include <libaegis/introspector/time.h>

introspector::pointer
test::fred_introspector_factory()
{
    return introspector_time::create ( introspector_time::adapter_by_method
        < test > ::create ( this, &test::fred_set, &test::fred_is_set));
}
#include <libaegis/introspector/structure.h>

introspector::pointer
test::introspector_factory()
{
    trace(("%s\n", __PRETTY_FUNCTION__));
    introspector_structure::pointer ip =
        introspector_structure::create("test");
    ip->register_member ( "fred", introspector_structure::adapter_by_method
        < test > ::create ( this, &test::fred_introspector_factory,
            &test::fred_is_set));
    return ip;
};

void
test::write(const output::pointer &fp)
    const
{
    trace(("%s\n{\n", __PRETTY_FUNCTION__));
    introspector_time::write(fp, "fred", fred, false);
    trace(("}\n"));
}

void
test::write_xml(const output::pointer &fp)
    const
{
    trace(("%s\n{\n", __PRETTY_FUNCTION__));
    fp->fputs("<test>\n");
    introspector_time::write_xml(fp, "fred", fred, false);
    fp->fputs("</test>\n");
    trace(("}\n"));
}

void
test::trace_print(const char *name_)
    const
{
    if (name_ && *name_)
    {
        trace_printf("%s = ", name_);
    }
    trace_printf("{\n");
    trace_time_real("fred", fred_get());
    trace_printf("}");
    trace_printf((name_ && *name_) ? ";\n" : ",\n");
}
#include <libaegis/meta_context/introspector.h>
#include <libaegis/os.h>

test::pointer
test::create_from_file(const nstring &filename)
{
    trace(("%s\n{\n", __PRETTY_FUNCTION__));
    pointer p = create();
    introspector::pointer ip = p->introspector_factory();
    meta_context_introspector meta;
    os_become_must_be_active();
    meta.parse_file(filename, ip);
    trace(("}\n"));
    return p;
}
#include <libaegis/io.h>
#include <libaegis/output/file.h>
#include <libaegis/output/filter/gzip.h>
#include <libaegis/output/filter/indent.h>

void
test::write_file(const nstring &filename, bool needs_compression)
    const
{
    trace(("%s\n{\n", __PRETTY_FUNCTION__));
    if (filename)
        os_become_must_be_active();
    output::pointer fp = (needs_compression ?
        output_filter_gzip::create(output_file::binary_open(filename)) :
        output_file::text_open(filename));
    fp = output_filter_indent::create(fp);
    io_comment_emit(fp);
    write(fp);
    type_enum_option_clear();
    trace(("}\n"));
}

void
test::report_init(void)
{
    trace(("%s\n{\n", __PRETTY_FUNCTION__));
    trace(("}\n"));
}
fubar
test $? -eq 0 || no_result

fmtgen -ic test.fmtgen.cc test.def
test $? -eq 0 || fail

diff test.fmtgen.cc.ok test.fmtgen.cc
test $? -eq 0 || fail

#
# Only definite negatives are possible.
# The functionality exercised by this test appears to work,
# no other guarantees are made.
#
pass


// vim: set ts=8 sw=4 et :
