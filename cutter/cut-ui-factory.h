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

#ifndef __CUT_UI_FACTORY_H__
#define __CUT_UI_FACTORY_H__

#include <glib-object.h>

#include <cutter/cut-ui.h>

G_BEGIN_DECLS

#define CUT_TYPE_UI_FACTORY            (cut_ui_factory_get_type ())
#define CUT_UI_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_UI_FACTORY, CutUIFactory))
#define CUT_UI_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_UI_FACTORY, CutUIFactoryClass))
#define CUT_IS_UI_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_UI_FACTORY))
#define CUT_IS_UI_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_UI_FACTORY))
#define CUT_UI_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_UI_FACTORY, CutUIFactoryClass))

typedef struct _CutUIFactory         CutUIFactory;
typedef struct _CutUIFactoryClass    CutUIFactoryClass;

struct _CutUIFactory
{
    GObject object;
};

struct _CutUIFactoryClass
{
    GObjectClass parent_class;

    void         (*set_option_group)    (CutUIFactory *factory,
                                         GOptionContext *context);
    CutUI       *(*create)              (CutUIFactory *factory);
};

GType           cut_ui_factory_get_type    (void) G_GNUC_CONST;

void            cut_ui_factory_init        (void);
void            cut_ui_factory_quit        (void);

const gchar    *cut_ui_factory_get_default_module_dir   (void);
void            cut_ui_factory_set_default_module_dir   (const gchar *dir);

void            cut_ui_factory_load        (const gchar *base_dir);
void            cut_ui_factory_unload      (void);
GList          *cut_ui_factory_get_registered_types (void);
GList          *cut_ui_factory_get_log_domains      (void);
GList          *cut_ui_factory_get_names   (void);

CutUIFactory   *cut_ui_factory_new         (const gchar *name,
                                            const gchar *first_property,
                                            ...);

void         cut_ui_factory_set_option_group (CutUIFactory *factory,
                                              GOptionContext *context);
CutUI       *cut_ui_factory_create           (CutUIFactory *factory);
const gchar *cut_ui_factory_get_name         (CutUIFactory *factory);

G_END_DECLS

#endif /* __CUT_UI_FACTORY_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/