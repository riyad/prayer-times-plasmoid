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

#include <QTime>
#include <QVector>

#include <KSystemTimeZones>

#include <Plasma/DataEngine>

class PrayerTimesEngine : public Plasma::DataEngine
{
	Q_OBJECT

	KTimeZone localTimeZone;
	int calculationMethod;

public:
		PrayerTimesEngine(QObject* parent, const QVariantList& args);
		~PrayerTimesEngine();

		void init();

protected:
		bool sourceRequestEvent(const QString &name);
		bool updateSourceEvent(const QString& source);

private:
	void parseLocation(QString coords, Location *location);
	void calculatePrayerTimes(Location *location, QVector<QTime> *prayerTimes, double *qiblaDegrees);
};

#endif
