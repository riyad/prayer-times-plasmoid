// Own
#include "prayerdelegate.h"

// Qt
#include <QAbstractItemDelegate>
#include <QColor>
#include <QFontMetrics>
#include <QModelIndex>
#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QSize>
#include <QStyleOptionViewItem>

// Plasma
#include <Plasma/PaintUtils>
#include <Plasma/Theme>

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
	const int rows = index.model()->rowCount();
	const int columns = index.model()->columnCount();
	const int radius = 5;
	bool drawLeft = false;
	bool drawRight = false;
	bool drawTop = false;
	bool drawBottom = false;

	QColor backgroundColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));

	if (index.column() % 2 == 0) {
		backgroundColor.setAlphaF(0.1);
	} else {
		backgroundColor.setAlphaF(0.2);
	}

	QRect backgroundRect(option.rect);
	//backgroundRect.adjust(0, backgroundRect.height() / 4.2, 0, -backgroundRect.height() / 4.2);

	if (index.column() == 0) {
		drawLeft = true;
	}
	if (index.column() == columns - 1) {
		drawRight = true;
	}
	if (index.row() == 0) {
		drawTop = true;
	}
	if (index.row() == rows - 1) {
		drawBottom = true;
	}

	if (!drawLeft && !drawRight && !drawTop && !drawBottom) {
		painter->fillRect(backgroundRect, backgroundColor);
	} else {
		QPainterPath path;
		QRect backgroundRectToClip(backgroundRect);

		if (!drawLeft) {
				backgroundRectToClip.setLeft(backgroundRectToClip.left()-radius);
		}
		if (!drawRight) {
				backgroundRectToClip.setRight(backgroundRectToClip.right()+radius);
		}
		if (!drawTop) {
				backgroundRectToClip.setTop(backgroundRectToClip.top()-radius);
		}
		if (!drawBottom) {
				backgroundRectToClip.setBottom(backgroundRectToClip.bottom()+radius);
		}

		path = Plasma::PaintUtils::roundedRectangle(backgroundRectToClip, radius);


		QPainterPath clipPath;
		clipPath.addRect(backgroundRect);
		path = path.intersected(clipPath);

		painter->setRenderHint(QPainter::Antialiasing);
		painter->setPen(Qt::NoPen);
		painter->setBrush(backgroundColor);
		painter->drawPath(path);
	}

	QString titleText = index.data(Qt::DisplayRole).value<QString>();
	QFont titleFont = qvariant_cast<QFont>(index.data(Qt::FontRole)).resolve(option.font);
	QColor titleColor = qvariant_cast<QColor>(index.data(Qt::ForegroundRole));
	QRect titleRect = QStyle::alignedRect(option.direction,
																				option.decorationPosition == QStyleOptionViewItem::Left ?
																				Qt::AlignRight : Qt::AlignLeft,
																				option.rect.size(),
																				option.rect);

	if (titleColor.isValid()) {
		painter->setPen(titleColor);
	} else {
		painter->setPen(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
	}
	painter->setFont(titleFont);
	painter->drawText(titleRect, Qt::AlignCenter, titleText);
}

#include <prayerdelegate.moc>
