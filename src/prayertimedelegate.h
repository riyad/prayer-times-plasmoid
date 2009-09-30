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
 * Contains a custom item delegate to draw the items in the @ref PrayerTimesView.
 */

#ifndef PRAYERDELEGATE_H
#define PRAYERDELEGATE_H

// Own
#include "dataengine/prayertimesengine.h"
class PrayerTimeDelegatePrivate;

// Qt
#include <qabstractitemdelegate.h>

/**
 * @class PrayerTimeDelegate
 * A custom item delegate for drawing the items in the @ref PrayerTimesView.
 */
class PrayerTimeDelegate :  public QAbstractItemDelegate
{
	Q_OBJECT

public:
	PrayerTimeDelegate(QObject *parent = 0);
	~PrayerTimeDelegate();

	/**
	* Gives you the prayer time that is highlighted as the current prayer.
	* @return The prayer.
	*/
	PrayerTime currentPrayer();
	/**
	* Sets the prayer to be highlighted as the current prayer.
	* @param prayer The current prayer.
	*/
	void setCurrentPrayer(PrayerTime prayer);

	/**
	 * Calculates the size hint for a given item.
	 * It respects the width and hight of the prayer times text to be shown.
	 * @param option The style options to consider.
	 * @param index The index of the item.
	 * @return The size hint for the item at @a index.
	 */
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

	/**
	 * Paints the item at the given @a index using the given @a painter and @a options.
	 * @param painter The painter to use.
	 * @param option The style options to use.
	 * @param index The index of the item to paint.
	 */
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
	/**
	 * Paints the solid background (with rounded corners).
	 * \see paint
	 */
	void paintBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

	/**
	* Paints the highlight of the current prayer.
	* \see paint
	*/
	void paintHighlight(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

	/**
	* Paints the text of an item.
	* \see paint
	*/
	void paintText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

	PrayerTimeDelegatePrivate * const d;
};

#endif // PRAYERDELEGATE_H
