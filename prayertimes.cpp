// mine
#include "prayertimes.h"

// Qt
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
#include <Plasma/Svg>
#include <Plasma/Theme>

// Marble
#include <marble/MarbleModel.h>
#include <marble/MarbleWidget.h>

PrayerTimes::PrayerTimes(QObject *parent, const QVariantList &args)
	: Plasma::Applet(parent, args),
	m_kaabaSvg(this),
	m_town("Makkah"),
	m_latitude(21.416667), m_longitude(39.816667), // Makkah
	m_calculationMethod(5) // Muslim World League
{
	// this will get us the standard applet background, for free!
	setBackgroundHints(DefaultBackground);

	m_kaabaSvg.setImagePath("widgets/prayertimes");
	m_kaabaSvg.setContainsMultipleImages(false);

	setHasConfigurationInterface(true);

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
}

void PrayerTimes::init()
{
	KConfigGroup cg = config();
	m_town = cg.readEntry("town", m_town);
	m_latitude = cg.readEntry("latitude", m_latitude);
	m_longitude = cg.readEntry("longitude", m_longitude);

	connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(updateColors()));

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

	m_qibla = data["Qibla"].toDouble();
}

void PrayerTimes::createConfigurationInterface(KConfigDialog* parent)
{
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);

	parent->addPage(widget, i18n("Location"), "marble");
	ui.townLineEdit->setText(m_town);

	Marble::MarbleWidget* map = ui.mapWidget;
	map->setProjection(Marble::Equirectangular);
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

	connect(ui.mapWidget, SIGNAL(mouseMoveGeoPosition(QString)), this, SLOT(configMouseMoveGeoPosition(QString)));

	connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
	connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
}

void PrayerTimes::configAccepted()
{
	disconnectSources();

	KConfigGroup cg = config();

	QString town = ui.townLineEdit->text();
	if(m_town != town) {
		m_town = town;
		cg.writeEntry("town", m_town);
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

void PrayerTimes::configMouseMoveGeoPosition(QString geopos) {
	Marble::MarbleWidget* map = ui.mapWidget;
	double lon = ui.mapWidget->centerLongitude();
	double lat = ui.mapWidget->centerLatitude();
	int x, y;
	map->screenCoordinates(lon, lat, x, y);
	QVector<QPersistentModelIndex> featureList = map->model()->whichFeatureAt(QPoint(x, y));
	if(!featureList.isEmpty()) {
		ui.townLineEdit->setText(featureList.at(0).data().toString());
	}
}

void PrayerTimes::paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
	int fontSize = 20;

	QRect iconRect(contentsRect);
	iconRect.setWidth(contentsRect.width()/2);
	iconRect.setHeight(contentsRect.height() - fontSize);
	QRect labelsRect(contentsRect);
	labelsRect.setLeft(contentsRect.width()/2 + 0*contentsRect.width()/4);
	labelsRect.setWidth(contentsRect.width()/4);
	labelsRect.setHeight(contentsRect.height() - fontSize);
	QRect timesRect(contentsRect);
	timesRect.setLeft(contentsRect.width()/2 + 1*contentsRect.width()/4);
	timesRect.setWidth(contentsRect.width()/4);
	timesRect.setHeight(contentsRect.height() - fontSize);

	p->setRenderHint(QPainter::SmoothPixmapTransform);
	p->setRenderHint(QPainter::Antialiasing);

	// Now we draw the applet, starting with our svg
	int kaabaSize = qMin(iconRect.width(), iconRect.height());
	m_kaabaSvg.resize(kaabaSize, kaabaSize);
	m_kaabaSvg.paint(p, iconRect.left(), iconRect.top(), "kaaba");

	p->save();

	p->setPen(Qt::white);

	QTextOption labelsTextOption(Qt::AlignRight | Qt::AlignHCenter);
	QTextOption timesTextOption(Qt::AlignLeft | Qt::AlignHCenter);

	QFont normalFont(p->font());
	QFont boldFont(p->font());
	boldFont.setBold(true);

	for(int prayer=Fajr; prayer < NextFajr; ++prayer) {
		if(prayerTimeFor(prayer) <= QTime::currentTime() && QTime::currentTime() < prayerTimeFor((prayer+1)%NextFajr)) {
			p->setFont(boldFont);
		} else {
			p->setFont(normalFont);
		}

		QRect labelRect(labelsRect);
		labelRect.setTop(labelsRect.top() + prayer*labelsRect.height()/6);
		labelRect.setHeight(labelsRect.height()/6);
		p->drawText(labelRect,
			QString("%1:").arg(labelFor(prayer)),
			labelsTextOption);

		QRect timeRect(timesRect);
		timeRect.setTop(timesRect.top() + prayer*timesRect.height()/6);
		timeRect.setHeight(timesRect.height()/6);
		p->drawText(timeRect,
			QString("%1").arg(prayerTimeFor(prayer).toString()),
			timesTextOption);
	}

	p->setFont(normalFont);

	QTextOption townTextOption(Qt::AlignCenter | Qt::AlignBottom);
	//townTextOption.setWrapMode(QTextOption::WordWrap);
	p->drawText(contentsRect,
		i18n("Prayer times for %1 on %2").arg(m_town).arg(QDate::currentDate().toString()),
		townTextOption);

	p->restore();
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

const int PrayerTimes::currentPrayer() {

}

const QString& PrayerTimes::labelFor(int prayer)
{
	static const QString labels[7] = {i18n("Fajr"),
		i18n("Shorooq"),
		i18n("Dhuhr"),
		i18n("Asr"),
		i18n("Maghrib"),
		i18n("Ishaa"),
		i18n("Next Fajr")};

	return labels[prayer];
}

const QTime& PrayerTimes::prayerTimeFor(int prayer)
{
	const QTime* times[7] = {&m_fajr,
		&m_shorooq,
		&m_dhuhr,
		&m_asr,
		&m_maghrib,
		&m_ishaa,
		&m_nextFajr};

	return *times[prayer];
}

#include "prayertimes.moc"
