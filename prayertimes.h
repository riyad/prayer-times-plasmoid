// Here we avoid loading the header multiple times
#ifndef PRAYERTIMES_HEADER
#define PRAYERTIMES_HEADER

#include <Plasma/Applet>
#include <Plasma/DataEngine>

#include "ui_prayertimesLocationConfig.h"

class QiblaGraphicsWidget;

class QGraphicsLinearLayout;
class QGraphicsWidget;
class QPainter;
class QRect;
class QString;
class QStyleOptionGraphicsItem;
class QTime;
class QTimer;

class KConfigDialog;

namespace Plasma {
	class Label;
	class Svg;
	class TabBar;
}

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

		void init();

	public slots:
		void dataUpdated(const QString &source, const Plasma::DataEngine::Data &data);

	protected Q_SLOTS:
		void configAccepted();
		void configMouseGeoPositionChanged();
		void repaintNeeded();

	protected:
		void createConfigurationInterface(KConfigDialog *parent);

	private:
		QGraphicsWidget* prayerTimesWidget();
		QGraphicsWidget* qiblaWidget();

		void connectSources();
		void disconnectSources();
		QString sourceName();

		int currentPrayer() const;
		static const QString& labelFor(int prayer);
		int nextPrayer() const;
		const QTime& prayerTimeFor(int prayer) const;

		QList<Plasma::Label*> m_prayerLabels;
		QList<Plasma::Label*> m_prayerTimeLabels;
		Plasma::Label *m_locationLabel;
		Plasma::Label *m_nextPrayerLabel;
		Plasma::Svg *m_prayertimesSvg;
		QiblaGraphicsWidget *m_qiblaWidget;

		QString m_locationName;
		double m_latitude, m_longitude;
		int m_calculationMethod;

		QTime m_fajr, m_shorooq, m_dhuhr, m_asr, m_maghrib, m_ishaa, m_nextFajr;
		double m_qibla;

		QTimer* m_updateTimer;

		// Configuration dialog
		Ui::prayertimesLocationConfig ui;
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(prayertimes, PrayerTimes)
#endif
