//
//	aegis - project change supervisor
//	Copyright (C) 1991-1993, 1998, 2002, 2005-2008 Peter Miller
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

#ifndef AEGIS_AENC_H
#define AEGIS_AENC_H

#include <common/main.h>
#include <libaegis/user.h>

void new_change(void);

struct project_ty; // existence

void new_change_check_permission(project_ty *pp, user_ty::pointer up);

#endif // AEGIS_AENC_H
