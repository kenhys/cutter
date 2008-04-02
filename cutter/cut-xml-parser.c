/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-xml-parser.h"

typedef struct _ParseData
{
    CutTestResult *result;
    gchar *option_name;
} ParseData;


static void
set_parse_error (GMarkupParseContext *context,
                 GError             **error,
                 const gchar         *format, ...)
{
    gint line = 0, chr = 0;
    gchar *message, *user_message;
    va_list var_args;

    va_start(var_args, format);
    user_message = g_strdup_vprintf(format, var_args);
    va_end(var_args);

    g_markup_parse_context_get_position(context, &line, &chr);

    message = g_strdup_printf("%s\n at line %d char %d.",
                              user_message, line, chr);

    *error = g_error_new(G_MARKUP_ERROR,
                         G_MARKUP_ERROR_INVALID_CONTENT,
                         message);
    g_free(message);
    g_free(user_message);
}

static void
start_element_handler (GMarkupParseContext *context,
                       const gchar         *element_name,
                       const gchar        **attr_names,
                       const gchar        **attr_values,
                       gpointer             user_data,
                       GError             **error)
{
    ParseData *data = (ParseData *)user_data;

    if (!g_ascii_strcasecmp("result", element_name)) {
        data->result = g_object_new(CUT_TYPE_TEST_RESULT, NULL);
    } else if (!g_ascii_strcasecmp("test-case", element_name)) {
        CutTestCase *test_case;
        test_case = cut_test_case_new(NULL,
                                      NULL, NULL,
                                      NULL, NULL,
                                      NULL, NULL);
        cut_test_result_set_test_case(data->result,
                                      test_case);
        g_object_unref(test_case); 
    } else if (!g_ascii_strcasecmp("test", element_name)) {
        CutTest *test;
        test = cut_test_new(NULL, NULL);
        cut_test_result_set_test(data->result, test);
        g_object_unref(test); 
    }
}

static void
end_element_handler (GMarkupParseContext *context,
                     const gchar         *element_name,
                     gpointer             user_data,
                     GError             **error)
{
}

static CutTestResultStatus
result_name_to_status (const gchar *name)
{
    if (!g_ascii_strcasecmp(name, "success"))
        return CUT_TEST_RESULT_SUCCESS;
    else if (!g_ascii_strcasecmp(name, "failure"))
        return CUT_TEST_RESULT_FAILURE;
    else if (!g_ascii_strcasecmp(name, "error"))
         return CUT_TEST_RESULT_ERROR;
    else if (!g_ascii_strcasecmp(name, "pending"))
         return CUT_TEST_RESULT_PENDING;
    else if (!g_ascii_strcasecmp(name, "notification"))
        return CUT_TEST_RESULT_NOTIFICATION;

    return -1;
}

static const gchar *
get_parent_element (GMarkupParseContext *context)
{
    const GSList *elements, *node;

    elements = g_markup_parse_context_get_element_stack(context);

    node = g_slist_next(elements);

    return node ? node->data : NULL;
}

static void
free_option_name (ParseData *data)
{
    if (data->option_name) {
        g_free(data->option_name);
        data->option_name = NULL;
    }
}

static void
set_option_name (ParseData *data, const gchar *option_name)
{
    free_option_name(data);

    if (option_name)
        data->option_name = g_strdup(option_name);
}

static void
set_option_value (GMarkupParseContext *context,
                  ParseData *data,
                  const gchar *value,
                  GError **error)
{
    const gchar *parent;

    parent = get_parent_element(context);

    if (!g_ascii_strcasecmp("option", parent)) {
        if (data->option_name) {
            CutTest *test;
            test = cut_test_result_get_test(data->result);
            cut_test_set_attribute(test, data->option_name, value);
        } else {
            set_parse_error(context, error, 
                            "option name is not set.");
        }
    } else {
        set_parse_error(context, error, 
                        "<value> element should be in <option>.");
    }
}

static void
text_handler (GMarkupParseContext *context,
              const gchar         *text,
              gsize                text_len,
              gpointer             user_data,
              GError             **error)
{
    ParseData *data = (ParseData *)user_data;
    const gchar *element, *parent;

    element = g_markup_parse_context_get_element(context);
    parent = get_parent_element(context);
    
    if (!g_ascii_strcasecmp("name", element)) {
        if (!g_ascii_strcasecmp("test-case", parent)) {
            cut_test_set_name(CUT_TEST(cut_test_result_get_test_case(data->result)),
                              text);
        } else if (!g_ascii_strcasecmp("test", parent)) {
            cut_test_set_name(cut_test_result_get_test(data->result), text);
        } else if (!g_ascii_strcasecmp("option", parent)) {
            set_option_name(data, text);
        }
    } else if (!g_ascii_strcasecmp("description", element)) {
        cut_test_set_attribute(cut_test_result_get_test(data->result),
                               "description", text);
    } else if (!g_ascii_strcasecmp("value", element)) {
        set_option_value(context, data, text, error);
    } else if (!g_ascii_strcasecmp("detail", element)) {
        cut_test_result_set_system_message(data->result, text);
    } else if (!g_ascii_strcasecmp("file", element)) {
        cut_test_result_set_filename(data->result, text);
    } else if (!g_ascii_strcasecmp("line", element)) {
        cut_test_result_set_line(data->result, atoi(text));
    } else if (!g_ascii_strcasecmp("info", element)) {
        cut_test_result_set_message(data->result, text);
    } else if (!g_ascii_strcasecmp("status", element)) {
        cut_test_result_set_status(data->result, result_name_to_status(text));
    } else if (!g_ascii_strcasecmp("elapsed", element)) {
        cut_test_result_set_elapsed(data->result, g_ascii_strtod(text, NULL));
    }
}

static void
passthrough_handler (GMarkupParseContext *context,
                     const gchar         *text,
                     gsize                text_len,
                     gpointer             user_data,
                     GError             **error)
{
}

static void
error_handler (GMarkupParseContext *context,
               GError              *error,
               gpointer             user_data)
{
}

static GMarkupParser parser = {
    start_element_handler,
    end_element_handler,
    text_handler,
    passthrough_handler,
    error_handler,
};

static void
init_parse_data (ParseData *data)
{
    data->result = NULL;
    data->option_name = NULL;
}

static void
free_parse_data (ParseData *data)
{
    if (data->option_name)
        g_free(data->option_name);
}

CutTestResult *
cut_xml_parse_test_result_xml (const gchar *xml, gssize len)
{
    GMarkupParseContext *parse_context;
    ParseData data;
    GError *error = NULL;

    init_parse_data(&data);

    parse_context = g_markup_parse_context_new(&parser, 
                                               G_MARKUP_TREAT_CDATA_AS_TEXT,
                                               &data, NULL);
    if (!g_markup_parse_context_parse(parse_context, xml, len, &error)) {
        g_error_free(error);
    }

    g_markup_parse_context_free(parse_context);
    free_parse_data(&data);

    return data.result;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
