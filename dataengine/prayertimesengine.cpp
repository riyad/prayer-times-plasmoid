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

#include <QDate>

PrayerTimesEngine::PrayerTimesEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent)
{
	// We ignore any arguments - data engines do not have much use for them
	Q_UNUSED(args);

	// update every minute, maybe the date has changed
	setMinimumPollingInterval(1000*60);
}

PrayerTimesEngine::~PrayerTimesEngine()
{
}

void  PrayerTimesEngine::init() {
	localTimeZone = KSystemTimeZones::local();
	// TODO: read method from config files
}


bool PrayerTimesEngine::sourceRequestEvent(const QString &name)
{
	// We do not have any special code to execute the
	// first time a source is requested, so we just call
	// updateSourceEvent().
	return updateSourceEvent(name);
}

bool PrayerTimesEngine::updateSourceEvent(const QString &name)
{
	Location location;
	QVector<QTime> prayerTimes;
	double qiblaDegrees;

	parseLocation(name, &location);
	calculatePrayerTimes(&location, &prayerTimes, &qiblaDegrees);

	setData(name, I18N_NOOP("prayerTimeFajr"),    prayerTimes[0]);
	setData(name, I18N_NOOP("prayerTimeShorooq"), prayerTimes[1]);
	setData(name, I18N_NOOP("prayerTimeDhuhr"),   prayerTimes[2]);
	setData(name, I18N_NOOP("prayerTimeAsr"),     prayerTimes[3]);
	setData(name, I18N_NOOP("prayerTimeMaghrib"), prayerTimes[4]);
	setData(name, I18N_NOOP("prayerTimeIshaa"),   prayerTimes[5]);

	setData(name, I18N_NOOP("qiblaDegrees"), qiblaDegrees);

	return true;
}

void PrayerTimesEngine::parseLocation(QString coords, Location *location)
{
	if(coords.isEmpty()) {
		kDebug() << "Error: coords is empty: " << coords;
		return;
	}
	if(coords.isNull()) {
		kDebug() << "Error: coords is null";
		return;
	}
	if(location == 0L) {
		kDebug() << "Error: location is null";
		return;
	}
	
	coords.remove(" ");
	QStringList splitCoords = coords.split(",");
	location->degreeLong = splitCoords[0].toDouble();
	location->degreeLat = splitCoords[1].toDouble();

	location->gmtDiff = double(localTimeZone.currentOffset())/3600;

	location->seaLevel = 0; // default, just for simplicity
	location->pressure = 1010; // default from itl's prayer.h
	location->temperature = 10; // default from itl's prayer.
}

void PrayerTimesEngine::calculatePrayerTimes(Location *location, QVector<QTime> *prayerTimes, double *qiblaDegrees)
{
	if(location == 0L) {
		kDebug() << "Error: location is null";
		return;
	}
	if(prayerTimes == 0L) {
		kDebug() << "Error: prayerTimes is null";
		return;
	}
	if(qiblaDegrees == 0L) {
		kDebug() << "Error: qiblaDegrees is null";
		return;
	}

	Prayer prayers[6];

	Method method;
	getMethod(calculationMethod, &method);

	Date date;
	date.day = QDate::currentDate().day();
	date.month = QDate::currentDate().month();
	date.year = QDate::currentDate().year();

	kDebug() << "Latitude: " << location->degreeLat;
	kDebug() << "Longitude: " << location->degreeLong;
	kDebug() << "Method: " << calculationMethod;

	getPrayerTimes(location, &method, &date, prayers);

	prayerTimes->resize(6);
	for(int i = 0; i < 6; ++i) {
		(*prayerTimes)[i].setHMS(prayers[i].hour, prayers[i].minute, prayers[i].second);
	}

	*qiblaDegrees = -getNorthQibla(location);
}

// This does the magic that allows Plasma to load
// this plugin.  The first argument must match
// the X-Plasma-EngineName in the .desktop file.
K_EXPORT_PLASMA_DATAENGINE(prayertimes, PrayerTimesEngine)

// this is needed since TestTimeEngine is a QObject
#include "prayertimesengine.moc"
