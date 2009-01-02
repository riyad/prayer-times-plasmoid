// Here we avoid loading the header multiple times
#ifndef PRAYERTIMES_HEADER
#define PRAYERTIMES_HEADER

#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include <Plasma/Svg>

#include "ui_prayertimesLocationConfig.h"

class QPainter;
class QRect;
class QString;
class QStyleOptionGraphicsItem;
class QTime;

class KConfigDialog;

namespace Marble {
	class MarbleWidget;
}

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

	protected Q_SLOTS:
		void configAccepted();
		void configMouseGeoPositionChanged();

	protected:
		void createConfigurationInterface(KConfigDialog *parent);

	private:
		void connectSources();
		void disconnectSources();
		QString sourceName();

		const int currentPrayer();
		static const QString& labelFor(int prayer);
		const QTime& prayerTimeFor(int prayer);

		Plasma::Svg m_kaabaSvg;

		QString m_locationName;
		double m_latitude, m_longitude;
		int m_calculationMethod;

		QTime m_fajr, m_shorooq, m_dhuhr, m_asr, m_maghrib, m_ishaa, m_nextFajr;
		double m_qibla;

		// Configuration dialog
		Ui::prayertimesLocationConfig ui;
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(prayertimes, PrayerTimes)
#endif
