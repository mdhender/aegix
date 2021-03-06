//
// aegis - project change supervisor
// Copyright (C) 2004-2006, 2008, 2012 Peter Miller
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
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

#include <common/trace.h>
#include <libaegis/os.h>
#include <libaegis/undo/item.h>


undo_item::~undo_item()
{
}


undo_item::undo_item() :
    uid(0),
    gid(0),
    umask(022)
{
    os_become_query(&uid, &gid, &umask);
    assert(uid != 0);
    trace(("uid = %d\n", uid));
    // assert(gid != 0);
    trace(("gid = %d\n", gid));
}


void
undo_item::act()
{
    os_become(uid, gid, umask);
    action();
    os_become_undo();
}


void
undo_item::unfinished()
{
    // do nothing
}


// vim: set ts=8 sw=4 et :
