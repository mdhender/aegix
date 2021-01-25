//
//      aegis - project change supervisor
//      Copyright (C) 1999, 2001-2008, 2011, 2012 Peter Miller
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

#include <common/nstring/list.h>
#include <common/trace.h>
#include <common/wstring/list.h>
#include <libaegis/project/history.h>
#include <libaegis/sub.h>
#include <libaegis/sub/project/integra_list.h>
#include <libaegis/sub/user.h>
#include <libaegis/user.h>


static nstring
get_user_name(user_ty::pointer up)
{
    return up->name();
}


//
// NAME
//      sub_integrator_list - the integrator_list substitution
//
// SYNOPSIS
//      wstring_ty *sub_integrator_list(wstring_list_ty *arg);
//
// DESCRIPTION
//      The sub_integrator_list function implements the integrator_list
//      substitution.  The integrator_list substitution is replaced by a
//      space separated list of the project's integrators.
//
// ARGUMENTS
//      arg     - list of arguments, including the function name as [0]
//
// RETURNS
//      a pointer to a string in dynamic memory;
//      or NULL on error, setting suberr appropriately.
//

wstring
sub_integrator_list(sub_context_ty *scp, const wstring_list &arg)
{
    trace(("sub_integrator_list()\n{\n"));
    wstring result;
    project *pp = sub_context_project_get(scp);
    if (!pp)
    {
        scp->error_set(i18n("not valid in current context"));
        trace(("}\n"));
        return result;
    }
    sub_user_func_ptr func = get_user_name;
    switch (arg.size())
    {
    default:
        scp->error_set(i18n("requires one argument"));
        trace(("}\n"));
        return result;

    case 1:
        break;

    case 2:
        {
            nstring s = arg[1].to_nstring();
            func = sub_user_func(s);
            if (!func)
            {
                scp->error_set(i18n("unknown substitution variant"));
                trace(("}\n"));
                return result;
            }
        }
        break;
    }

    //
    // build a string containing all of the project integrators
    //
    nstring_list wl;
    for (size_t j = 0; ; ++j)
    {
        nstring s(project_integrator_nth(pp, j));
        if (s.empty())
            break;
        user_ty::pointer up = user_ty::create(s);
        s = func(up);
        wl.push_back(s);
    }
    result = wstring(wl.unsplit());

    trace(("return %p;\n", result.get_ref()));
    trace(("}\n"));
    return result;
}


// vim: set ts=8 sw=4 et :
