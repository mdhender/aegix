//
//      aegis - project change supervisor
//      Copyright (C) 2002-2008, 2012 Peter Miller
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 3 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program. If not, see
//      <http://www.gnu.org/licenses/>.
//

#include <libaegis/file.h>
#include <libaegis/os.h>
#include <libaegis/sub.h>
#include <libaegis/sub/read_file.h>
#include <common/trace.h>
#include <common/wstring/list.h>


//
// NAME
//      sub_read_file - the read_file substitution
//
// SYNOPSIS
//      string_ty *sub_read_file(wstring_list_ty *arg);
//
// DESCRIPTION
//      The sub_read_file function implements the read_file substitution.
//      The read_file substitution is replaced by the contents of the
//      named file.  An absolute path must be supplied.
//
// ARGUMENTS
//      arg     - list of arguments, including the function name as [0]
//
// RETURNS
//      a pointer to a string in dynamic memory;
//      or NULL on error, setting suberr appropriately.
//

wstring
sub_read_file(sub_context_ty *scp, const wstring_list &arg)
{
    trace(("sub_read_file()\n{\n"));
    wstring result;
    if (arg.size() != 2)
    {
        scp->error_set(i18n("requires one argument"));
        trace(("}\n"));
        return result;
    }

    if (arg[1][0] != '/')
    {
        scp->error_set(i18n("absolute path required"));
        trace(("}\n"));
        return result;
    }

    nstring s1 = arg[1].to_nstring();
    os_become_orig();
    nstring s2 = read_whole_file(s1);
    os_become_undo();
    result = wstring(s2);
    trace(("return %p;\n", result.get_ref()));
    trace(("}\n"));
    return result;
}


// vim: set ts=8 sw=4 et :
