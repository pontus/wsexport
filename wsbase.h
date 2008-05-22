/* ============================================================
 * File  : wsbase.h
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

#ifndef WSBASE_H
#define WSBASE_H

#include <qstring.h>
#include <qdom.h>
#include <qstringlist.h>
#include <q3ptrqueue.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <kwallet.h>

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

namespace KIO
{
    class Job;
}

namespace KWallet
{
    class Wallet;
}

class KURL;

namespace WSExport
{
  class WSBase : public QObject
    {
      Q_OBJECT

	public:
      WSBase();

      ~WSBase();


      virtual bool isLoggedIn();
      virtual bool albumExists(QString name);

      virtual bool supportsThemes();
      virtual bool supportsConfigurations();
      virtual bool supportsCategories();
      virtual bool supportsSubcategories();

      virtual bool supportsAlbumComments();
      virtual bool supportsImageComments();

      virtual QStringList availableThemes();    
      virtual QStringList availableAlbums();    
      virtual QStringList availableConfigurations();
      virtual QStringList availableCategories();
      virtual QStringList availableSubcategories();

      virtual QStringList listAccounts();

      virtual void setInterface(KIPI::Interface* newInterface);

      virtual QString comment(const KIPI::ImageCollection& ic);
      virtual QString comment(const KURL& img);
      
      virtual QString name(const KIPI::ImageCollection& ic);
      virtual QString name(const KURL& img);

      virtual QStringList tags(const KURL& img);
      virtual int rating(const KURL& img);


      virtual void uploadImage();
      virtual void doCreateGallery(QString name, QString description);

      virtual void enqueueImagesForNewAlbum(QString albumId);

      virtual void startUpload(KIPI::ImageCollection i);



      virtual void startUpload(KIPI::ImageCollection i, 
			       int albumIndex);

      virtual void startUpload(Q3ValueList <KIPI::ImageCollection> albums);


      public slots:

	virtual void changeCurrentAccount(const QString& newAccount);
	virtual void changeConfig(unsigned int newValue);
	virtual void changeCategory(unsigned int newValue);
	virtual void changeSubcategory(unsigned int newValue);



      virtual bool getAuthenticationInformation();
      virtual bool authenticate();

      virtual bool addAccount();



      // For testing
    virtual void doLogin();
    protected:
      virtual void doRequest(QStringList urlParams, 
			     QStringList headerParam=0, 
			     QString base="", 
			     QByteArray data=0,
			     QString contentType=0,
			     bool getNotPost=false);

      virtual QString urlEncode(QStringList toEncode);

      virtual void doNextUpload();

      virtual bool readAccounts();
      virtual bool writeAccounts();
      virtual bool setupWallet();

      virtual void stateHandler();

      virtual void statusUpdate();

      virtual QDomNode traverse(QString data, QStringList path);

      bool loggedIn;

      Q3PtrQueue<KIPI::ImageCollection> albumQueue;
      Q3PtrQueue<QPair<QString,KURL::List> > imageQueue;

      QString version;

    protected slots:
    
      virtual void slotResult (KIO::Job *j);
    virtual void slotData(KIO::Job *j, const QByteArray& data);
    virtual void slotSpeed(KIO::Job* j, unsigned long speed);



    signals:
    void signalBusy( bool value);
    void refreshUI();
    void status(const bool busy, const QString& message);
    
    protected:
    KWallet::Wallet* wallet;
      KIO::Job* job;
      QString username;
      QString password;
      QString baseURL;
      QString serviceName;
      QStringList accounts;
      QString rcvdData; // Approximate 

      KIPI::Interface* interface;
      bool privateMode;
      int state;
      unsigned long currentSpeed;


      QString configId;
      QString categoryId;
      QString subcategoryId;

      
      QMap<QString,QString> albums;
      QMap<QString,QString> categories;
      QMap<QString,QString> subcategories;
      QMap<QString,QString> templates;



      QStringList lastUrlParams;
      QStringList lastHeaderParams;
      QString lastBase;
      QByteArray lastData;
      QString lastContentType;
      bool lastGetNotPost;
    };
};

#endif /* WSBASE_H */
