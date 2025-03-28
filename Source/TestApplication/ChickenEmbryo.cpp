/*!
* \file
* Member function definitions for ChickenEmbryo class.
*
* \remarks
*
* \authors
* lpapp
*/


#include <TestApplication/ChickenEmbryo.h>
#include <FileIo/TabularDataFileIo.h>
#include <QDebug>

//-----------------------------------------------------------------------------

namespace muw
{

ChickenEmbryo::ChickenEmbryo( QString aProjectFolderPath, QString aVolumeFeatureName )
:
	mProjectFolderPath( aProjectFolderPath ),
	mVolumeFeatureName( aVolumeFeatureName ),
	mCorrelationMatrix(),
	mLabelMatrix(),
	mFeatureCount( 0 ),
	mRedundantGroups(),
	mSpearmanRankThreshold( -1.0 ),
	mFeatureNames(),
	mHighRankingFeatureNames()
{
	qDebug() << "Project folder:" << mProjectFolderPath;

	lpmlfio::TabularDataFileIo loader;
	loader.load( aProjectFolderPath + "/CM.csv", mCorrelationMatrix );
	loader.load( aProjectFolderPath + "/CL.csv", mLabelMatrix );

	validateInputs();
}

//-----------------------------------------------------------------------------

ChickenEmbryo::~ChickenEmbryo()
{
	mCorrelationMatrix.clear();
	mLabelMatrix.clear();
	mRedundantGroups.clear();
	mFeatureNames.clear();
	mHighRankingFeatureNames.clear();
}

//-----------------------------------------------------------------------------

void ChickenEmbryo::saveResults()
{
	lpmlfio::TabularDataFileIo saver;
	saver.save( mProjectFolderPath + "/result.csv", mResultTable );
}

//-----------------------------------------------------------------------------

void ChickenEmbryo::execute( double aSpearmanRankThreshold )
{
	if ( mFeatureCount == 0 )
	{
		qDebug() << "Errors detected.";
		return;
	}

	qDebug() << "Detected number of features" << mFeatureCount;

	mSpearmanRankThreshold = std::abs( aSpearmanRankThreshold );

	qDebug() << "Spearman Rank Threshold (absolute)" << mSpearmanRankThreshold;

	generateRedundantGroups();
	selectHighRankingFeatures();
	generateVolumeEffectTable();
	saveResults();
}

//-----------------------------------------------------------------------------

void ChickenEmbryo::generateVolumeEffectTable()
{
	QString labelName = mLabelMatrix.headerNames().at( 0 );
	QStringList correlationColumnNames = mCorrelationMatrix.headerNames();

	if ( mVolumeFeatureName == "" )
	{
		mResultTable.setHeader( { labelName } );
		for ( int i = 0; i < mHighRankingFeatureNames.size(); ++i )
		{
			auto currentFeature = mHighRankingFeatureNames.at( i );
			QVariantList row;
			row.push_back( mLabelMatrix.valueAt( currentFeature, 0 ) );

			mResultTable.insert( currentFeature, row );
		}
	}
	else
	{
		mResultTable.setHeader( { labelName, mVolumeFeatureName } );
		for ( int i = 0; i < mHighRankingFeatureNames.size(); ++i )
		{
			auto currentFeature = mHighRankingFeatureNames.at( i );
			QVariantList row;
			row.push_back( mLabelMatrix.valueAt( currentFeature, 0 ) );

			auto volumeCorrelationRow = mCorrelationMatrix.value( mVolumeFeatureName );
			int columnIndex = correlationColumnNames.indexOf( currentFeature );
			double correlationWithVolume = volumeCorrelationRow.at( columnIndex ).toDouble();

			row.push_back( correlationWithVolume );

			mResultTable.insert( currentFeature, row );
		}
	}
}

//-----------------------------------------------------------------------------

void ChickenEmbryo::generateRedundantGroups()
{
	mRedundantGroups.clear();
	auto columnFeatureNames = mCorrelationMatrix.headerNames();
	QStringList processedFeatures;
	int groupIndex = 0;

	for ( int i = 0; i < mFeatureNames.size(); ++i )
	{
		auto currentRowFeature = mFeatureNames.at( i );

		if ( !processedFeatures.contains( currentRowFeature ) )  // We found a new feature which has not been in any redundancy group.
		{
			QStringList newGroup;
			newGroup.push_back( currentRowFeature );
			mRedundantGroups.push_back( newGroup );
			processedFeatures.push_back( currentRowFeature );
			groupIndex++;
		}

		for ( int j = 0; j < columnFeatureNames.size(); ++j )
		{
			auto currentColumnFeature = columnFeatureNames.at( j );

			if ( i == j ) continue;
			if ( processedFeatures.contains( currentColumnFeature ) ) continue;

			double currentCorrelationValue = mCorrelationMatrix.valueAt( currentRowFeature, j ).toDouble();
			if ( std::abs( currentCorrelationValue ) >= mSpearmanRankThreshold )
			{
				mRedundantGroups[ groupIndex - 1 ].push_back( currentColumnFeature );
				processedFeatures.push_back( currentColumnFeature );
			}
		}
	}

	int checkSum = 0;
	qDebug() << "Number of redundant groups" << mRedundantGroups.size();
	for ( int i = 0; i < mRedundantGroups.size(); ++i )
	{
		qDebug() << "Group" << i << mRedundantGroups.at( i ).size();
		checkSum += mRedundantGroups.at( i ).size();
	}

	if ( checkSum != mFeatureCount )
	{
		qDebug() << "ERROR - The number of features stored in groups is not equal with the feature count" << checkSum << "vs" << mFeatureCount;
	}
}

//-----------------------------------------------------------------------------

void ChickenEmbryo::selectHighRankingFeatures()
{
	mHighRankingFeatureNames.clear();

	for ( int i = 0; i < mRedundantGroups.size(); ++i )
	{
		auto currentGroup = mRedundantGroups.at( i );
		
		double currentMaxLabelCorrelation = -DBL_MAX;
		QString currentBestFeature;
		for ( int j = 0; j < currentGroup.size(); ++j )
		{
			auto currentFeature = currentGroup.at( j );
			double currentLabelCorrelation = std::abs( mLabelMatrix.valueAt( currentFeature, 0 ).toDouble() );

			if ( currentLabelCorrelation > currentMaxLabelCorrelation )
			{
				currentBestFeature = currentFeature;
				currentMaxLabelCorrelation = currentLabelCorrelation;
			}
		}

		mHighRankingFeatureNames.push_back( currentBestFeature );
	}

	if ( mRedundantGroups.size() != mHighRankingFeatureNames.size() )
	{
		qDebug() << "ERROR - The number of redundant groups does not match the number of highest-raking features" << mRedundantGroups.size() << mHighRankingFeatureNames.size();
	}
}

//-----------------------------------------------------------------------------

void ChickenEmbryo::validateInputs()
{
	qDebug() << "Correlation matrix loaded with dimensions" << mCorrelationMatrix.rowCount() << "x" << mCorrelationMatrix.columnCount();
	qDebug() << "Label matrix loaded with dimensions" << mLabelMatrix.rowCount() << "x" << mLabelMatrix.columnCount();

	auto CMKeys = mCorrelationMatrix.keys();
	auto CLKeys = mLabelMatrix.keys();

	std::sort( CMKeys.begin(), CMKeys.end() );
	std::sort( CLKeys.begin(), CLKeys.end() );

	mFeatureCount = CMKeys.size();
	mFeatureNames = CMKeys;

	if ( CMKeys.size() != CLKeys.size() )
	{
		qDebug() << "ERROR - Feature name list size mismatch" << CMKeys.size() << "vs" << CLKeys.size();
		mFeatureCount = 0;
	}
	else
	{
		for ( int i = 0; i < CMKeys.size(); ++i )
		{
			auto currentCMKey = CMKeys.at( i );
			auto currentCLKey = CLKeys.at( i );

			if ( currentCMKey != currentCLKey )
			{
				qDebug() << "ERROR - Feature names mismatch at position" << i << currentCMKey << "vs" << currentCLKey;
				mFeatureCount = 0;
			}
		}
	}
}

//-----------------------------------------------------------------------------

}