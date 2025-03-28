#ifndef TESTAPPLICATION_H
#define TESTAPPLICATION_H

#include <QtWidgets/QMainWindow>
//#include "ui_TestApplication.h"
#include <Evaluation/Validator.h>
#include <Evaluation/Normalizer.h>
#include <Evaluation/Delineator.h>
#include <Evaluation/ImageFeatureExtractor.h>
#include <Evaluation/KernelDensityExtractor.h>

#include <QPixmap>
#include <QImage>
#include <QSettings>

class TestApplication : public QMainWindow
{
	Q_OBJECT

public:
	TestApplication(QWidget *parent = 0);
	~TestApplication();

	void update();
	void setUpSettingsTable();
	QPixmap createDelineatedImage( lpmldata::Volume& aRectVolume, lpmldata::Volume& aVOIVolume, int aMaximumWidth, int aMaximumHeight );
	QPixmap createNormalizedImage( lpmldata::Volume& aRectVolume, int aMaximumWidth, int aMaximumHeight );
	ulint voiSampleCount( lpmldata::Volume& aVoiVolume );
	void validateVOISampleCounts( int aPatient );
	void disableTab( int aTabIndex );
	void enableTab( int aTabIndex );
	void initPatientList();

public slots:

	void proceed();

	// Setting tab
	void openFolderDialog();
	void settingCellChanged( int aRow, int aColumn );

	// Validator tab
	void doAllValidation();

	// Normalization tab
	void executeNormalization();
	void doAllNormalization();
	void updateNormalization();

	// Delineation tab
	void executeDelineation();
	void doAllDelineation();
	void updateDelineation();

	// Feature extraction tab
	void setUpBinningSettingsTable();
	void executeFeatureExtraction();
	void doAllFeatureExtraction();
	void updateFeatureExtraction();
	void binningSettingCellChanged( int aRow, int aColumn );

	//Feature kernel density tab
	void updateFeatureKernelDensity();
	void setUpFeatureKernelPage();
	void featureChanged( int aFeatureIndex );
	void labelChanged( int aLabelIndex );
	void fDBSliderChanged( int aPosition );
	void buildUpFDBList();
	

	// Global
	void sliderchanged( int aValue );
	void patientChanged( int aValue );
	void executeAll();	
	void executeOne();
	void activeTabPageChanged( int aValue );
	
private:

	QSettings* mSettingsMap;
	QSettings* mSettingsDescriptors;
	QSettings* mBinningSettingsMap;

	int mProcessID;
	int mProcessCount;
	int mPatientCount;

	bool mIstooLessSamples;
	QString mSampleCountMessage;

	muweval::Validator* valid;
	muweval::Normalizer* norm;
	muweval::Delineator* delin;
	muweval::ImageFeatureExtractor* extractor;
	

	QStringList mModalityRectNames;
	QMap< QString, lpmldata::Volume > mModalityRectsOrig;
	QMap< QString, lpmldata::Volume > mModalityRectsNorm;

	QMap< QString, lpmldata::Volume > mModalityRectsDel;
	QMap< QString, lpmldata::Volume > mVOIs;
	QMap< QString, QPair< double, double > > mVOIMinMaxes;

	lpmldata::TabularData mFDB;

	double mSliceRatio;
	double mMinimumSampleCount;

	int mActivePatient;
	int mActiveTabPage;

	bool canProceed;
	bool mIsNormalizedImagesDirty;
	bool mIsDelineatedImagesDirty;
	bool mIsFeatureVectorDatabaseDirty;
	bool mIsBatchMode;

	bool mIsNormalizationSkipped;
	bool mIsDelineationSkipped;

	int mLastNormalizationPatientIndex;
	int mLastDelineationPatientIndex;

	int mFirstStepToPerform;
	int mActiveFeature;
	int mActiveLabel;

	lpmleval::KernelDensityExtractor* mKDExtractor;

	lpmldata::TabularData mFDBLoaded;
	lpmldata::TabularData mLDBLoaded;

	QStringList mFDBList;

};

#endif // TESTAPPLICATION_H
