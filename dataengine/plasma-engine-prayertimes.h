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

#include <QString>
#include <QTime>
#include <QDate>

#include <KSystemTimeZones>

#include <Plasma/DataEngine>

class PrayerTimesEngine : public Plasma::DataEngine
{
    Q_OBJECT

    public:
        PrayerTimesEngine(QObject* parent, const QVariantList& args);
        ~PrayerTimesEngine();

    protected:
        bool sourceRequestEvent(const QString &name);
        bool updateSourceEvent(const QString& source);

    private:
	void recalculate();

    private:
	Location l;
	Prayer prayers[6];
	Method m;
	Date d;

	QTime prayerTimes[6];
	double degrees;
	int deg, min;
	double sec;
	int prayerIndex;
	QString gmtSeparator;

	QDate today;
	QString timezone;
	KTimeZone local;
	double latitude;
	double longitude;
	int calculationMethod;
	int gmtDiff;
};

K_EXPORT_PLASMA_DATAENGINE(prayertimes, PrayerTimesEngine)

#endif
