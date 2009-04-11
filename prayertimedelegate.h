/*
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
