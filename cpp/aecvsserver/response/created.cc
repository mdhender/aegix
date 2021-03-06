//
// aegis - project change supervisor
// Copyright (C) 2004-2008, 2012 Peter Miller
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
//
// Created <client-side-dir>/
// <server-side-dir>/<filename>
// /<filename>/<version>///
// <file-mode>
// <length>
// <length-bytes-of-contents>
//
// This is just like Updated and takes the same additional data, but
// is used only if no Entry, Modified, or Unchanged request has been
// sent for the file in question.
//
// The distinction between Created and Update-existing is so that the
// client can give an error message in several cases:
// (1) there is a file in the working directory, but not one for which
//     Entry, Modified, or Unchanged was sent (for example, a file which
//     was ignored, or a file for which Questionable was sent),
// (2) there is a file in the working directory whose name differs
//     from the one mentioned in Created in ways that the client is
//     unable to use to distinguish files.  For example, the client is
//     case-insensitive and the names differ only in case.
//
//
// The output looks something like this...
//
// S: M U supermunger/mungeall.c
// S: Created supermunger/
// S: /u/cvsroot/supermunger/mungeall.c
// S: /mungeall.c/1.1///
// S: u=rw,g=r,o=r
// S: 26
// S: int mein () { abort (); }
//

#include <common/ac/string.h>

#include <libaegis/input.h>
#include <libaegis/os.h>
#include <libaegis/output.h>
#include <aecvsserver/response/created.h>
#include <aecvsserver/server.h>


response_created::~response_created()
{
    str_free(client_side);
    client_side = 0;
    str_free(server_side);
    server_side = 0;

    //
    // This may need to unlink,
    // so we have to have the file ownership right.
    //
    os_become_orig();
    source.close();
    os_become_undo();

    str_free(version);
    version = 0;
}


response_created::response_created(string_ty *arg1, string_ty *, input &arg3,
        int arg4, string_ty *arg5) :
    client_side(str_copy(arg1)),
    server_side(str_copy(arg1)),
    source(arg3),
    mode(arg4),
    version(str_copy(arg5))
{
}


void
response_created::write(output::pointer op)
{
    //
    // The output looks something like this...
    //
    // S: M U supermunger/mungeall.c
    // S: Created supermunger/
    // S: /u/cvsroot/supermunger/mungeall.c
    // S: /mungeall.c/1.1///
    // S: u=rw,g=r,o=r
    // S: 26
    // S: int mein () { abort (); }
    //
    string_ty *short_dir_name = os_dirname_relative(client_side);
    string_ty *short_file_name = os_entryname_relative(client_side);
    op->fprintf("M U %s\n", short_file_name->str_text);
    op->fprintf("Created %s/\n", short_dir_name->str_text);
    op->fprintf(ROOT_PATH "/%s\n", server_side->str_text);
    op->fprintf
    (
        "/%s/%s///\n",
        short_file_name->str_text,
        version->str_text
    );
    output_mode_string(op, mode);
    os_become_orig();
    off_t length = source->length();
    if (length > 0)
    {
        //
        // There is the ability to compress the file contents, but the
        // client has to ask for it.  Slightly difficult: we have to
        // send the *compressed* file size first, which isn't easy using
        // a naive usage of output_gzip().  Just say no.
        //
        op->fprintf(OFF_T_FMT "\n", length);
        op << source;
    }
    else
    {
        op->fputs("0\n");
    }
    os_become_undo();
    str_free(short_dir_name);
    str_free(short_file_name);
}


response_code_ty
response_created::code_get()
    const
{
    return response_code_Created;
}


bool
response_created::flushable()
    const
{
    return true;
}


// vim: set ts=8 sw=4 et :
