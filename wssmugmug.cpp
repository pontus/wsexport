/* ============================================================
 * File  : wssmugmug.cpp
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

// KDE
#include <kio/job.h>
#include <kdebug.h>
#include <kmdcodec.h>

#include "wsbase.h"
#include "wssmugmug.h"


namespace WSExport
{

  enum states { ST_LOGIN,
		ST_GETALBUMS, 
		ST_GETCATEGORIES, 
		ST_GETTEMPLATES,
		ST_GETSUBCATEGORIES,
		ST_CREATEGALLERY,
		ST_LOGOUT,
		ST_UPLOADIMAGE,
		ST_UPLOADIMAGECHANGEKEYWORDS
  };
  
  WSSmugmug::WSSmugmug() 
  {
    baseURL = "https://api.smugmug.com/hack/rest/1.1.1/";
    APIkey = "mM2cGVYczseyw4rjhNv0eWZ9Jw2AXrRb";
    serviceName = "Smugmug";
    sessionId = "";
    readAccounts();
  }
  


  WSSmugmug::~WSSmugmug()
  {

    simpleMethodCall("smugmug.logout", ST_LOGOUT);
    disconnect(this);
    job=0;
    
  }




      


  
  
  bool WSSmugmug::albumExists(QString name)
  {
    return albums.values().contains(name);

  }
  


  bool WSSmugmug::supportsImageComments()
  {
    return true;
  }

  bool WSSmugmug::supportsAlbumComments()
  {
    return true;
  }


  bool WSSmugmug::supportsThemes()
  {
    return true;
  }


  bool WSSmugmug::supportsConfigurations()
  {
    return true;
  }

  bool WSSmugmug::supportsCategories()
  {
    return true;
  }

  bool WSSmugmug::supportsSubCategories()
  {
    return true;
  }


  bool WSSmugmug::responseOk(QString data)
  {
    QDomNode n = traverse(data,"rsp");

    if (n.nodeName() == "rsp")
      {
	QDomNamedNodeMap attr = n.attributes();
	QDomNode statNode = attr.namedItem("stat");
	
	if (statNode.nodeValue().upper() == "OK")
	  return true;
      }

    return false;
  }


  void WSSmugmug::doLogin()
  {
    if (!authenticate())
      return;
      
    emit status(true, tr("Logging in to account"));

    QStringList url;
    url.append("method=smugmug.login.withPassword");
    url.append("EmailAddress="+username);
    url.append("Password="+password);
    url.append("APIKey="+APIkey);

    state = ST_LOGIN;
    doRequest(url);
  }


  void WSSmugmug::handleLoginResponse()
  {
    QStringList p;
    p.append("rsp");
    p.append("Login");
    p.append("SessionID");


    if (responseOk(rcvdData))
    {
      QDomNode n = traverse(rcvdData,p);
      
      if (n.nodeName() == "SessionID")
	{
	  loggedIn = true;
	  sessionId = n.firstChild().nodeValue();
	  doGetAlbums();
	 
	}
      
      return;
      
    }

    // Response not OK.
    
    kdDebug(51000) << "Authentication failed!\n";
	
  }


  void WSSmugmug::simpleMethodCall(QString method, int newState)
  {
    rcvdData = "";

    if (!authenticate())
      return;
      
    QStringList url;
    url.append("method=" + method);
    url.append("SessionID="+sessionId);

    state = newState;
    doRequest(url);

  }


  void WSSmugmug::doGetAlbums()
  {
    emit status(true, tr("Fetching album list"));
    simpleMethodCall("smugmug.albums.get", ST_GETALBUMS);
  }


  QMap<QString,QString> WSSmugmug::getIdAndTitle(QDomNode n,
					      QString nodeName, 
					      QString titleName="Title")
  {

    QMap<QString,QString> result;
    QDomNamedNodeMap attr;
    QDomNodeList nList;

    QString id;
    QString title;
	
    nList = n.childNodes();
    for (uint i=0; i<nList.length(); i++)
	  {
	    n = nList.item(i);
	    	    
	    if (n.nodeName() == nodeName)
	      {
		attr = n.attributes();
		QDomNode idNode = attr.namedItem("id");
		QDomNode tiNode = n.namedItem(titleName); 

		if (!idNode.isNull() && !tiNode.isNull())
		  {
		    id = idNode.nodeValue();
		    title = tiNode.firstChild().nodeValue();
		    result.insert(id, title);
		  }
	      }
	  }
  
    return result;
  }



  void WSSmugmug::handleGetAlbumsResponse()
  {
    QStringList p;
    p.append("rsp");
    p.append("Albums");


    if (responseOk(rcvdData))
      {
	QDomNode n = traverse(rcvdData,p);
	
	if (n.nodeName() == "Albums")
	  albums = getIdAndTitle(n,"Album");
      }
  
    doGetTemplates();
    return;
  }



  void WSSmugmug::doGetTemplates()
  {
    emit status(true, tr("Fetching album configuration list"));
    simpleMethodCall("smugmug.albumtemplates.get", ST_GETTEMPLATES);
  }

  void WSSmugmug::handleGetTemplatesResponse()
  {
    QStringList p;
    p.append("rsp");
    p.append("AlbumTemplates");

    if (responseOk(rcvdData))
      {
	QDomNode n = traverse(rcvdData,p);
	
	if (n.nodeName() == "AlbumTemplates")
	  templates = getIdAndTitle(n, "AlbumTemplate",
				    "AlbumTemplateName");
	
	if (templates.keys().size())
	  configId = templates.keys()[0];

      }

    doGetCategories();
    return;

  }



  void WSSmugmug::doGetCategories()
  {
    emit status(true, tr("Fetching category list"));
    simpleMethodCall("smugmug.categories.get", ST_GETCATEGORIES);
  }

  void WSSmugmug::handleGetCategoriesResponse()
  {

    QStringList p;
    p.append("rsp");
    p.append("Categories");

    QDomNode n = traverse(rcvdData,p);

    if (n.nodeName() == "Categories")
      categories = getIdAndTitle(n, "Category");

    if (categories.keys().size())
      categoryId = categories.keys()[0];

    doGetSubCategories();
    return;
  }


  void WSSmugmug::doGetSubCategories()
  {
    simpleMethodCall("smugmug.subcategories.get", ST_GETSUBCATEGORIES);
  }

  void WSSmugmug::handleGetSubCategoriesResponse()
  {
    QStringList p;
    p.append("rsp");
    p.append("Subcategories");

    QDomNode n = traverse(rcvdData,p);

    if (n.nodeName() == "Subcategories")
      subcategories = getIdAndTitle(n, "SubCategory");


    if (subcategories.keys().size())
      subcategoryId = subcategories.keys()[0];

    emit refreshUI();
    emit status(false,"");

    return;
  }





  void WSSmugmug::doCreateGallery(QString title, 
				  QString description)
				
  {
    if (!authenticate())
      return;
      
    emit status(true, tr("Creating gallery %1").arg(title));

    QStringList url;
    url.append("method=smugmug.albums.create");
    url.append("SessionId="+sessionId);

    if (categoryId.length())
      url.append("CategoryID="+categoryId);

    if (subcategoryId.length())
      url.append("SubCategoryID="+subcategoryId);

    if (configId.length())
      url.append("AlbumTemplateId="+configId);
 
    url.append("Title="+title);
    url.append("Description="+description);

    state = ST_CREATEGALLERY;
    doRequest(url);
  }

  void WSSmugmug::handleCreateGalleryResponse()
  {
    QStringList p;
    p.append("rsp");
    p.append("Create");
    p.append("Album");
    QString albumId;

    emit status(false,"");

    QDomNode n = traverse(rcvdData,p);

    if (n.nodeName() == "Album")
      {
	QDomNamedNodeMap attr = n.attributes();
	QDomNode idNode = attr.namedItem("id");
		
	if (!idNode.isNull())
	  {
	    albumId = idNode.nodeValue();
	    enqueueImagesForNewAlbum(albumId);
	  }
      }
  }





  void WSSmugmug::uploadImage()
  {
    if (!authenticate())
      return;
    
    QPair<QString, KURL::List>* curPost = imageQueue.head();
    
    if(!curPost)
      return;

    if (curPost->second.isEmpty())
      {
	delete curPost;
	imageQueue.dequeue();
	return;
      }

    QString albumId = curPost->first;
    KURL currentImage = curPost->second.first();



    KMimeType::Ptr type = KMimeType::findByURL(currentImage);

    if (type->name() == KMimeType::defaultMimeType() || 
	!type->name().startsWith("image/",false))
      {
	// FIXME: For now, just ignore images.

	curPost->second.pop_front();
	doNextUpload();
	return;
      }
      

    KIPI::ImageInfo info = interface->info(currentImage);

    QFile image(currentImage.path());
    image.open(QIODevice::ReadOnly);
    QByteArray data = image.readAll();
    KMD5 checksum(data);
    

    QStringList header;

    header.append("X-Smug-SessionID: "+sessionId);
    header.append("Content-MD5: "+checksum.hexDigest());

    if( name(currentImage).length() )
      header.append("X-Smug-FileName: "+name(currentImage).utf8());

    if( comment(currentImage).length() )
      header.append("X-Smug-Caption: "+comment(currentImage).utf8());

    if ( tags(currentImage).size() )
      header.append("X-Smug-Keywords: "+
		       tags(currentImage).join(" ").utf8());

    header.append("X-Smug-AlbumID: "+albumId);
    header.append("X-Smug-Version: 1.1.1");
    header.append("X-Smug-ResponseType: REST");


    doRequest(0, 
	      header, 
	      "http://upload.smugmug.com/photos/xmlrawadd.mg",
	      data,
	      "Content-Type: application/octet-stream");

    statusUpdate();

    state = ST_UPLOADIMAGE;
  }
    



  




  void WSSmugmug::handleUploadImageResponse()
  {
    QStringList p;
    p.append("rsp");
    p.append("ImageID");

    QDomNode n = traverse(rcvdData,p);

    emit status(false,"");
    if (n.nodeName() == "ImageID")
      {

	QString imageId=n.firstChild().nodeValue();

	QPair<QString, KURL::List>* curPost = imageQueue.head();
    
	if(!curPost)
	  // Something's fishy here, we don't dequeue until after 
	  return;

	if (curPost->second.isEmpty())
	  {
	    delete curPost;
	    imageQueue.dequeue();
	    return;	    
	  }



	KURL currentImage = curPost->second.first();
	KIPI::ImageInfo info = interface->info(currentImage);

	curPost->second.pop_front();
 

	if (tags(currentImage).size()) // Any use in setting keywords?
	  {

	    QStringList url;
	    url.append("method=smugmug.images.changeSettings");
	    url.append("SessionID="+sessionId);
	    url.append("ImageID="+imageId);
	    
	    url.append("Keywords="+tags(currentImage).join(" ").utf8());
	    
	
	    state = ST_UPLOADIMAGECHANGEKEYWORDS;
	    doRequest(url);

	    emit status(true,tr("Setting keywords for %1").arg(name(currentImage)));
	  }
	else
	  doNextUpload();

	
      }




    return;
  }



  void WSSmugmug::handleUploadImageChangeKeywordsResponse()
  {
    QStringList p;
    p.append("rsp");
    p.append("Create");
    p.append("Album");
   

    emit status(false,"");
    QDomNode n = traverse(rcvdData,p);

    doNextUpload();
  }



  void WSSmugmug::stateHandler()
  {
    switch(state)
      {
      case  ST_LOGIN:
	handleLoginResponse();
	break;
	  
      case  ST_GETALBUMS:
	handleGetAlbumsResponse();
	break;
	  

      case  ST_GETTEMPLATES:
	handleGetTemplatesResponse();
	break;

      case  ST_GETCATEGORIES:
	handleGetCategoriesResponse();
	break;
	  

      case  ST_GETSUBCATEGORIES:
	handleGetSubCategoriesResponse();
	break;
	  

      case  ST_CREATEGALLERY:
	handleCreateGalleryResponse();
	break;

      case  ST_UPLOADIMAGE:
	handleUploadImageResponse();
	break;
	  

      case  ST_UPLOADIMAGECHANGEKEYWORDS:
	handleUploadImageChangeKeywordsResponse();
	break;
	  
	  
      }

  }
}

#include "wssmugmug.moc"
