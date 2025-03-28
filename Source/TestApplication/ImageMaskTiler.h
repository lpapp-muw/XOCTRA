/*!
* The ImageMaskTiler class loads OCT slices and their respective binary masks are processed.
* First, the image pairs are loaded up, followed by morphological closing of the mask image.
* Afterwards, small non-overlapping tile images are generated from which radiomic features can be extracted.
*
* \remarks
*
* \authors
* lpapp
*/

#pragma once

#include <QList>
#include <QString>
#include <QImage>

//-----------------------------------------------------------------------------

namespace muw
{

class ImageMaskTiler
{

public:

	ImageMaskTiler( QString aProjectFolderPath, int aTileSize );
	~ImageMaskTiler();

	void execute();

private:

	void scanImagePairPaths();
	void processImagePairs( QString aImagePairFolderPath );
	void tileImages( QImage aImage, QImage aMask, QString aImagePairFolderPath );
	QImage morphClose( QImage aMask, int aRounds );
	QImage morphErode( QImage aMask );
	QImage morphDilate( QImage aMask );

	QPair< int, int > detectIdealTileStart( QImage aMask, QString aScanPath );
	bool validTile( QImage aMask, int aStartX, int aStartY, int aEndX, int aEndY );
	QImage tile( QImage aImage, int aStartX, int aStartY, int aEndX, int aEndY );

private:

	QString       mProjectFolderPath;
	QStringList   mImagePairPaths;
	int           mTileSize;

};

}

//-----------------------------------------------------------------------------