//
// aegis - project change supervisor
// Copyright (C) 2004-2006, 2008, 2012 Peter Miller
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

#ifndef LIBAEGIS_PROJECT_HISTORY_UUID_TRANS_H
#define LIBAEGIS_PROJECT_HISTORY_UUID_TRANS_H

#include <libaegis/fstate.fmtgen.h>

/**
  * The project_history_uuid_translate function is used to translate a
  * file's UUID into a relative path below the history directory.
  */
string_ty *project_history_uuid_translate(fstate_src_ty *);

#endif // LIBAEGIS_PROJECT_HISTORY_UUID_TRANS_H
// vim: set ts=8 sw=4 et :
