//
//      aegis - project change supervisor
//      Copyright (C) 2002, 2005, 2006, 2008, 2011, 2012 Peter Miller
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

#ifndef AECOMPLETE_COMPLETE_USER_INTEGRATOR_H
#define AECOMPLETE_COMPLETE_USER_INTEGRATOR_H

#include <aecomplete/complete.h>

struct project; // forward

complete_ty *complete_user_integrator(struct project *);
complete_ty *complete_user_integrator_not(struct project *);

#endif // AECOMPLETE_COMPLETE_USER_INTEGRATOR_H
// vim: set ts=8 sw=4 et :
