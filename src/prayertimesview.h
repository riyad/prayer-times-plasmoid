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
 * Contains a custom view to show a day's prayer times.
 */

#ifndef PRAYERTIMESVIEW_H
#define PRAYERTIMESVIEW_H

// Own
#include "dataengine/prayertimesengine.h"
class PrayerTimeDelegate;

// Plasma
#include <Plasma/TreeView>

/**
 * @class PrayerTimesView
 * A customized Plasma tree view to show a day's prayer times.
 * It displays a table of prayer times with the names in the left and the times in the right column.
 * The current prayer is highlighted.
 */
class PrayerTimesView : public Plasma::TreeView
{
	Q_OBJECT

	public:
		explicit PrayerTimesView(QGraphicsWidget *parent = 0);
		~PrayerTimesView();

		/**
		 * Gives you the prayer time that is highlighted as the current prayer.
		 * @return The prayer.
		 */
		PrayerTime currentPrayer() const;
		/**
		 * Sets the prayer to be highlighted as the current prayer.
		 * @param prayer The current prayer.
		 */
		void setCurrentPrayer(PrayerTime prayer);

	protected:
		/**
		 * This is called upon resizes.
		 * It is used to update header and icon sizes.
		 * @todo make it change the font size too.
		 */
		void resizeEvent(QGraphicsSceneResizeEvent *event);

	private:
		//! The custom item delgate.
		PrayerTimeDelegate *m_delegate;
};

#endif // PRAYERTIMESVIEW_H
