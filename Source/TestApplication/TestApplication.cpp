#include "TestApplication.h"

#include <DataRepresentation/Volume.h>
#include <FileIo/HermesVoiFileIo.h>
#include <FileIo/TabularDataFileIo.h>
#include <Evaluation/TabularDataFilter.h>
#include <Evaluation/FeatureKernel.h>

#include <QTimer>
#include <QThread>
#include <QTime>

#include <QDebug>
#include <omp.h>

#include <QImage>
#include <QLabel>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>

//-----------------------------------------------------------------------------

TestApplication::TestApplication( QWidget *parent )
:
	QMainWindow( parent ),
	mSettingsMap( nullptr ),
	mSettingsDescriptors( nullptr ),
	valid( nullptr ),
	norm( nullptr ),
	delin( nullptr ),
	extractor( nullptr ),
	mProcessID( 0 ),
	mProcessCount( 5 ),
	mSliceRatio( 0.0 ),
	mActivePatient( 0 ),
	mMinimumSampleCount( 10 ),
	mActiveTabPage( 0 ),
	canProceed( true ),
	mIsNormalizedImagesDirty( true ),
	mIsDelineatedImagesDirty( true ),
	mIsFeatureVectorDatabaseDirty( true ),
	mIsBatchMode( false ),
	mLastNormalizationPatientIndex( -1 ),
	mLastDelineationPatientIndex( -1 ),
	mFDB(),
	mFirstStepToPerform( 0 ),
	mIsNormalizationSkipped( false ),
	mIsDelineationSkipped(false ),
	mActiveFeature( 0 ),
	mActiveLabel( 0 ),
	mKDExtractor(),
	mFDBList()
{

	ui.setupUi( this );

	QString masterPath = "g:/_PROJECTS-2018/Prostate-PETMRI/";

	ui.folderPath->setText( masterPath );
	mSettingsMap = new QSettings( masterPath + "/Settings/settings.ini", QSettings::IniFormat );
	mBinningSettingsMap = new QSettings( masterPath + "/Settings/binningSettings.ini", QSettings::IniFormat );
	mSettingsDescriptors = new QSettings( masterPath + "/Settings/settingsDescriptors.ini", QSettings::IniFormat );

	connect( ui.sliceSlider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderchanged( int ) ) );
	connect( ui.patientScrollBar, SIGNAL( valueChanged( int ) ), this, SLOT( patientChanged( int ) ) );
	connect( ui.pushButtonExecuteOneStep, SIGNAL( clicked() ), this, SLOT( executeOne() ) );
	connect( ui.pushButtonExecuteAllSteps, SIGNAL( clicked() ), this, SLOT( executeAll() ) );
	connect( ui.tabWidget, SIGNAL( currentChanged( int ) ), this, SLOT( activeTabPageChanged( int ) ) );
	connect( ui.pushButtonOpenProjectFolder, SIGNAL( clicked() ), this, SLOT( openFolderDialog() ) );
	connect( ui.settingsTable, SIGNAL( cellChanged( int, int ) ), this, SLOT( settingCellChanged( int, int ) ) );
	connect( ui.binningSettingsTable, SIGNAL( cellChanged( int, int ) ), this, SLOT( binningSettingCellChanged( int, int ) ) );
	connect( ui.scrollBarFeatures, SIGNAL( valueChanged( int ) ), this, SLOT( featureChanged( int ) ) );
	connect( ui.scrollBarLabels, SIGNAL( valueChanged( int ) ), this, SLOT( labelChanged( int ) ) );
	connect( ui.scrollBarFDB,    SIGNAL( valueChanged( int ) ), this, SLOT( fDBSliderChanged( int ) ) );

	ui.patientScrollBar->setMinimum( 0 );
	ui.patientScrollBar->setMaximum( 0 );
	ui.patientScrollBar->setValue( 0 );

	ui.progressBar->setRange( 0, 100 );
	ui.progressBar->setValue( 0 );

	ui.tabWidget->setCurrentIndex( 0 );
	ui.sliceSlider->setValue( ( ui.sliceSlider->maximum() - ui.sliceSlider->minimum() ) / 2 );

	valid = new muweval::Validator( ui.folderPath->text(), "timeee" );
	ui.tabWidget->setTabEnabled( 0, true );
	ui.tabWidget->setCurrentIndex( mActiveTabPage );

	doAllValidation();
	initPatientList();

	setUpSettingsTable();
	setUpBinningSettingsTable();
	//setUpFeatureKernelPage();

	//buildUpFDBList();
}

//-----------------------------------------------------------------------------

void TestApplication::fDBSliderChanged( int aPosition )
{
	lpmlfio::TabularDataFileIo loader;

	QString actualFDBPath = mFDBList.at( aPosition );

	ui.labelOpenedFDB->setText( actualFDBPath );

	loader.load( actualFDBPath, mFDBLoaded );

	if ( mKDExtractor != nullptr )
	{
		delete mKDExtractor;
		mKDExtractor = new lpmleval::KernelDensityExtractor( mFDBLoaded, mLDBLoaded, mActiveLabel );
	}	 

	update();
}

//-----------------------------------------------------------------------------

void TestApplication::buildUpFDBList()
{
	lpmlfio::TabularDataFileIo loader;
	lpmleval::TabularDataFilter filter;

	// TODO: build up the fdblist
	QDirIterator it( ui.folderPath->text() + "/L2Features/" );
	while ( it.hasNext() )
	{
		it.next();
		QString path = it.filePath();
		QString name = it.fileName();

		if ( name != "." && name != ".." && name.contains( "FDB" ) )
		{
			mFDBList.push_back( path.split( ".csv" ).at( 0 ) );
		}
	}

	loader.load( mFDBList.at( 0 ), mFDBLoaded );
	loader.load( ui.folderPath->text() + "/Labels/LDB", mLDBLoaded );

	mKDExtractor = new lpmleval::KernelDensityExtractor( mFDBLoaded, mLDBLoaded, mActiveLabel );
	QMap< double, int > overlapR = mKDExtractor->overlapRatios();

	ui.scrollBarFDB->setMaximum( mFDBList.size() - 1 );

	update();
}


//-----------------------------------------------------------------------------

void TestApplication::initPatientList()
{
	QString projectFolder = ui.folderPath->text();

	// Determine patient count here.

	mPatientCount = 0;// valid->patientFolders().size();

	QDir normDir( projectFolder + "/" + "Normalized" );
	if ( normDir.exists() )
	{
		mIsNormalizedImagesDirty = false;
		mFirstStepToPerform = 1;
		disableTab( 1 );
		mIsNormalizationSkipped = true;
	}

	QDir delDir( projectFolder + "/" + "Delineated" );
	if ( delDir.exists() )
	{
		mIsDelineatedImagesDirty = false;
		mFirstStepToPerform = 2;
		disableTab( 2 );
		mIsDelineationSkipped = true;
	}

	
}

//-----------------------------------------------------------------------------

void TestApplication::doAllValidation()
{
	if ( valid->passed() )
	{
		canProceed = true;
		//mModalityRectNames = valid->modalityRects();
	}
	else
	{
		canProceed = false;

		QMessageBox* message = new QMessageBox();
		message->setWindowTitle( "Error - Invalid project folder!" );
		//message->setText( "Errors: \n\n " + valid->validationText() );
		message->show();
	}	

	mVOIMinMaxes.clear();
	for ( int i = 0; i < mModalityRectNames.size(); ++i )
	{
		mVOIMinMaxes.insert( mModalityRectNames.at( i ), QPair< double, double >( DBL_MAX, -DBL_MAX ) );
	}
}

//-----------------------------------------------------------------------------

void TestApplication::proceed()
{
	if ( !canProceed )
	{
		qDebug() << "Sorry, cannot proceed!";
		return;
	}

	mActiveTabPage = mProcessID + 1;

	switch ( mProcessID )
	{
	case 0: // Normalization
	{
		qDebug() << "Settings -> Normalization";
		if ( mIsNormalizedImagesDirty  )
		{
			if ( norm != nullptr ) delete norm;
			norm = new muweval::Normalizer( ui.folderPath->text(), "timee" );
			doAllNormalization();
		}
		else
		{
			qDebug() << "No need to do Normalization, images are valid.";
		}
		
		break;
	}
	case 1:  // Normalization -> proceed to Delineation
	{
		qDebug() << "Normalization -> Delineation";
		if ( mIsDelineatedImagesDirty  )
		{
			if ( delin != nullptr ) delete delin;
			delin = new muweval::Delineator( ui.folderPath->text(), "timee" );
			doAllDelineation();
		}
		else
		{
			qDebug() << "No need to do Delineation, images are valid.";
		}
		break;
	}	
	case 2:  // Delineation -> proceed to Feature Extraction
	{
		qDebug() << "Delineation -> Feature extraction";
		if ( mIsFeatureVectorDatabaseDirty  )
		{
			setUpBinningSettingsTable();
			if ( extractor != nullptr ) delete extractor;
			//extractor = new muweval::ImageFeatureExtractor( ui.folderPath->text(), mBinningSettingsMap );
			doAllFeatureExtraction();
		}
		else
		{
			qDebug() << "No need to do Feature extraction, values are valid.";
		}
		break;
	}
	case 3:  // Here we repeat the last step.
	{
		//setUpBinningSettingsTable();
		//if ( extractor != nullptr ) delete extractor;
		//extractor = new muweval::ImageFeatureExtractor( ui.folderPath->text(), mBinningSettingsMap );
		////if ( mIsFeatureVectorDatabaseDirty )
		//{
		//	doAllFeatureExtraction();
		//}
		break;
	}
	}
}

//-----------------------------------------------------------------------------

void TestApplication::disableTab( int aTabIndex )
{
	switch ( aTabIndex )
	{
		case 0:  // Settings cannot be disabled
		{
			break;
		}
		case 1:  // Normalization
		{
			ui.tabWidget->setTabEnabled( 1, false );
		}
		case 2:  // Delineation
		{
			ui.tabWidget->setTabEnabled( 2, false );
		}
		case 3:  // Feature extraction
		{
			ui.tabWidget->setTabEnabled( 3, false );
		}

	}
}

//-----------------------------------------------------------------------------

void TestApplication::enableTab( int aTabIndex )
{
	switch ( aTabIndex )
	{
		case 4:  // Feature extraction
		{
			ui.tabWidget->setTabEnabled( 4, true );
		}
		case 3:  // Feature extraction
		{
			ui.tabWidget->setTabEnabled( 3, true );
		}
		case 2:  // Delineation
		{
			ui.tabWidget->setTabEnabled( 2, true );
		}
		case 1:  // Normalization
		{
			ui.tabWidget->setTabEnabled( 1, true );
		}
		case 0:  // Settings cannot be disabled
		{
			break;
		}
	}
}

//-----------------------------------------------------------------------------

void TestApplication::setUpSettingsTable()
{
	ui.settingsTable->blockSignals( true );

	ui.settingsTable->clearContents();
	ui.settingsTable->setRowCount( 0 );

	QStringList childKeys = mSettingsMap->allKeys();
	foreach( const QString &childKey, childKeys )
	{
		ui.settingsTable->insertRow( 0 );

		QString settingName = childKey;
		QVariant settingValue = mSettingsMap->value( childKey );
		QVariant settingExplanation = mSettingsDescriptors->value( childKey );

		QTableWidgetItem *item = new QTableWidgetItem( settingName );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable ); // non editable
		ui.settingsTable->setItem( 0, 0, item );

		item = new QTableWidgetItem( settingValue.toString() );
		ui.settingsTable->setItem( 0, 1, item );

		item = new QTableWidgetItem( settingExplanation.toString() );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable ); // non editable
		ui.settingsTable->setItem( 0, 2, item );	
	}

	ui.settingsTable->blockSignals( false );
	ui.settingsTable->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
	ui.settingsTable->update();

	disableTab( 1 );
}

//-----------------------------------------------------------------------------

void TestApplication::setUpBinningSettingsTable()
{
	ui.binningSettingsTable->blockSignals( true );

	ui.binningSettingsTable->clearContents();
	ui.binningSettingsTable->setRowCount( 0 );

	// Modify rangeMin and rangeMax based on settings and mVOIMinMaxes

	for ( int i = 0; i < mModalityRectNames.size(); ++i )
	{
		int rangeBy = mBinningSettingsMap->value( mModalityRectNames.at( i ) + "/RangeBy" ).toInt();

		switch ( rangeBy )
		{
			case 0:  // Manual
			{
				// Nothing to do here, the size will be read out later on.
				break;
			}
			case 1:  // Sample
			{
				mBinningSettingsMap->setValue( mModalityRectNames.at( i ) + "/RangeMin", mVOIMinMaxes.value( mModalityRectNames.at( i ) ).first );
				mBinningSettingsMap->setValue( mModalityRectNames.at( i ) + "/RangeMax", mVOIMinMaxes.value( mModalityRectNames.at( i ) ).second );
			}
		}
	}

	// Go through the setting map and set up the settings table.
	for ( int i = 0; i < mModalityRectNames.size(); ++i )
	{
		ui.binningSettingsTable->insertRow( 0 );
		QString modalityName = mModalityRectNames.at( i );

		QTableWidgetItem* item = new QTableWidgetItem( modalityName );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable ); // non editable
		ui.binningSettingsTable->setItem( 0, 0, item );

		ui.binningSettingsTable->setItem( 0, 1, new QTableWidgetItem( mBinningSettingsMap->value( modalityName + "/" + "RangeBy" ).toString() ) );
		ui.binningSettingsTable->setItem( 0, 2, new QTableWidgetItem( mBinningSettingsMap->value( modalityName + "/" + "OutsideRange" ).toString() ) );
		ui.binningSettingsTable->setItem( 0, 3, new QTableWidgetItem( mBinningSettingsMap->value( modalityName + "/" + "Method" ).toString() ) );
		ui.binningSettingsTable->setItem( 0, 4, new QTableWidgetItem( mBinningSettingsMap->value( modalityName + "/" + "Width" ).toString() ) );
		ui.binningSettingsTable->setItem( 0, 5, new QTableWidgetItem( mBinningSettingsMap->value( modalityName + "/" + "Size" ).toString() ) );
		ui.binningSettingsTable->setItem( 0, 6, new QTableWidgetItem( mBinningSettingsMap->value( modalityName + "/" + "RangeMin" ).toString() ) );
		ui.binningSettingsTable->setItem( 0, 7, new QTableWidgetItem( mBinningSettingsMap->value( modalityName + "/" + "RangeMax" ).toString() ) );
	}

	ui.binningSettingsTable->blockSignals( false );
	ui.binningSettingsTable->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
	ui.binningSettingsTable->update();
}

//-----------------------------------------------------------------------------

void TestApplication::settingCellChanged( int aRow, int aColumn )
{
	QString key = ui.settingsTable->item( aRow, 0 )->text();
	QString value = ui.settingsTable->item( aRow, 1 )->text();

	mSettingsMap->setValue( key, value );

	if ( key.contains( "Normalization" ) )
	{
		mIsNormalizedImagesDirty = true;
		mIsDelineatedImagesDirty = true;
		disableTab( 1 );
	}
	else if ( key.contains( "Delineation" ) )
	{
		mIsDelineatedImagesDirty = true;
		disableTab( 2 );
	}

	mIsFeatureVectorDatabaseDirty = true;  // This changes if any of the above changes.

	canProceed = true;
}

//-----------------------------------------------------------------------------

void TestApplication::binningSettingCellChanged( int aRow, int aColumn )
{
	QString value = ui.binningSettingsTable->item( aRow, aColumn )->text();

	// TODO: Determine the key based on row.
	QString modalityKey = ui.binningSettingsTable->item( aRow, 0 )->text();
	QString columnKey = ui.binningSettingsTable->horizontalHeaderItem( aColumn )->text();
	QString key = modalityKey + "/" + columnKey;

	mBinningSettingsMap->setValue( key, value );

	// Based on modified key, modify dependent other setting values.
	int binMethod   = mBinningSettingsMap->value( modalityKey + "/Method" ).toInt();
	int rangeBy     = mBinningSettingsMap->value( modalityKey + "/RangeBy" ).toInt();
	double rangeMin = mBinningSettingsMap->value( modalityKey + "/RangeMin" ).toDouble();
	double rangeMax = mBinningSettingsMap->value( modalityKey + "/RangeMax" ).toDouble();
	int    binSize  = mBinningSettingsMap->value( modalityKey + "/Size" ).toInt();
	double binWidth = mBinningSettingsMap->value( modalityKey + "/Width" ).toDouble();

	if ( columnKey == "Size" )
	{
		binWidth = ( rangeMax - rangeMin ) / binSize;
		mBinningSettingsMap->setValue( modalityKey + "/Width", binWidth );
	}
	else if ( columnKey == "Width" )
	{
		binSize = ( rangeMax - rangeMin ) / binWidth;
		mBinningSettingsMap->setValue( modalityKey + "/Size", binSize );
	}
	else if ( columnKey.contains( "RangeM" ) )
	{
		switch ( binMethod )
		{
		case ( 0 ) :  // BySize
		{
			binWidth = ( rangeMax - rangeMin ) / binSize;
			mBinningSettingsMap->setValue( modalityKey + "/Width", binWidth );
			break;
		}
		case ( 1 ) :  // ByWidth
		{
			binSize = ( rangeMax - rangeMin ) / binWidth;
			mBinningSettingsMap->setValue( modalityKey + "/Size", binSize );
			break;
		}
		}
	}

	// TODO: RangeBy changed? Recalculate everything.


	setUpBinningSettingsTable();

	qDebug() << "Binning cell changed!" << endl;

	mIsFeatureVectorDatabaseDirty = true;
	disableTab( 3 );

	canProceed = true;
}

//-----------------------------------------------------------------------------

void TestApplication::openFolderDialog()
{
	QFileDialog dialog;
	dialog.setFileMode( QFileDialog::Directory );
	dialog.setOption( QFileDialog::ShowDirsOnly );
	ui.folderPath->setText( dialog.getExistingDirectory() );

	if ( valid != nullptr ) delete valid;
	valid = new muweval::Validator( ui.folderPath->text(), "timeee" );
	ui.tabWidget->setTabEnabled( mActiveTabPage, true );
	ui.tabWidget->setCurrentIndex( mActiveTabPage );
	doAllValidation();

	delete mSettingsMap;
	delete mSettingsDescriptors;

	mSettingsMap = new QSettings( ui.folderPath->text()  + "/Settings/settings.ini", QSettings::IniFormat );
	mSettingsDescriptors = new QSettings( ui.folderPath->text() + "/Settings/settingsDescriptors.ini", QSettings::IniFormat );
	mBinningSettingsMap = new QSettings( ui.folderPath->text() + "/Settings/binningSettings.ini", QSettings::IniFormat );

	setUpSettingsTable();
	setUpBinningSettingsTable();

	mIsNormalizedImagesDirty = true;
	mIsDelineatedImagesDirty = true;
	mIsFeatureVectorDatabaseDirty = true;

	disableTab( 1 );
}

//-----------------------------------------------------------------------------

void TestApplication::doAllNormalization()
{

	enableTab( mActiveTabPage );
	ui.tabWidget->setCurrentIndex( mActiveTabPage );


	QMap< QString, QString > patientFolders; // = norm->patientFolders();
	for ( int i = 0; i < patientFolders.size(); ++i )
	{
		//ui.patientScrollBar->setValue( i );
		patientChanged( i );
		ui.progressBar->setValue( 100 * float( i + 1 ) / float( patientFolders.size() ) );
		QCoreApplication::processEvents();
	}

	ui.progressBar->setValue( 0 );

	ui.patientScrollBar->blockSignals( true );
	ui.patientScrollBar->setMaximum( patientFolders.size() - 1 );
	ui.patientScrollBar->setValue( patientFolders.size() - 1 );
	ui.patientScrollBar->blockSignals( false );

	mIsNormalizedImagesDirty = false;

	//ui.tabWidget->setTabEnabled( 1, true );
}

//-----------------------------------------------------------------------------

void TestApplication::doAllDelineation()
{

	enableTab( mActiveTabPage );
	ui.tabWidget->setCurrentIndex( mActiveTabPage );

	mIstooLessSamples = false;
	mSampleCountMessage = "";

	QMap< QString, QString > patientFolders = delin->patientFolders();
	for ( int i = 0; i < patientFolders.size(); ++i )
	{
		//ui.patientScrollBar->setValue( i );
		patientChanged( i );
		ui.progressBar->setValue( 100 * float( i + 1 ) / float( patientFolders.size() ) );
		QCoreApplication::processEvents();
		validateVOISampleCounts( i + 1 );
	}

	if ( mIstooLessSamples )
	{
		QMessageBox* message = new QMessageBox();
		message->setWindowTitle( "Error - Not enough samples in VOIs" );
		message->setText( "The following patient VOIs do not contain enough samples for feature extraction: \n\n" + mSampleCountMessage );
		message->show();
		canProceed = false;
	}
	ui.progressBar->setValue( 0 );

	/*ui.patientScrollBar->blockSignals( true );
	ui.patientScrollBar->setValue( patientFolders.size() - 1 );
	ui.patientScrollBar->blockSignals( false );*/

	mIsDelineatedImagesDirty = false;
}

//-----------------------------------------------------------------------------

void TestApplication::doAllFeatureExtraction()
{

	enableTab( mActiveTabPage );
	ui.tabWidget->setCurrentIndex( mActiveTabPage );
	QCoreApplication::processEvents();

	setUpBinningSettingsTable();

	QStringList patientFolders;// = extractor->patientFolders();

	// TODO: Make the program reading up the patient folders upon folder loading. Then already made steps do not need to be repeated. 
	// TODO: Put back the checkboxes on the top!!!

	mFDB.clear();	

#pragma omp parallel for schedule(dynamic, 1)
	for ( int i = 0; i < patientFolders.size(); ++i )
	{
		//muweval::ImageFeatureExtractor extrPerPatient( ui.folderPath->text(), mBinningSettingsMap );
		QString patientSourceFolder = patientFolders.at( i );

		qDebug() << "--------- STARTED: Patient " << i << patientSourceFolder << endl;
		//extrPerPatient.executePatient( patientSourceFolder );
		//lpmldata::TabularData FDBPatient = extrPerPatient.featureDatabase();
		qDebug() << "--------------------- FINISHED: Patient " << i << patientSourceFolder << endl;

#pragma omp critical
		{
			//mFDB.merge( FDBPatient );
		}
	}

	update();

	qDebug() << "Feature extraction execution finished" << endl;

	mIsFeatureVectorDatabaseDirty = false;

	// Save the feature DB
	lpmlfio::TabularDataFileIo FDBFile;

	QString resolution = QString::number( mSettingsMap->value( "Normalization/Resolution" ).toDouble(), 'f', 2 );
	QString FDBFileName = "FDB-" + resolution;

	for ( int i = 0; i < mModalityRectNames.size(); ++i )
	{
		QString binSize = mBinningSettingsMap->value( mModalityRectNames.at( i ) + "/Size" ).toString();
		double rangeMin = mBinningSettingsMap->value( mModalityRectNames.at( i ) + "/RangeMin" ).toDouble();
		double rangeMax = mBinningSettingsMap->value( mModalityRectNames.at( i ) + "/RangeMax" ).toDouble();

		QString modality = mModalityRectNames.at( i ).split( "." ).at( 0 );

		FDBFileName += "-" + modality + "(" + binSize + ", " + QString::number( rangeMin, 'f', 2 ) + "," + QString::number( rangeMax, 'f', 2 ) + " )";
	}
	
	QString FDBFilePath = ui.folderPath->text() + "/L2Features/";
	QDir FDBFilePathDir( FDBFilePath );
	if ( !FDBFilePathDir.exists() )
	{
		QDir().mkdir( FDBFilePath );
	}
	
	qDebug() << "Saving DFB...";
	FDBFile.save( FDBFilePath  + "/" + FDBFileName, mFDB );
	qDebug() << "Saved!";
}

//-----------------------------------------------------------------------------

void TestApplication::validateVOISampleCounts( int aPatient )
{
	
	QMap< QString, lpmldata::Volume >::iterator voiIt;
	for ( voiIt = mVOIs.begin(); voiIt != mVOIs.end(); ++voiIt )
	{
		QString VOIName = voiIt.key();
		VOIName = VOIName.split( "." ).at( 0 );
		lpmldata::Volume VOIVolume = voiIt.value();

		ulint sampleCount = voiSampleCount( VOIVolume );

		if ( sampleCount < mMinimumSampleCount )
		{
			mIstooLessSamples = true;
			
			mSampleCountMessage += "- Patient " + QString::number( aPatient ) + " - " + VOIName + ": " + QString::number( sampleCount ) + "\n";
		}
	}
}

//-----------------------------------------------------------------------------

void TestApplication::activeTabPageChanged( int aValue )
{
	mActiveTabPage = aValue;
	mProcessID = aValue;

	if ( mIsBatchMode ) return;

	switch ( mActiveTabPage )
	{
		case 0:  // Settings
		{
			break;
		}
		case 1:  // Normalization
		{
			if ( mActivePatient != mLastNormalizationPatientIndex && !mIsNormalizationSkipped )
			{
				executeNormalization();
			}
			break;
		}
		case 2:  // Delineation
		{
			if ( mActivePatient != mLastDelineationPatientIndex && !mIsDelineationSkipped )
			{
				executeDelineation();
			}
			break;
		}
		case 3:  // Feature extraction
		{
			break;
		}

		case 4:
		{
			updateFeatureKernelDensity();
			break;
		}
	}
}

//-----------------------------------------------------------------------------

void TestApplication::executeNormalization()
{
	// Read out the active patient folders
	QMap< QString, QString > patientFolders; // = norm->patientFolders();
	QString patientSourceFolder = patientFolders.keys().at( mActivePatient );
	QString patientDestinationFolder = patientFolders.value( patientSourceFolder );
	
	// Create original images.
	norm->setInterpolationMethod( "NN" );
	//norm->executePatient( patientSourceFolder, patientDestinationFolder );
	//mModalityRectsOrig = norm->modalityRects();

	// Create interpolated images.
	//norm->setInterpolationMethod( mSettingsMap->value( "Normalization/Method" ).toInt() ) );
	//norm->executePatient( patientSourceFolder, patientDestinationFolder );
	//mModalityRectsNorm = norm->modalityRects();

	mLastNormalizationPatientIndex = mActivePatient;

	mIsDelineatedImagesDirty = true;
	mIsFeatureVectorDatabaseDirty = true;

	update();
}

//-----------------------------------------------------------------------------

void TestApplication::executeDelineation()
{
	// Read out the active patient folders
	QMap< QString, QString > patientFolders = delin->patientFolders();

	QString patientSourceFolder = patientFolders.keys().at( mActivePatient ); 
	QString patientDestinationFolder = patientFolders.value( patientSourceFolder );

	//delin->executePatient( patientSourceFolder, patientDestinationFolder );

	// Read out the Rect and VOI volumes.
	mModalityRectsDel = delin->modalityRects();
	QMap< QString, double > modalityBcks = delin->modalityBackgrounds();
	mVOIs = delin->VOIs();

	mLastDelineationPatientIndex = mActivePatient;

	// Determine min-max of the VOIs for each modality.
	QStringList rectKeys = mModalityRectsDel.keys();
	QStringList VOIkeys = mVOIs.keys();
	
	bool fuckNegativeValue = false;

	for ( int i = 0; i < rectKeys.size(); ++i )
	{
		for ( int j = 0; j < VOIkeys.size(); ++j )
		{
			QPair <double, double > minMax = mVOIMinMaxes.value( rectKeys.at( i ) );

			lpmldata::Volume maskedVOI = mModalityRectsDel.value( rectKeys.at( i ) );	
			lpmldata::Volume binaryVOI = mVOIs.value( VOIkeys.at( j ) );

			// Optional backgroudn normalization.
			if ( modalityBcks.keys().contains( rectKeys.at( i ) ) )
			{
				maskedVOI.multiply( 1.0 / modalityBcks.value( rectKeys.at( i ) ) );
			}

			// Go through (background normalized) values where the VOI mask is positive.
			lpmldata::Index imageSize = maskedVOI.size();
			for ( ulint k = 0; k < imageSize.at( 0 ) * imageSize.at( 1 ) * imageSize.at( 2 ); ++k )
			{
				if ( binaryVOI.at( k ) > 0.0 )
				{
					minMax.first  = std::min( minMax.first,  double( maskedVOI.at( k ) ) );
					minMax.second = std::max( minMax.second, double( maskedVOI.at( k ) ) );

					if ( double( maskedVOI.at( k ) ) < 0.0 )
					{
						fuckNegativeValue = true;
					}
				}
			}	

			mVOIMinMaxes[ rectKeys.at( i ) ] = minMax;
		}	
	}

	if ( fuckNegativeValue )
	{
		qDebug() << "OOPS! Negative minimum in VOI!: " << patientSourceFolder << endl;
	}

	update();
}

//-----------------------------------------------------------------------------

void TestApplication::executeFeatureExtraction()
{
	if ( !mIsFeatureVectorDatabaseDirty ) return;

	// Read out the active patient folders.
	//QStringList patientFolders = extractor->patientFolders();
	//QString patientSourceFolder = patientFolders.at( mActivePatient );
	//extractor->executePatient( patientSourceFolder );

	update();
}

//-----------------------------------------------------------------------------

void TestApplication::executeAll()
{
	qDebug() << "Execute All!";

	mIsBatchMode = true;

	mProcessID = 0;
	mActiveTabPage = 0;
	mActivePatient = 0;
	canProceed = true;

	for ( int i = mFirstStepToPerform; i < mProcessCount; ++i )
	{
		mProcessID = i;
		if ( canProceed ) proceed();
	}

	mIsBatchMode = false;

}

//-----------------------------------------------------------------------------

void TestApplication::executeOne()
{
	qDebug() << "Execute One!";

	switch ( mActiveTabPage )
	{
		case 0:
		{
			mIsNormalizedImagesDirty = true;
			mIsDelineatedImagesDirty = true;
			mIsFeatureVectorDatabaseDirty = true;
			mIsNormalizationSkipped = false;
			disableTab( 1 );
			break;
		}
		case 1:
		{
			mIsDelineatedImagesDirty = true;
			mIsFeatureVectorDatabaseDirty = true;
			mIsDelineationSkipped = false;
			disableTab( 2 );
			break;
		}
		case 2:
		{
			mIsFeatureVectorDatabaseDirty = true;
			disableTab( 3 );
			break;
		}
	}

	mIsBatchMode = true;

	if ( canProceed )
	{
		proceed();
	}
	else
	{
		qDebug() << "Sorry, cannot proceed!";
	}
	mProcessID = mActiveTabPage;

	mIsBatchMode = false;
}

//-----------------------------------------------------------------------------

void TestApplication::patientChanged( int aValue )
{
	mActivePatient = aValue;
	ui.patientInfoLabel->setText( "Patient " + QString::number( mActivePatient + 1 ) + " / " + QString::number( mPatientCount ) );

	switch ( mActiveTabPage )
	{
		case 0:
		{
			break;
		}
		case 1:
		{
			executeNormalization();
			break;
		}
		case 2:
		{
			executeDelineation();
			break;
		}
		case 3:
		{
			executeFeatureExtraction();
			break;
		}
	}
}

//-----------------------------------------------------------------------------

void TestApplication::sliderchanged( int aValue )
{
	double minimum = ui.sliceSlider->minimum();
	double maximum = ui.sliceSlider->maximum();

	mSliceRatio = ( double( aValue ) - minimum ) / ( maximum - minimum );

	update();
}

//-----------------------------------------------------------------------------

ulint TestApplication::voiSampleCount( lpmldata::Volume& aVoiVolume )
{
	lpmldata::Index imageSize = aVoiVolume.size();
	ulint voxelCount = imageSize.at( 0 ) * imageSize.at( 1 ) * imageSize.at( 2 );
	ulint sampleCount = 0;

	for ( ulint i = 0; i < voxelCount; ++i )
	{
		if ( aVoiVolume.at( i ) > 0.0f )
		{
			++sampleCount;
		}
	}

	return sampleCount;
}

//-----------------------------------------------------------------------------

void TestApplication::update()
{

	switch ( mActiveTabPage )
	{
		case 0:
		{
			break;
		}
		case 1:
		{
			updateNormalization();
			break;
		}
		case 2:
		{
			updateDelineation();
			break;
		}
		case 3:
		{
			updateFeatureExtraction();
			break;
		}
		case 4:
		{
			updateFeatureKernelDensity();
			break;
		}
	}
}

//-----------------------------------------------------------------------------

void TestApplication::updateNormalization()
{
	if ( mModalityRectsNorm.size() == 0 ) return;

	if ( ui.ViewerWidgetNormalization->layout() != NULL )
	{
		QLayoutItem* item;
		while ( ( item = ui.ViewerWidgetNormalization->layout()->takeAt( 0 ) ) != NULL )
		{
			delete item->widget();
			delete item;
		}
		delete ui.ViewerWidgetNormalization->layout();
	}


	int rows = 2;
	int columns = mModalityRectsNorm.size();

	QGridLayout* centralLayout = new QGridLayout();
	centralLayout->setContentsMargins( 0, 0, 0, 0 );


	int maxResHeight = ui.viewerWidgetDelineation->height() / rows;
	int maxResWidth = ui.viewerWidgetDelineation->width() / columns;

	int columnIndex = -1;
	// Original images
	QMap< QString, lpmldata::Volume >::iterator rectIt;
	for ( rectIt = mModalityRectsOrig.begin(); rectIt != mModalityRectsOrig.end(); ++rectIt )
	{
		++columnIndex;

		QString modalityRectName = rectIt.key();
		lpmldata::Volume modalityRectVolume = rectIt.value();

		QLabel* imageLabel = new QLabel( this );
		imageLabel->setPixmap( createNormalizedImage( modalityRectVolume, maxResWidth, maxResHeight ) );
		centralLayout->addWidget( imageLabel, 0, columnIndex );
	}

	columnIndex = -1;
	// Normalized images
	for ( rectIt = mModalityRectsNorm.begin(); rectIt != mModalityRectsNorm.end(); ++rectIt )
	{
		++columnIndex;

		QString modalityRectName = rectIt.key();
		lpmldata::Volume modalityRectVolume = rectIt.value();

		QLabel* imageLabel = new QLabel( this );
		imageLabel->setPixmap( createNormalizedImage( modalityRectVolume, maxResWidth, maxResHeight ) );
		centralLayout->addWidget( imageLabel, 1, columnIndex );
	}


	ui.ViewerWidgetNormalization->setLayout( centralLayout );
}

//-----------------------------------------------------------------------------

void TestApplication::updateDelineation()
{
	if ( mModalityRectsDel.size() == 0 ) return;

	if ( ui.viewerWidgetDelineation->layout() != NULL )
	{
		QLayoutItem* item;
		while ( ( item = ui.viewerWidgetDelineation->layout()->takeAt( 0 ) ) != NULL )
		{
			delete item->widget();
			delete item;
		}
		delete ui.viewerWidgetDelineation->layout();
	}

	int rows = mVOIs.size();
	int columns = mModalityRectsDel.size();

	QGridLayout* centralLayout = new QGridLayout();
	centralLayout->setContentsMargins( 0, 0, 0, 0 );


	int maxResHeight = ui.viewerWidgetDelineation->height() / rows;
	int maxResWidth = ui.viewerWidgetDelineation->width() / columns;

	int rowIndex = -1;	

	QMap< QString, lpmldata::Volume >::iterator voiIt;
	for ( voiIt = mVOIs.begin(); voiIt != mVOIs.end(); ++voiIt )
	{
		++rowIndex;
		int columnIndex = -1;

		QString VOIName = voiIt.key();
		VOIName = VOIName.split( "." ).at( 0 );
		lpmldata::Volume VOIVolume = voiIt.value();

		QMap< QString, lpmldata::Volume >::iterator rectIt;
		for ( rectIt = mModalityRectsDel.begin(); rectIt != mModalityRectsDel.end(); ++rectIt )
		{
			++columnIndex;

			QString modalityRectName = rectIt.key();
			lpmldata::Volume modalityRectVolume = rectIt.value();

			QLabel* imageLabel = new QLabel( this );
			imageLabel->setPixmap( createDelineatedImage( modalityRectVolume, VOIVolume, maxResWidth, maxResHeight ) );
			centralLayout->addWidget( imageLabel, rowIndex, columnIndex );
		}
	}

	ui.viewerWidgetDelineation->setLayout( centralLayout );

}

//-----------------------------------------------------------------------------

void TestApplication::updateFeatureExtraction()
{
	lpmldata::TabularData featureDatabase = mFDB;  //extractor->featureDatabase();
	lpmldata::TabularDataHeader header = mFDB.header();  //featureDatabase.header();
	ui.featureExtractionTable->blockSignals( true );

	ui.featureExtractionTable->clear();
	ui.featureExtractionTable->setRowCount( 0 );
	ui.featureExtractionTable->setColumnCount( 0 );

	// Set up header.
	ui.featureExtractionTable->setColumnCount( header.size() + 1 );
	ui.featureExtractionTable->setHorizontalHeaderItem( 0, new QTableWidgetItem( "Key" ) );


	for ( int headerIndex = 0; headerIndex < header.size(); ++headerIndex )
	{
		QStringList headerColumnDescriptor = header.value( QString::number( headerIndex ) ).toStringList();
		ui.featureExtractionTable->setHorizontalHeaderItem( headerIndex + 1, new QTableWidgetItem( headerColumnDescriptor.at( 0 ) ) );
	}

	// Set up table.
	QStringList keys = featureDatabase.keys();
	for ( int rowIndex = 0; rowIndex < keys.size(); ++rowIndex )
	{
		// Add the key cell.
		ui.featureExtractionTable->insertRow( 0 );
		ui.featureExtractionTable->setItem( 0, 0, new QTableWidgetItem( keys.at( rowIndex ) ) );

		// Add the feature vector cells.
		QVariantList featureVector = featureDatabase.value( keys.at( rowIndex ) );

		for ( int columnIndex = 0; columnIndex < featureDatabase.columnCount(); ++columnIndex )
		{
			QTableWidgetItem *item = new QTableWidgetItem( featureVector.at( columnIndex ).toString() );
			item->setFlags( item->flags() & ~Qt::ItemIsEditable ); // non editable
			ui.featureExtractionTable->setItem( 0, columnIndex + 1, item );
		}	
	}

	ui.featureExtractionTable->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
	ui.featureExtractionTable->blockSignals( false );

	ui.featureExtractionTable->update();
}

//-----------------------------------------------------------------------------

void TestApplication::updateFeatureKernelDensity()
{
	QVector< QRgb > RGBLUT  = { qRgb( 255, 0, 0 ), qRgb( 0, 255, 0 ), qRgb( 0, 0, 255 ) };
	QVector< QRgb > RGBLUT2 = { qRgb( 128, 0, 0 ), qRgb( 0, 128, 0 ), qRgb( 0, 0, 128 ) };

	int fIndex = mActiveFeature;
	int labelIndex = mActiveLabel;
	QStringList labelGroups = mKDExtractor->labelGroups();

	int featureSize = mKDExtractor->featureCount();

	int width = 1000;

	ui.labelFeatureName->setText( mKDExtractor->columnName( fIndex ) );
	ui.labelFeatureIndex->setText( QString::number( fIndex ) );
	ui.labelLabelIndex->setText( QString::number( labelIndex ) );

	double margin = ( mKDExtractor->maximum( fIndex ) - mKDExtractor->minimum( fIndex ) ) / 6.0;

	double min = mKDExtractor->minimum( fIndex ) - margin;
	double max = mKDExtractor->maximum( fIndex ) + margin;

	QImage densityImage( width + 20, 500, QImage::Format_RGB32 );
	densityImage.fill( Qt::GlobalColor::black );

	int maxSize = 0;

	QString outcomes;
	double maxDensity = 0.0;
	
	for ( int labelOutcomeIndex = 0; labelOutcomeIndex < labelGroups.size(); ++labelOutcomeIndex )
	{
		outcomes += labelGroups.at( labelOutcomeIndex ) + " ";

		ui.labelLabelOutcomes->setText( outcomes );

		lpmleval::FeatureKernel* kernel = mKDExtractor->kernel( labelOutcomeIndex, fIndex );

		QVector< double > KDE = kernel->render( min, max );

		for ( int i = 0; i < width; ++i )
		{
			QRgb color = RGBLUT.at( labelOutcomeIndex );

			int indexNorm = int( double( i ) * ( double( KDE.size() ) / double( width ) ) );
			densityImage.setPixel( i + 10, 489 - ( KDE.at( indexNorm ) * 479 ), color );
			densityImage.setPixel( i + 9, 489 - ( KDE.at( indexNorm ) * 479 ), color );
			densityImage.setPixel( i + 10, 488 - ( KDE.at( indexNorm ) * 478 ), color );
			densityImage.setPixel( i + 9, 488 - ( KDE.at( indexNorm ) * 478 ), color );
		}

		for ( int i = 0; i < width; ++i )
		{
			QRgb color = qRgb( 255, 255, 255 );

			densityImage.setPixel( i + 10, 489, color );
		}
	}

	ui.labelKernelDensity->setPixmap( QPixmap::fromImage( densityImage ) );

}

//-----------------------------------------------------------------------------

void TestApplication::setUpFeatureKernelPage()
{
	ui.scrollBarFeatures->setMaximum( 65 );
	ui.scrollBarLabels->setMaximum( 4 );

}

//-----------------------------------------------------------------------------

void TestApplication::featureChanged( int aFeatureIndex )
{
	mActiveFeature = aFeatureIndex;
	update();
}

//-----------------------------------------------------------------------------

void TestApplication::labelChanged( int aLabelIndex )
{
	mActiveLabel = aLabelIndex;
	if ( mKDExtractor != nullptr ) delete mKDExtractor;
	mKDExtractor = new lpmleval::KernelDensityExtractor( mFDBLoaded, mLDBLoaded, mActiveLabel );

	QMap< double, int > overlapR = mKDExtractor->overlapRatios();

	update();
}

//-----------------------------------------------------------------------------

QPixmap TestApplication::createDelineatedImage( lpmldata::Volume& aRectVolume, lpmldata::Volume& aVOIVolume, int aMaximumWidth, int aMaximumHeight )
{
	lpmldata::Index rectVolSize = aRectVolume.size();

	float min = aRectVolume.minimum();
	float max = aRectVolume.maximum();

	QImage origImage( rectVolSize.at( 0 ), rectVolSize.at( 1 ), QImage::Format_RGB32 );

	for ( int i = 0; i < rectVolSize.at( 0 ); ++i )
	{
		for ( int j = 0; j < rectVolSize.at( 1 ); ++j )
		{
			int volumeValue = int( ( 255.0f * ( aRectVolume.at( i, j, lint( ( rectVolSize.at( 2 ) - 1 ) * mSliceRatio ) ) - min ) ) / ( max - min ) );
			int VOIValue = int( aVOIVolume.at( i, j, lint( ( rectVolSize.at( 2 ) - 1 ) * mSliceRatio ) ) );
			QRgb value;

			if ( VOIValue == 0 ) // Outside of VOI
			{
				value = qRgb( std::min( 255, volumeValue + 30 ), volumeValue, volumeValue );
			}
			else
			{
				value = qRgb( volumeValue, volumeValue, volumeValue );
			}

			origImage.setPixel( i, j, value );
		}
	}

	float canvasRatio = float( aMaximumWidth ) / float( aMaximumHeight );
	float imageRatio = float( origImage.width() ) / float( origImage.height() );

	if ( canvasRatio > imageRatio )
	{
		origImage = origImage.scaledToHeight( aMaximumHeight );
	}
	else
	{
		origImage = origImage.scaledToWidth( aMaximumWidth );
	}
	

	return QPixmap::fromImage( origImage );

}

//-----------------------------------------------------------------------------

QPixmap TestApplication::createNormalizedImage( lpmldata::Volume& aRectVolume, int aMaximumWidth, int aMaximumHeight )
{
	lpmldata::Index rectVolSize = aRectVolume.size();

	float min = aRectVolume.minimum();
	float max = aRectVolume.maximum();

	//min = min + ( 0.5 * ( max - min ) );
	//max = max + ( 0.1 * ( max - min ) );

	QImage origImage( rectVolSize.at( 0 ), rectVolSize.at( 1 ), QImage::Format_RGB32 );

	for ( int i = 0; i < rectVolSize.at( 0 ); ++i )
	{
		for ( int j = 0; j < rectVolSize.at( 1 ); ++j )
		{
			float volOrig = std::max( aRectVolume.at( i, j, lint( ( rectVolSize.at( 2 ) - 1 ) * mSliceRatio ) ), min );
			volOrig = std::min( volOrig, max );

			int volumeValue = int( ( 255.0f * ( volOrig - min ) ) / ( max - min ) );
			QRgb value = qRgb( volumeValue, volumeValue, volumeValue );
			origImage.setPixel( i, j, value );
		}
	}

	float canvasRatio = float( aMaximumWidth ) / float( aMaximumHeight );
	float imageRatio = float( origImage.width() ) / float( origImage.height() );

	if ( canvasRatio > imageRatio )
	{
		origImage = origImage.scaledToHeight( aMaximumHeight );
	}
	else
	{
		origImage = origImage.scaledToWidth( aMaximumWidth );
	}


	return QPixmap::fromImage( origImage );

}

//-----------------------------------------------------------------------------

TestApplication::~TestApplication()
{
	delete valid;
	delete norm;
	delete delin;
	delete extractor;
}

//-----------------------------------------------------------------------------