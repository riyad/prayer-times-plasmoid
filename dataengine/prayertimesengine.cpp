/*
  Copyright 2008 Hesham Wahba

  This Prayer Times Plasmoid is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "prayertimesengine.h"

// Qt
#include <QDate>
#include <QRegExp>
#include <QString>
#include <QTime>
#include <QVector>

// KDE
#include <KSystemTimeZone>
#include <KTimeZone>

// Marble
#include <marble/GeoDataCoordinates.h>

PrayerTimesEngine::PrayerTimesEngine(QObject* parent, const QVariantList& args)
	: Plasma::DataEngine(parent),
	localTimeZone(0)
{
	// We ignore any arguments - data engines do not have much use for them
	Q_UNUSED(args);

	// update every minute, maybe the date has changed
	setMinimumPollingInterval(1000*60);
}

PrayerTimesEngine::~PrayerTimesEngine()
{
	delete localTimeZone;
}

void  PrayerTimesEngine::init()
{
	// use local time zone for prayer time calculations
	localTimeZone = new KTimeZone(KSystemTimeZones::local());
}

bool PrayerTimesEngine::sourceRequestEvent(const QString& name)
{
	// We do not have any special code to execute the
	// first time a source is requested, so we just call
	// updateSourceEvent().
	return updateSourceEvent(name);
}

/**
 * Calculates/updates the prayer times and the qibla for the given coordinates.
 * @param name the coordinats in decimal format (e.g. "53.07,8.8" for 53.07 N 8.8 E)
 */
bool PrayerTimesEngine::updateSourceEvent(const QString& name)
{
	Location location;
	int methodNum;
	QVector<QTime> prayerTimes;
	double qiblaDegrees;

	// populate the location struct
	parseSource(name, location, methodNum);

	calculatePrayerTimes(location, methodNum, prayerTimes);
	calculateQibla(location, qiblaDegrees);

	setData(name, I18N_NOOP("Fajr"),    prayerTimes[0]);
	setData(name, I18N_NOOP("Shorooq"), prayerTimes[1]);
	setData(name, I18N_NOOP("Dhuhr"),   prayerTimes[2]);
	setData(name, I18N_NOOP("Asr"),     prayerTimes[3]);
	setData(name, I18N_NOOP("Maghrib"), prayerTimes[4]);
	setData(name, I18N_NOOP("Ishaa"),   prayerTimes[5]);
	setData(name, I18N_NOOP("NextFajr"), prayerTimes[6]);

	setData(name, I18N_NOOP("Qibla"), qiblaDegrees);

	return true;
}

/**
 * Generates a Location from the given coordinates.
 * @param source the source to parse consits of 2 parts separated by "/" namely:
 *               - the calculation method
 *               - coordinates in decimal format (e.g. "53.07,8.8" for 53.07 N 8.8 E)
 * @param location for setting the requested location
 * @param methodNum for setting the requested calculation method
 * @note uses the local time zone for setting the corresponding fields in location
 * @note location will be overwritten
 */
void PrayerTimesEngine::parseSource(const QString& source, Location& location, int& methodNum)
{
	Marble::GeoDataCoordinates coords;
	QString methodName;
	QStringList sourceParts = source.split("|");
	foreach(QString sourcePart, sourceParts) {
		QRegExp regex;
		if(sourcePart.contains(regex = QRegExp("^\\s*location:\\s*"))) { // location
			sourcePart = sourcePart.remove(regex);
			bool success = false;
			coords = Marble::GeoDataCoordinates::fromString(sourcePart, success);
			Q_ASSERT(success);
			kDebug() << "Parsed location: " << coords.toString();
		} else if(sourcePart.contains(regex = QRegExp("^\\s*method:\\s*"))) { // calculation method
			methodName = sourcePart.remove(regex);
			kDebug() << "Parsed method: " << methodName;
		} else {
			kDebug() << "Unrecognized source parameter: " << sourcePart;
		}
	}

	if(source.isEmpty()) {
		kDebug() << "Error: source is empty";
		return;
	}

	// looking up the requested calculation method
	methodNum = CALCULATION_METHODS-1;
	while(methodNum) {
		if(methodName == calculationMethodName[methodNum]) {
			break;
		}
		--methodNum;
	}

	// extracting latitude and longitude
	location.degreeLat = coords.latitude(Marble::GeoDataCoordinates::Degree);
	location.degreeLong = coords.longitude(Marble::GeoDataCoordinates::Degree);

	location.dst = localTimeZone->isDstAtUtc(QDateTime::currentDateTime().toUTC()) ? 1 : 0;
	location.gmtDiff = double(localTimeZone->currentOffset())/3600 - location.dst;

	location.seaLevel = 0; // just for simplicity
	location.pressure = 1010; // default from itl's prayer.h
	location.temperature = 10; // default from itl's prayer.
}

/**
 * Calculates the prayer times for the given location for the current day.
 * @param location the location for calculating the prayer times
 * @param prayerTimes the six times (i.e. fajr, shorook, dhuhr, ..., ishaa) for the current day
 * @note prayerTimes will be overwritten
 */
void PrayerTimesEngine::calculatePrayerTimes(const Location& location, const int& methodNum, QVector<QTime>& prayerTimes)
{
	// for retrieving the results of the calculation
	Prayer prayers[PRAYER_TIMES];

	// filling in the parameters for the "calculationMethod" method
	Method method;
	getMethod(methodNum, &method);

	// the date of the day the prayer times are goign to be calculated for
	QDate currentDate = QDate::currentDate();
	Date date;
	date.day = currentDate.day();
	date.month = currentDate.month();
	date.year = currentDate.year();

	// the actual prayer times calculation
	getPrayerTimes(&location, &method, &date, prayers);
	getNextDayFajr(&location, &method, &date, &prayers[6]);

	// transfering the calculation result into our result structure
	prayerTimes.resize(PRAYER_TIMES);
	for(int prayer = Fajr; prayer <= NextFajr; ++prayer) {
		prayerTimes[prayer].setHMS(prayers[prayer].hour, prayers[prayer].minute, prayers[prayer].second);
	}
}

/**
 * Calculates the qibla for the given location.
 * @param location the location for calculating the qibla
 * @param qiblaDegrees the qibla from the location in degrees
 * @note qiblaDegrees will be overwritten
 */
void PrayerTimesEngine::calculateQibla(const Location& location, double& qiblaDegrees)
{
	qiblaDegrees = -getNorthQibla(&location);
}

// This does the magic that allows Plasma to load this plugin.
// The first argument must match the X-Plasma-EngineName in the .desktop file.
K_EXPORT_PLASMA_DATAENGINE(prayertimes, PrayerTimesEngine)

// this is needed since TestTimeEngine is a QObject
#include "prayertimesengine.moc"
