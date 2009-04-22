// Here we avoid loading the header multiple times
#ifndef PRAYERTIMES_HEADER
#define PRAYERTIMES_HEADER

#include <Plasma/Applet>
#include <Plasma/DataEngine>

#include "ui_prayertimesLocationConfig.h"
#include "ui_prayertimesCalculationMethodConfig.h"

#include "dataengine/prayertimesengine.h"
class CompassGraphicsWidget;
class PrayerTimesView;

class QGraphicsWidget;
class QStandardItemModel;
class QString;
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
class PrayerTimesApplet : public Plasma::Applet
{
	Q_OBJECT

	PrayerTimesView *m_prayerTimesView;
	Plasma::Label *m_locationLabel;
	Plasma::Label *m_dateLabel;
	Plasma::Label *m_nextPrayerLabel;
	Plasma::Label *m_qiblaOrientationLabel;
	Plasma::Svg *m_prayertimesSvg;
	CompassGraphicsWidget *m_qiblaWidget;

	QString m_locationName;
	double m_latitude, m_longitude;
	int m_calculationMethod;

	QTime m_fajr, m_shorooq, m_dhuhr, m_asr, m_maghrib, m_ishaa, m_nextFajr;
	double m_qibla;

	QStandardItemModel *m_prayerTimesModel;

	QTimer* m_updateTimer;

	// Configuration dialog
	Ui::prayertimesLocationConfig locationConfigUi;
	Ui::prayertimesCalculationMethodConfig calculationMethodConfigUi;

	public:
		// Basic Create/Destroy
		PrayerTimesApplet(QObject *parent, const QVariantList &args);
		~PrayerTimesApplet();

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

		PrayerTime currentPrayer() const;
		static const QString& labelFor(PrayerTime prayer);
		PrayerTime nextPrayer() const;
		const QTime& prayerTimeFor(PrayerTime prayer) const;
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(prayertimes, PrayerTimesApplet)
#endif