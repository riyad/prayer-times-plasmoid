/*
	Copyright (C) 2008-2009  Riyad Preukschas <includeme@users.sourceforge.net>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 3 as
	published by the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Here we avoid loading the header multiple times
#ifndef PRAYERTIMES_HEADER
#define PRAYERTIMES_HEADER

#include <Plasma/Applet>

// Own
#include "ui_prayertimesNotificationsConfig.h"
#include "ui_prayertimesLocationConfig.h"
#include "ui_prayertimesCalculationMethodConfig.h"

#include "dataengine/prayertimesengine.h"
class CompassGraphicsWidget;
class PrayerTimesView;

// KDE
class KConfigDialog;

// Plasma
namespace Plasma {
	class Label;
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

	// Configuration variables
	QString m_locationName;
	Marble::GeoDataCoordinates m_location;
	int m_calculationMethod;
	bool m_notifyBeforeNextPrayer;
	int m_notifyMinutesBeforeNextPrayer;
	bool m_notifyOnNextPrayer;
	bool m_showPopupOnNextPrayer;
	bool m_playAdhanOnNextPrayer;
	KUrl m_adhanFileUrl;

	QTime m_fajr, m_shorooq, m_dhuhr, m_asr, m_maghrib, m_ishaa, m_nextFajr;
	double m_qibla;

	QTimer* m_updateTimer;

	// Configuration dialog
	Ui::prayertimesNotificationsConfig notificationsConfigUi;
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
		void notify();
		void showNotification(const QString &message) const;

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
