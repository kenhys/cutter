/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gmodule.h>

#include "cut-loader.h"

#define CUT_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_LOADER, CutLoaderPrivate))

typedef struct _CutLoaderPrivate	CutLoaderPrivate;
struct _CutLoaderPrivate
{
    gchar *so_filename;
    GModule *module;
    CutTestEntry *tests;
    guint *tests_len;
};

enum
{
    PROP_0,
    PROP_SO_FILENAME
};

G_DEFINE_TYPE (CutLoader, cut_loader, G_TYPE_OBJECT)

static void dispose         (GObject               *object);
static void set_property    (GObject               *object,
                             guint                  prop_id,
                             const GValue          *value,
                             GParamSpec            *pspec);
static void get_property    (GObject               *object,
                             guint                  prop_id,
                             GValue                *value,
                             GParamSpec            *pspec);

static void
cut_loader_class_init (CutLoaderClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("so-filename",
                               ".so filename",
                               "The filename of shared object",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_SO_FILENAME, spec);

    g_type_class_add_private(gobject_class, sizeof(CutLoaderPrivate));
}

static void
cut_loader_init (CutLoader *loader)
{
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(loader);

    priv->so_filename = NULL;
    priv->tests = NULL;
    priv->tests_len = NULL;
}

static void
dispose (GObject *object)
{
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(object);

    if (priv->so_filename) {
        g_free(priv->so_filename);
        priv->so_filename = NULL;
    }

    if (priv->module) {
        g_module_close(priv->module);
        priv->module = NULL;
    }

    G_OBJECT_CLASS(cut_loader_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SO_FILENAME:
        if (priv->so_filename)
            g_free(priv->so_filename);
        priv->so_filename = g_value_dup_string(value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SO_FILENAME:
        g_value_set_string(value, priv->so_filename);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutLoader *
cut_loader_new (const gchar *soname)
{
    return g_object_new(CUT_TYPE_LOADER,
                        "so-filename", soname,
                        NULL);
}

CutTestCase *
cut_loader_load_test_case (CutLoader *loader)
{
    guint i;
    CutTestCase *test_case;
    CutSetupFunction setup_function = NULL;
    CutTearDownFunction teardown_function = NULL;
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(loader);

    if (!priv->so_filename)
        return NULL;

    priv->module = g_module_open(priv->so_filename, G_MODULE_BIND_LAZY);
    if (!priv->module)
        return NULL;

    g_module_symbol(priv->module,
                    "cut_tests",
                    (gpointer)&priv->tests);
    g_module_symbol(priv->module,
                    "cut_tests_len",
                    (gpointer)&priv->tests_len);

    if (!priv->tests || !priv->tests_len || !*priv->tests_len)
        return NULL;

    g_module_symbol(priv->module,
                    "setup",
                    (gpointer)&setup_function);
    g_module_symbol(priv->module,
                    "teardown",
                    (gpointer)&teardown_function);

    test_case = cut_test_case_new(setup_function, teardown_function);
    for (i = 0; i < *priv->tests_len; i++) {
        CutTestEntry t = priv->tests[i];
        CutTest *test;
        test = cut_test_new(t.function);
        cut_test_case_add_test(test_case, test);
    }

    return test_case;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
