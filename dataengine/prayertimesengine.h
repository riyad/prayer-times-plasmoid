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

#ifndef PRAYERTIMESENGINE_H
#define PRAYERTIMESENGINE_H

#include <itl/prayer.h>

class QString;
class QTime;
template <typename T> class QVector;

class KTimeZone;

#include <KLocalizedString>

#define CALCULATION_METHODS 8
// according to itl/prayer.h
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

#define PRAYER_TIMES 7

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

class PrayerTimesEngine : public Plasma::DataEngine
{
	Q_OBJECT

	KTimeZone* localTimeZone;

public:
		PrayerTimesEngine(QObject* parent, const QVariantList& args);
		~PrayerTimesEngine();

		void init();

protected:
		bool sourceRequestEvent(const QString& name);
		bool updateSourceEvent(const QString& source);

private:
	void parseSource(const QString& coords, Location* location, int* methodNum);
	void calculatePrayerTimes(const Location* location, const int methodNum, QVector<QTime>* prayerTimes);
	void calculateQibla(const Location* location, double* qiblaDegrees);
};

#endif
