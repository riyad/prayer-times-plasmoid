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

#include <Plasma/DataContainer>

#include <math.h>

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

bool PrayerTimesEngine::sourceRequestEvent(const QString &name)
{
	// We do not have any special code to execute the
	// first time a source is requested, so we just call
	// updateSourceEvent().
	return updateSourceEvent(name);
}

bool PrayerTimesEngine::updateSourceEvent(const QString &name)
{
	recalculate();
	
	setData(name, I18N_NOOP("prayerTimeFajr"), prayerTimes[0]);
	setData(name, I18N_NOOP("prayerTimeShorooq"), prayerTimes[1]);
	setData(name, I18N_NOOP("prayerTimeDhuhr"), prayerTimes[2]);
	setData(name, I18N_NOOP("prayerTimeAsr"), prayerTimes[3]);
	setData(name, I18N_NOOP("prayerTimeMaghrib"), prayerTimes[4]);
	setData(name, I18N_NOOP("prayerTimeIshaa"), prayerTimes[5]);
	
	return true;
}

void PrayerTimesEngine::recalculate()
{
/*
  local = KSystemTimeZones::zone(timezone);

  l = Location();
  l.degreeLong = longitude;
  l.degreeLat = latitude;
  l.gmtDiff = double(localTimeZone.currentOffset())/3600.;
  l.seaLevel = 0; // default, just for simplicity
  l.pressure = 1010; // default from itl's prayer.h
  l.temperature = 10; // default from itl's prayer.h

  m = Method();
  getMethod(calculationMethod,&m);

  d = Date();
  d.day = today.day();
  d.month = today.month();
  d.year = today.year();

  getPrayerTimes(&l, &m, &d, prayers);

  for(int i = 0; i < 6; ++i) {
    prayerTimes[i] = QTime(prayers[i].hour, prayers[i].minute, prayers[i].second);
  }
  
  degrees = -getNorthQibla(&l);
  decimal2Dms(degrees, &deg, &min, &sec);

  gmtDiff = l.gmtDiff;
  gmtSeparator = (gmtDiff >= 0)?"+":"";
*/
}

// This does the magic that allows Plasma to load
// this plugin.  The first argument must match
// the X-Plasma-EngineName in the .desktop file.
K_EXPORT_PLASMA_DATAENGINE(prayertimes, PrayerTimesEngine)

// this is needed since TestTimeEngine is a QObject
#include "prayertimesengine.moc"
