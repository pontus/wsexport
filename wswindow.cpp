/* ============================================================
 * File  : wswindow.cpp
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

// QT Includes

#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <Q3ValueList>

// KDE includes.
  
#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kled.h>


// LibKIPi includes.

#include <libkipi/imagecollection.h>

// Local includes.

#include "wswindow.h"
#include "wswidget.h"

#include "wssmugmug.h"
#include "wspicasa.h"

namespace KIPIWSExport
{
  
  WSWindow::WSWindow(KIPI::Interface* interface,
		     QWidget *parent) :
    KDialogBase(parent, 0, false, i18n("Web Service Export"),
		Help|Close, Close, false),
    interface(interface), service(0)
  {

     about = new KIPIPlugins::KPAboutData(I18N_NOOP("Web Service Export"),
                                            NULL,
                                            KAboutData::License_GPL,
                                            I18N_NOOP("Kipi plugin to export to a web service"),
                                            "(c) 2007, Pontus Freyhult");

     about->addAuthor("Pontus Freyhult", I18N_NOOP("Author and maintainer"),
                      "pontus_wsexport@soua.net");

     helpButton = actionButton( Help );
     KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
     helpButton->setPopup( helpMenu->menu() );

     widget = new WSWidget( this );
     setMainWidget( widget );
     //     widget->setMinimumSize( widget->minimumSizeHint() );
     widget->setMinimumSize( 800,500);


     if( !interface->hasFeature(KIPI::ImagesHasComments) &&
	 !interface->hasFeature(KIPI::AlbumsHaveComments))
       {
	 widget->privateMode->setChecked(true);
	 widget->privateMode->setEnabled(false);
       }


     connect(  dynamic_cast< QObject* >( widget->serviceSelector),
	       SIGNAL( activated(int) ), 
	       SLOT( slotServiceChanged(int)));

     connect(  dynamic_cast< QObject* >( widget->uploadButton),
	       SIGNAL( clicked() ), 
	       SLOT( slotUploadButton()));



     connect(  dynamic_cast< QObject* >( widget->addToAlbum),
	       SIGNAL( clicked() ), 
	       SLOT( slotAddToAlbum()));


     connect(  dynamic_cast< QObject* >( widget->uploadAlbums),
	       SIGNAL( clicked() ), 
	       SLOT( slotUploadAlbums()));


     connect(  dynamic_cast< QObject* >( widget->uploadAllAlbums),
	       SIGNAL( clicked() ), 
	       SLOT( slotUploadAllAlbums()));


     connect(  dynamic_cast< QObject* >( widget->configuration),
	       SIGNAL( activated(int) ), 
	       SLOT( slotConfigurationChanged(int)));

     connect(  dynamic_cast< QObject* >( widget->category),
	       SIGNAL( activated(int) ), 
	       SLOT( slotCategoryChanged(int)));

     connect(  dynamic_cast< QObject* >( widget->subcategory),
	       SIGNAL( activated(int) ), 
	       SLOT( slotSubcategoryChanged(int)));






     connect(  interface,
	       SIGNAL( selectionChanged(bool) ), 
	       SLOT( slotRefreshUI()));




     widget->statusLed->setColor(yellow);

     KConfig config("kipirc");
     config.setGroup("WSExport Settings");


     currentService = -1;
     slotServiceChanged( config.readNumEntry("Service",0) );
     widget->serviceSelector->setCurrentItem(currentService);


  }


  WSWindow::~WSWindow()
  {

    // kdDebug(51000) << "Closing, setting service to "<<currentService <<"\n";
    // write config
    KConfig config("kipirc");
    config.setGroup("WSExport Settings");
    config.writeEntry("Service", currentService);

    delete widget;
    delete about;

    if (service)
      delete service;
}

void WSWindow::slotHelp()
{
    KApplication::kApplication()->invokeHelp("WSExport", "kipi-plugins");
}

void WSWindow::slotDoLogin()
{
}


void WSWindow::slotConfigurationChanged(int newConfig)
{
  if (service)
    service->changeConfig(newConfig);
}

void WSWindow::slotCategoryChanged(int newCategory)
{
  if (service)
    service->changeCategory(newCategory);
}

void WSWindow::slotSubcategoryChanged(int newSubcategory)
{
  if (service)
      service->changeSubcategory(newSubcategory);
}




void WSWindow::slotServiceChanged(int newService)
{

  if (currentService == newService)
    return;

  currentService = newService;
  
  //KConfig config("kipirc");
  //config.setGroup("WSExport Settings");
  //config.writeEntry("Service", currentService);

  if (service)
    {
      service->disconnect();
      delete service;
    }

  switch (newService)
    {
    case 0:
      service = new WSExport::WSSmugmug();
      break;

    default:
    case 1:
      service = new WSExport::WSPicasa();
      break;
    }


  service->setInterface(interface);

  // Replace account selector contents

  refreshAccounts();
  slotRefreshUI();
  
  connect(  dynamic_cast< QObject* >( widget->addAccount),
	    SIGNAL( clicked() ),
	    service,
	    SLOT( addAccount()));


  connect(  dynamic_cast< QObject* >( widget->accountSelector),
	    SIGNAL( activated(const QString& ) ), 
	    service,
	    SLOT( changeCurrentAccount(const QString&)));

  connect(  service,
	       SIGNAL( refreshUI() ), 
	       SLOT( slotRefreshUI()));

  connect(  service,
	       SIGNAL( status(const bool, const QString&) ), 
	       SLOT( slotStatus(const bool, const QString&)));


  service->doLogin();
}





void WSWindow::slotStatus(const bool busy, const QString& status)
{
  if (busy)
    {
      widget->statusLed->on();
      widget->uploadButton->setText(tr("Cancel"));
    }
  else
    {
      widget->statusLed->off();
      widget->uploadButton->setText(tr("Start Upload"));
    }
  widget->statusText->setText(status);

}


void WSWindow::slotAddToAlbum()
{
  widget->existingAlbum->setEnabled(true);
  widget->newAlbum->setEnabled(true);
  widget->album->setEnabled(true);
  widget->addPhotosToNewAlbumName->setEnabled(true);
  widget->matchCategories->setEnabled(false);
}

void WSWindow::slotUploadAlbums()
{
  widget->existingAlbum->setEnabled(false);
  widget->newAlbum->setEnabled(false);
  widget->album->setEnabled(false);
  widget->addPhotosToNewAlbumName->setEnabled(false);
  widget->matchCategories->setEnabled(false);
}

void WSWindow::slotUploadAllAlbums()
{
  widget->existingAlbum->setEnabled(false);
  widget->newAlbum->setEnabled(false);
  widget->album->setEnabled(false);
  widget->addPhotosToNewAlbumName->setEnabled(false);
  widget->matchCategories->setEnabled(false);
}


void WSWindow::slotUploadButton()
{

  KIPI::ImageCollection ic;

  if (widget->addToAlbum->isChecked())
    {
      ic = interface->currentSelection();
      service->startUpload(ic, widget->album->currentItem());
      }

    if (widget->uploadAlbums->isChecked())
      {
	ic = interface->currentAlbum();

	Q3ValueList<KIPI::ImageCollection> toUpload;
	toUpload.append(ic);
		
	service->startUpload(toUpload);
      }

    if (widget->uploadAllAlbums->isChecked())
      service->startUpload(interface->allAlbums());
      
}

void WSWindow::replaceContents(QComboBox* w, QStringList newContent)
{
  int count = w->count();
  for (int i=0; i<count; i++)
    w->removeItem(0);

  
  w->insertStringList(newContent);
}

void WSWindow::refreshAccounts()
{
  //  Get the new lists of account into the selector
  replaceContents(widget->accountSelector, service->listAccounts());
  service->changeCurrentAccount(service->listAccounts().first());
}


void WSWindow::slotRefreshUI()
{

  if (!service || !service->isLoggedIn())
    {
      widget->existingAlbum->setEnabled(false);
      widget->newAlbum->setEnabled(false);
      widget->addToAlbum->setEnabled(false);
      widget->uploadAlbums->setEnabled(false);
      widget->uploadAllAlbums->setEnabled(false);
      widget->album->setEnabled(false);
      widget->addPhotosToNewAlbumName->setEnabled(false);
      widget->configuration->setEnabled(false);
      widget->category->setEnabled(false);
      widget->subcategory->setEnabled(false);
      widget->matchCategories->setEnabled(false);
      widget->privateMode->setEnabled(false);

      return;
    }

  widget->existingAlbum->setEnabled(true);
  widget->newAlbum->setEnabled(true);




  widget->matchCategories->setEnabled(true);
  widget->privateMode->setEnabled(true);





  // Update lists of albums we can upload to
  replaceContents(widget->album,service->availableAlbums());



  // Fix count in upload all albums
  if(service->availableAlbums().size())
    {
      Q3ValueList<KIPI::ImageCollection> ic = interface->allAlbums();
      int newAlbums=0;

      if (service->availableAlbums().size() && ic.size())
	{	  
	  for (unsigned int i=0; i<ic.size(); i++)
	    if (service->name(ic[i]).length() &&
		!service->albumExists(service->name(ic[i])))
	      
	      newAlbums++;
	}

      if (newAlbums)
	{
	  widget->uploadAllAlbums->setEnabled(true);
	  widget->uploadAllAlbums->setText(tr(
					      "Upload all (%1) albums " 
					      "that do not exist already").
					   arg(newAlbums));
	}
      else
	{
	  widget->uploadAllAlbums->setEnabled(false);
	  widget->uploadAllAlbums->setText(tr("Upload all albums "
					      "that do not exist already"));
	}
    }  /* Albums available? */

    

  // Update list of possible configurations if applicable

  if(service->supportsConfigurations() && 
     service->availableConfigurations().size())
    {
    replaceContents(widget->configuration,
		    service->availableConfigurations());
    widget->configuration->setEnabled(true);
    }
  else
    widget->configuration->setEnabled(false);

  // Update list of possible categories  
  if(service->supportsCategories() && service->availableCategories().size())
    {
      replaceContents(widget->category,
		      service->availableCategories());
      widget->category->setEnabled(true);
    }
  else
    widget->category->setEnabled(false);
  
  if(service->supportsSubcategories() && 
     service->availableSubcategories().size())
    {
    replaceContents(widget->subcategory,
		    service->availableSubcategories());
    widget->subcategory->setEnabled(true);
    }
  else
    widget->subcategory->setEnabled(false);



  KIPI::ImageCollection ic = interface->currentSelection();

  if (!ic.isValid() || !ic.images().size() || 
      !service->availableAlbums().size() )
    // No selection, or at least not any images.
    {
    widget->addToAlbum->setEnabled(false);
    widget->album->setEnabled(false);
    widget->addPhotosToNewAlbumName->setEnabled(false);
    }
  else
    {
      widget->addToAlbum->setEnabled(true);
      widget->addToAlbum->setText(tr("Add %1 selected images to")
				  .arg(ic.images().size()));
      widget->album->setEnabled(true);
      widget->addPhotosToNewAlbumName->setEnabled(true);
    }


  ic = interface->currentAlbum();

  if (!ic.isValid() || !ic.images().size())
    // No selection, or at least not any images.
    widget->uploadAlbums->setEnabled(false);
  else
    {
      widget->uploadAlbums->setEnabled(true);
      widget->uploadAlbums->setText(tr(
				       "Upload selected album \"%1\"")
				    .arg(service->name(ic).utf8()));
    }



}



}

#include "wswindow.moc"
