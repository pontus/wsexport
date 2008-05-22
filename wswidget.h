/****************************************************************************
** Form interface generated from reading ui file 'wswidget.ui'
**
** Created: sön maj 20 15:31:14 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.7   edited Aug 31 2005 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef WSWIDGET_H
#define WSWIDGET_H

#include <qvariant.h>
#include <qwidget.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class QPushButton;
class QComboBox;
class QLabel;
class Q3ButtonGroup;
class QRadioButton;
class QLineEdit;
class Q3GroupBox;
class KLed;
class QCheckBox;

class WSWidget : public QWidget
{
    Q_OBJECT

public:
    WSWidget( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
    ~WSWidget();

    QPushButton* addAccount;
    QComboBox* accountSelector;
    QLabel* textLabel1_2;
    QLabel* textLabel2;
    QComboBox* serviceSelector;
    Q3ButtonGroup* buttonGroup3;
    Q3ButtonGroup* buttonGroup4;
    QRadioButton* existingAlbum;
    QRadioButton* newAlbum;
    QRadioButton* addToAlbum;
    QRadioButton* uploadAlbums;
    QRadioButton* uploadAllAlbums;
    QComboBox* album;
    QLineEdit* addPhotosToNewAlbumName;
    Q3GroupBox* groupBox2;
    QComboBox* configuration;
    QLabel* textLabel6;
    QComboBox* category;
    QComboBox* subcategory;
    QLabel* textLabel4;
    QLabel* textLabel5;
    KLed* statusLed;
    QLabel* statusText;
    QCheckBox* matchCategories;
    QPushButton* uploadButton;
    QCheckBox* privateMode;
    QCheckBox* skipExisting;

public slots:
    virtual void slotExport();
    virtual void slotUploadSelectedAlbums();
    virtual void slotAddImagesToAlbum();

protected:

protected slots:
    virtual void languageChange();

};

#endif // WSWIDGET_H
