/*
*/

#ifndef PRAYERDELEGATE_H
#define PRAYERDELEGATE_H

#include <qabstractitemdelegate.h>


class PrayerDelegate :  public QAbstractItemDelegate
{
	Q_OBJECT

	public:
		PrayerDelegate(QObject *parent = 0);
		~PrayerDelegate();

		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

	private:
};

#endif // PRAYERDELEGATE_H
