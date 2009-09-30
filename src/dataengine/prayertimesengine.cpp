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

#include "prayertimesengine.h"

// KDE
#include <KSystemTimeZone>

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

void PrayerTimesEngine::parseSource(const QString& source, Location& location, int& methodNum)
{
	Marble::GeoDataCoordinates parsedLocation;
	QString parsedName;
	QStringList sourceParts = source.split("|");
	foreach(QString sourcePart, sourceParts) {
		QRegExp regex;
		if(sourcePart.contains(regex = QRegExp("^\\s*location:\\s*"))) { // location
			sourcePart = sourcePart.remove(regex);
			bool success = false;
			parsedLocation = Marble::GeoDataCoordinates::fromString(sourcePart, success);
			Q_ASSERT(success);
			kDebug() << "Parsed location: " << parsedLocation.toString();
		} else if(sourcePart.contains(regex = QRegExp("^\\s*method:\\s*"))) { // calculation method
			parsedName = sourcePart.remove(regex);
			kDebug() << "Parsed method: " << parsedName;
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
		if(parsedName == calculationMethodName[methodNum]) {
			break;
		}
		--methodNum;
	}

	// extracting latitude and longitude
	location.degreeLat = parsedLocation.latitude(Marble::GeoDataCoordinates::Degree);
	location.degreeLong = parsedLocation.longitude(Marble::GeoDataCoordinates::Degree);
	kDebug() << "Location lon / lat:" << location.degreeLong << location.degreeLat;

	location.dst = localTimeZone->isDstAtUtc(QDateTime::currentDateTime().toUTC()) ? 1 : 0;
	kDebug() << "Location dst: " << location.dst;
	location.gmtDiff = double(localTimeZone->currentOffset())/3600 - location.dst;
	kDebug() << "Location gmtDiff: " << location.gmtDiff;

	location.seaLevel = 0; // just for simplicity
	location.pressure = 1010; // default from itl's prayer.h
	location.temperature = 10; // default from itl's prayer.
}

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
		if (prayers[prayer].isExtreme) {
			kDebug() << "Calculated prayer time for prayer" << prayer << "is extreme.";
		}
	}
}

void PrayerTimesEngine::calculateQibla(const Location& location, double& qiblaDegrees)
{
	qiblaDegrees = -getNorthQibla(&location);
}

// This does the magic that allows Plasma to load this plugin.
// The first argument must match the X-Plasma-EngineName in the .desktop file.
K_EXPORT_PLASMA_DATAENGINE(prayertimes, PrayerTimesEngine)

// this is needed since TestTimeEngine is a QObject
#include "prayertimesengine.moc"
