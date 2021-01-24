//
//	aegis - project change supervisor
//	Copyright (C) 2001, 2002, 2005, 2006, 2008 Peter Miller
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program. If not, see
//	<http://www.gnu.org/licenses/>.
//

#ifndef LIBAEGIS_OUTPUT_UUENCODE_H
#define LIBAEGIS_OUTPUT_UUENCODE_H

#include <common/nstring.h>
#include <libaegis/output.h>

/**
  * The output_uuencode class is used to represent the state of a
  * filter which unix-to-unix encodes the data being written.  See
  * uuencode(1) for more information.
  */
class output_uuencode:
    public output
{
public:
    /**
      * The destructor.
      */
    virtual ~output_uuencode();

private:
    /**
      * The constructor.
      *
      * \param deeper
      *     The deeper output stream this filter is to write its output to.
      */
    output_uuencode(const output::pointer &deeper);

public:
    /**
      * The constructor.
      *
      * \param deeper
      *     The deeper output stream this filter is to write its output to.
      */
    static pointer create(const output::pointer &deeper);

protected:
    // See base class for documentation.
    nstring filename() const;

    // See base class for documentation.
    const char *type_name() const;

    // See base class for documentation.
    long ftell_inner() const;

    // See base class for documentation.
    void write_inner(const void *data, size_t length);

    // See base class for documentation.
    void end_of_line_inner();

    // See base class for documentation.
    int page_width() const;

    // See base class for documentation.
    int page_length() const;

private:
    /**
      * The deeeper instance variable is used to remember the deeper
      * output stream this filter is to write its output to.
      */
    output::pointer deeper;

    /**
      * The residual_value instance variable is used to remember the
      * cumulative bits, accumulated 8 bits per inoput characters,
      * depleted at 6 bits per output character.
      */
    unsigned residual_value;

    /**
      * The residual_bits instance variable is used to remember the
      * number of bits accumulated fo far in the residual_value.
      */
    int residual_bits;

    /**
      * The obuf instance variable is used to remember the output line
      * being accumulated.
      */
    char obuf[64 * 4 / 3];

    /**
      * The opos instance variable is used to remember how much of the
      * obuf has been consumed to date, the numer of output characters.
      */
    int opos;

    /**
      * The ipos instance variable is used to remember how many input
      * characters have been consumed to date in building the obuf.
      */
    int ipos;

    /**
      * The pos instance variable is used to remember the current output
      * file position.
      */
    long pos;

    /**
      * The bol instance variable is used to remember whether or not we
      * are at the start of a line.
      */
    bool bol;

    /**
      * The begun instance variable is used to remember whether or not
      * we have written the "begin" line yet.
      */
    bool begun;

    /**
      * The file_name instance variable is used to remember the name of
      * the input file, with any ".uu*" suffix removed.
      */
    nstring file_name;

    /**
      * The default constructor.  Do not use.
      */
    output_uuencode();

    /**
      * The copy constructor.  Do not use.
      */
    output_uuencode(const output_uuencode &);

    /**
      * The assignment operator.  Do not use.
      */
    output_uuencode &operator=(const output_uuencode &);
};

#endif // LIBAEGIS_OUTPUT_UUENCODE_H
