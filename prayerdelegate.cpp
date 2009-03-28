/*
*/

#include "prayerdelegate.h"

PrayerDelegate::PrayerDelegate(QObject* parent)
	: QAbstractItemDelegate(parent)
{
}

PrayerDelegate::~PrayerDelegate()
{
}


QSize PrayerDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize size = option.rect.size();

	QFontMetrics metrics(option.font);

	size.setHeight(qMax(option.decorationSize.height(), metrics.height()));

	size.setWidth(option.decorationSize.width() + metrics.width(index.data(Qt::DisplayRole).value<QString>()));

	return size;
}

void PrayerDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{

}

#include <prayerdelegate.moc>
