/* ============================================================
 * File  : plugin_wsexport.cpp
 * Author: Pontus Freyhult <pontus_wsexport@soua.net>
 * Date  : 2007-04-20
 * Description :
 *
 * Copyright 2007 by Renchi Raju
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * ============================================================ */

// KDE includes.
  
#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>

// LibKIPi includes.

#include <libkipi/imagecollection.h>

// Local includes.

#include "plugin_wsexport.h"
#include "wswindow.h"

// A macro from KDE KParts to export the symbols for this plugin and
// create the factory for it. The first argument is the name of the
// plugin library and the second is the genericfactory templated from
// the class for your plugin

typedef KGenericFactory<Plugin_WSExport> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_wsexport,
                            Factory("kipiplugin_wsexport"));

Plugin_WSExport::Plugin_WSExport(QObject *parent,
                                     const char*,
                                     const QStringList&)
    : KIPI::Plugin( Factory::instance(), parent, "WSExport")
{
  //kdDebug( 51001 ) << "Plugin_WSExport plugin loaded" << endl;
}

void Plugin_WSExport::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    
    // this is our action shown in the menubar/toolbar of the mainwindow
    m_actionWSExport = new KAction (i18n("Export to web service"),
				   0,// do never set shortcuts from plugins.
                                   this,
                                   SLOT(slotActivate()),
                                   actionCollection(),
                                   "wsexport");
    
    addAction( m_actionWSExport );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
    
    if ( !m_interface ) 
           {
           kdError( 51000 ) << "Kipi interface is null!" << endl;
           return;
           }
}

void Plugin_WSExport::slotActivate()
{
  // kdDebug( 51000 ) << "Plugin_WSExport slot activated" << endl;

    // Print some information about the capabilities of the host application.
    // kdDebug( 51000 ) << "Features supported by the host application:" 
    //                 << endl;




    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }


	
    KIPIWSExport::WSWindow *dlg=new KIPIWSExport::WSWindow(interface,
							   kapp->activeWindow());
    dlg->show();


  // kdDebug( 51000 ) << "  AlbumsHaveComments:  " 
  // << (m_interface->hasFeature( KIPI::AlbumsHaveComments ) ? "Yes" : "No") 
			  //                << endl;
    
//     kdDebug( 51000 ) << "  ImagesHasComments:   " 
//                      << (m_interface->hasFeature( KIPI::ImagesHasComments ) ? "Yes" : "No") 
//                      << endl;
                     
//     kdDebug( 51000 ) << "  ImagesHasTime:       " 
//                      << (m_interface->hasFeature( KIPI::ImagesHasTime ) ? "Yes" : "No") 
//                      << endl;
                     
//     kdDebug( 51000 ) << "  SupportsDateRanges:  " 
//                      << (m_interface->hasFeature( KIPI::SupportsDateRanges ) ? "Yes" : "No") 
//                      << endl;
                     
//     kdDebug( 51000 ) << "  AcceptNewImages:     " 
//                      << (m_interface->hasFeature( KIPI::AcceptNewImages ) ? "Yes" : "No") 
//                      << endl;
                     
//     kdDebug( 51000 ) << "  ImageTitlesWritable: " 
//                      << (m_interface->hasFeature( KIPI::ImageTitlesWritable ) ? "Yes" : "No") 
//                      << endl;
                     
//     kdDebug( 51000 ) << "  AlbumsHaveCategory: " 
//                      << (m_interface->hasFeature( KIPI::AlbumsHaveCategory ) ? "Yes" : "No") 
//                      << endl;
                     
//     kdDebug( 51000 ) << "  AlbumsHaveCreationDate: " 
//                      << (m_interface->hasFeature( KIPI::AlbumsHaveCreationDate ) ? "Yes" : "No") 
//                      << endl;

//     kdDebug( 51000 ) << "  AlbumsUseFirstImagePreview: " 
//                      << (m_interface->hasFeature( KIPI::AlbumsUseFirstImagePreview ) ? "Yes" : "No") 
//                      << endl;
                         

    // ================================================== Selection
    
//     kdDebug( 51000 ) << endl
//                      << "==================================================" << endl
//                      << "                    Selection                     " << endl
//                      << "==================================================" << endl;
    
    KIPI::ImageCollection selection = m_interface->currentSelection();
    
    if ( !selection.isValid() ) {
      // kdDebug( 51000) << "No Selection!" << endl;
    }
    else {
        KURL::List images = selection.images();
        
        for( KURL::List::Iterator selIt = images.begin(); selIt != images.end(); ++selIt ) {
	  // kdDebug( 51000 ) <<  *selIt << endl;
            KIPI::ImageInfo info = m_interface->info( *selIt );
	    //   kdDebug( 51000 ) << "\ttitle: " << info.title() << endl;
            if ( m_interface->hasFeature( KIPI::ImagesHasComments ) )
	      {
		//   kdDebug( 51000 ) << "\tdescription: " << info.description() << endl;
	      }
        }
    }

    // ================================================== Current Album
    
    // kdDebug( 51000 ) << endl
    //                  << "==================================================" << endl
//                      << "                    Current Album                 " << endl
//                      << "==================================================" << endl;
    
    KIPI::ImageCollection album = m_interface->currentAlbum();
    
    if ( !album.isValid() ) {
      // kdDebug( 51000 ) << "No album!" << endl;
    }
    else {
        KURL::List images = album.images();
        
        for( KURL::List::Iterator albumIt = images.begin(); albumIt != images.end(); ++albumIt ) {
	  //   kdDebug( 51000 ) <<  *albumIt << endl;
        }

	//        kdDebug( 51000 ) << "Album name: " << album.name() << endl;
        
        if ( m_interface->hasFeature( KIPI::AlbumsHaveComments ) ) {
          //  kdDebug( 51000 ) << "Album Comment: " << album.comment() << endl;
        }
    }
}

KIPI::Category Plugin_WSExport::category( KAction* action ) const
{
    if ( action == m_actionWSExport )
       return KIPI::EXPORTPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::EXPORTPLUGIN; // no warning from compiler, please
}

#include "plugin_wsexport.moc"
