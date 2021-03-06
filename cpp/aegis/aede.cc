//
// aegis - project change supervisor
// Copyright (C) 1991-2012 Peter Miller
// Copyright (C) 2008, 2011 Walter Franzini
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

#include <common/ac/assert.h>
#include <common/ac/stdio.h>
#include <common/ac/stdlib.h>
#include <common/ac/string.h>
#include <common/ac/time.h>
#include <common/ac/sys/types.h>
#include <common/ac/sys/stat.h>

#include <common/progname.h>
#include <common/quit.h>
#include <common/sizeof.h>
#include <common/trace.h>
#include <libaegis/ael/change/by_state.h>
#include <libaegis/arglex2.h>
#include <libaegis/arglex/change.h>
#include <libaegis/arglex/project.h>
#include <libaegis/attribute.h>
#include <libaegis/change/attributes.h>
#include <libaegis/change/branch.h>
#include <libaegis/change/develop_direct/read_only.h>
#include <libaegis/change/file.h>
#include <libaegis/change/identifier.h>
#include <libaegis/change/signedoffby.h>
#include <libaegis/col.h>
#include <libaegis/commit.h>
#include <libaegis/common.fmtgen.h>
#include <libaegis/file.h>
#include <libaegis/help.h>
#include <libaegis/lock.h>
#include <libaegis/option.h>
#include <libaegis/os.h>
#include <libaegis/project/active.h>
#include <libaegis/project/file.h>
#include <libaegis/project.h>
#include <libaegis/project/history.h>
#include <libaegis/rss.h>
#include <libaegis/sub.h>
#include <libaegis/undo.h>
#include <libaegis/user.h>

#include <aegis/aede.h>


static void
develop_end_usage(void)
{
    const char      *progname;

    progname = progname_get();
    fprintf(stderr, "usage: %s -Develop_End [ <option>... ]\n", progname);
    fprintf(stderr, "       %s -Develop_End -List [ <option>... ]\n", progname);
    fprintf(stderr, "       %s -Develop_End -Help\n", progname);
    quit(1);
}


static void
develop_end_help(void)
{
    help("aede", develop_end_usage);
}


static void
develop_end_list(void)
{
    trace(("develop_end_list()\n{\n"));
    arglex();
    change_identifier cid;
    cid.command_line_parse_rest(develop_end_usage);
    list_changes_in_state_mask(cid, 1 << cstate_state_being_developed);
    trace(("}\n"));
}


static void
develop_end_main(void)
{
    sub_context_ty  *scp;
    string_ty       *dd;
    cstate_ty       *cstate_data;
    int             j;
    cstate_history_ty *history_data;
    int             diff_whine;
    time_t          youngest;
    string_ty       *youngest_name;
    int             config_seen;
    int             is_a_branch;

    trace(("develop_end_main()\n{\n"));
    arglex();
    string_ty *reason = 0;
    change_identifier cid;
    while (arglex_token != arglex_token_eoln)
    {
        switch (arglex_token)
        {
        default:
            generic_argument(develop_end_usage);
            continue;

        case arglex_token_change:
        case arglex_token_number:
        case arglex_token_project:
        case arglex_token_string:
            cid.command_line_parse(develop_end_usage);
            continue;

        case arglex_token_wait:
        case arglex_token_wait_not:
            user_ty::lock_wait_argument(develop_end_usage);
            break;

        case arglex_token_signed_off_by:
        case arglex_token_signed_off_by_not:
            option_signed_off_by_argument(develop_end_usage);
            break;

        case arglex_token_reason:
            if (reason)
            duplicate_option(develop_end_usage);
            switch (arglex())
            {
            default:
                option_needs_string(arglex_token_reason, develop_end_usage);
                // NOTREACHED

            case arglex_token_string:
            case arglex_token_number:
                reason = str_from_c(arglex_value.alv_string);
                break;
            }
            break;
        }
        arglex();
    }
    cid.command_line_check(develop_end_usage);
    user_ty::pointer up_admin;

    //
    // Project administrators are allowed to end the development of
    // a branch, no matter who created it.
    //
    if
    (
        cid.get_cp()->is_a_branch()
    &&
        nstring(cid.get_cp()->developer_name()) != cid.get_up()->name()
    &&
        project_administrator_query(cid.get_pp(), cid.get_up()->name())
    )
    {
        up_admin = cid.get_up();
        cid.get_up() = user_ty::create(nstring(cid.get_cp()->developer_name()));
    }

    //
    // Take an advisory write lock on the appropriate row of the change
    // table.  Take an advisory write lock on the appropriate row of the
    // user table.  Block until can get both simultaneously.
    //
    cid.get_pp()->pstate_lock_prepare();
    change_cstate_lock_prepare(cid.get_cp());
    cid.get_up()->ustate_lock_prepare();
    lock_take();
    cstate_data = cid.get_cp()->cstate_get();

    //
    // It is an error if the change is not in the being_developed state.
    // It is an error if the change is not assigned to the current user.
    // It is an error if the change has no current diff.
    // It is an error if the change has no current build.
    // It is an error if the change has no current test pass.
    // It is an error if the change has no current baseline test pass.
    // It is an error if the change has no new test associated with it.
    //
    if (cstate_data->state != cstate_state_being_developed)
        change_fatal(cid.get_cp(), 0, i18n("bad de state"));
    if (nstring(cid.get_cp()->developer_name()) != cid.get_up()->name())
        change_fatal(cid.get_cp(), 0, i18n("not developer"));
    if (!change_file_nth(cid.get_cp(), (size_t)0, view_path_first))
        change_fatal(cid.get_cp(), 0, i18n("no files"));
    int number_of_errors = 0;
    diff_whine = 0;
    config_seen = 0;

    //
    // It is an error if change change attributes mention
    // architectures not in the project.
    //
    change_check_architectures(cid.get_cp());

    //
    // It is an error if any files in the change file table have
    // been modified since the last build.
    //
    youngest = 0;
    youngest_name = 0;
    is_a_branch = cid.get_cp()->is_a_branch();
    pconf_ty *pconf_data = change_pconf_get(cid.get_cp(), 1);
    bool entire_source_hide = true;
    bool local_source_hide = true;
    for (j = 0;; ++j)
    {
        fstate_src_ty   *c_src_data;
        fstate_src_ty   *p_src_data;
        string_ty       *path;
        string_ty       *path_d;
        int             same;
        int             same_d;
        int             file_required;

        c_src_data = change_file_nth(cid.get_cp(), j, view_path_first);
        if (!c_src_data)
            break;
        trace(("file_name = \"%s\"\n", c_src_data->file_name->str_text));

        //
        // We need to know if all the files in this change set are
        // marked entire-source-hide so that we can mark the change set
        // with the aeget:inventory:hide attribute.
        //
        bool esh =
            attributes_list_find_boolean
            (
                c_src_data->attribute,
                "entire-source-hide"
            );
        bool lsh =
            attributes_list_find_boolean
            (
                c_src_data->attribute,
                "local-source-hide"
            );
        if (!esh)
            entire_source_hide = false;
        if (!esh && !lsh)
            local_source_hide = false;

        switch (pconf_data->unchanged_file_develop_end_policy)
        {
        case pconf_unchanged_file_develop_end_policy_ignore:
            break;

        case pconf_unchanged_file_develop_end_policy_warning:
            if (cid.get_cp()->file_unchanged(c_src_data, cid.get_up()))
            {
                sub_context_ty sc;
                sc.var_set_string("File_Name", c_src_data->file_name);
                change_warning(cid.get_cp(), &sc, i18n("$filename unchanged"));
            }
            break;

        case pconf_unchanged_file_develop_end_policy_error:
            if (cid.get_cp()->file_unchanged(c_src_data, cid.get_up()))
            {
                sub_context_ty sc;
                sc.var_set_string("File_Name", c_src_data->file_name);
                change_error(cid.get_cp(), &sc, i18n("$filename unchanged"));
                ++number_of_errors;
            }
            break;
        }

        file_required = 1;
        bool diff_file_required = change_diff_required(cid.get_cp());
        trace_bool(diff_file_required);
        switch (c_src_data->action)
        {
        case file_action_create:
        case file_action_modify:
        case file_action_insulate:
            break;

        case file_action_remove:
            file_required = 0;
            if (is_a_branch)
            {
                fstate_src_ty   *src;

                src =
                    cid.get_pp()->file_find
                    (
                        c_src_data->file_name,
                        view_path_extreme
                    );
                if (!src)
                {
                    diff_file_required = false;
                }
            }

            //
            // The removed half of a move is not differenced.
            //
            if
            (
                c_src_data->move
            &&
                cid.get_cp()->file_find
                (
                    nstring(c_src_data->move),
                    view_path_first
                )
            )
                diff_file_required = false;
            break;

        case file_action_transparent:
            //
            // Don't check anything for branches (the file is going).
            // For changes, make sure it's the same as the ancestor.
            //
            diff_file_required = false;
            file_required = 0;
            if (!is_a_branch)
            {
                assert(cid.get_pp()->parent_get());
                if (!cid.get_pp()->is_a_trunk())
                {
                    fstate_src_ty   *pp_src;

                    pp_src =
                        cid.get_pp()->parent_get()->file_find
                        (
                            c_src_data->file_name,
                            view_path_extreme
                        );
                    if (pp_src)
                    {
                        string_ty       *blf;
                        int             different;

                        path = cid.get_cp()->file_path(c_src_data->file_name);
                        blf =
                            project_file_path
                            (
                                cid.get_pp()->parent_get(),
                                c_src_data->file_name
                            );
                        assert(blf);
                        cid.get_up()->become_begin();
                        different = files_are_different(path, blf);
                        cid.get_up()->become_end();
                        str_free(blf);
                        str_free(path);
                        if (different)
                        {
                            scp = sub_context_new();
                            sub_var_set_string
                            (
                                scp,
                                "File_Name",
                                c_src_data->file_name
                            );
                            change_error
                            (
                                cid.get_cp(),
                                scp,
                                i18n("$filename altered")
                            );
                            sub_context_delete(scp);
                            ++number_of_errors;
                        }
                    }
                }
            }
            break;
        }
        switch (c_src_data->usage)
        {
        case file_usage_build:
            file_required = 0;
            diff_file_required = false;
            break;

        case file_usage_source:
        case file_usage_config:
        case file_usage_test:
        case file_usage_manual_test:
            break;
        }

        //
        // the config file in a change
        // implies additional tests
        //
        if (cid.get_cp()->file_is_config(c_src_data->file_name))
            config_seen++;

        //
        // make sure the file exists and has not altered
        //
        path = cid.get_cp()->file_path(c_src_data->file_name);
        if (file_required)
        {
            if (!c_src_data->file_fp)
            {
                c_src_data->file_fp =
                    (fingerprint_ty *)fingerprint_type.alloc();
            }
            assert(c_src_data->file_fp->youngest>=0);
            assert(c_src_data->file_fp->oldest>=0);
            cid.get_up()->become_begin();
            same = change_fingerprint_same(c_src_data->file_fp, path, 0);
            cid.get_up()->become_end();
            assert(c_src_data->file_fp->youngest>0);
            assert(c_src_data->file_fp->oldest>0);
            trace(("same = %d\n", same));

            if (!c_src_data->file_fp || !c_src_data->file_fp->youngest)
            {
                scp = sub_context_new();
                sub_var_set_string(scp, "File_Name", c_src_data->file_name);
                change_error(cid.get_cp(), scp, i18n("$filename not found"));
                sub_context_delete(scp);
                str_free(path);
                ++number_of_errors;
                continue;
            }

            if (c_src_data->file_fp->oldest > youngest)
            {
                youngest = c_src_data->file_fp->oldest;
                youngest_name = c_src_data->file_name;
            }
        }
        else
            same = 1;

        //
        // make sure the filename conforms to length limits
        //
        // Scenario: user copies "config", alters filename
        // constraints, creates file, uncopies "config".
        // Reviewer will not necessarily notice, especially when
        // expecting aegis to notice for him.
        //
        if (!is_a_branch && file_required)
        {
            string_ty       *e;

            e = change_filename_check(cid.get_cp(), c_src_data->file_name);
            if (e)
            {
                scp = sub_context_new();
                sub_var_set_string(scp, "MeSsaGe", e);
                str_free(e);
                change_error(cid.get_cp(), scp, i18n("$message"));
                sub_context_delete(scp);
                ++number_of_errors;
            }
        }

        //
        // make sure the difference file exists and has not been altered
        //
        if (diff_file_required)
        {
            path_d = str_format("%s,D", path->str_text);
            if (!c_src_data->diff_file_fp)
            {
                c_src_data->diff_file_fp =
                    (fingerprint_ty *)fingerprint_type.alloc();
            }
            cid.get_up()->become_begin();
            same_d =
                change_fingerprint_same(c_src_data->diff_file_fp, path_d, 0);
            cid.get_up()->become_end();
            trace(("same_d = %d\n", same_d));
            str_free(path_d);

            if
            (
                !c_src_data->diff_file_fp
            ||
                !c_src_data->diff_file_fp->youngest
            )
            {
                scp = sub_context_new();
                sub_var_set_format
                (
                    scp,
                    "File_Name",
                    "%s,D",
                    c_src_data->file_name->str_text
                );
                change_error(cid.get_cp(), scp, i18n("$filename not found"));
                sub_context_delete(scp);
                ++number_of_errors;
            }
        }
        else
            same_d = 1;
        str_free(path);

        //
        // check that a difference has been done,
        // and that no files have been modified since.
        //
        if (!diff_whine && diff_file_required)
        {
            if (file_required && !same && c_src_data->file_fp->oldest)
            {
                scp = sub_context_new();
                sub_var_set_string(scp, "File_Name", c_src_data->file_name);
                change_error
                (
                    cid.get_cp(),
                    scp,
                    i18n("$filename changed after diff")
                );
                sub_context_delete(scp);
                diff_whine++;
                ++number_of_errors;
            }
            else if (!same_d && c_src_data->diff_file_fp->youngest)
            {
                scp = sub_context_new();
                sub_var_set_format
                (
                    scp,
                    "File_Name",
                    "%s,D",
                    c_src_data->file_name->str_text
                );
                change_error
                (
                    cid.get_cp(),
                    scp,
                    i18n("$filename changed after diff")
                );
                sub_context_delete(scp);
                diff_whine++;
                ++number_of_errors;
            }
            else if ((file_required && !same) || !same_d)
            {
                change_error(cid.get_cp(), 0, i18n("diff required"));
                diff_whine++;
                ++number_of_errors;
            }
        }

        // for checking, we need the p_src *before* shallowing
        fstate_src_ty *p_src_orig =
            cid.get_pp()->file_find(c_src_data->file_name, view_path_none);
        trace(("p_src_orig = %p\n", p_src_orig));

        //
        // For each change file that is acting on a project file
        // from a deeper level than the immediate parent
        // project, the file needs to be added to the immediate
        // parent project.
        //
        // This is where the about_to_be_copied_by attribute comes from.
        // Nothing is done for files being created.
        //
        trace(("shallowing \"%s\"\n", c_src_data->file_name->str_text));
        project_file_shallow
        (
            cid.get_pp(),
            c_src_data->file_name,
            magic_zero_encode(cid.get_change_number())
        );

        //
        // Find the project after "shallowing" it, as this
        // gives the project file on the immediate branch,
        // rather than deeper down the family tree.
        //
        p_src_data =
            cid.get_pp()->file_find(c_src_data->file_name, view_path_none);
        trace(("p_src_data = %p\n", p_src_data));
        assert(!p_src_data == !p_src_orig);

        //
        // It is an error if any files in the change
        // file table have different edit numbers to the
        // baseline file table edit numbers.
        //
        trace(("c_src_data->action = %s\n",
            file_action_ename(c_src_data->action)));
        trace(("c_src_data->usage = %s\n",
            file_usage_ename(c_src_data->usage)));
        switch (c_src_data->action)
        {
        case file_action_remove:
            //
            // file being removed
            //
            if
            (
                !is_a_branch
            &&
                !cid.get_pp()->file_find
                (
                    c_src_data->file_name,
                    view_path_extreme
                )
            )
            {
                scp = sub_context_new();
                sub_var_set_string(scp, "File_Name", c_src_data->file_name);
                change_error
                (
                    cid.get_cp(),
                    scp,
                    i18n("no $filename in baseline")
                );
                sub_context_delete(scp);
                ++number_of_errors;
            }

            //
            // Make sure the file exists in the project.
            //
            // This can happen for aede on branches which
            // have had a new file created and then deleted;
            // thus the file will not exist in the branch's
            // project.
            //
            if (!p_src_data)
            {
                p_src_data = cid.get_pp()->file_new(c_src_data);
                p_src_data->action = file_action_transparent;
                p_src_data->about_to_be_created_by = cid.get_change_number();
                assert(c_src_data->edit || c_src_data->edit_origin);
                p_src_data->edit =
                    history_version_copy
                    (
                        c_src_data->edit
                    ?
                        c_src_data->edit
                    :
                        c_src_data->edit_origin
                    );
                p_src_data->edit_origin =
                    history_version_copy(p_src_data->edit);
                break;
            }

            //
            // Make sure the edit numbers match.  If it
            // matches any of the ancestral edits, it does
            // not require a merge.
            //
            if
            (
                file_required
            &&
                !change_file_up_to_date(cid.get_pp(), c_src_data)
            )
            {
                scp = sub_context_new();
                sub_var_set_string(scp, "File_Name", c_src_data->file_name);
                if (is_a_branch)
                {
                    change_error
                    (
                        cid.get_cp(),
                        scp,
                        i18n("baseline $filename changed, merge in new change")
                    );
                }
                else
                {
                    change_error
                    (
                        cid.get_cp(),
                        scp,
                        i18n("baseline $filename changed")
                    );
                }
                sub_context_delete(scp);
                ++number_of_errors;
            }

            //
            // make sure we can lock the file
            //
            if (p_src_data->locked_by)
            {
                scp = sub_context_new();
                sub_var_set_string(scp, "File_Name", c_src_data->file_name);
                sub_var_set_long(scp, "Number", p_src_data->locked_by);
                change_error
                (
                    cid.get_cp(),
                    scp,
                    i18n("file \"$filename\" locked for change $number")
                );
                sub_context_delete(scp);
                ++number_of_errors;
            }
            else
                p_src_data->locked_by = cid.get_change_number();
            break;

        case file_action_transparent:
            //
            // Do absolutely nothing for transparent branch files.
            //
            if (cid.get_cp()->was_a_branch())
                break;
            // fall through...

        case file_action_modify:
            //
            // file being modified
            //
            if
            (
                !is_a_branch
            &&
                !cid.get_pp()->file_find
                (
                    c_src_data->file_name,
                    view_path_extreme
                )
            )
            {
                scp = sub_context_new();
                sub_var_set_string(scp, "File_Name", c_src_data->file_name);
                change_error
                (
                    cid.get_cp(),
                    scp,
                    i18n("no $filename in baseline")
                );
                sub_context_delete(scp);
                ++number_of_errors;
                continue;
            }

            //
            // Make sure the file exists in the project.
            //
            // This can happen for aede on branches which
            // have had a new file created and then modified;
            // thus the file will not exist in the branch's
            // project.
            //
            if (!p_src_data)
            {
                p_src_data = cid.get_pp()->file_new(c_src_data);
                p_src_data->action = file_action_transparent;
                p_src_data->about_to_be_created_by = cid.get_change_number();
                assert(c_src_data->edit || c_src_data->edit_origin);
                p_src_data->edit =
                    history_version_copy
                    (
                        c_src_data->edit
                    ?
                        c_src_data->edit
                    :
                        c_src_data->edit_origin
                    );
                p_src_data->edit_origin =
                    history_version_copy(p_src_data->edit);
                break;
            }

            //
            // Make sure the edit numbers match.  If it
            // matches any of the ancestral edits, it does
            // not require a merge.
            //
            if (!change_file_up_to_date(cid.get_pp(), c_src_data))
            {
                switch (c_src_data->usage) {
                //
                // Build files can't be merged so they must pass
                // anyway to the upper branch.
                //
                case file_usage_build:
                    break;

                case file_usage_config:
                case file_usage_source:
                case file_usage_test:
                case file_usage_manual_test:
#ifndef DEBUG
                default:
#endif
                    scp = sub_context_new();
                    sub_var_set_string(scp, "File_Name", c_src_data->file_name);
                    change_error
                    (
                        cid.get_cp(),
                        scp,
                        i18n("baseline $filename changed")
                    );
                    sub_context_delete(scp);
                    ++number_of_errors;
                    break;
                }
            }

            //
            // make sure we can lock the file
            //
            if (p_src_data->locked_by)
            {
                scp = sub_context_new();
                sub_var_set_string(scp, "File_Name", c_src_data->file_name);
                sub_var_set_long(scp, "Number", p_src_data->locked_by);
                change_error
                (
                    cid.get_cp(),
                    scp,
                    i18n("file \"$filename\" locked for change $number")
                );
                sub_context_delete(scp);
                ++number_of_errors;
            }
            else
                p_src_data->locked_by = cid.get_change_number();
            break;

        case file_action_create:
            //
            // file being created
            //
            if (p_src_data)
            {
                trace(("p_src_data->action = %s\n",
                    file_action_ename(p_src_data->action)));
                trace(("p_src_orig->action = %s\n",
                    file_action_ename(p_src_orig->action)));
                if (p_src_data->about_to_be_created_by)
                {
                    scp = sub_context_new();
                    sub_var_set_string(scp, "File_Name", c_src_data->file_name);
                    sub_var_set_long
                    (
                        scp,
                        "Number",
                        c_src_data->about_to_be_created_by
                    );
                    change_error
                    (
                        cid.get_cp(),
                        scp,
                        i18n("file \"$filename\" locked for change $number")
                    );
                    sub_context_delete(scp);
                    ++number_of_errors;
                }
                else if (p_src_orig->action == file_action_remove)
                {
                    // OK.
                    trace(("let it slide\n"));
                }
                else
                {
                    // if usage==build, no logical conflict
                    if (p_src_data->usage != file_usage_build)
                    {
                        // Note: it doesn't matter if we are changing it
                        // to something other than file_usage_build.
                        file_already_in_baseline:
                        sub_context_ty sc;
                        sc.var_set_string("File_Name", c_src_data->file_name);
                        change_error
                        (
                            cid.get_cp(),
                            &sc,
                            i18n("$filename in baseline")
                        );
                        ++number_of_errors;
                        break;
                    }
                    else
                    {
                        switch (c_src_data->action)
                        {
                        case file_action_create:
                            c_src_data->action = file_action_modify;
                            break;

                        case file_action_modify:
                            break;

                        default:
                            goto file_already_in_baseline;
                        }
                    }
                }
            }
            else
            {
                //
                // add a new entry to the pstate src list,
                // and mark it as "about to be created".
                //
                p_src_data = cid.get_pp()->file_new(c_src_data);
                p_src_data->action = file_action_transparent;
                p_src_data->about_to_be_created_by = cid.get_change_number();
            }
            p_src_data->locked_by = cid.get_change_number();
            break;

        case file_action_insulate:
            //
            // There should be no insulation files in the
            // change at develop end time.  This is because
            // if the change is still being insulated from
            // the baseline, something could easily be
            // overlooked.
            //
            scp = sub_context_new();
            sub_var_set_string(scp, "File_Name", c_src_data->file_name);
            change_error(cid.get_cp(), scp, i18n("$filename is insulation"));
            sub_context_delete(scp);
            ++number_of_errors;
            break;
        }
    }

    //
    // If all the files in this change set are marked entire-source-hide
    // we mark the change set with the aeget:inventory:hide attribute,
    // unless the user has already done so.
    //
    if
    (
        (entire_source_hide || local_source_hide)
    &&
        /* see lib/en/man1/aeca.1 */
        !change_attributes_find_boolean(cid.get_cp(), "aeget:inventory:hide")
    )
    {
        change_attributes_append(cstate_data, "aeget:inventory:hide", "true");
    }

    //
    // It is an error if this change is a branch, and there are any
    // changes outstanding on the branch.
    //
    if (is_a_branch)
        project_active_check_branch(cid.get_cp(), 0);

    //
    // if the config file changes,
    // make sure the project file names still conform
    //
    if (config_seen)
    {
        for (j = 0;; ++j)
        {
            fstate_src_ty   *p_src_data;
            string_ty       *e;

            p_src_data = cid.get_pp()->file_nth(j, view_path_extreme);
            if (!p_src_data)
                break;
            if
            (
                cid.get_cp()->file_find
                (
                    nstring(p_src_data->file_name),
                    view_path_first
                )
            )
                continue;

            e = change_filename_check(cid.get_cp(), p_src_data->file_name);
            if (e)
            {
                scp = sub_context_new();
                sub_var_set_string(scp, "MeSsaGe", e);
                str_free(e);
                change_error(cid.get_cp(), scp, i18n("project $message"));
                sub_context_delete(scp);
                ++number_of_errors;
            }
        }
    }

    //
    // verify that the youngest file is older than the
    // build, test, test -bl and test -reg times
    //
    trace(("build required = %s\n",
                (change_build_required(cid.get_cp()) ? "true" : "false")));
    if
    (
        change_build_required(cid.get_cp())
    &&
        (
            !cstate_data->build_time
        ||
            (
                os_unthrottle()
            ?
                youngest > cstate_data->build_time
            :
                youngest >= cstate_data->build_time
            )
        )
    )
    {
        trace(("mark\n"));
        if (youngest_name && cstate_data->build_time)
        {
            scp = sub_context_new();
            sub_var_set_charstar
            (
                scp,
                "Outstanding",
                change_outstanding_builds(cid.get_cp(), youngest)
            );
            sub_var_optional(scp, "Outstanding");
            sub_var_set_string(scp, "File_Name", youngest_name);
            change_error
            (
                cid.get_cp(),
                scp,
                i18n("$filename changed after build")
            );
            sub_context_delete(scp);
        }
        else
        {
            scp = sub_context_new();
            sub_var_set_charstar
            (
                scp,
                "Outstanding",
                change_outstanding_builds(cid.get_cp(), youngest)
            );
            sub_var_optional(scp, "Outstanding");
            change_error(cid.get_cp(), scp, i18n("build required"));
            sub_context_delete(scp);
        }
        ++number_of_errors;
    }
    if
    (
        !cstate_data->test_exempt
    &&
        (
            !cstate_data->test_time
        ||
            (
                os_unthrottle()
            ?
                youngest > cstate_data->test_time
            :
                youngest >= cstate_data->test_time
            )
        )
    )
    {
        scp = sub_context_new();
        sub_var_set_charstar
        (
            scp,
            "Outstanding",
            change_outstanding_tests(cid.get_cp(), youngest)
        );
        sub_var_optional(scp, "Outstanding");
        change_error(cid.get_cp(), scp, i18n("test required"));
        sub_context_delete(scp);
        ++number_of_errors;
    }
    if
    (
        !cstate_data->test_baseline_exempt
    &&
        (
            !cstate_data->test_baseline_time
        ||
            (
                os_unthrottle()
            ?
                youngest > cstate_data->test_baseline_time
            :
                youngest >= cstate_data->test_baseline_time
            )
        )
    )
    {
        scp = sub_context_new();
        sub_var_set_charstar
        (
            scp,
            "Outstanding",
            change_outstanding_tests_baseline(cid.get_cp(), youngest)
        );
        sub_var_optional(scp, "Outstanding");
        change_error(cid.get_cp(), scp, i18n("test -bl required"));
        sub_context_delete(scp);
        ++number_of_errors;
    }
    if
    (
        !cstate_data->regression_test_exempt
    &&
        (
            !cstate_data->regression_test_time
        ||
            (
                os_unthrottle()
            ?
                youngest > cstate_data->regression_test_time
            :
                youngest >= cstate_data->regression_test_time
            )
        )
    )
    {
        scp = sub_context_new();
        sub_var_set_charstar
        (
            scp,
            "Outstanding",
            change_outstanding_tests_regression(cid.get_cp(), youngest)
        );
        sub_var_optional(scp, "Outstanding");
        change_error(cid.get_cp(), scp, i18n("test -reg required"));
        sub_context_delete(scp);
        ++number_of_errors;
    }

    //
    // if there was any problem,
    // stay in 'being developed' state.
    //
    if (number_of_errors)
    {
        scp = sub_context_new();
        sub_var_set_long(scp, "Number", number_of_errors);
        sub_var_optional(scp, "Number");
        change_fatal(cid.get_cp(), scp, i18n("develop end fail"));
        sub_context_delete(scp);
    }
    dd = str_copy(change_development_directory_get(cid.get_cp(), 1));
    str_free(dd);

    //
    // As a last line of defence, run the develop_end_policy_command
    //
    change_run_develop_end_policy_command(cid.get_cp(), cid.get_up());

    //
    // add to history for state change
    // Advance the change to the being-reviewed state.
    //
    history_data = change_history_new(cid.get_cp(), cid.get_up());
    history_data->what = cstate_history_what_develop_end;
    cstate_data->state = cstate_state_being_reviewed;
    switch (project_develop_end_action_get(cid.get_pp()))
    {
    case cstate_branch_develop_end_action_goto_being_reviewed:
        history_data->what = cstate_history_what_develop_end;
        cstate_data->state = cstate_state_being_reviewed;
        break;

    case cstate_branch_develop_end_action_goto_awaiting_review:
        history_data->what = cstate_history_what_develop_end_2ar;
        cstate_data->state = cstate_state_awaiting_review;
        break;

    case cstate_branch_develop_end_action_goto_awaiting_integration:
        history_data->what = cstate_history_what_develop_end_2ai;
        cstate_data->state = cstate_state_awaiting_integration;
        break;
    }
    if (up_admin)
    {
        string_ty *r2 =
            str_format
            (
                "Forced by administrator %s.",
                up_admin->name().quote_c().c_str()
            );
        if (reason)
        {
            string_ty *r1 = reason;
            reason = str_format("%s\n%s", r1->str_text, r2->str_text);
            str_free(r1);
            str_free(r2);
        }
        else
            reason = r2;
    }
    history_data->why = reason;

    //
    // It is tempting to call
    //
    //     change_build_times_clear(cid.get_cp());
    //
    // at this point, but we don't.  This is so that in the event
    // of an aedeu (to get out of the way of another change, maybe)
    // a full build-and-test cycle is only required of they actually
    // edit something.
    //

    //
    // Remove the change from the list of assigned changes in the user
    // change table (in the user row).
    //
    cid.get_up()->own_remove(cid.get_pp(), cid.get_change_number());

    //
    // Make the development directory read only.
    //
    // This is actually conditional upon project_protect_development_
    // directory_get(cid.get_pp()) but the test is inside the change_
    // development_directory_chmod_read_only(cid.get_cp()) function, because it
    // also makes sure the source files are readable by the reviewers.
    //
    change_development_directory_chmod_read_only(cid.get_cp());

    //
    // If the project is configured to use Signed-off-by lines in
    // change descriptions, append a Signed-off-line to this change's
    // description.
    //
    // If the change has a UUID, it means that it almost certainly
    // arrived via aedist or aepatch.  This means that the change
    // (a) was signed off by the sender of the change set and (b)
    // the receiver of the change set has not edited it in any way;
    // therefore there is no need for *this* user to sign off.
    //
    if
    (
        cstate_data->uuid
    ?
        option_signed_off_by_get(false)
    :
        change_signed_off_by_get(cid.get_cp())
    )
        change_signed_off_by(cid.get_cp(), cid.get_up());

    //
    // Write the change table row.
    // Write the user table row.
    // Release advisory locks.
    //
    cid.get_cp()->cstate_write();
    cid.get_pp()->pstate_write();
    cid.get_up()->ustate_write();
    commit();
    lock_release();

    //
    // run the notify command
    //
    switch (project_develop_end_action_get(cid.get_pp()))
    {
#ifndef DEBUG
    default:
#endif
    case cstate_branch_develop_end_action_goto_being_reviewed:
    case cstate_branch_develop_end_action_goto_awaiting_review:
        cid.get_cp()->run_develop_end_notify_command();
        break;

    case cstate_branch_develop_end_action_goto_awaiting_integration:
        cid.get_cp()->run_review_pass_notify_command();
        break;
    }

    //
    // Update the RSS feed file if necessary.
    //
    rss_add_item_by_change(cid.get_pp(), cid.get_cp());

    //
    // verbose success message
    //
    change_verbose(cid.get_cp(), 0, i18n("development completed"));
    trace(("}\n"));
}


void
develop_end(void)
{
    static arglex_dispatch_ty dispatch[] =
    {
        { arglex_token_help, develop_end_help, 0 },
        { arglex_token_list, develop_end_list, 0 },
    };

    trace(("develop_end()\n{\n"));
    arglex_dispatch(dispatch, SIZEOF(dispatch), develop_end_main);
    trace(("}\n"));
}


// vim: set ts=8 sw=4 et :
