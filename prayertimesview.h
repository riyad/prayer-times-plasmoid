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

#ifndef PRAYERTIMESVIEW_H
#define PRAYERTIMESVIEW_H

#include <Plasma/TreeView>

#include "dataengine/prayertimesengine.h"
class PrayerTimeDelegate;

class QGraphicsSceneResizeEvent;
class QGraphicsWidget;

class PrayerTimesView : public Plasma::TreeView
{
	Q_OBJECT

	public:
		explicit PrayerTimesView(QGraphicsWidget *parent = 0);
		~PrayerTimesView();

		PrayerTime currentPrayer() const;
		void setCurrentPrayer(PrayerTime prayer);

	protected:
		void resizeEvent(QGraphicsSceneResizeEvent *event);

	private:
		PrayerTimeDelegate *m_delegate;
};

#endif // PRAYERTIMESVIEW_H
