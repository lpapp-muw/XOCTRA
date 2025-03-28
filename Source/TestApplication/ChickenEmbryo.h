/*!
* The ChickenEmbryo class loads up a pre-generated correlation matrix (CM.csv) and a correlation with Progression (CL.csv).
* CM contains all radiomic features Spearman ranked to one another.
* CL contains the Spearman ranks of each radiomic feature with the label Progression. 
* A Spearman Rank-based clustering is performed in CM to build up redundant cluster groups, from which the feature with the highest Spearman Rank to
* Progression (CL) is selected per-cluster. Analysis results are stored in "results.csv".
*
* \remarks
*
* \authors
* lpapp
*/

#pragma once

#include <DataRepresentation/TabularData.h>
#include <QVector>
#include <QMap>
#include <QString>
#include <QVariant>

//-----------------------------------------------------------------------------

namespace muw
{

class ChickenEmbryo
{

public:

	ChickenEmbryo( QString aProjectFolderPath, QString aVolumeFeatureName = "" );
	~ChickenEmbryo();

	void execute( double aSpearmanRankThreshold );

private:
	ChickenEmbryo();
	void validateInputs();
	void generateRedundantGroups();
	void selectHighRankingFeatures();
	void generateVolumeEffectTable();
	void saveResults();

private:

	QString                 mProjectFolderPath;
	QString                 mVolumeFeatureName;
	lpmldata::TabularData   mCorrelationMatrix;
	lpmldata::TabularData   mLabelMatrix;
	int                     mFeatureCount;
	QVector< QStringList >  mRedundantGroups;
	double                  mSpearmanRankThreshold;
	QStringList             mFeatureNames;
	QStringList             mHighRankingFeatureNames;
	lpmldata::TabularData   mResultTable;


};

}

//-----------------------------------------------------------------------------