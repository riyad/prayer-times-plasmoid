// Own
#include "prayertimes.h"

#include "compassgraphicswidget.h"
#include "dataengine/prayertimesengine.h"

// Qt
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QString>
#include <QTextOption>
#include <QTime>

// KDE
#include <KConfigDialog>
#include <KIcon>
#include <KLocale>

// Plasma
#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include <Plasma/IconWidget>
#include <Plasma/Label>
#include <Plasma/Svg>
#include <Plasma/TabBar>
#include <Plasma/Theme>

// Marble
#include <marble/MarbleModel.h>
#include <marble/MarbleWidget.h>

PrayerTimes::PrayerTimes(QObject *parent, const QVariantList &args)
	: Plasma::Applet(parent, args),
	m_prayertimesSvg(0),
	m_locationName("Makkah"),
	m_latitude(21.416667), m_longitude(39.816667), // Makkah
	m_calculationMethod(5), // Muslim World League
	m_updateTimer(0)
{
	// this will get us the standard applet background, for free!
	setBackgroundHints(DefaultBackground);

	m_prayertimesSvg = new Plasma::Svg(this);
	m_prayertimesSvg->setImagePath("widgets/prayertimes");
	m_prayertimesSvg->setContainsMultipleImages(true);

	setHasConfigurationInterface(true);

	setAspectRatioMode(Plasma::IgnoreAspectRatio);
	setMinimumSize(270, 150);

	Plasma::TabBar* tabbar = new Plasma::TabBar(this);
	tabbar->addTab("Times", createPrayerTimesWidget());
	tabbar->addTab("Qibla", createQiblaWidget());

	QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);
	layout->addItem(tabbar);
	setLayout(layout);

	resize(layout->preferredSize());
}


PrayerTimes::~PrayerTimes()
{
	if (hasFailedToLaunch()) {
		// Do some cleanup here
	} else {
		// Save settings
	}

	m_updateTimer->stop();
}

void PrayerTimes::init()
{
	KConfigGroup cg = config();
	m_locationName = cg.readEntry("locationName", m_locationName);
	m_latitude = cg.readEntry("latitude", m_latitude);
	m_longitude = cg.readEntry("longitude", m_longitude);
	m_calculationMethod = cg.readEntry("calculationMethod", m_calculationMethod);

	connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(updateColors()));

	m_updateTimer = new QTimer(this);
	connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateInterface()));
	m_updateTimer->start(60*1000);

	connectSources();
}

void PrayerTimes::dataUpdated(const QString &source, const Plasma::DataEngine::Data &data)
{
	Q_UNUSED(source);

	m_fajr = data["Fajr"].toTime();
	m_shorooq = data["Shorooq"].toTime();
	m_dhuhr = data["Dhuhr"].toTime();
	m_asr = data["Asr"].toTime();
	m_maghrib = data["Maghrib"].toTime();
	m_ishaa = data["Ishaa"].toTime();
	m_nextFajr = data["NextFajr"].toTime();

	m_qibla = data["Qibla"].toDouble();

	updateInterface();
}

void PrayerTimes::createConfigurationInterface(KConfigDialog* parent)
{
	QWidget *locationWidget = new QWidget(parent);
	locationConfigUi.setupUi(locationWidget);

	parent->addPage(locationWidget, i18n("Location"), "marble");
	locationConfigUi.locationNameLineEdit->setText(m_locationName);

	Marble::MarbleWidget* map = locationConfigUi.mapWidget;
	map->setProjection(Marble::Spherical);
	//Set how we want the map to look
	map->centerOn(m_longitude, m_latitude);
	map->zoomView(map->maximumZoom());
	map->setMapThemeId( "earth/atlas/atlas.dgml" );
	map->setShowGrid       ( true );
	map->setShowPlaces     ( true );
	map->setShowOtherPlaces( false );
	//map->setShowBorders    ( true );
	map->setShowCities     ( true );
	map->setShowCompass    ( false );
	map->setShowCrosshairs ( true );
	map->setShowScaleBar   ( false );
	map->setShowClouds     ( false );

	connect(locationConfigUi.mapWidget, SIGNAL(mouseMoveGeoPosition(QString)), this, SLOT(configMouseGeoPositionChanged()));
	connect(locationConfigUi.mapWidget, SIGNAL(zoomChanged(int)), this, SLOT(configMouseGeoPositionChanged()));

	QWidget *methodWidget = new QWidget(parent);
	calculationMethodConfigUi.setupUi(methodWidget);

	parent->addPage(methodWidget, i18n("Calculation Method"), "prayertimes");
	calculationMethodConfigUi.methodComboBox->setCurrentIndex(m_calculationMethod);

	connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
	connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
}

void PrayerTimes::configAccepted()
{
	disconnectSources();

	KConfigGroup cg = config();

	QString locationName = locationConfigUi.locationNameLineEdit->text();
	if(m_locationName != locationName) {
		m_locationName = locationName;
		cg.writeEntry("locationName", m_locationName);
	}

	double latitude = locationConfigUi.mapWidget->centerLatitude();
	if(m_latitude != latitude) {
		m_latitude = latitude;
		cg.writeEntry("latitude", m_latitude);
	}

	double longitude = locationConfigUi.mapWidget->centerLongitude();
	if(m_longitude != longitude) {
		m_longitude = longitude;
		cg.writeEntry("longitude", m_longitude);
	}

	double method = calculationMethodConfigUi.methodComboBox->currentIndex();
	if(m_calculationMethod != method) {
		m_calculationMethod = method;
		cg.writeEntry("calculationMethod", m_calculationMethod);
	}

	connectSources();

	updateInterface();

	emit configNeedsSaving();
}

void PrayerTimes::configMouseGeoPositionChanged()
{
	Marble::MarbleWidget* map = locationConfigUi.mapWidget;
	double lon = locationConfigUi.mapWidget->centerLongitude();
	double lat = locationConfigUi.mapWidget->centerLatitude();
	int x, y;
	map->screenCoordinates(lon, lat, x, y);
	QVector<QModelIndex> featureList = map->model()->whichFeatureAt(QPoint(x, y));
	if(!featureList.isEmpty()) {
		locationConfigUi.locationNameLineEdit->setText(featureList.at(0).data().toString());
	}
}

void PrayerTimes::updateInterface()
{
	QFont normalFont(font());
	QFont boldFont(font());
	boldFont.setBold(true);

	for(int prayer = Fajr; prayer <= Ishaa; ++prayer) {
		m_prayerTimeLabels[prayer]->setText(prayerTimeFor(prayer).toString("hh:mm"));
		QFont* prayerFont = 0;
		if(prayer != Shorooq && prayer == currentPrayer()) {
			prayerFont = &boldFont;
		} else {
			prayerFont = &normalFont;
		}
		static_cast<QLabel*>(m_prayerLabels[prayer]->widget())->setFont(*prayerFont);
		static_cast<QLabel*>(m_prayerTimeLabels[prayer]->widget())->setFont(*prayerFont);
	}
	int diffMSecs = QTime::currentTime().msecsTo(prayerTimeFor(nextPrayer()));
	QTime nextPrayerTime = QTime().addMSecs(diffMSecs);
	m_nextPrayerLabel->setText(i18nc("<time> to <prayer>", "%1 to %2", nextPrayerTime.toString("hh:mm"), labelFor(nextPrayer() == NextFajr ? Fajr : nextPrayer())));
	m_locationLabel->setText(i18nc("Prayer times for <town> on <date>", "Prayer times for %1 on %2", m_locationName, QDate::currentDate().toString()));

	m_qiblaWidget->setNeedle(m_qibla);
	m_qiblaOrientationLabel->setText(i18nc("Qibla direction is <orientation>", "Qibla direction is %1", m_qiblaWidget->needleOrientation()));
}

QGraphicsWidget* PrayerTimes::createPrayerTimesWidget()
{
	QGraphicsGridLayout* layout = new QGraphicsGridLayout();
	layout->setVerticalSpacing(0);

	layout->setColumnStretchFactor(0, 2);
	layout->setColumnStretchFactor(1, 1);
	layout->setColumnStretchFactor(2, 1);

	m_locationLabel = new Plasma::Label(this);
	m_locationLabel->setAlignment(Qt::AlignCenter);
	m_locationLabel->setText(QString("Prayer times for %1 on %2"));
	layout->addItem(m_locationLabel, 0, 0, 1, 3);

	Plasma::IconWidget* kaabaIconWidget = new Plasma::IconWidget(this);
	kaabaIconWidget->setSvg(m_prayertimesSvg->imagePath(), "kaaba");
	layout->addItem(kaabaIconWidget, 1, 0, 5, 1);

	for(int prayer = Fajr; prayer <= Ishaa; ++prayer) {
		Plasma::Label *prayerLabel = new Plasma::Label(this);
		prayerLabel->setAlignment(Qt::AlignRight | Qt::AlignHCenter);
		prayerLabel->setText(labelFor(prayer));
		layout->addItem(prayerLabel, 1+prayer, 1);
		m_prayerLabels.append(prayerLabel);

		Plasma::Label *prayerTimesLabel = new Plasma::Label(this);
		prayerTimesLabel->setAlignment(Qt::AlignLeft | Qt::AlignHCenter);
		prayerTimesLabel->setText("%1");
		layout->addItem(prayerTimesLabel, 1+prayer, 2);
		m_prayerTimeLabels.append(prayerTimesLabel);
	}

	m_nextPrayerLabel = new Plasma::Label(this);
	m_nextPrayerLabel->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	m_nextPrayerLabel->setText(QString("%1 to %2"));
	layout->addItem(m_nextPrayerLabel, 6, 0, 1, 1);

	QGraphicsWidget* widget = new QGraphicsWidget(this);
	widget->setLayout(layout);
	return widget;
}

QGraphicsWidget* PrayerTimes::createQiblaWidget()
{
	QGraphicsGridLayout* layout = new QGraphicsGridLayout();
	layout->setRowStretchFactor(0, 1);
	layout->setRowStretchFactor(1, 6);

	m_qiblaOrientationLabel = new Plasma::Label(this);
	m_qiblaOrientationLabel->setAlignment(Qt::AlignCenter);
	m_qiblaOrientationLabel->setText(QString("Qibla direction is %1"));
	layout->addItem(m_qiblaOrientationLabel, 0, 0, 1, 2);

	Plasma::IconWidget* kaabaIconWidget = new Plasma::IconWidget(this);
	kaabaIconWidget->setSvg(m_prayertimesSvg->imagePath(), "kaaba");
	layout->addItem(kaabaIconWidget, 1, 0);

	m_qiblaWidget = new CompassGraphicsWidget(this);
	layout->addItem(m_qiblaWidget, 1, 1);

	QGraphicsWidget* widget = new QGraphicsWidget(this);
	widget->setLayout(layout);
	return widget;
}


void PrayerTimes::connectSources()
{
	Plasma::DataEngine* prayerTimesEngine;
	prayerTimesEngine = dataEngine("prayertimes");
	prayerTimesEngine->connectSource(sourceName(), this, 1000*60, Plasma::AlignToMinute);
}

void PrayerTimes::disconnectSources()
{
	Plasma::DataEngine* prayerTimesEngine;
	prayerTimesEngine = dataEngine("prayertimes");
	prayerTimesEngine->disconnectSource(sourceName(), this);
}

const QString PrayerTimes::sourceName() const
{
	return QString("%1/%2,%3").arg(calculationMethodName[m_calculationMethod]).arg(m_latitude).arg(m_longitude);
}

int PrayerTimes::currentPrayer() const
{
	for(int prayer=Fajr; prayer <= Ishaa; ++prayer) {
		if(prayerTimeFor(prayer) <= QTime::currentTime() && QTime::currentTime() < prayerTimeFor((prayer+1)%PRAYER_TIMES)) {
			return prayer;
		}
	}
	return Ishaa;
}

const QString& PrayerTimes::labelFor(int prayer)
{
	static const QString labels[PRAYER_TIMES] = {i18n("Fajr"),
		i18n("Shorooq"),
		i18n("Dhuhr"),
		i18n("Asr"),
		i18n("Maghrib"),
		i18n("Ishaa"),
		i18n("Next Fajr")};

	return labels[prayer];
}

int PrayerTimes::nextPrayer() const
{
	return (currentPrayer()+1)%PRAYER_TIMES;
}

const QTime& PrayerTimes::prayerTimeFor(int prayer) const
{
	const QTime* times[PRAYER_TIMES] = {&m_fajr,
		&m_shorooq,
		&m_dhuhr,
		&m_asr,
		&m_maghrib,
		&m_ishaa,
		&m_nextFajr};

	return *times[prayer];
}

#include "prayertimes.moc"
