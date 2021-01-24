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

#include <common/ac/string.h>

#include <libaegis/arglex2.h>
#include <common/error.h> // for assert
#include <libaegis/os.h>
#include <libaegis/sub.h>
#include <libaegis/user.h>


void
os_edit(const nstring &filename, edit_ty et)
{
    //
    // find the editor to use
    //
    os_become_must_not_be_active();
    nstring editor;
    if (et == edit_background)
    {
	editor = user_ty::create()->editor_command();
    }
    else
    {
	editor = user_ty::create()->visual_command();
	const char *cp = strrchr(editor.c_str(), '/');
	if (!cp)
            cp = editor.c_str();
	if (0 == strcmp(cp, "ed"))
            et = edit_background;
    }

    //
    // make sure we are in a position to edit
    //
    if (et != edit_background && os_background())
    {
	sub_context_ty sc;
	sc.var_set_charstar("Name", arglex_token_name(arglex_token_edit));
	sc.fatal_intl(i18n("may not use $name in the background"));
	// NOTREACHED
    }

    //
    // edit the file
    //
    // Please note: we ignore the exit status on purpose.
    // This is because vi (amongst others) returns a silly exit status.
    //
    nstring cmd = nstring::format("%s %s", editor.c_str(), filename.c_str());
    os_become_orig();
    nstring cwd(os_curdir());
    assert(cwd);
    os_execute(cmd, OS_EXEC_FLAG_INPUT | OS_EXEC_FLAG_ERROK, cwd);
    os_become_undo();
}


void
os_edit(string_ty *filename, edit_ty et)
{
    os_edit(nstring(filename), et);
}
