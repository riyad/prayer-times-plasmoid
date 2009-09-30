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
/**
 * @file
 * Contains a Plasma applet for showing prayer times and qibla direction.
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
/**
 * @class PrayerTimesApplet
 * Implements a Plasma applet for showing and configuring prayer times.
 */
class PrayerTimesApplet : public Plasma::Applet
{
	Q_OBJECT

	PrayerTimesView *m_prayerTimesView; //!< The prayer times table.
	Plasma::Label *m_locationLabel; //!< The label holding the configured location.
	Plasma::Label *m_dateLabel; //!< The label holding the current date.
	Plasma::Label *m_nextPrayerLabel; //!< The label stating how much time is left till the next prayer.
	Plasma::Label *m_qiblaOrientationLabel; //!< The label stating the qibla direction.
	Plasma::Svg *m_prayertimesSvg; //!< The prayer times SVG.
	CompassGraphicsWidget *m_qiblaWidget; //!< The compass.

	// Configuration variables
	QString m_locationName; //!< The name of the configured location.
	Marble::GeoDataCoordinates m_location; //!< The coordinates of the configured location.
	int m_calculationMethod; //!< The configured calculation method.
	bool m_notifyBeforeNextPrayer; //!< Holds whether to notyfy the user @em before the next prayer.
	int m_notifyMinutesBeforeNextPrayer; //!< Holds how long before the next prayer to notify the user.
	bool m_notifyOnNextPrayer; //!< Holds whether to notfy the user @em on entering a new prayer time.
	bool m_showPopupOnNextPrayer; //!< Holds whether to show a popup on entering a new prayer time.
	bool m_playAdhanOnNextPrayer; //!< Holds whether to play athan on entering a new prayer time.
	KUrl m_adhanFileUrl; //!< The location of the athan file to play.

	QTime m_fajr, m_shorooq, m_dhuhr, m_asr, m_maghrib, m_ishaa, m_nextFajr;
	double m_qibla; //!< The qibla direction in decrees from north.

	QTimer* m_updateTimer; //!< A timer to update the UI regularly.

	// Configuration dialog
	//! The configuration page for notifications.
	Ui::prayertimesNotificationsConfig notificationsConfigUi;
	//! The configureation page for the location.
	Ui::prayertimesLocationConfig locationConfigUi;
	//! The configuration page for the calculation method.
	Ui::prayertimesCalculationMethodConfig calculationMethodConfigUi;

public:
	// Basic Create/Destroy
	PrayerTimesApplet(QObject *parent, const QVariantList &args);
	~PrayerTimesApplet();

	/**
	 * Initializes the plasmoid.
	 * It reads the applet's configuration and connects to the source if possible.
	 * @see connectSources
	 */
	void init();

public Q_SLOTS:
	/**
	 * This is called by the data engine on updates.
	 * It caches the presented data locally.
	 * @param source The name of the requested source as reported back from the data engine (see \ref sourceName)
	 * @param data The data returned from the data engine.
	 *             This contains the prayer times and qibla direction.
	 * @note The UI gets updated.
	 * @note Notifications are triggered if appropriate.
	 */
	void dataUpdated(const QString &source, const Plasma::DataEngine::Data &data);

protected Q_SLOTS:
	/**
	 * For the configuration dialog to call when the configuration should be applied.
	 * All configuration variables and the applet's configuration is updated and Plasma told to save it.
	 * @note The connection to the data engine is reset,
	 */
	void configAccepted();

	/**
	 * For the Marble widget in the location configuration page to call when the view is moved.
	 * It updates the location configuration page's elements according to what the cursor is positioned over.
	 */
	void configMouseGeoPositionChanged();

	/**
	 * Updates all UI elements to show the latest data.
	 */
	void updateInterface();

	/**
	 * Determines whether a user notification is in order and notifies him.
	 * @see showNotification
	 */
	void notify();

	/**
	 * Sends out a KNotification event with an icon and the given message.
	 * @param message The notification message.
	 * @see notify
	 * @see KNotification::event
	 */
	void showNotification(const QString &message) const;

protected:
	/**
	 * Creates the custom configuration pages of the applet's configuration dialog.
	 * @param parent The configuration dialog.
	 * @see configAccepted
	 * @see configMouseGeoPositionChanged
	 */
	void createConfigurationInterface(KConfigDialog *parent);

private:
	/**
	 * Constructs the widget for the applet's prayer times tab.
	 * @return The prayer times widget.
	 */
	QGraphicsWidget* createPrayerTimesWidget();

	/**
	* Constructs the  widget for the applet's qibla tab.
	* @return The qibla widget.
	*/
	QGraphicsWidget* createQiblaWidget();

	/**
	 * Connects the applet to the data engine if the configuration permits.
	 * If necessary it also notifies Plasma that the applet requires configuration.
	 * @see setConfigurationRequired
	 */
	void connectSources();

	/**
	 * Disconnects the applet from the data engine,
	 */
	void disconnectSources();

	/**
	 * Constructs the source name for the data engine from the applet's configuration.
	 * @return The source name.
	 * @see PrayerTimesEngine::sourceRequestEvent
	 */
	const QString sourceName() const;

	/**
	 * Determines the current prayer depending on the time of day and the calculated prayer times.
	 * @return The current prayer.
	 * @note This expects the applet do have received data from the data engine.
	 */
	PrayerTime currentPrayer() const;

	/**
	 * Determines the (translated) name of a given prayer.
	 * @param prayer The prayer.
	 * @return The name of @a prayer.
	 */
	static const QString& labelFor(PrayerTime prayer);

	/**
	 * Determines the next prayer depending on the time of day and the calculated prayer times.
	 * @return The next prayer.
	 * @note This expects the applet do have received data from the data engine.
	 */
	PrayerTime nextPrayer() const;

	/**
	 * Determines the calculated prayer time for a given prayer.
	 * @param prayer The prayer.
	 * @return The time for @a prayer.
	 * @note This expects the applet do have received data from the data engine.
	 */
	const QTime& prayerTimeFor(PrayerTime prayer) const;
};

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(prayertimes, PrayerTimesApplet)
#endif
