/* ============================================================
 * File  : wspicasa.h
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

#ifndef WSPICASA_H
#define WSPICASA_H

#include "wsbase.h"

namespace WSExport
{
  class WSPicasa : public WSBase
    {
      Q_OBJECT

    public:
      WSPicasa();
      ~WSPicasa();

      virtual bool albumExists(QString name);

      virtual bool supportsThemes();
      virtual bool supportsConfigurations();
      virtual bool supportsCategories();
      virtual bool supportsSubCategories();
      virtual bool supportsAlbumComments();
      virtual bool supportsImageComments();


      public slots:

	//virtual bool authenticate();

    protected:
      QString authHeader;
      QString sessionId;
      QString APIkey;

      virtual void stateHandler();
      virtual void simpleMethodCall(QString method, int newState);

      virtual void handleLoginResponse();    
      virtual void handleGetAlbumsResponse();
      virtual void handleCreateGalleryResponse();
      virtual void handleUploadImageResponse();
      virtual void handleUploadImageChangeKeywordsResponse();


      virtual bool responseOk(QString data);



      virtual void uploadImage();

      virtual void doCreateGallery(QString title, 
				   QString description);





      QMap<QString,QString> getIdAndTitle(QDomNode n);

    protected slots:

      virtual void doLogin();
      virtual void doGetAlbums();


    };
};

#endif /* WSPICASA_H */
