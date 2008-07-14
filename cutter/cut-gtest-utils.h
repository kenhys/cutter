/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_GTEST_UTILS_H__
#define __CUT_GTEST_UTILS_H__

#include <cutter/cut-gpublic.h>

G_BEGIN_DECLS

/**
 * SECTION: cut-gtest-utils
 * @title: Test Utilities with GLib support
 * @short_description: Utilities to write tests more easily
 * with GLib support.
 *
 * To write tests, you need to write codes that
 * set up/tear down test environment, prepare expected and
 * actual values and so on. Cutter provides test utilities
 * to you write your tests more easily.
 *
 * The utilities work with GLib.
 */


/**
 * cut_take_g_object:
 * @object: the GObject * to be owned by Cutter.
 *
 * Passes ownership of @object to Cutter and returns @object.
 *
 * Returns: @object owned by Cutter. Don't g_object_unref() it.
 */
#define cut_take_g_object(object)                                       \
    cut_test_context_take_g_object(get_current_test_context(), (object))

/**
 * cut_take_g_error:
 * @error: the GError * to be owned by Cutter.
 *
 * Passes ownership of @error to Cutter and returns
 * a GError * that has same contents of @error.
 *
 * Returns: a GError * owned by Cutter. Don't g_error_free()
 * it.
 */
#define cut_take_g_error(error)                                         \
    cut_test_context_take_g_error(get_current_test_context(), (error))

/**
 * cut_take_g_list:
 * @list: the GList * to be owned by Cutter.
 * @destroy_function: the destroy function
 * (#CutDestroyFunction) that destroys the elements of
 * @list, or NULL.
 *
 * Passes ownership of @list to Cutter and returns
 * a GList * that has same elements of @list.
 *
 * Returns: a GList * owned by Cutter. Don't g_list_free()
 * it.
 */
#define cut_take_g_list(list, destroy_function)                         \
    cut_test_context_take_g_list(get_current_test_context(),            \
                                 (list), (destroy_function))

G_END_DECLS

#endif /* __CUT_GTEST_UTILS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
