#include <stdlib.h>
#include <QMap>
#include <QString>
#include <QDebug>
#include <ChickenEmbryo.h>
#include <ImageMaskTiler.h>

//-----------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
	// Modify these variables for your own environment and needs.
	QString projectFolderPath = "c:/XOCTRA/";
	int tileWidthHeight = 80;
	double SpearmanRankThreshold = 0.7;

	// Use Case 1: Tiling OCT images and their corresponding masks.
	muw::ImageMaskTiler IMT( projectFolderPath + "/ToTile/", tileWidthHeight );
	IMT.execute();

	// USe Case 2: Radiomics redundancy clustering and ranking
	muw::ChickenEmbryo Embryo( projectFolderPath + "/Radiomics/" );
	Embryo.execute( SpearmanRankThreshold );

	return 0;
}

//-----------------------------------------------------------------------------