// Here we avoid loading the header multiple times
#ifndef PRAYERTIMES_HEADER
#define PRAYERTIMES_HEADER

#include <QString>
#include <QTime>
#include <KIcon>

#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include <Plasma/Svg>

class QSizeF;

// Define our plasma Applet
class PrayerTimes : public Plasma::Applet
{
	Q_OBJECT
public:
		// Basic Create/Destroy
		PrayerTimes(QObject *parent, const QVariantList &args);
		~PrayerTimes();

		// The paintInterface procedure paints the applet to screen
		void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect& contentsRect);
		void init();

public slots:
	void dataUpdated(const QString &source, const Plasma::DataEngine::Data &data);

private:
		QString locationCoords();

		Plasma::Svg m_svg;
		KIcon m_icon;

		double m_latitude, m_longitude;
		int m_calculationMethod;

		QTime m_fajr, m_shorooq, m_dhuhr, m_asr, m_maghrib, m_ishaa;
		double m_qibla;
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(prayertimes, PrayerTimes)
#endif
