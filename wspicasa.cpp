/* ============================================================
 * File  : wspicasa.cpp
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
#include "wspicasa.h"


namespace WSExport
{

  enum states { ST_LOGIN,
		ST_GETALBUMS, 
		ST_CREATEGALLERY,
		ST_LOGOUT,
		ST_UPLOADIMAGE,
		ST_UPLOADIMAGECHANGEKEYWORDS
  };
  
  WSPicasa::WSPicasa() 
  {
    baseURL = "https://api.picasa.com/hack/rest/1.1.1/";
    APIkey = "mM2cGVYczseyw4rjhNv0eWZ9Jw2AXrRb";
    serviceName = "Picasa";
    sessionId = "";
    readAccounts();
  }
  


  WSPicasa::~WSPicasa()
  {

    disconnect(this);
    job=0;
    
  }




      


  
  
  bool WSPicasa::albumExists(QString name)
  {
    return albums.values().contains(name);

  }
  


  bool WSPicasa::supportsImageComments()
  {
    return true;
  }

  bool WSPicasa::supportsAlbumComments()
  {
    return true;
  }


  bool WSPicasa::supportsThemes()
  {
    return false;
  }


  bool WSPicasa::supportsConfigurations()
  {
    return false;
  }

  bool WSPicasa::supportsCategories()
  {
    return false;
  }

  bool WSPicasa::supportsSubCategories()
  {
    return true;
  }


  bool WSPicasa::responseOk(QString data)
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


  void WSPicasa::doLogin()
  {
    if (!authenticate())
      return;
      
    emit status(true, tr("Logging in to account"));

    QStringList params;
    params.append("accountType=HOSTED_OR_GOOGLE");
    params.append("Email="+username);
    params.append("Passwd="+password);
    params.append("service=lh2");
    params.append("source=KIPI WSExport plugin");


    state = ST_LOGIN;
    doRequest("", // Two empty stringlists for url and header params
	      "",
	      "https://www.google.com/accounts/ClientLogin",
	      urlEncode(params).local8Bit() );
  }


  void WSPicasa::handleLoginResponse()
  {
    QStringList p = QStringList::split("\n",rcvdData);

    for (unsigned int i=0; i<p.size(); i++)
      if (p[i].left(5).upper() == "AUTH=")
	{
	  loggedIn = true;
	  authHeader = "Authorization: GoogleLogin "+p[i];

	  QString picasaUser = username.section("@",0,0);
	  baseURL = "http://picasaweb.google.com/data/feed/api/user/"+
	    picasaUser;
	  
	  doGetAlbums();
	  return;
	}
   

    // Response not OK.

    kdDebug(51000) << "Authentication failed!\n";

  }


  void WSPicasa::simpleMethodCall(QString method, int newState)
  {
    rcvdData = "";

    if (!authenticate())
      return;
      
    QStringList url;
    url.append("kind=" + method);

    state = newState;
    doRequest(url,authHeader,"",0,"",true);
  }


  void WSPicasa::doGetAlbums()
  {
    emit status(true, tr("Fetching album list"));
    simpleMethodCall("album", ST_GETALBUMS);
  }


  QMap<QString,QString> WSPicasa::getIdAndTitle(QDomNode n)
  {

    QMap<QString,QString> result;
    QDomNodeList nList;

    QString id;
    QString title;
	
    nList = n.childNodes();
    for (uint i=0; i<nList.length(); i++)
	  {
	    n = nList.item(i);
	    	    
	    if (n.nodeName() == "entry")
	      {
		QDomNode idNode = n.namedItem("gphoto:name");
		QDomNode tiNode = n.namedItem("title"); 

		if (!idNode.isNull() && !tiNode.isNull())
		  {
		    id = idNode.firstChild().nodeValue();
		    title = tiNode.firstChild().nodeValue();

		    result.insert(id, title);
		  }
	      }
	  }
  
    return result;
  }



  void WSPicasa::handleGetAlbumsResponse()
  {



    QStringList p;
    p.append("feed");
    
    QDomNode n = traverse(rcvdData,p);
	
    albums = getIdAndTitle(n);
  

    emit status(false, "");
    emit refreshUI();
    return;
  }






  void WSPicasa::doCreateGallery(QString title, 
				  QString description)
				
  {
    if (!authenticate())
      return;
      
    emit status(true, tr("Creating gallery %1").arg(title));

    QString entry ="<entry xmlns='http://www.w3.org/2005/Atom' "
      "xmlns:media='http://search.yahoo.com/mrss/' "
      "xmlns:gphoto='http://schemas.google.com/photos/2007'>";
    

    if (title.length())
      entry = entry + "<title type='text'><![CDATA["+title+"]]></title>";

    if (description.length())
      entry = entry + "<summary type='text'>"
	"<![CDATA["+description+"]]></summary>";

    entry = entry +"<category scheme='http://schemas.google.com/g/2005#kind' "
      "term='http://schemas.google.com/photos/2007#album'></category>"
      "</entry>\n";


    QFile f("/tmp/tryentry");
    f.open(QIODevice::WriteOnly);

    f.writeBlock(entry.utf8(),entry.utf8().length());
    f.close();


    QFile entry2("/tmp/entry");
    entry2.open(QIODevice::ReadOnly);
    QByteArray data = entry2.readAll();

    // kdDebug(51000) << entry ;

    state = ST_CREATEGALLERY;
    doRequest("", // Two empty stringlists for url and header params
	      authHeader,
	      "",
	      entry.utf8(),
	      "Content-type: application/atom+xml");


  }

  void WSPicasa::handleCreateGalleryResponse()
  {

    kdDebug(51000) << rcvdData;

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





  void WSPicasa::uploadImage()
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
	      "http://upload.picasa.com/photos/xmlrawadd.mg",
	      data,
	      "Content-Type: application/octet-stream");

    statusUpdate();

    state = ST_UPLOADIMAGE;
  }
    



  




  void WSPicasa::handleUploadImageResponse()
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
	    url.append("method=picasa.images.changeSettings");
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



  void WSPicasa::handleUploadImageChangeKeywordsResponse()
  {
    QStringList p;
    p.append("rsp");
    p.append("Create");
    p.append("Album");
   

    emit status(false,"");
    QDomNode n = traverse(rcvdData,p);

    doNextUpload();
  }



  void WSPicasa::stateHandler()
  {
    switch(state)
      {
      case  ST_LOGIN:
	handleLoginResponse();
	break;
	  
      case  ST_GETALBUMS:
	handleGetAlbumsResponse();
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

#include "wspicasa.moc"
