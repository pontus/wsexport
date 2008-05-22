/****************************************************************************
** Form implementation generated from reading ui file 'wswidget.ui'
**
** Created: sön maj 20 15:31:29 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.7   edited Aug 31 2005 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "wswidget.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <q3groupbox.h>
#include <kled.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include "wswidget.ui.h"

/*
 *  Constructs a WSWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
WSWidget::WSWidget( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "WSWidget" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 800, 500 ) );
    setMaximumSize( QSize( 760, 610 ) );

    addAccount = new QPushButton( this, "addAccount" );
    addAccount->setGeometry( QRect( 480, 60, 228, 32 ) );

    accountSelector = new QComboBox( FALSE, this, "accountSelector" );
    accountSelector->setGeometry( QRect( 190, 60, 280, 32 ) );
    accountSelector->setMinimumSize( QSize( 100, 0 ) );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 10, 60, 130, 32 ) );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setGeometry( QRect( 12, 20, 345, 32 ) );

    serviceSelector = new QComboBox( FALSE, this, "serviceSelector" );
    serviceSelector->setGeometry( QRect( 363, 20, 345, 32 ) );

    buttonGroup3 = new Q3ButtonGroup( this, "buttonGroup3" );
    buttonGroup3->setGeometry( QRect( 10, 120, 470, 140 ) );
    buttonGroup3->setLineWidth( 0 );

    buttonGroup4 = new Q3ButtonGroup( buttonGroup3, "buttonGroup4" );
    buttonGroup4->setGeometry( QRect( 320, 0, 150, 90 ) );
    buttonGroup4->setLineWidth( 0 );

    existingAlbum = new QRadioButton( buttonGroup4, "existingAlbum" );
    existingAlbum->setGeometry( QRect( 0, 10, 140, 21 ) );
    existingAlbum->setChecked( TRUE );

    newAlbum = new QRadioButton( buttonGroup4, "newAlbum" );
    newAlbum->setGeometry( QRect( 0, 30, 120, 31 ) );

    addToAlbum = new QRadioButton( buttonGroup3, "addToAlbum" );
    addToAlbum->setGeometry( QRect( 10, 30, 270, 24 ) );

    uploadAlbums = new QRadioButton( buttonGroup3, "uploadAlbums" );
    uploadAlbums->setGeometry( QRect( 10, 70, 460, 24 ) );
    uploadAlbums->setChecked( FALSE );

    uploadAllAlbums = new QRadioButton( buttonGroup3, "uploadAllAlbums" );
    uploadAllAlbums->setGeometry( QRect( 10, 110, 441, 21 ) );

    album = new QComboBox( FALSE, this, "album" );
    album->setGeometry( QRect( 490, 120, 280, 30 ) );

    addPhotosToNewAlbumName = new QLineEdit( this, "addPhotosToNewAlbumName" );
    addPhotosToNewAlbumName->setGeometry( QRect( 490, 150, 290, 23 ) );

    groupBox2 = new Q3GroupBox( this, "groupBox2" );
    groupBox2->setGeometry( QRect( 0, 270, 530, 100 ) );
    groupBox2->setLineWidth( 0 );

    configuration = new QComboBox( FALSE, groupBox2, "configuration" );
    configuration->setGeometry( QRect( 190, 10, 320, 31 ) );

    textLabel6 = new QLabel( groupBox2, "textLabel6" );
    textLabel6->setGeometry( QRect( 10, 70, 180, 31 ) );

    category = new QComboBox( FALSE, groupBox2, "category" );
    category->setGeometry( QRect( 210, 40, 210, 31 ) );

    subcategory = new QComboBox( FALSE, groupBox2, "subcategory" );
    subcategory->setGeometry( QRect( 210, 70, 210, 31 ) );

    textLabel4 = new QLabel( groupBox2, "textLabel4" );
    textLabel4->setGeometry( QRect( 10, 20, 168, 21 ) );

    textLabel5 = new QLabel( groupBox2, "textLabel5" );
    textLabel5->setGeometry( QRect( 10, 50, 90, 21 ) );

    statusLed = new KLed( this, "statusLed" );
    statusLed->setGeometry( QRect( 620, 380, 30, 40 ) );

    statusText = new QLabel( this, "statusText" );
    statusText->setGeometry( QRect( 491, 430, 290, 50 ) );
    QFont statusText_font(  statusText->font() );
    statusText_font.setPointSize( 8 );
    statusText->setFont( statusText_font ); 
    statusText->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop | QLabel::AlignHCenter ) );

    matchCategories = new QCheckBox( this, "matchCategories" );
    matchCategories->setGeometry( QRect( 460, 230, 281, 21 ) );
    QFont matchCategories_font(  matchCategories->font() );
    matchCategories_font.setPointSize( 9 );
    matchCategories->setFont( matchCategories_font ); 

    uploadButton = new QPushButton( this, "uploadButton" );
    uploadButton->setGeometry( QRect( 80, 450, 220, 32 ) );

    privateMode = new QCheckBox( this, "privateMode" );
    privateMode->setGeometry( QRect( 0, 380, 520, 21 ) );

    skipExisting = new QCheckBox( this, "skipExisting" );
    skipExisting->setGeometry( QRect( 0, 410, 520, 21 ) );
    languageChange();
    resize( QSize(800, 500).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
WSWidget::~WSWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void WSWidget::languageChange()
{
    setCaption( tr( "WSWidget" ) );
    addAccount->setText( tr( "Add account" ) );
    textLabel1_2->setText( tr( "Use account:" ) );
    textLabel2->setText( tr( "Web service to export to:" ) );
    serviceSelector->clear();
    serviceSelector->insertItem( tr( "Smugmug" ) );
    serviceSelector->insertItem( tr( "Picasa" ) );
    serviceSelector->insertItem( tr( "23" ) );
    QToolTip::add( serviceSelector, tr( "Select the service to upload images to." ) );
    buttonGroup3->setTitle( tr( "Upload images" ) );
    buttonGroup4->setTitle( QString::null );
    existingAlbum->setText( tr( "Existing album" ) );
    newAlbum->setText( tr( "New album" ) );
    addToAlbum->setText( tr( "Add selected images to" ) );
    uploadAlbums->setText( tr( "Upload selected album \"1234567890123456789\"" ) );
    uploadAllAlbums->setText( tr( "Upload all albums that do not exist already" ) );
    groupBox2->setTitle( tr( "New album settings" ) );
    textLabel6->setText( tr( "Sub category" ) );
    textLabel4->setText( tr( "Album configuration" ) );
    textLabel5->setText( tr( "Category" ) );
    QToolTip::add( statusLed, tr( "Working." ) );
    statusText->setText( tr( "Statusbox" ) );
    matchCategories->setText( tr( "Try to match categories" ) );
    QToolTip::add( matchCategories, tr( "If selected and an existing category matches the local category for an album, use that category instead of the chosen one." ) );
    uploadButton->setText( tr( "Start Upload" ) );
    privateMode->setText( tr( "Private mode" ) );
    QToolTip::add( privateMode, tr( "Only names (of images and albums) are transferred, tags, ratings and comments are not." ) );
    skipExisting->setText( tr( "Skip existing images" ) );
    QToolTip::add( skipExisting, tr( "If checked, WSExport will try to verify whatever the image already exists on the server and skip the upload if possible." ) );
}

