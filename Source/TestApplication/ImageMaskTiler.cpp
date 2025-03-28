/*!
* \file
* Member function definitions for ImageMaskTiler class.
*
* \remarks
*
* \authors
* lpapp
*/

#include <TestApplication/ImageMaskTiler.h>
#include <QDirIterator>
#include <QDebug>
#include <random>

//-----------------------------------------------------------------------------

namespace muw
{

//-----------------------------------------------------------------------------

ImageMaskTiler::ImageMaskTiler( QString aProjectFolderPath, int aTileSize )
:
	mProjectFolderPath( aProjectFolderPath ),
	mImagePairPaths(),
	mTileSize( aTileSize )
{
}

//-----------------------------------------------------------------------------

ImageMaskTiler::~ImageMaskTiler()
{
	mImagePairPaths.clear();
}

//-----------------------------------------------------------------------------

void ImageMaskTiler::execute()
{
	scanImagePairPaths();

	for ( auto imagePairPath : mImagePairPaths )
	{
		processImagePairs( imagePairPath );
	}
}

//-----------------------------------------------------------------------------

void ImageMaskTiler::scanImagePairPaths()
{
	mImagePairPaths.clear();

	QDirIterator it( mProjectFolderPath, QDirIterator::Subdirectories );
	while ( it.hasNext() )
	{
		it.next();
		QString name = it.fileName();
		QString path = it.filePath();

		if ( name.contains( "Mask.tif" ) )
		{
			mImagePairPaths.push_back( path.split( name ).at( 0 ) );
		}
	}
}

//-----------------------------------------------------------------------------

void ImageMaskTiler::processImagePairs( QString aImagePairFolderPath )
{
	QDirIterator it( aImagePairFolderPath, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories );

	QImage mask;
	QImage image;

	bool isMaskLoaded  = false;
	bool isImageLoaded = false;

	qDebug() << "Processing" << aImagePairFolderPath;

	while ( it.hasNext() )
	{
		it.next();
		QString name = it.fileName();
		QString path = it.filePath();

		if ( name.contains( ".tif" ) )
		{
			if ( name.contains( "Mask.tif" ) )
			{
				mask.load( path, "tiff" );
				isMaskLoaded = true;

				mask = morphClose( mask, 3 );
			}
			else
			{
				image.load( path, "tiff" );
				isImageLoaded = true;
			}
		}

		if ( isMaskLoaded && isImageLoaded )
		{
			tileImages( image, mask, aImagePairFolderPath );
			isMaskLoaded = false;
			isImageLoaded = false;
		}
		
	}
}

//-----------------------------------------------------------------------------

void ImageMaskTiler::tileImages( QImage aImage, QImage aMask, QString aImagePairFolderPath )
{
	auto size = aImage.size();

	QString sampleTilePath = aImagePairFolderPath.split( mProjectFolderPath ).at( 1 );
	QString targetTileProjectFolderPath = mProjectFolderPath + "/TILES-"+ QString::number( mTileSize ) + "x" + QString::number( mTileSize ) + "/" + sampleTilePath;
	QDir dir;

	if ( !dir.exists( targetTileProjectFolderPath ) )
	{
		dir.mkpath( targetTileProjectFolderPath );
	}

	qDebug() << "--------------------------------------------------";
	qDebug() << "Target folder" << targetTileProjectFolderPath;
	qDebug() << "Image and mask size" << size;
	auto idealTileStart = detectIdealTileStart( aMask, sampleTilePath.replace("/","-") );
	qDebug() << "Ideal tile start" << idealTileStart;
	
	int tileCountX = size.width() / mTileSize;
	int tileCountY = size.height() / mTileSize;

	for ( int tX = 0; tX < tileCountX; ++tX )
	{
		for ( int tY = 0; tY < tileCountY; ++tY )
		{
			int currentStartX = idealTileStart.first  + ( tX * mTileSize );
			int currentStartY = idealTileStart.second + ( tY * mTileSize );

			if ( validTile( aMask, currentStartX, currentStartY, currentStartX + mTileSize, currentStartY + mTileSize ) )
			{
				auto currentTile = tile( aImage, currentStartX, currentStartY, currentStartX + mTileSize, currentStartY + mTileSize );
				QString tileFileName = targetTileProjectFolderPath + "/TILE-" + QString::number( tX ) + "-" + QString::number( tY ) + "-"
					+ QString::number( currentStartX ) + ","
					+ QString::number( currentStartY ) + ","
					+ QString::number( currentStartX + mTileSize ) + ","
					+ QString::number( currentStartY + mTileSize )
					+ ".tif";
				qDebug() << "Saving tile" << tX << tY;
				currentTile.save( tileFileName, "tif" );
			}
		}
	}
}

//-----------------------------------------------------------------------------

QImage ImageMaskTiler::morphClose( QImage aMask, int aRounds )
{
	for ( int i = 0; i < aRounds; ++i )
	{
		aMask = morphDilate( aMask );
	}
	for ( int i = 0; i < aRounds; ++i )
	{
		aMask = morphErode( aMask );
	}

	return aMask;
}

//-----------------------------------------------------------------------------

QImage ImageMaskTiler::morphErode( QImage aMask )
{
	qDebug() << "morphErode";

	QImage eroded = aMask;
	auto size = aMask.size();
	eroded.fill( Qt::GlobalColor::black );

	for ( int i = 0; i < size.width(); ++i )
	{
		for ( int j = 0; j < size.height(); ++j )
		{
			int min = 255;

			for ( int ii = i - 1; ii < i + 2; ++ii )
			{
				for ( int jj = j - 1; jj < j + 2; ++jj )
				{
						
						if ( ii > -1 && jj > -1 && ii < size.width() && jj < size.height() )
						{
							int value = aMask.pixelColor( QPoint( ii, jj ) ).lightness();
							if ( value < min )
							{
								min = value;
							}
						}
						
				}
			}

			eroded.setPixelColor( QPoint( i, j ), QColor( min, min, min ) );
		}
	}

	return eroded;
}

//-----------------------------------------------------------------------------

QImage ImageMaskTiler::morphDilate( QImage aMask )
{
	qDebug() << "morphDilate";

	QImage dilated = aMask;
	auto size = aMask.size();
	dilated.fill( Qt::GlobalColor::black );

	for ( int i = 0; i < size.width(); ++i )
	{
		for ( int j = 0; j < size.height(); ++j )
		{
			int max = 0;

			for ( int ii = i - 1; ii < i + 2; ++ii )
			{
				for ( int jj = j - 1; jj < j + 2; ++jj )
				{

					if ( ii > -1 && jj > -1 && ii < size.width() && jj < size.height() )
					{
						int value = aMask.pixelColor( QPoint( ii, jj ) ).lightness();
						if ( value > max )
						{
							max = value;
						}
					}

				}
			}

			dilated.setPixelColor( QPoint( i, j ), QColor( max, max, max ) );
		}
	}

	return dilated;
}

//-----------------------------------------------------------------------------

QPair< int, int > ImageMaskTiler::detectIdealTileStart( QImage aMask, QString aScanPath )
{
	std::random_device rd;
	std::mt19937 eng( rd() );
	std::uniform_int_distribution< int > RGB( 0, 255 );

	auto maskSize = aMask.size();
	int tileCountX = maskSize.width()  / mTileSize;
	int tileCountY = maskSize.height() / mTileSize;

	bool isLog = true;

	qDebug() << "Number of maximum tiles" << tileCountX << "x" << tileCountY;

	QPair< int, int > bestCoordinate;
	int               maxTileCount = 0;

	for ( int sX = 0; sX < mTileSize / 2; ++sX )
	{
		for ( int sY = 0; sY < mTileSize / 2; ++sY )
		{
			QImage logTile = aMask;

			// Check number of valid tiles with current sX and sY shifts.
			int validTileCount = 0;
			for ( int tX = 0; tX < tileCountX; ++tX )
			{
				for ( int tY = 0; tY < tileCountY; ++tY )
				{
					int currentStartX = sX + ( tX * mTileSize );
					int currentStartY = sY + ( tY * mTileSize );

					if ( validTile( aMask, currentStartX, currentStartY, currentStartX + mTileSize, currentStartY + mTileSize ) )
					{
						int logPixelValueR = RGB( eng );
						int logPixelValueG = RGB( eng );
						int logPixelValueB = RGB( eng );

						++validTileCount;
						if ( isLog )
						{
							for ( int logX = currentStartX; logX < currentStartX + mTileSize; ++logX )
							{
								for ( int logY = currentStartY; logY < currentStartY + mTileSize; ++logY )
								{
									
									logTile.setPixelColor( QPoint( logX, logY ), QColor( logPixelValueR, logPixelValueG, logPixelValueB ) );
								}
							}
						}
					}
				}
			}

			if ( validTileCount > maxTileCount )
			{
				maxTileCount = validTileCount;
				bestCoordinate.first  = sX;
				bestCoordinate.second = sY;
				if ( isLog ) logTile.save( mProjectFolderPath + "/log/BestTile-" + aScanPath + ".tif", "tif" );
			}
		}
	}

	qDebug() << "number of tiles identified:" << maxTileCount << "with start x,y" << bestCoordinate;

	return bestCoordinate;
}

//-----------------------------------------------------------------------------

bool ImageMaskTiler::validTile( QImage aMask, int aStartX, int aStartY, int aEndX, int aEndY )
{
	auto maskSize = aMask.size();

	if ( aEndX > maskSize.width() || aEndY > maskSize.height() ) return false;

	for ( int i = aStartX; i < aEndX; ++i )
	{
		for ( int j = aStartY; j < aEndY; ++j )
		{
			int value = aMask.pixelColor( QPoint( i, j ) ).lightness();
			if ( value == 0 ) return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------

QImage ImageMaskTiler::tile( QImage aImage, int aStartX, int aStartY, int aEndX, int aEndY )
{
	int width  = aEndX - aStartX;
	int height = aEndY - aStartY;

	qDebug() << "Input image size" << aImage.size();
	qDebug() << aStartX << aStartY << aEndX << aEndY;

	QImage tile( width, height, QImage::Format::Format_RGB32 );

	qDebug() << "Tiled image size" << tile.size();

	for ( int i = 0; i < width; ++i )
	{
		for ( int j = 0; j < height; ++j )
		{
			int maskX = i + aStartX;
			int maskY = j + aStartY;

			int value = aImage.pixelColor( QPoint( maskX, maskY ) ).lightness();
			tile.setPixelColor( QPoint( i, j ), QColor( value, value, value ) );
		}
	}

	return tile;
}

//-----------------------------------------------------------------------------

}

//-----------------------------------------------------------------------------