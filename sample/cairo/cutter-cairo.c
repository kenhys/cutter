/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 8; -*- */

#include <stdlib.h>

#include <cutter.h>
#include <cut-main.h>

#include <cairo.h>
#ifdef CAIRO_HAS_SVG_SURFACE
#  include <cairo-svg.h>
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
#  include <cairo-pdf.h>
#endif
#ifdef CAIRO_HAS_PS_SURFACE
#  include <cairo-ps.h>
#endif

static gboolean
run_test (const gchar *directory, GCallback create_surface)
{
    CutContext *context;
    CutTestSuite *suite;
    gboolean success;
    cairo_surface_t *surface;

    context = cut_create_context ();
    g_signal_connect (context, "start-test", create_surface, &surface);

    suite = cut_create_test_suite (directory);
    success = cut_run_test_suite (suite, context);
    g_object_unref (suite);

    g_signal_handlers_disconnect_by_func (context, create_surface, &surface);
    g_object_unref (context);

    return success;
}

static void
destroy_surface (gpointer data)
{
    cairo_surface_t **surface = data;

    if (*surface)
        cairo_surface_destroy (*surface);
    *surface = NULL;
}

static void
create_image_surface (CutContext *context, CutTest *test,
                      CutTestContext *test_context, gpointer data)
{
    cairo_surface_t **surface = data;

    *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 100, 100);
    cut_test_context_set_user_data (test_context, *surface, destroy_surface);
}

#ifdef CAIRO_HAS_SVG_SURFACE
static void
create_svg_surface (CutContext *context, CutTest *test,
                    CutTestContext *test_context, gpointer data)
{
    cairo_surface_t **surface = data;

    *surface = cairo_svg_surface_create ("nil-surface.svg", 100, 100);
    cut_test_context_set_user_data (test_context, *surface, destroy_surface);
}
#endif

#ifdef CAIRO_HAS_PDF_SURFACE
static void
create_pdf_surface (CutContext *context, CutTest *test,
                    CutTestContext *test_context, gpointer data)
{
    cairo_surface_t **surface = data;

    *surface = cairo_pdf_surface_create ("nil-surface.pdf", 100, 100);
    cut_test_context_set_user_data (test_context, *surface, destroy_surface);
}
#endif

#ifdef CAIRO_HAS_PS_SURFACE
static void
create_ps_surface (CutContext *context, CutTest *test,
                   CutTestContext *test_context, gpointer data)
{
    cairo_surface_t **surface = data;

    *surface = cairo_ps_surface_create ("nil-surface.ps", 100, 100);
    cut_test_context_set_user_data (test_context, *surface, destroy_surface);
}
#endif

int
main (int argc, char *argv[])
{
    gboolean success = TRUE;

    cut_init (&argc, &argv);

    if (!run_test (argv[1], G_CALLBACK(create_image_surface)))
        success = FALSE;
#ifdef CAIRO_HAS_SVG_SURFACE
    if (!run_test (argv[1], G_CALLBACK(create_svg_surface)))
        success = FALSE;
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
    if (!run_test (argv[1], G_CALLBACK(create_pdf_surface)))
        success = FALSE;
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    if (!run_test (argv[1], G_CALLBACK(create_ps_surface)))
        success = FALSE;
#endif

    exit (success ? 0 : 1);
}
