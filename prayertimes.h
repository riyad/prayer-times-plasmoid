// Here we avoid loading the header multiple times
#ifndef PRAYERTIMES_HEADER
#define PRAYERTIMES_HEADER

#include <Plasma/Applet>
#include <Plasma/DataEngine>

#include "ui_prayertimesLocationConfig.h"
#include "ui_prayertimesCalculationMethodConfig.h"

class CompassGraphicsWidget;

class QGraphicsWidget;
class QString;
class QTime;
class QTimer;

class KConfigDialog;

namespace Plasma {
	class IconWidget;
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

	QList<Plasma::IconWidget*> m_prayerIcons;
	QList<Plasma::Label*> m_prayerLabels;
	QList<Plasma::Label*> m_prayerTimeLabels;
	Plasma::Label *m_locationLabel;
	Plasma::Label *m_nextPrayerLabel;
	Plasma::Label *m_qiblaOrientationLabel;
	Plasma::Svg *m_prayertimesSvg;
	CompassGraphicsWidget *m_qiblaWidget;

	QString m_locationName;
	double m_latitude, m_longitude;
	int m_calculationMethod;

	QTime m_fajr, m_shorooq, m_dhuhr, m_asr, m_maghrib, m_ishaa, m_nextFajr;
	double m_qibla;

	QTimer* m_updateTimer;

	// Configuration dialog
	Ui::prayertimesLocationConfig locationConfigUi;
	Ui::prayertimesCalculationMethodConfig calculationMethodConfigUi;

	public:
		// Basic Create/Destroy
		PrayerTimes(QObject *parent, const QVariantList &args);
		~PrayerTimes();

		void init();

	public Q_SLOTS:
		void dataUpdated(const QString &source, const Plasma::DataEngine::Data &data);

	protected Q_SLOTS:
		void configAccepted();
		void configMouseGeoPositionChanged();
		void updateInterface();

	protected:
		void createConfigurationInterface(KConfigDialog *parent);

	private:
		QGraphicsWidget* createPrayerTimesWidget();
		QGraphicsWidget* createQiblaWidget();

		void connectSources();
		void disconnectSources();
		const QString sourceName() const;

		int currentPrayer() const;
		static const QString& labelFor(int prayer);
		int nextPrayer() const;
		const QTime& prayerTimeFor(int prayer) const;
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(prayertimes, PrayerTimes)
#endif
