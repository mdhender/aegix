//
// aegis - project change supervisor
// Copyright (C) 2004-2006, 2008, 2012 Peter Miller
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

#ifndef AECVSSERVER_FILE_INFO_H
#define AECVSSERVER_FILE_INFO_H

struct file_info_ty
{
    struct string_ty *version;
    int modified;
    int mode;
    int questionable;
};


file_info_ty *file_info_new(void);
void file_info_reset(file_info_ty *);
void file_info_delete(file_info_ty *);

#endif // AECVSSERVER_FILE_INFO_H
// vim: set ts=8 sw=4 et :
