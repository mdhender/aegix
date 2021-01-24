//
//	aegis - project change supervisor
//	Copyright (C) 1990-1993, 2003, 2005, 2006, 2008 Peter Miller.
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

#ifndef COMMON_ENV_H
#define COMMON_ENV_H

/** \addtogroup Environment
  * \brief Process environment manipulation
  * \ingroup Common
  * @{
  */
#include <common/main.h>

void env_initialize(void);
void env_set(const char *name, const char *value);
void env_setf(const char *name, const char *value, ...);
void env_set_page(void);
void env_unset(const char *name);

/** @} */
#endif // COMMON_ENV_H