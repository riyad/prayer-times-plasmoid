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
 * The Islamic Prayer Times data engine.
 */

#ifndef PRAYERTIMESENGINE_H
#define PRAYERTIMESENGINE_H

#include <itl/prayer.h>

class QString;
class QTime;
template <typename T> class QVector;

class KTimeZone;

#include <KLocalizedString>

/**
* A helper defnition.
* It should be equal to the number of elements in the @ref calculationMethodName array.
*/
#define CALCULATION_METHODS 8

/**
 * Translatable names of the predefined calculation methods in ITL.
 */
static const QString calculationMethodName[] = {
      i18nc("No calculation method specified.", "None"),
      i18n("Egyptian General Authority of Survey"),
      i18n("University of Islamic Sciences, Karachi (Shaf'i)"),
      i18n("University of Islamic Sciences, Karachi (Hanafi)"),
      i18n("Islamic Society of North America"),
      i18n("Muslim World League (MWL)"),
      i18n("Umm Al-Qurra, Saudi Arabia"),
      i18n("Fixed Ishaa Interval (always 90)")
};

/**
 * A helper defnition.
 * It should be equal to the number of elements in the @ref PrayerTime enum.
 */
#define PRAYER_TIMES 7

/**
 * An enumeration of all provided prayer times.
 */
enum PrayerTime {
	Fajr,
	Shorooq,
	Dhuhr,
	Asr,
	Maghrib,
	Ishaa,
	NextFajr
};

#include <Plasma/DataEngine>

/**
 * @class PrayerTimesEngine
 * Implements a Plasma data engine for parts of the ITL.
 * Exposed items include prayer times and qibla directions.
 * The minimum polling interval is 1 minute.
 */
class PrayerTimesEngine : public Plasma::DataEngine
{
	Q_OBJECT

	KTimeZone* localTimeZone;

public:
	PrayerTimesEngine(QObject* parent, const QVariantList& args);
	~PrayerTimesEngine();

	/**
	 * Initializes the data engine.
	 * It currently only sets the time zone to the the system's local time zone.
	 * @note The local time zone will be used for all calculations.
	 */
	void init();

protected:
	/**
	 * @see updateSourceEvent()
	 */
	bool sourceRequestEvent(const QString& name);

	/*!
	* Calculates/updates the prayer times and the qibla for the given source.
	* A source name might look like "location:53.07 N 8.8 E|method:5" (see @a name for more details).
	* @param name the source name to parse consits of parts separated by "|" namely:
	*             - "method": the calculation method as defined in ITL's prayer.h
	*             - "location": coordinates in decimal format (e.g. "53.07,8.8" for 53.07 N 8.8 E)
	*/
	bool updateSourceEvent(const QString& name);

private:
	/**
	* Extracts and generates the necessary structures from the source name.
	* @param name as in updateSourceEvent()
	* @param location for setting the requested location
	* @param methodNum for setting the requested calculation method
	* @note uses the local time zone for setting the corresponding fields in location
	* @note @a location will be overwritten
	* @note @a methodNum will be overwritten
	*/
	void parseSource(const QString& name, Location& location, int& methodNum);

	/**
	* Calculates the prayer times for the current day.
	* @param location the location for which to calculate the prayer times
	* @param prayerTimes the six times (i.e. fajr, shorook, dhuhr, ..., ishaa) for the current day
	* @note @a prayerTimes will be overwritten
	*/
	void calculatePrayerTimes(const Location& location, const int& methodNum, QVector<QTime>& prayerTimes);

	/**
	* Calculates the qibla direction.
	* @param location the location for which to calculate the qibla
	* @param qiblaDegrees the qibla from the location in degrees from north
	* @note @a qiblaDegrees will be overwritten
	*/
	void calculateQibla(const Location& location, double& qiblaDegrees);
};

#endif
