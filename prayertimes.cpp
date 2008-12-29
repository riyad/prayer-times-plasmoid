#include "prayertimes.h"

#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QTextOption>

#include <KLocale>

#include <Plasma/Theme>

PrayerTimes::PrayerTimes(QObject *parent, const QVariantList &args)
	: Plasma::Applet(parent, args),
	m_kaabaSvg(this),
	m_latitude(53.01),
	m_longitude(8.8),
	m_calculationMethod(5)
{
	// this will get us the standard applet background, for free!
	setBackgroundHints(DefaultBackground);
	m_kaabaSvg.setImagePath("widgets/prayertimes");
	m_kaabaSvg.setContainsMultipleImages(false);
	setHasConfigurationInterface(true);
	resize(300, 150);
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
	Plasma::DataEngine* prayerTimesEngine;
	prayerTimesEngine = dataEngine("prayertimes");

	prayerTimesEngine->setProperty("latitude", m_latitude);
	prayerTimesEngine->setProperty("longitude", m_longitude);

	prayerTimesEngine->connectSource(locationCoords(), this, 1000*60, Plasma::AlignToMinute);
	connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(updateColors()));
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

	p->restore();
}

QString PrayerTimes::locationCoords()
{
	return QString("%1,%2").arg(m_latitude).arg(m_longitude);
}

#include "prayertimes.moc"
