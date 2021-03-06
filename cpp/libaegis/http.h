//
//      aegis - project change supervisor
//      Copyright (C) 2003-2008, 2011, 2012 Peter Miller
//      Copyright (C) 2007 Walter Franzini
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

#ifndef AEGET_HTTP_H
#define AEGET_HTTP_H

#include <libaegis/change.h>

class nstring; // forward
struct project; // forward
struct string_ty; // forward
class nstring; // forward
struct string_list_ty; // forward

enum http_error_t
{
    http_error_continue = 100,
    http_error_switching_protocols = 101,
    http_error_ok = 200,
    http_error_created = 201,
    http_error_accepted = 202,
    http_error_non_authoritative_information = 203,
    http_error_no_content = 204,
    http_error_reset_content = 205,
    http_error_partial_content = 206,
    http_error_multiple_choices = 300,
    http_error_moved_permanently = 301,
    http_error_moved_temporarily = 302,
    http_error_see_other = 303,
    http_error_not_modified = 304,
    http_error_use_proxy = 305,
    http_error_bad_request = 400,
    http_error_unauthorized = 401,
    http_error_payment_required = 402,
    http_error_forbidden = 403,
    http_error_not_found = 404,
    http_error_method_not_allowed = 405,
    http_error_not_acceptable = 406,
    http_error_proxy_authentication_required = 407,
    http_error_request_time_out = 408,
    http_error_conflict = 409,
    http_error_gone = 410,
    http_error_length_required = 411,
    http_error_precondition_failed = 412,
    http_error_request_entity_too_large = 413,
    http_error_request_url_too_large = 414,
    http_error_unsupported_media_type = 415,
    http_error_internal_server = 500,
    http_error_not_implemented = 501,
    http_error_bad_gateway = 502,
    http_error_out_of_resources = 503,
    http_error_gateway_time_out = 504,
    http_error_version_not_supported = 505,
};

extern bool http_fatal_noerror;

void http_fatal(http_error_t oops, const char *fmt, ...)      ATTR_PRINTF(2, 3);
const char *http_getenv(const char *);
void html_escape_string(string_ty *);
void html_escape_string(const nstring &);
void html_escape_charstar(const char *);
void html_encode_string(string_ty *);
void html_encode_string(const nstring &);
void html_encode_charstar(const char *);
void http_content_type_header(string_ty *);

/**
  * The html_header function is used to emit the initial portion of a HTML
  * file, for the necessary meta attrributes and style sheet information.
  *
  * The "html:meta" attributes are inserted at this point.
  */
void html_header(project *, change::pointer);

/**
  * The html_header_os function is used to emit the "html:body-begin"
  * attributes.
  */
void html_header_ps(project *, change::pointer);

/**
  * The html_footer function is used to emit the final page footer,
  * including date and version.
  *
  * The "html:body-end" attributes are inserted at this point.
  */
void html_footer(project *, change::pointer);

/**
  * The emit_change function is used to emit the project name and change
  * number cross linked to all of the relevant pages.  This is used in
  * web page headings.
  */
void emit_change(change::pointer);

/**
  * The emit_change function is used to emit the project name and change
  * number cross linked to all of the relevant pages, except the last
  * element.  This is used in web page headings.
  */
void emit_change_but1(change::pointer);

void
emit_change_uuid_href(change::pointer, const nstring &, const nstring &,
    const nstring &);

/**
  * The http_script_name is used to obtainb the value sof the SCRIPT_NAME
  * environment variable.  This is necessary for web pages which need
  * to refer to other web pages generated by the same script.
  */
const char *http_script_name(void);

/**
  * The emit_project_href function is used to print the leading <a>
  * portion of a project reference.
  */
void emit_project_href(project *pp);
void emit_project_href(project *pp, const char *modifier, ...)
                                                              ATTR_PRINTF(2, 3);

/**
  * The emit_change_href function is used to print the leading <a>
  * portion of a change reference.
  */
void emit_change_href(change::pointer cp, const char *modifier);


/**
  * The emit_change_href_n function is used to print the leading <a>
  * portion of a change reference, with an explict change number.
  */
void emit_change_href_n(project *pp, long change_number,
    const char *modifier);

/**
  * The emit_file_href function is used to print the leading <a> portion
  * of a file reference.
  *
  * \param cp
  *     The change to work within.
  * \param filename
  *     The name of the file being linked.
  * \param modifier
  *     extra equery elements
  */
void emit_file_href(change::pointer cp, const nstring &filename,
    const char *modifier);

/**
  * The emit_file_href function is used to print the leading <a> portion
  * of a file reference.
  *
  * \param cp
  *     The change to work within.
  * \param filename
  *     The name of the file being linked.
  * \param modifier
  *     extra equery elements
  * \note
  *     This function will be DEPRECATED one day
  */
void emit_file_href(change::pointer cp, string_ty *filename,
    const char *modifier);

/**
  * The emit_rect_image function is used to print an &lt;img&gt; of a
  * rectangle, use for drawing histograms.
  *
  * @param width
  *     The width of the image
  * @param height
  *     The height of the image
  * @param label
  *     The label to write on the rectangle
  * @param hspace
  *     The horizontal space desired around the image.
  */
void emit_rect_image(int width, int height, const char *label, int hspace = -1);

/**
  * The emit_rect_image_rgb function is used to print an &lt;img&gt; of a
  * rectangle, use for drawing histograms, of s specific color.
  *
  * @param width
  *     The width of the rectangle image
  * @param height
  *     The height of the rectangle image
  * @param color
  *     The color of the rectangle image
  * @param hspace
  *     The horizontal spacing to add around the rectangle image.
  */
void emit_rect_image_rgb(int width, int height, const char *color,
    int hspace = -1);

/**
  * The modifier_test_and_clear function may be used to look for the
  * named modifier in the modifier list.
  *
  * \param modifiers
  *     The modifiers specified for this web page GET.
  * \param name
  *     The name of the modifier to look for.
  * \returns
  *     true if the modifier is present, false if not
  */
bool modifier_test(string_list_ty *modifiers, const char *name);

/**
  * The modifier_test_and_clear function may be used to look for the
  * named modifier in the modifier list.  The modifier will be premoved
  * if it is found.
  *
  * \param modifiers
  *     The modifiers specified for this web page GET.
  * \param name
  *     The name of the modifier to look for.
  * \returns
  *     true if the modifier is present, false if not
  */
bool modifier_test_and_clear(string_list_ty *modifiers, const char *name);

/**
  * Insert an RSS icon image and link it to an RSS feed file.
  *
  * \param pp
  *      The project details.
  * \param rss_filename
  *      The file name of the RSS feed file.  Not the full path.
  */
void emit_rss_icon_with_link(project *pp, const nstring &rss_filename);

/**
  * Print RSS-related meta data.
  *
  * \param pp
  *      The project details.
  * \param rss_filename
  *      The file name of the RSS feed file.  Not the full path.
  */
void emit_rss_meta_data(project *pp, const nstring &rss_filename);

#define HISTOGRAM_HEIGHT 12
#define HISTOGRAM_WIDTH 120

/**
  * The http_sanitize_content_type function is used to turn
  * "text/whatever" content types into "test/plain" content types,
  * except "text/html".  Other content types are passed through
  * unchanged.
  *
  * @param content_type
  *     The content-type to clean up
  * @returns
  *     text/plain for most text types, otherwise unchanged
  */
nstring http_sanitize_content_type(const nstring &content_type);

#endif // AEGET_HTTP_H
// vim: set ts=8 sw=4 et :
