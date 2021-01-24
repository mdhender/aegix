//
//	aegis - project change supervisor
//	Copyright (C) 2004-2006, 2008 Peter Miller
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

#ifndef AECVSSERVER_REQUEST_INIT_H
#define AECVSSERVER_REQUEST_INIT_H

#include <aecvsserver/request.h>

/**
  * The request_init class is used to represent an init request from a
  * CVS client.
  */
class request_init:
    public request
{
public:
    /**
      * The destructor.
      */
    virtual ~request_init();

    /**
      * The default constructor.
      */
    request_init();

    // See base class for documentation.
    void run_inner(server_ty *sp, string_ty *fn) const;

    // See base class for documentation.
    const char *name() const;

    // See base class for documentation.
    bool reset() const;

private:
    /**
      * The copy constructor.  Do not use.
      */
    request_init(const request_init &);

    /**
      * The assignment operator.  Do not use.
      */
    request_init &operator=(const request_init &);
};

#endif // AECVSSERVER_REQUEST_INIT_H
