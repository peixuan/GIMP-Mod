## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"Gimp-Display\"

INCLUDES = \
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_builddir)/app	\
	-I$(top_srcdir)/app	\
	$(GEGL_CFLAGS)		\
	$(GTK_CFLAGS)		\
	-I$(includedir)

noinst_LIBRARIES = libappdisplay.a

libappdisplay_a_sources = \
	display-enums.h				\
	display-types.h				\
	gimpcanvas.c				\
	gimpcanvas.h				\
	gimpcanvasarc.c				\
	gimpcanvasarc.h				\
	gimpcanvasboundary.c			\
	gimpcanvasboundary.h			\
	gimpcanvascorner.c			\
	gimpcanvascorner.h			\
	gimpcanvascursor.c			\
	gimpcanvascursor.h			\
	gimpcanvasgrid.c			\
	gimpcanvasgrid.h			\
	gimpcanvasgroup.c			\
	gimpcanvasgroup.h			\
	gimpcanvasguide.c			\
	gimpcanvasguide.h			\
	gimpcanvashandle.c			\
	gimpcanvashandle.h			\
	gimpcanvasitem.c			\
	gimpcanvasitem.h			\
	gimpcanvasitem-utils.c			\
	gimpcanvasitem-utils.h			\
	gimpcanvaslayerboundary.c		\
	gimpcanvaslayerboundary.h		\
	gimpcanvasline.c			\
	gimpcanvasline.h			\
	gimpcanvaspassepartout.c		\
	gimpcanvaspassepartout.h		\
	gimpcanvaspath.c			\
	gimpcanvaspath.h			\
	gimpcanvaspen.c				\
	gimpcanvaspen.h				\
	gimpcanvaspolygon.c			\
	gimpcanvaspolygon.h			\
	gimpcanvasprogress.c			\
	gimpcanvasprogress.h			\
	gimpcanvasproxygroup.c			\
	gimpcanvasproxygroup.h			\
	gimpcanvasrectangle.c			\
	gimpcanvasrectangle.h			\
	gimpcanvasrectangleguides.c		\
	gimpcanvasrectangleguides.h		\
	gimpcanvassamplepoint.c			\
	gimpcanvassamplepoint.h			\
	gimpcanvastextcursor.c			\
	gimpcanvastextcursor.h			\
	gimpcanvastransformguides.c		\
	gimpcanvastransformguides.h		\
	gimpcanvastransformpreview.c		\
	gimpcanvastransformpreview.h		\
	gimpcursorview.c			\
	gimpcursorview.h			\
	gimpdisplay.c				\
	gimpdisplay.h				\
	gimpdisplay-foreach.c			\
	gimpdisplay-foreach.h			\
	gimpdisplay-handlers.c			\
	gimpdisplay-handlers.h			\
	gimpdisplay-utils.c			\
	gimpdisplay-utils.h			\
	gimpdisplayshell.c			\
	gimpdisplayshell.h			\
	gimpdisplayshell-appearance.c		\
	gimpdisplayshell-appearance.h		\
	gimpdisplayshell-autoscroll.c		\
	gimpdisplayshell-autoscroll.h		\
	gimpdisplayshell-callbacks.c		\
	gimpdisplayshell-callbacks.h		\
	gimpdisplayshell-close.c		\
	gimpdisplayshell-close.h		\
	gimpdisplayshell-cursor.c		\
	gimpdisplayshell-cursor.h		\
	gimpdisplayshell-dnd.c			\
	gimpdisplayshell-dnd.h			\
	gimpdisplayshell-draw.c			\
	gimpdisplayshell-draw.h			\
	gimpdisplayshell-expose.c		\
	gimpdisplayshell-expose.h		\
	gimpdisplayshell-grab.c			\
	gimpdisplayshell-grab.h			\
	gimpdisplayshell-handlers.c		\
	gimpdisplayshell-handlers.h		\
	gimpdisplayshell-filter.c		\
	gimpdisplayshell-filter.h		\
	gimpdisplayshell-filter-dialog.c	\
	gimpdisplayshell-filter-dialog.h	\
	gimpdisplayshell-layer-select.c		\
	gimpdisplayshell-layer-select.h		\
	gimpdisplayshell-icon.c			\
	gimpdisplayshell-icon.h			\
	gimpdisplayshell-items.c		\
	gimpdisplayshell-items.h		\
	gimpdisplayshell-progress.c		\
	gimpdisplayshell-progress.h		\
	gimpdisplayshell-render.c		\
	gimpdisplayshell-render.h		\
	gimpdisplayshell-scale.c		\
	gimpdisplayshell-scale.h		\
	gimpdisplayshell-scale-dialog.c		\
	gimpdisplayshell-scale-dialog.h		\
	gimpdisplayshell-scroll.c		\
	gimpdisplayshell-scroll.h		\
	gimpdisplayshell-selection.c		\
	gimpdisplayshell-selection.h		\
	gimpdisplayshell-style.c		\
	gimpdisplayshell-style.h		\
	gimpdisplayshell-title.c		\
	gimpdisplayshell-title.h		\
	gimpdisplayshell-tool-events.c		\
	gimpdisplayshell-tool-events.h		\
	gimpdisplayshell-transform.c		\
	gimpdisplayshell-transform.h		\
	gimpimagewindow.c			\
	gimpimagewindow.h			\
	gimpmotionbuffer.c			\
	gimpmotionbuffer.h			\
	gimpmultiwindowstrategy.c		\
	gimpmultiwindowstrategy.h		\
	gimpnavigationeditor.c			\
	gimpnavigationeditor.h			\
	gimpscalecombobox.c			\
	gimpscalecombobox.h			\
	gimpsinglewindowstrategy.c		\
	gimpsinglewindowstrategy.h		\
	gimpstatusbar.c				\
	gimpstatusbar.h				\
	gimptooldialog.c			\
	gimptooldialog.h			\
	gimpwindowstrategy.c			\
	gimpwindowstrategy.h

libappdisplay_a_built_sources = display-enums.c

libappdisplay_a_SOURCES = \
	$(libappdisplay_a_built_sources)	\
	$(libappdisplay_a_sources)

#
# rules to generate built sources
#
# setup autogeneration dependencies
gen_sources = xgen-dec
CLEANFILES = $(gen_sources)

display-enums.c: $(srcdir)/display-enums.h $(GIMP_MKENUMS)
	$(GIMP_MKENUMS) \
		--fhead "#include \"config.h\"\n#include <glib-object.h>\n#include \"libgimpbase/gimpbase.h\"\n#include \"display-enums.h\"\n#include\"gimp-intl.h\"" \
		--fprod "\n/* enumerations from \"@filename@\" */" \
		--vhead "GType\n@enum_name@_get_type (void)\n{\n  static const G@Type@Value values[] =\n  {" \
		--vprod "    { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
		--vtail "    { 0, NULL, NULL }\n  };\n" \
		--dhead "  static const Gimp@Type@Desc descs[] =\n  {" \
		--dprod "    { @VALUENAME@, @valuedesc@, @valuehelp@ }," \
		--dtail "    { 0, NULL, NULL }\n  };\n\n  static GType type = 0;\n\n  if (G_UNLIKELY (! type))\n    {\n      type = g_@type@_register_static (\"@EnumName@\", values);\n      gimp_type_set_translation_context (type, \"@enumnick@\");\n      gimp_@type@_set_value_descriptions (type, descs);\n    }\n\n  return type;\n}\n" \
		$(srcdir)/display-enums.h > xgen-dec \
	&& cp xgen-dec $(@F) \
	&& rm -f xgen-dec
