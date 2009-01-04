// mine
#include "prayertimes.h"

#include "QiblaGraphicsWidget.h"
#include "dataengine/prayertimesengine.h"

// Qt
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QPainter>
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
	setMinimumSize(320, 170);
	resize(350, 170);
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

	connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(updateColors()));

	m_updateTimer = new QTimer(this);
	connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(repaintNeeded()));
	m_updateTimer->start(60*1000);

	connectSources();

	Plasma::TabBar* tabbar = new Plasma::TabBar(this);
	tabbar->addTab("Times", prayerTimesWidget());
	tabbar->addTab("Qibla", qiblaWidget());

	QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);
	layout->addItem(tabbar);
	setLayout(layout);

	resize(layout->preferredSize());

	repaintNeeded();
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

	update();
}

void PrayerTimes::createConfigurationInterface(KConfigDialog* parent)
{
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);

	parent->addPage(widget, i18n("Location"), "marble");
	ui.locationNameLineEdit->setText(m_locationName);

	Marble::MarbleWidget* map = ui.mapWidget;
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

	connect(ui.mapWidget, SIGNAL(mouseMoveGeoPosition(QString)), this, SLOT(configMouseGeoPositionChanged()));
	connect(ui.mapWidget, SIGNAL(zoomChanged(int)), this, SLOT(configMouseGeoPositionChanged()));

	connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
	connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
}

void PrayerTimes::configAccepted()
{
	disconnectSources();

	KConfigGroup cg = config();

	QString locationName = ui.locationNameLineEdit->text();
	if(m_locationName != locationName) {
		m_locationName = locationName;
		cg.writeEntry("locationName", m_locationName);
	}

	double latitude = ui.mapWidget->centerLatitude();
	if(m_latitude != latitude) {
		m_latitude = latitude;
		cg.writeEntry("latitude", m_latitude);
	}

	double longitude = ui.mapWidget->centerLongitude();
	if(m_longitude != longitude) {
		m_longitude = longitude;
		cg.writeEntry("longitude", m_longitude);
	}

	connectSources();

	emit configNeedsSaving();
}

void PrayerTimes::configMouseGeoPositionChanged()
{
	Marble::MarbleWidget* map = ui.mapWidget;
	double lon = ui.mapWidget->centerLongitude();
	double lat = ui.mapWidget->centerLatitude();
	int x, y;
	map->screenCoordinates(lon, lat, x, y);
	QVector<QPersistentModelIndex> featureList = map->model()->whichFeatureAt(QPoint(x, y));
	if(!featureList.isEmpty()) {
		ui.locationNameLineEdit->setText(featureList.at(0).data().toString());
	}
}

void PrayerTimes::repaintNeeded()
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
	m_nextPrayerLabel->setText(QString("%1 to %2").arg(nextPrayerTime.toString("hh:mm")).arg(labelFor(nextPrayer() == NextFajr ? Fajr : nextPrayer())));
	m_locationLabel->setText(QString("Prayer times for %1 on %2").arg(m_locationName).arg(QDate::currentDate().toString()));
}

QGraphicsWidget* PrayerTimes::prayerTimesWidget()
{
	QGraphicsGridLayout* layout = new QGraphicsGridLayout();
	layout->setVerticalSpacing(0);

	layout->setColumnStretchFactor(0, 2);
	layout->setColumnStretchFactor(1, 1);
	layout->setColumnStretchFactor(2, 1);

	Plasma::IconWidget* kaabaIconWidget = new Plasma::IconWidget(this);
	kaabaIconWidget->setSvg(m_prayertimesSvg->imagePath(), "kaaba");
	layout->addItem(kaabaIconWidget, 0, 0, 6, 1);

	for(int prayer = Fajr; prayer <= Ishaa; ++prayer) {
		Plasma::Label *prayerLabel = new Plasma::Label(this);
		prayerLabel->setAlignment(Qt::AlignRight);
		prayerLabel->setText(labelFor(prayer));
		layout->addItem(prayerLabel, prayer, 1);
		m_prayerLabels.append(prayerLabel);

		Plasma::Label *prayerTimesLabel = new Plasma::Label(this);
		//prayerTimesLabel->setAlignment(Qt::AlignLeft);
		prayerTimesLabel->setText("%1");
		layout->addItem(prayerTimesLabel, prayer, 2);
		m_prayerTimeLabels.append(prayerTimesLabel);
	}

	m_nextPrayerLabel = new Plasma::Label(this);
	m_nextPrayerLabel->setAlignment(Qt::AlignCenter);
	m_nextPrayerLabel->setText(i18n("%1 to %2"));
	layout->addItem(m_nextPrayerLabel, 6, 0, 1, 3);

	m_locationLabel = new Plasma::Label(this);
	m_locationLabel->setAlignment(Qt::AlignCenter);
	m_locationLabel->setText(i18n("Prayer times for %1 on %2"));
	layout->addItem(m_locationLabel, 7, 0, 1, 3);

	QGraphicsWidget* widget = new QGraphicsWidget(this);
	widget->setLayout(layout);
	return widget;
}

QGraphicsWidget* PrayerTimes::qiblaWidget()
{
	QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Horizontal);

	Plasma::IconWidget* kaabaIconWidget = new Plasma::IconWidget(this);
	kaabaIconWidget->setSvg(m_prayertimesSvg->imagePath(), "kaaba");
	layout->addItem(kaabaIconWidget);

	QiblaGraphicsWidget* m_qiblaWidget = new QiblaGraphicsWidget(this);
	layout->addItem(m_qiblaWidget);

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

QString PrayerTimes::sourceName()
{
	return QString("%1,%2").arg(m_latitude).arg(m_longitude);
}

const int PrayerTimes::currentPrayer()
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

const int PrayerTimes::nextPrayer()
{
	return (currentPrayer()+1)%PRAYER_TIMES;
}

const QTime& PrayerTimes::prayerTimeFor(int prayer)
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
