//
// aegis - project change supervisor
// Copyright (C) 2005-2008, 2010, 2012 Peter Miller
// Copyright (C) 2008 Walter Franzini
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

#include <common/ac/assert.h>

#include <libaegis/attribute.h>
#include <libaegis/change/file.h>
#include <libaegis/fstate.fmtgen.h>
#include <libaegis/input/file.h>
#include <libaegis/os.h>
#include <libaegis/sub.h>

#include <aede-policy/validation/files/crlf.h>


validation_files_crlf::~validation_files_crlf()
{
}


validation_files_crlf::validation_files_crlf()
{
}


validation::pointer
validation_files_crlf::create(void)
{
    return pointer(new validation_files_crlf());
}


bool
validation_files_crlf::check(change::pointer cp, fstate_src_ty *src)
{
    nstring path(cp->file_path(src));
    assert(!path.empty());
    if (path.empty())
        return true;

    //
    // Don't perform this check for files marked as being allowed to
    // have crlf.
    //
    if
    (
        attributes_list_find_boolean
        (
            src->attribute,
            "aede-policy-crlf-allowed"
        )
    )
        return true;

    os_become_orig();
    bool crlf_seen = false;
    bool nul_seen = false;
    input ip = input_file_open(path);
    int state = 0;
    for (;;)
    {
        int c = ip->getch();
        if (c < 0)
            break;
        if (c == 0)
        {
            //
            // These days, the only difference between text files
            // and binary files is the presence of NUL characters.
            // All other characters can be part of an international
            // character set encoding.
            //
            nul_seen = true;
        }
        if (state == '\r' && c == '\n')
            crlf_seen = true;
        state = c;
    }
    ip.close();
    os_become_undo();
    bool result = nul_seen || !crlf_seen;
    if (!result)
    {
        sub_context_ty sc;
        sc.var_set_string("File_Name", src->file_name);
        change_error(cp, &sc, i18n("$filename: dos line termination"));
    }
    return result;
}


bool
validation_files_crlf::check_binaries(void)
    const
{
    return false;
}


// vim: set ts=8 sw=4 et :
