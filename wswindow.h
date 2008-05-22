/* ============================================================
 * File  : wswindow.h
 * Author: Pontus Freyhult <pontus_smugmugexport@soua.net>
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

#ifndef WSWINDOW_H
#define WSWINDOW_H

#include <kdialogbase.h>
#include <q3valuelist.h>
#include <qpair.h>
#include <q3intdict.h>

#include <libkipi/interface.h>
#include <libkipi/imagedialog.h>

// Local includes

#include "kpaboutdata.h"
#include "wswidget.h"
#include "wsbase.h"

class Q3ListView;
class QPushButton;
class QSpinBox;
class QCheckBox;
class Q3ProgressDialog;
class KHTMLPart;
class KURL;
class QLineEdit;

namespace KIPI
{
class Interface;
}

namespace KWallet
{
class Wallet;
}

namespace KIPIWSExport
{

class WSWindow : public KDialogBase
{
    Q_OBJECT

public:

    WSWindow(KIPI::Interface *interface,QWidget *parent);
    ~WSWindow();

    public slots:
    void slotServiceChanged(int newService);

    void slotConfigurationChanged(int newConfig);
    void slotCategoryChanged(int newCategory);
    void slotSubcategoryChanged(int newSubcategory);

private:

    void refreshAccounts();

     QPushButton              *helpButton;   

     WSWidget *widget;
     KIPI::Interface          *interface;
                                                                              
     KIPIPlugins::KPAboutData    *about; 
     WSExport::WSBase* service;
     int currentService;

     void replaceContents(QComboBox* w, QStringList newContent);



private slots:
  //    void slotTokenObtained(const QString& token);
    void slotDoLogin();
    //void slotLoginFailed( const QString& msg );
  //  void slotBusy( bool val );
  //  void slotError( const QString& msg );
  //  void slotAlbums( const QValueList<GAlbum>& albumList );
  //  void slotPhotos( const QValueList<GPhoto>& photoList );
  //  void slotTagSelected();
    //void slotOpenPhoto( const KURL& url );
  //  void slotNewPhotoSet();
  //  void slotUserChangeRequest();
    //    void slotListPhotoSetsResponse(const QValueList <FPhotoSet>& photoSetList);
  //  void slotAddPhotos();
  //	void slotUploadImages();
  //  void slotAddPhotoNext();
  //  void slotAddPhotoSucceeded();
  //  void slotAddPhotoFailed( const QString& msg );
  //  void slotAddPhotoCancel();
  //  void slotAuthCancel();
    void slotHelp();

    void slotRefreshUI();
    void slotStatus(const bool busy, const QString& status);
    void slotUploadButton();

    void slotAddToAlbum();
    void slotUploadAlbums();
    void slotUploadAllAlbums();
};

}

#endif /* WSWINDOW_H */
