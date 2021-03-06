//
//	aegis - project change supervisor
//	Copyright (C) 1994-1996, 2004-2008 Peter Miller
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

#include <common/ac/ctype.h>
#include <common/ac/stdlib.h>


#ifndef HAVE_MBLEN

// either mblen not available, or it is broken

#undef mblen

int
mblen(const char *s, size_t)
{
	return (s && *s);
}


#undef mbtowc

int
mbtowc(wchar_t *pwc, const char *s, size_t)
{
	if (!s)
		return 0;
	if (pwc)
		*pwc = *(unsigned char *)s;
	return (*s != 0);
}


#undef wctomb

int
wctomb(char *s, wchar_t wc)
{
	if (!s)
		return 0;
	*s = wc;
	return 1;
}

#endif // !HAVE_MBLEN

#ifndef HAVE_STRTOUL

unsigned long
strtoul(const char *nptr, char **endptr, int base)
{
	const char	*s;
	int		neg;
	unsigned long	n;
	int		ndigits;
	unsigned char	c;

	//
	// This is not an ANSI C conforming implementation.
	// Don't use it if you have a choice.
	//
	neg = 0;
	s = nptr;
	for (;;)
	{
		c = (unsigned char)*s++;
		if (!isspace((unsigned char)c))
			break;
	}
	if (c == '-')
	{
		neg = 1;
		c = (unsigned char)*s++;
	}
	else if (c == '+')
		c = (unsigned char)*s++;
	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X'))
	{
		++s;
		c = (unsigned char)*s++;
		base = 16;
	}
	if (base == 0)
		base = (c == '0' ? 8 : 10);
	n = 0;
	ndigits = 0;
	for (;;)
	{
		if (isdigit((unsigned char)c))
                        c -= '0';
		else if (isupper((unsigned char)c))
			c -= 'A' - 10;
                else if (islower((unsigned char)c))
                        c += 'a' - 10;
                else
                        break;
                if (c >= base)
                        break;
		n = n * base + c;
		++ndigits;
		c = (unsigned char)*s++;
        }
        if (endptr)
                *endptr = (char *)(ndigits ? s - 1 : nptr);
        return (neg ? -n : n);
}

#endif // !HAVE_STRTOUL
