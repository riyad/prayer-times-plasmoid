/*
*/

#ifndef PRAYERDELEGATE_H
#define PRAYERDELEGATE_H

#include <qabstractitemdelegate.h>

#include "dataengine/prayertimesengine.h"
class PrayerDelegatePrivate;

class PrayerDelegate :  public QAbstractItemDelegate
{
	Q_OBJECT

	public:
		PrayerDelegate(QObject *parent = 0);
		~PrayerDelegate();

		PrayerTime currentPrayer();
		void setCurrentPrayer(PrayerTime prayer);

		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

	private:
		PrayerDelegatePrivate * const d;
};

#endif // PRAYERDELEGATE_H
