//
//	aegis - project change supervisor
//	Copyright (C) 2002-2008 Peter Miller
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

#include <libaegis/arglex2.h>
#include <libaegis/change.h>
#include <libaegis/cstate.h>
#include <libaegis/project.h>
#include <libaegis/user.h>
#include <libaegis/zero.h>

#include <aecomplete/command/aet.h>
#include <aecomplete/command/generic.h>
#include <aecomplete/command/private.h>
#include <aecomplete/complete/change/number.h>
#include <aecomplete/complete/project/file_union.h>
#include <aecomplete/complete/project/name.h>


static void
destructor(command_ty *)
{
}


static void
do_nothing(void)
{
}


static complete_ty *
completion_get(command_ty *)
{
    int             usage_mask;
    complete_ty     *result;
    int             incomplete_change_number;
    string_ty       *project_name;
    long            change_number;
    project_ty      *pp;
    user_ty::pointer up;
    change::pointer cp;
    int             baserel;
    int		    manual_test = 0;
    int		    automatic_test = 0;

    arglex2_retable(0);
    arglex();
    incomplete_change_number = 0;
    project_name = 0;
    change_number = 0;
    while (arglex_token != arglex_token_eoln)
    {
	switch (arglex_token)
	{
	default:
	    result = generic_argument_complete();
	    if (result)
		return result;
	    continue;

	case arglex_token_directory:
	case arglex_token_file:
	    // noise word, ignore
	    break;

	case arglex_token_string:
	    // whole file name, ignore
	    break;

	case arglex_token_string_incomplete:
	    // incomplete filename
	    break;

	case arglex_token_change:
	    switch (arglex())
	    {
	    default:
		continue;

	    case arglex_token_number:
		if (arglex_value.alv_number == 0)
		    change_number = MAGIC_ZERO;
		else if (arglex_value.alv_number > 0)
		    change_number = arglex_value.alv_number;
		break;

	    case arglex_token_number_incomplete:
	    case arglex_token_string_incomplete:
		incomplete_change_number = 1;
		break;
	    }
	    break;

	case arglex_token_number:
	    if (arglex_value.alv_number == 0)
		change_number = MAGIC_ZERO;
	    else if (arglex_value.alv_number > 0)
		change_number = arglex_value.alv_number;
	    break;

	case arglex_token_number_incomplete:
	    incomplete_change_number = 1;
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

	case arglex_token_base_relative:
	case arglex_token_current_relative:
	    user_ty::relative_filename_preference_argument(do_nothing);
	    break;

	case arglex_token_manual:
	    manual_test = 1;
	    break;

	case arglex_token_automatic:
	    automatic_test = 1;
	    break;
	}
	arglex();
    }
    if (!manual_test && !automatic_test)
    {
	manual_test = 1;
	automatic_test = 1;
    }

    //
    // Work out which project to use.
    //
    if (!project_name)
    {
        nstring n = user_ty::create()->default_project();
	project_name = str_copy(n.get_ref());
    }
    pp = project_alloc(project_name);
    pp->bind_existing();

    //
    // If we need to complete a change number, we have the project now.
    //
    if (incomplete_change_number)
    {
	return
	    complete_change_number
	    (
		pp,
		(
		    (1 << cstate_state_being_developed)
		|
		    (1 << cstate_state_being_integrated)
		)
	    );
    }

    //
    // locate user data
    //
    up = user_ty::create();

    //
    // locate change data
    //
    if (!change_number)
	change_number = up->default_change(pp);
    cp = change_alloc(pp, change_number);
    change_bind_existing(cp);

    //
    // Figure out whether we are using base relative file names, or
    // current directory relative file names.
    //
    baserel =
	(
	    up->relative_filename_preference
	    (
		uconf_relative_filename_preference_current
	    )
	==
	    uconf_relative_filename_preference_base
	);

    //
    // We are going to complete a change file name.
    //
    usage_mask = 0;
    if (automatic_test)
	usage_mask |= (1 << file_usage_test);
    if (manual_test)
	usage_mask |= (1 << file_usage_manual_test);
    return complete_project_file_union(cp, baserel, usage_mask);
}


static command_vtbl_ty vtbl =
{
    destructor,
    completion_get,
    sizeof(command_ty),
    "aet",
};


command_ty *
command_aet()
{
    return command_new(&vtbl);
}