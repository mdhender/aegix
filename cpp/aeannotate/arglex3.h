//
//	aegis - project change supervisor
//	Copyright (C) 2002, 2005, 2006, 2008 Peter Miller
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

#ifndef AEANNOTATE_ARGLEX3_H
#define AEANNOTATE_ARGLEX3_H

#include <libaegis/arglex2.h>

enum
{
    arglex_token_column = ARGLEX2_MAX,
    arglex_token_filestat,
    arglex_token_filestat_not,
    arglex_token_diff_option,
    ARGLEX3_MAX
};

void arglex3_init(int, char **);

#endif // AEANNOTATE_ARGLEX3_H
