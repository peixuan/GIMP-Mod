## Process this file with automake to produce Makefile.in

libgimpbase = $(top_builddir)/libgimpbase/libgimpbase-$(GIMP_API_VERSION).la
libgimpconfig = $(top_builddir)/libgimpconfig/libgimpconfig-$(GIMP_API_VERSION).la
libgimpcolor = $(top_builddir)/libgimpcolor/libgimpcolor-$(GIMP_API_VERSION).la
libgimpmath = $(top_builddir)/libgimpmath/libgimpmath-$(GIMP_API_VERSION).la
libgimpmodule = $(top_builddir)/libgimpmodule/libgimpmodule-$(GIMP_API_VERSION).la
libgimpwidgets = $(top_builddir)/libgimpwidgets/libgimpwidgets-$(GIMP_API_VERSION).la
libgimpthumb = $(top_builddir)/libgimpthumb/libgimpthumb-$(GIMP_API_VERSION).la

# Sort this by architectural dependencies, lowest level at the top,
# so that when e.g. changing a header-file the subdirs are built in
# the right order
SUBDIRS = \
	base		\
	paint-funcs	\
	config		\
	composite	\
	core		\
	gegl		\
	text		\
	vectors		\
	paint		\
	plug-in		\
	xcf		\
	file		\
	pdb		\
	widgets		\
	display		\
	tools		\
	dialogs		\
	actions		\
	menus		\
	gui		\
	.		\
	tests

# Put the GIMP core in a lib so we can conveniently link against that
# in test cases
noinst_LIBRARIES = libapp.a

if ENABLE_GIMP_CONSOLE
bin_PROGRAMS = gimp-2.7 gimp-console-2.7
else
bin_PROGRAMS = gimp-2.7
endif

libapp_sources = \
	about.h		\
	app.c		\
	app.h		\
	batch.c		\
	batch.h		\
	errors.c	\
	errors.h	\
	language.c	\
	language.h	\
	sanity.c	\
	sanity.h	\
	signals.c	\
	signals.h	\
	tests.c		\
	tests.h		\
	unique.c	\
	unique.h	\
	units.c		\
	units.h		\
	version.c	\
	version.h	\
	gimp-debug.c	\
	gimp-debug.h	\
	gimp-log.c	\
	gimp-log.h	\
	gimp-intl.h

libapp_generated_sources = \
	git-version.h

# Build git-version.h before anything in the subdirs as this is needed
# in the about dialog.
BUILT_SOURCES = $(libapp_generated_sources)

CLEANFILES = $(libapp_generated_sources)

$(srcdir)/version.c: git-version.h

libapp_a_SOURCES = $(libapp_sources) $(libapp_generated_sources)

gimp_2_7_SOURCES = $(libapp_sources) main.c


if OS_WIN32
win32_ldflags = -mwindows $(WIN32_LARGE_ADDRESS_AWARE)
endif

if USE_BINRELOC
munix = -Wl,-rpath '-Wl,$$ORIGIN/../lib'
endif

if HAVE_WINDRES
include $(top_srcdir)/build/windows/gimprc.rule
GIMPRC = gimp-$(GIMP_APP_VERSION).rc.o
GIMPCONSOLERC = gimp-console-$(GIMP_APP_VERSION).rc.o
endif

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"Gimp\"		\
	-DGIMP_APP_GLUE_COMPILATION

INCLUDES = \
	-I$(top_srcdir)		\
	$(GTK_CFLAGS)		\
	$(PANGOCAIRO_CFLAGS)	\
	$(GEGL_CFLAGS)		\
	-I$(includedir)

# FIXME: core should not depend on xcf
workaround_that_core_depends_on_xcf = \
	-u $(SYMPREFIX)xcf_init

# FIXME: core should not depend on pdb
workaround_that_core_depends_on_pdb = \
	-u $(SYMPREFIX)internal_procs_init		\
	-u $(SYMPREFIX)gimp_plug_in_manager_restore

# FIXME: plug-in should not depend on pdb
workaround_that_plug_in_depends_on_pdb = \
	-u $(SYMPREFIX)gimp_pdb_compat_param_spec

# FIXME: file should not depend on plug-in
workaround_that_file_depends_on_plug_in = \
	-u $(SYMPREFIX)plug_in_icc_profile_apply_rgb	\
	-u $(SYMPREFIX)gimp_image_map_config_get_type

# core, vectors and gegl are on the same architectural layer, prevent
# the linker from panicing
calm_down_linker = \
	-u $(SYMPREFIX)gimp_vectors_undo_get_type	\
	-u $(SYMPREFIX)gimp_vectors_mod_undo_get_type	\
	-u $(SYMPREFIX)gimp_vectors_prop_undo_get_type	\
	-u $(SYMPREFIX)gimp_curve_map_pixels

AM_LDFLAGS = \
	$(munix)					\
	$(CARBON_LDFLAGS)				\
	$(calm_down_linker)				\
	$(workaround_that_core_depends_on_xcf)		\
	$(workaround_that_core_depends_on_pdb)		\
	$(workaround_that_plug_in_depends_on_pdb)	\
	$(workaround_that_file_depends_on_plug_in)

gimpconsoleldadd = \
	xcf/libappxcf.a			\
	pdb/libappinternal-procs.a	\
	pdb/libapppdb.a			\
	plug-in/libappplug-in.a		\
	vectors/libappvectors.a		\
	core/libappcore.a		\
	file/libappfile.a		\
	text/libapptext.a		\
	paint/libapppaint.a		\
	composite/libappcomposite.a	\
	gegl/libappgegl.a		\
	config/libappconfig.a		\
	paint-funcs/libapppaint-funcs.a	\
	base/libappbase.a		\
	$(libgimpconfig)		\
	$(libgimpmath)			\
	$(libgimpthumb)			\
	$(libgimpcolor)			\
	$(libgimpmodule)		\
	$(libgimpbase)			\
	$(GDK_PIXBUF_LIBS)		\
	$(FREETYPE_LIBS)		\
	$(FONTCONFIG_LIBS)		\
	$(PANGOCAIRO_LIBS)		\
	$(CAIRO_LIBS)			\
	$(GEGL_LIBS)			\
	$(GLIB_LIBS)			\
	$(INTLLIBS)			\
	$(RT_LIBS)

gimp_2_7_LDFLAGS = $(AM_LDFLAGS) $(win32_ldflags)

gimp_2_7_LDADD = \
	gui/libappgui.a		\
	menus/libappmenus.a	\
	actions/libappactions.a	\
	dialogs/libappdialogs.a	\
	tools/libapptools.a	\
	display/libappdisplay.a	\
	widgets/libappwidgets.a	\
	$(libgimpwidgets)	\
	$(GTK_LIBS)		\
	$(DBUS_GLIB_LIBS)	\
	$(gimpconsoleldadd)	\
	$(GIMPRC)


if ENABLE_GIMP_CONSOLE

gimp_console_2_7_SOURCES = $(libapp_sources) main.c

gimp_console_2_7_CPPFLAGS = \
	$(AM_CPPFLAGS)			\
	-DGIMP_CONSOLE_COMPILATION

gimp_console_2_7_LDADD = \
	$(gimpconsoleldadd) \
	$(GIMPCONSOLERC)

endif


install-exec-hook:
if DEFAULT_BINARY
	cd $(DESTDIR)$(bindir) \
	&& rm -f gimp$(EXEEXT) \
	&& $(LN_S) gimp-$(GIMP_APP_VERSION)$(EXEEXT) gimp$(EXEEXT)
if ENABLE_GIMP_CONSOLE
	cd $(DESTDIR)$(bindir) \
	&& rm -f gimp-console$(EXEEXT) \
	&& $(LN_S) gimp-console-$(GIMP_APP_VERSION)$(EXEEXT) gimp-console$(EXEEXT)
endif
endif

uninstall-local:
if DEFAULT_BINARY
	rm -f $(DESTDIR)$(bindir)/gimp$(EXEEXT)
if ENABLE_GIMP_CONSOLE
	rm -f $(DESTDIR)$(bindir)/gimp-console$(EXEEXT)
endif
endif


# require gimp-console when making dist
#
if ENABLE_GIMP_CONSOLE
dist-check-gimp-console:
else
dist-check-gimp-console:
	@echo "*** gimp-console must be enabled in order to make dist"
	@false
endif


# hook to assure that the system gimprc and the gimprc manpage are
# uptodate when a release is made
#
dist-dump-gimprc: gimp-console-$(GIMP_APP_VERSION)$(EXEEXT)
	./$< --dump-gimprc-system > gimprc.tmp	\
		&& sed -e "s/num-processors [0-9]*/num-processors 1/"	\
		   gimprc.tmp > gimprc.tmp2				\
		&& (cmp -s gimprc.tmp2 $(top_srcdir)/etc/gimprc ||	\
		    cp gimprc.tmp2 $(top_srcdir)/etc/gimprc)		\
		&& rm gimprc.tmp gimprc.tmp2
	./$< --dump-gimprc-manpage > gimprc.tmp	\
		&& sed -e "s/num-processors [0-9]*/num-processors 1/"	\
		   gimprc.tmp > gimprc.tmp2				\
		&& (cmp -s gimprc.tmp2 $(top_srcdir)/docs/gimprc.5.in ||\
		    cp gimprc.tmp2 $(top_srcdir)/docs/gimprc.5.in)	\
		&& rm gimprc.tmp gimprc.tmp2

dist-hook: dist-check-gimp-console dist-dump-gimprc

# If git is available, always check if git-version.h should be
# updated. If git is not available, don't do anything if git-version.h
# already exists because then we are probably working with a tarball
# in which case the git-version.h we ship is correct.
git-version.h: update-git-version-header
	@if test -d "$(top_srcdir)/.git"; then \
	  git_version="`git --git-dir=$(top_srcdir)/.git describe --always`"; \
	  git_last_commit_year="`git --git-dir=$(top_srcdir)/.git log -n1 --reverse --pretty=%ci | cut -b 1-4`"; \
	elif test ! -f "$@"; then \
	  git_version="Unknown, shouldn't happen"; \
	  git_last_commit_timestamp=-1; \
	  git_last_commit_year="`date -u '+%Y'`"; \
	fi; \
	if test -n "$$git_version"; then \
	  echo "#ifndef __GIT_VERSION_H__" > "$@.tmp"; \
	  echo "#define __GIT_VERSION_H__" >> "$@.tmp"; \
	  echo "#define GIMP_GIT_VERSION \"$$git_version\"" >> "$@.tmp"; \
	  echo "#define GIMP_GIT_LAST_COMMIT_YEAR \"$$git_last_commit_year\"" >> "$@.tmp"; \
	  echo "#endif /* __GIT_VERSION_H__ */" >> "$@.tmp"; \
	fi
	@if ( test -f "$@.tmp" && test -f "$@" && cmp "$@.tmp" "$@" > /dev/null ); then \
	  rm -f "$@.tmp"; \
	elif test -f "$@.tmp"; then \
	  mv "$@.tmp" "$@"; \
	  echo "  git HEAD changed: $@ regenerated"; \
	fi

.PHONY: update-git-version-header
