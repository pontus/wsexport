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
#include <qdom.h>
#include <qfile.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3ValueList>

// KDE
#include <kio/job.h>
#include <kio/authinfo.h>
#include <kio/passdlg.h>
#include <kio/slavebase.h>
#include <kwallet.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kurl.h>

#include <libkipi/imagecollection.h>

#include "wsbase.h"

namespace WSExport
{

  
  WSBase::WSBase()
  {
    username = QString();
    password = QString();
    privateMode = false;
    accounts = 0;
    wallet = 0;
    job = 0;
    serviceName = QString("None, you shouldn't see this");
    interface = 0;
    version = "pre 0.1";
    loggedIn = false;
  }
  

  WSBase::~WSBase()
  {
    writeAccounts();
    if (wallet)
      delete wallet;
  }


  void WSBase::setInterface(KIPI::Interface* newInterface)
  {
    interface = newInterface;

  }

  bool WSBase::writeAccounts()
  {
    KConfig config("kipirc");
    config.setGroup("WSExport Settings "+serviceName);
    config.writeEntry("Accounts", accounts);
    return true;
  }

  bool WSBase::readAccounts()
  {
    KConfig config("kipirc");

    config.setGroup("WSExport Settings "+serviceName);
    accounts = config.readListEntry("Accounts");


    while (accounts.isEmpty())
      addAccount();
    username = accounts.first();
    return true;
  }


  QStringList WSBase::listAccounts()
  {
    return accounts;
  }
      
  QString WSBase::urlEncode(QStringList toEncode)
  {
    QString encoded="";

    if (toEncode.size())
      for (unsigned int i=0; i<toEncode.size(); i++)
	{
	  QString par = toEncode[i]; 
	  
	  if(i)
	    encoded = encoded + "&";

	  encoded = encoded +
	      par.section('=',0,0) + "=" +
	      KURL::encode_string(par.section('=',1)); 
  	  
	}

    return encoded;
  }

  void WSBase::doRequest(QStringList urlParams,
			 QStringList headerParams, 
			 QString base,
			 QByteArray data,
			 QString contentType,
			 bool getNotPost)
  {
    if (job)
      {
	job->kill();
	job = 0;
      }

    lastUrlParams = urlParams;
    lastHeaderParams = headerParams;
    lastBase = base;
    lastData = data;
    lastContentType = contentType;
    lastGetNotPost = getNotPost;

    rcvdData = QString();
    currentSpeed = 0;

    QString postURL = baseURL;

    if (base.length())
      postURL = base;

    if (urlParams.size())
      postURL = postURL + "?" +urlEncode(urlParams);
  

    if (getNotPost)
      job = KIO::get(postURL,true,false);
    else
      {
	job = KIO::http_post(postURL,data,false);
	if (!contentType.length())
	  job->addMetaData("content-type", 
			   "Content-Type: application/x-www-form-urlencoded"
			     );
	else
	  job->addMetaData("content-type", contentType);	
      }

    job->addMetaData("UserAgent", 
		     "KIPIPlugin WSExport "+version+" "+serviceName);

    
    job->addMetaData("customHTTPHeader", headerParams.join("\r\n") );	
    
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
	    SLOT(slotData(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
	    SLOT(slotResult(KIO::Job *)));
    connect(job, SIGNAL(speed(KIO::Job*, unsigned long)),
	    SLOT(slotSpeed(KIO::Job*, unsigned long)));

	
    emit signalBusy( true );
  }





  bool WSBase::addAccount()
  {
    QString pwUsername;
    QString pwPassword;
    bool remember = true;

    KIO::PasswordDialog pw(tr("Edit account details for %1")
			   .arg(serviceName), 
			   0,
			   true);

    while (!pwUsername.length())
      {
	int ret = pw.getNameAndPassword(pwUsername,
					pwPassword,
					&remember,
					tr("Please enter account details"),
					false,
					tr("Edit account details for %1")
					.arg(serviceName));
	
	if(!ret)
	  return false;
	
      }
    
    username=pwUsername;
    password=pwPassword;

    accounts.append(pwUsername);

    if (remember)
      {
	if (setupWallet())
	  wallet->writePassword("WSExport"+serviceName+
				"account"+username,password);
      }
      
    return true;
  }

  bool WSBase::authenticate()
  {
    return getAuthenticationInformation();
  }
  
  bool WSBase::getAuthenticationInformation()
  {

    // Authenticated already?
    if (password.length() && username.length())
      return true;

    // Assume for now someone else selects an active account for us.
    QString pwPassword;

    if (setupWallet())
      if(!wallet->readPassword(
			       "WSExport"+serviceName+
			       "account"+username,
			       pwPassword)
	 && pwPassword.length())
	{
	  password=pwPassword;
	  return true;
	}


    KIO::PasswordDialog pw(tr("Give account details for %1").arg(serviceName),
			   0,
			   true);
    bool remember = true;
    QString pwUsername = username;

    while (!pwUsername.length() || !pwPassword.length())
      {
	int ret = pw.getNameAndPassword(pwUsername,
					pwPassword,
					&remember,
					tr("Please enter account details"),
					false,
					tr("Authenticate for %1")
					.arg(serviceName));
	

	if(!ret)
	  return false;
	
      }


    if (remember)
      {
	if (setupWallet())
	  wallet->writePassword("WSExport"+serviceName+
				"account"+pwUsername,
				pwPassword);
      }

	
    username=pwUsername;
    password=pwPassword;
    
    return true;
  }


  bool WSBase::setupWallet()
  {
    if (!wallet)     
      wallet = KWallet::Wallet::openWallet(
					   KWallet::Wallet::NetworkWallet());
	
    if (wallet && !wallet->hasFolder(KWallet::Wallet::PasswordFolder()))
      if (!wallet->createFolder(KWallet::Wallet::PasswordFolder()))
	{
	  //kdDebug(51000) << "failed creating password folder!\n";
	  delete wallet;
	  wallet = 0;
	};
  
    if (wallet && wallet->setFolder(KWallet::Wallet::PasswordFolder()))
      {
	return true;
      }	      
  
    return false;
  }

  
  bool WSBase::albumExists(QString name)
  {
    return name.length();
  }
  


  void WSBase::stateHandler()
  {
  }

  void WSBase::slotResult(KIO::Job *j)
  {
    job = 0;
    emit signalBusy( false );
    
    if ( j->error() )
      {
	QMessageBox m(tr("WSExport error in request"),
		      tr("There was an error while connecting, "
			 "  the error was:\n %1\n\n."
			 "You can abort or retry the request.")
		      .arg(j->errorString()),
		      QMessageBox::Warning,
		      QMessageBox::Abort | QMessageBox::Escape,
		      QMessageBox::Retry | QMessageBox::Default,
		      Qt::NoButton);
	
	if (m.exec() == QMessageBox::Retry)
	  doRequest(lastUrlParams,
		    lastHeaderParams,
		    lastBase,
		    lastData,
		    lastContentType,
		    lastGetNotPost);

	return;
      }

    // Convert here for now.

    QFile f("/tmp/wsexp"+QString::number((long) j));
    f.open(QIODevice::WriteOnly);

    f.writeBlock(rcvdData.latin1(),rcvdData.length());
    f.close();

    rcvdData = QString::fromUtf8(rcvdData.latin1());
    stateHandler();
  }


  void WSBase::slotData(KIO::Job *j, const QByteArray& data)
  {
    
    rcvdData = rcvdData + QString(data);

    if ( j->error() )
      {
	kdDebug(51000) << "Joberror in slotresult: " << j->errorString() << "\n";

      }
  }


  bool WSBase::supportsThemes()
  {
    return false;
  }

  bool WSBase::supportsConfigurations()
  {
    return false;
  }

  bool WSBase::supportsImageComments()
  {
    return false;
  }

  bool WSBase::supportsAlbumComments()
  {
    return false;
  }

  bool WSBase::supportsCategories()
  {
    return false;
  }

  bool WSBase::supportsSubcategories()
  {
    return false;
  }



  void WSBase::changeCurrentAccount(const QString& newAccount)
  {

    // kdDebug(51000) << "changeAccount " << newAccount << "\n";
    username = newAccount;
  }

  void WSBase::doLogin()
  {

    kdDebug(51000) << "doLogin \n";
  }


  QDomNode WSBase::traverse(QString data, QStringList path)
  {
    // Traverse according to path, return the last node in path.

    QStringList::iterator p = path.begin();

    QDomDocument doc;
    QDomNode node;
    doc.setContent(data);
    
    node = doc.documentElement();

    while (p != path.end()  && !node.isNull() && node.nodeName() == (*p))
      {

	// Just rewritten to use node.namedItem, can possibly be shortened.
	
	p++;
	
	if (p==path.end())
	  return node;
	
	
	node = node.namedItem(*p);
      }

    return node;
  }





QString WSBase::comment(const KIPI::ImageCollection& ic)
{
  if (ic.isValid() &&
      interface &&
      interface->hasFeature(KIPI::AlbumsHaveComments) &&
      !privateMode &&
      ic.comment().length())
    return QString::fromUtf8(ic.comment().latin1());

  return 0;
}

QString WSBase::comment(const KURL& img)
{
  if (img.isValid() &&
      interface &&
      interface->hasFeature(KIPI::ImagesHasComments) &&
      !privateMode &&
      interface->info(img).description().length())
    return QString::fromUtf8(interface->info(img).description().latin1());

  return 0;
}


QStringList WSBase::tags(const KURL& img)
{
  if (img.isValid() &&
      interface &&
      !privateMode &&
      interface->info(img).attributes().find("tags") !=  
      interface->info(img).attributes().end())
    {
      QStringList tags;
      QStringList origTags((*interface->info(img).attributes().find("tags")).toStringList());

      for (unsigned int i=0; i<origTags.size(); i++)
	tags.append(QString::fromUtf8(origTags[i].latin1())); 

      return tags;
    }

  return 0;
}

int WSBase::rating(const KURL& img)
{
  if (img.isValid() &&
      interface &&
      !privateMode &&
      interface->info(img).attributes().find("rating") !=  
      interface->info(img).attributes().end())
    {

      bool ok=false;
      int rating = (*interface->info(img).attributes().find("rating")).toInt(&ok);

      if (ok)
	return rating;
    }

  return 0;
}

QString WSBase::name(const KIPI::ImageCollection& ic)
{
  if (ic.isValid() &&
      ic.name().length())
    return QString::fromUtf8(ic.name().latin1());

  return 0;
}

QString WSBase::name(const KURL& img)
{
  if (img.isValid() &&
      interface &&
      interface->info(img).title().length())
    return interface->info(img).title();

  return 0;
}


void WSBase::slotSpeed(KIO::Job* j, unsigned long speed)
{
  if (j != job)
    return;

  currentSpeed = speed;
  statusUpdate();
}

void WSBase::statusUpdate()
{
  QString newStatus = "";
  
  

  QPair<QString,KURL::List>* currentImage = imageQueue.head();
  
  QString fileName;
  QString albumName;
  unsigned int leftInAlbum = 0;

  if (currentImage)    
    {
      fileName = name(currentImage->second.first());
      leftInAlbum = currentImage->second.count();
    }

  if (fileName.length())
    newStatus = tr("Uploading %1 (%2 images left in album)").arg(fileName).arg(leftInAlbum);
  else 
    newStatus = tr("Uploading");



  
  if (currentSpeed)
    newStatus = newStatus + tr(" current speed %1").arg(QString::number(currentSpeed));
  else
    newStatus = newStatus + tr(" (upload speed not avaialable)");

  emit status(true, newStatus);
}



void WSBase::doNextUpload()
  {
    QPair<QString, KURL::List>* nextPost = imageQueue.head();
    
    if (nextPost)
      // More images in current album.
      uploadImage();
    else
      {
      // Next album
	KIPI::ImageCollection* nextAlbum = albumQueue.head();
	
	if (!nextAlbum)
	  // All done.
	  return;

	QString albumName = name(*nextAlbum);
	QString albumComment = comment(*nextAlbum);
	
	doCreateGallery(albumName, albumComment);
	
      }
  }


void WSBase::uploadImage()
{
}

void WSBase::doCreateGallery(QString name, QString description)
{
  QString notUsed = name+description;
}




void WSBase::startUpload(KIPI::ImageCollection i)
{
  
  
  if (job)
    return;
  
  KIPI::ImageCollection* album = new KIPI::ImageCollection(i);
  albumQueue.enqueue(album);
  
    doCreateGallery(name(*album), 
		    comment(*album));
    
}


void WSBase::startUpload(KIPI::ImageCollection i, 
			 int albumIndex)
{
    // Pick the id for album we want to upload to.
  QString destAlbum =  (*albums.keys().at(albumIndex));
  
  QPair<QString, KURL::List>* toQueue = new 
    QPair<QString, KURL::List>(destAlbum,KURL::List(i.images()));
  imageQueue.enqueue(toQueue);
  
  doNextUpload();
}

void WSBase::startUpload(Q3ValueList<KIPI::ImageCollection> albums)
{
  // Pick the id for album we want to upload to.
  
    if (albums.size()) 
      for (unsigned int i=0; i<albums.size(); i++)
	if (albums[i].isValid())
	  {
	    KIPI::ImageCollection* currentAlbum = new KIPI::ImageCollection(albums[i]);
	    albumQueue.enqueue(currentAlbum);
	    
	  }
    doNextUpload();
}


void WSBase::enqueueImagesForNewAlbum(QString albumId)
{
  
  KIPI::ImageCollection* ic = albumQueue.dequeue();
  
  if (ic)
    {
      QPair<QString, KURL::List>* toQueue = new 
	QPair<QString, KURL::List>(albumId,KURL::List(ic->images()));
      imageQueue.enqueue(toQueue);

      delete ic;
    }
  
  doNextUpload();
}


  QStringList WSBase::availableThemes()
  {
    return 0;
  }

  QStringList WSBase::availableAlbums()
  {
    return albums.values();
  }

  QStringList WSBase::availableConfigurations()
  {
    return templates.values();
  }

  QStringList WSBase::availableCategories()
  {
    return categories.values();
  }

QStringList WSBase::availableSubcategories()
  {
    return subcategories.values();
  }


bool WSBase::isLoggedIn()
{
  return loggedIn;
}

void WSBase::changeConfig(unsigned int newValue)
{
  configId = "";

  if (newValue < templates.keys().size())
    configId = templates.keys()[newValue];

}

void WSBase::changeCategory(unsigned int newValue)
{
  categoryId = "";

  if (newValue < categories.keys().size())
    categoryId = categories.keys()[newValue];
}

void WSBase::changeSubcategory(unsigned int newValue)
{
  subcategoryId = "";

  if (newValue < subcategories.keys().size())
    subcategoryId = subcategories.keys()[newValue];
}

}

#include "wsbase.moc"
