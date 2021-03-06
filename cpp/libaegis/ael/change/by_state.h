//
//	aegis - project change supervisor
//	Copyright (C) 1999, 2002, 2003, 2005-2008 Peter Miller
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

#ifndef LIBAEGIS_AEL_CHANGE_BY_STATE_H
#define LIBAEGIS_AEL_CHANGE_BY_STATE_H

class change_identifier; // forward
struct string_ty; // forward

void list_changes_in_state_mask(change_identifier &cid, int mask);
void list_changes_in_state_mask_by_user(change_identifier &cid, int mask,
    string_ty *login);

#endif // LIBAEGIS_AEL_CHANGE_BY_STATE_H
