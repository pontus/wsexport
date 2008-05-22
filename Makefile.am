INCLUDES    = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_wsexport.la
kipiplugin_wsexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
# Srcs for the plugin
kipiplugin_wsexport_la_SOURCES = plugin_wsexport.cpp wswindow.cpp wsbase.cpp wswidget.cpp wswidget.ui.h wssmugmug.cpp wspicasa.cpp

# Libs needed by the plugin
kipiplugin_wsexport_la_LIBADD = $(LIBKIPI_LIBS) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_wsexport_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries)  -lkipiplugins


# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_wsexport.desktop

# i18n translation messages 
#Nota : no need a translation file for this plugin.
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_wsexport.pot



%.h: %.ui
	uic $< -o $@

%.cpp: %.ui
	uic -impl $*.h $< -o $@

#%.moc: %.h
#	moc $< > $@
