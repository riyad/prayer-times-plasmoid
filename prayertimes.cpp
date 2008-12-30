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
#include <marble/MarbleWidget.h>

PrayerTimes::PrayerTimes(QObject *parent, const QVariantList &args)
	: Plasma::Applet(parent, args),
	m_kaabaSvg(this),
	m_town("Makkah"),
	m_latitude(21.416667), m_longitude(39.816667), // Makkah
	m_calculationMethod(5), // Muslim World League
	m_map(0)
{
	// this will get us the standard applet background, for free!
	setBackgroundHints(DefaultBackground);

	m_kaabaSvg.setImagePath("widgets/prayertimes");
	m_kaabaSvg.setContainsMultipleImages(false);

	setHasConfigurationInterface(true);

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
	ui.latitudeLineEdit->setText(QString("%1").arg(m_latitude));
	ui.longitudeLineEdit->setText(QString("%1").arg(m_longitude));

	m_map = new Marble::MarbleWidget(parent);
	parent->addPage(m_map, i18n("Map"), "marble");
	m_map->setProjection(Marble::Equirectangular);
	//Set how we want the map to look
	m_map->centerOn(m_longitude, m_latitude);
	m_map->zoomView(m_map->maximumZoom());
	m_map->setMapThemeId( "earth/atlas/atlas.dgml" );
	m_map->setShowGrid       ( true );
	m_map->setShowPlaces     ( true );
	m_map->setShowOtherPlaces( false );
	//m_map->setShowBorders    ( true );
	m_map->setShowCities     ( true );
	m_map->setShowCompass    ( false );
	m_map->setShowCrosshairs ( true );
	m_map->setShowScaleBar   ( false );
	m_map->setShowClouds     ( false );

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

// 	double latitude = ui.latitudeLineEdit->text().toDouble();
	double latitude = m_map->centerLatitude();
	if(m_latitude != latitude) {
		m_latitude = latitude;
		cg.writeEntry("latitude", m_latitude);
	}

// 	double longitude = ui.longitudeLineEdit->text().toDouble();
	double longitude = m_map->centerLongitude();
	if(m_longitude != longitude) {
		m_longitude = longitude;
		cg.writeEntry("longitude", m_longitude);
	}

	connectSources();

	emit configNeedsSaving();
}

void PrayerTimes::paintInterface(QPainter *p,
        const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
	QRect iconRect(contentsRect);
	iconRect.setWidth(contentsRect.width()/2);
	QRect labelsRect(contentsRect);
	labelsRect.setLeft(contentsRect.width()/2 + 0*contentsRect.width()/4);
	labelsRect.setWidth(contentsRect.width()/4);
	QRect timesRect(contentsRect);
	timesRect.setLeft(contentsRect.width()/2 + 1*contentsRect.width()/4);
	timesRect.setWidth(contentsRect.width()/4);

	p->setRenderHint(QPainter::SmoothPixmapTransform);
	p->setRenderHint(QPainter::Antialiasing);

	// Now we draw the applet, starting with our svg
	int kaabaSize = qMin(iconRect.width(), iconRect.height());
	m_kaabaSvg.resize(kaabaSize, kaabaSize);
	m_kaabaSvg.paint(p, iconRect.left(), iconRect.top(), "kaaba");

	p->save();

	p->setPen(Qt::white);

	QTextOption labelsTextOption(Qt::AlignRight | Qt::AlignHCenter);
	//textOptions.setWrapMode(QTextOption::WordWrap);
	p->drawText(labelsRect,
		i18n("Fajr:\nShorooq:\nDhuhr:\nAsr:\nMaghrib:\nIshaa:"),
		labelsTextOption);

	QTextOption timesTextOption(Qt::AlignLeft | Qt::AlignHCenter);
	//textOptions.setWrapMode(QTextOption::WordWrap);
	p->drawText(timesRect,
		QString("%1\n%2\n%3\n%4\n%5\n%6")
			.arg(m_fajr.toString())
			.arg(m_shorooq.toString())
			.arg(m_dhuhr.toString())
			.arg(m_asr.toString())
			.arg(m_maghrib.toString())
			.arg(m_ishaa.toString()),
		timesTextOption);

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

#include "prayertimes.moc"
