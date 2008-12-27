#include "prayertimes.h"

#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QTextOption>

#include <KLocale>

#include <Plasma/Theme>

PrayerTimes::PrayerTimes(QObject *parent, const QVariantList &args)
	: Plasma::Applet(parent, args),
	m_svg(this),
	m_icon("document"),
	m_latitude(53.01),
	m_longitude(8.8),
	m_calculationMethod(5)
{
    // this will get us the standard applet background, for free!
    setBackgroundHints(DefaultBackground);
    m_svg.setImagePath("widgets/background");
    setHasConfigurationInterface(true);  
    resize(200, 200);
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
    // A small demonstration of the setFailedToLaunch function
    if (m_icon.isNull()) {
        setFailedToLaunch(true, i18n("No world to say hello"));
    }

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
	p->setRenderHint(QPainter::SmoothPixmapTransform);
	p->setRenderHint(QPainter::Antialiasing);

	// Now we draw the applet, starting with our svg
	//m_svg.resize((int)contentsRect.width(), (int)contentsRect.height());
	//m_svg.paint(p, (int)contentsRect.left(), (int)contentsRect.top());

	// We place the icon and text
	//p->drawPixmap(7, 0, m_icon.pixmap((int)contentsRect.width(),(int)contentsRect.width()-14));
	//p->save();

	p->setPen(Qt::white);

	QRect labelsRect(contentsRect);
	labelsRect.setWidth(contentsRect.width()/2);

	QTextOption labelsTextOption(Qt::AlignRight | Qt::AlignHCenter);
	//textOptions.setWrapMode(QTextOption::WordWrap);
	p->drawText(labelsRect,
		i18n("Fajr:\nShorooq:\nDhuhr:\nAsr:\nMaghrib:\nIshaa:"),
		labelsTextOption);

	QRect timesRect(contentsRect);
	timesRect.setLeft(contentsRect.width()/2);
	timesRect.setWidth(contentsRect.width()/2);

	QTextOption timesTextOption(Qt::AlignLeft | Qt::AlignHCenter);
	//textOptions.setWrapMode(QTextOption::WordWrap);
	p->drawText(timesRect,
		i18n("%1\n%2\n%3\n%4\n%5\n%6")
			.arg(m_fajr.toString())
			.arg(m_shorooq.toString())
			.arg(m_dhuhr.toString())
			.arg(m_asr.toString())
			.arg(m_maghrib.toString())
			.arg(m_ishaa.toString()),
		timesTextOption);
	//p->restore();
}

QString PrayerTimes::locationCoords()
{
	return QString("%1,%2").arg(m_latitude).arg(m_longitude);
}

#include "prayertimes.moc"
