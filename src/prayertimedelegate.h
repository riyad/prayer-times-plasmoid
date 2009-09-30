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

#ifndef PRAYERDELEGATE_H
#define PRAYERDELEGATE_H

#include <qabstractitemdelegate.h>

#include "dataengine/prayertimesengine.h"
class PrayerTimeDelegatePrivate;

class PrayerTimeDelegate :  public QAbstractItemDelegate
{
	Q_OBJECT

	public:
		PrayerTimeDelegate(QObject *parent = 0);
		~PrayerTimeDelegate();

		PrayerTime currentPrayer();
		void setCurrentPrayer(PrayerTime prayer);

		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

	private:
		void paintBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paintHighlight(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paintText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

		PrayerTimeDelegatePrivate * const d;
};

#endif // PRAYERDELEGATE_H
