//
// aegis - project change supervisor
// Copyright (C) 1999, 2002, 2005, 2006, 2008, 2011, 2012 Peter Miller
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//

#ifndef LIBAEGIS_OS_ISA_PATH_PREFIX_H
#define LIBAEGIS_OS_ISA_PATH_PREFIX_H

#include <common/nstring.h>

bool os_isa_path_prefix(const nstring &, const nstring &);
bool os_isa_path_prefix(struct string_ty *, struct string_ty *);

#endif // LIBAEGIS_OS_ISA_PATH_PREFIX_H
// vim: set ts=8 sw=4 et :
