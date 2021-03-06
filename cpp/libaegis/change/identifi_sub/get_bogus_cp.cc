//
// aegis - project change supervisor
// Copyright (C) 2008 Peter Miller
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
// You should have received a copy of the GNU General Public License along
// with this program. If not, see <http://www.gnu.org/licenses/>.
//

#include <libaegis/ael/change/inappropriat.h>
#include <libaegis/change/identifi_sub.h>
#include <libaegis/project/history.h>


change::pointer
change_identifier_subset::get_bogus_cp()
{
    if (!cp)
    {
        if (set())
            list_change_inappropriate();
        cp = change_alloc(get_pp(), project_next_change_number(get_pp(), 1));
        change_bind_new(cp);
        change_architecture_from_pconf(cp);
        cp->bogus = 1;
    }
    return cp;
}
