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
  Q_UNUSED(args);
  //setMinimumUpdateInterval(1000);
  setMinimumPollingInterval(1000);
}

PrayerTimesEngine::~PrayerTimesEngine()
{
}

bool PrayerTimesEngine::sourceRequestEvent(const QString &name)
{
    return updateSourceEvent(name);
}

bool PrayerTimesEngine::updateSourceEvent(const QString &name)
{
  today = property("today").toDate();
  latitude = property("latitude").toDouble();
  longitude = property("longitude").toDouble();
  calculationMethod = property("calculationMethod").toInt();
  timezone = property("timezone").toString();
  
  recalculate();
  
  setData(name, I18N_NOOP("prayerTimes1"), prayerTimes[0]);
  setData(name, I18N_NOOP("prayerTimes2"), prayerTimes[1]);
  setData(name, I18N_NOOP("prayerTimes3"), prayerTimes[2]);
  setData(name, I18N_NOOP("prayerTimes4"), prayerTimes[3]);
  setData(name, I18N_NOOP("prayerTimes5"), prayerTimes[4]);
  setData(name, I18N_NOOP("prayerTimes6"), prayerTimes[5]);
  setData(name, I18N_NOOP("deg"), deg);
  setData(name, I18N_NOOP("min"), min);
  setData(name, I18N_NOOP("sec"), sec);
  setData(name, I18N_NOOP("degrees"), degrees);
  setData(name, I18N_NOOP("gmtDiff"), gmtDiff);
  setData(name, I18N_NOOP("gmtSeparator"), gmtSeparator);
   
  return true;
}

void PrayerTimesEngine::recalculate()
{
  local = KSystemTimeZones::zone(timezone);

  l = Location();
  l.degreeLong = longitude;
  l.degreeLat = latitude;
  l.gmtDiff =   double(local.currentOffset())/3600.;
  l.seaLevel = 0;
  l.pressure = 1010;
  l.temperature = 10;

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
}

#include "prayertimesengine.moc"
