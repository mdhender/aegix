//
//      aegis - project change supervisor
//      Copyright (C) 2002-2008, 2011, 2012 Peter Miller
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

#include <libaegis/arglex2.h>
#include <libaegis/cstate.fmtgen.h>
#include <libaegis/project.h>
#include <libaegis/user.h>

#include <aecomplete/command/aedeu.h>
#include <aecomplete/command/generic.h>
#include <aecomplete/command/private.h>
#include <aecomplete/complete/change/number.h>
#include <aecomplete/complete/project/name.h>


static void
destructor(command_ty *)
{
}


static complete_ty *
completion_get(command_ty *)
{
    string_ty       *project_name;
    project      *pp;
    complete_ty     *result;

    project_name = 0;
    arglex2_retable(0);
    arglex();
    while (arglex_token != arglex_token_eoln)
    {
        switch (arglex_token)
        {
        default:
            result = generic_argument_complete();
            if (result)
                return result;
            continue;

        case arglex_token_change:
            switch (arglex())
            {
            default:
                continue;

            case arglex_token_number:
            case arglex_token_number_incomplete:
            case arglex_token_string_incomplete:
                break;
            }
            break;

        case arglex_token_number:
        case arglex_token_number_incomplete:
            break;

        case arglex_token_project:
            switch (arglex())
            {
            default:
                continue;

            case arglex_token_string:
                project_name = str_from_c(arglex_value.alv_string);
                break;

            case arglex_token_string_incomplete:
            case arglex_token_number_incomplete:
                return complete_project_name();
            }
            break;

        case arglex_token_string:
            project_name = str_from_c(arglex_value.alv_string);
            break;

        case arglex_token_string_incomplete:
            return complete_project_name();
        }
        arglex();
    }

    //
    // locate project data
    //
    if (!project_name)
    {
        nstring n = user_ty::create()->default_project();
        project_name = str_copy(n.get_ref());
    }
    pp = project_alloc(project_name);
    str_free(project_name);
    pp->bind_existing();

    //
    // We are going to complete a change number.
    //
    // Note: we tell them all the changes between awaiting review and
    // awaiting integration, whether or not they are the developer.
    //
    return
        complete_change_number
        (
            pp,
            (
                (1 << cstate_state_awaiting_review)
            |
                (1 << cstate_state_being_reviewed)
            |
                (1 << cstate_state_awaiting_integration)
            )
        );
}


static command_vtbl_ty vtbl =
{
    destructor,
    completion_get,
    sizeof(command_ty),
    "aedeu",
};


command_ty *
command_aedeu()
{
    return command_new(&vtbl);
}


// vim: set ts=8 sw=4 et :
