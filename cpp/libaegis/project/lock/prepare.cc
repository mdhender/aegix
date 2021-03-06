//
//      aegis - project change supervisor
//      Copyright (C) 2006-2008, 2011, 2012 Peter Miller
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

#include <common/ac/assert.h>

#include <common/trace.h>
#include <libaegis/change.h>
#include <libaegis/lock.h>
#include <libaegis/project.h>
#include <libaegis/sub.h>
#include <libaegis/user.h>


static void
waiting_for_lock(void *p)
{
    project *pp = (project *)p;
    if (user_ty::create()->lock_wait())
        project_error(pp, 0, i18n("waiting for lock"));
    else
        project_fatal(pp, 0, i18n("lock not available"));
}


void
project::pstate_lock_prepare()
{
    trace(("project::pstate_lock_prepare(this = %p)\n{\n", this));
    if (parent)
    {
        assert(pcp);
        change_cstate_lock_prepare(change_get());
    }
    else
        lock_prepare_pstate(name, waiting_for_lock, this);
    trace(("}\n"));
}


// vim: set ts=8 sw=4 et :
