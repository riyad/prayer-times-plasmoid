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

#include "prayertimedelegate.h"

// Qt
#include <QPainter>

// Plasma
#include <Plasma/PaintUtils>
#include <Plasma/Theme>

class PrayerTimeDelegatePrivate
{
public:
    PrayerTimeDelegatePrivate()
        : currentPrayer(NextFajr)
    {
    }

    ~PrayerTimeDelegatePrivate() {
    }

    PrayerTime currentPrayer;
};

PrayerTimeDelegate::PrayerTimeDelegate(QObject* parent)
	: QAbstractItemDelegate(parent),
	d(new PrayerTimeDelegatePrivate)
{
}

PrayerTimeDelegate::~PrayerTimeDelegate()
{
	delete d;
}

PrayerTime PrayerTimeDelegate::currentPrayer() {
	return d->currentPrayer;
}

void PrayerTimeDelegate::setCurrentPrayer(PrayerTime prayer) {
	d->currentPrayer = prayer;
}

QSize PrayerTimeDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize size = option.rect.size();

	QFontMetrics metrics(option.font);

	size.setHeight(qMax(option.decorationSize.height(), metrics.height()));

	size.setWidth(/*option.decorationSize.width() +*/ metrics.width(index.data().toString()));

	return size;
}

void PrayerTimeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	paintBackground(painter, option, index);
	paintHighlight(painter, option, index);
	paintText(painter, option, index);
}

void PrayerTimeDelegate::paintBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QRect backgroundRect(option.rect);

	const int rows = index.model()->rowCount();
	const int columns = index.model()->columnCount();
	const int radius = 0.3*backgroundRect.height(); // radius for rounded corners
	bool drawLeftBorder = false;
	bool drawRightBorder = false;
	bool drawTopBorder = false;
	bool drawBottomBorder = false;

	QColor backgroundColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
	backgroundColor.setAlphaF(0.1);

	// drawing left-most column
	if (index.column() == 0) {
		drawLeftBorder = true;
	}
	// drawing right-most column
	if (index.column() == columns - 1) {
		drawRightBorder = true;
	}
	// drawing top-most row
	if (index.row() == 0) {
		drawTopBorder = true;
	}
	// drawing bottom-most row
	if (index.row() == rows - 1) {
		drawBottomBorder = true;
	}

	// drawing something in the middle
	if (!drawLeftBorder && !drawRightBorder && !drawTopBorder && !drawBottomBorder) {
		painter->fillRect(backgroundRect, backgroundColor);
	} else { // drawing something on the border
		QRect roundedBackgroundRect(backgroundRect);

		// extend borders that are not going to be rounded
		if (!drawLeftBorder) {
			roundedBackgroundRect.setLeft(roundedBackgroundRect.left()-radius);
		}
		if (!drawRightBorder) {
			roundedBackgroundRect.setRight(roundedBackgroundRect.right()+radius);
		}
		if (!drawTopBorder) {
			roundedBackgroundRect.setTop(roundedBackgroundRect.top()-radius);
		}
		if (!drawBottomBorder) {
			roundedBackgroundRect.setBottom(roundedBackgroundRect.bottom()+radius);
		}

		// round background rect
		QPainterPath roundedBackgroundPath = Plasma::PaintUtils::roundedRectangle(roundedBackgroundRect, radius);

		// clip extended and rounded background rect with original beackground rect
		QPainterPath backgroundClipPath;
		backgroundClipPath.addRect(backgroundRect);
		QPainterPath path = roundedBackgroundPath.intersected(backgroundClipPath);

		// set up painter and paint
		painter->setRenderHint(QPainter::Antialiasing);
		painter->setPen(Qt::NoPen);
		painter->setBrush(backgroundColor);
		painter->drawPath(path);
	}
}

void PrayerTimeDelegate::paintHighlight(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// skip if this row is not highlighted
	if (index.row() != d->currentPrayer) {
		return;
	}

	const int columns = index.model()->columnCount();
	const QRect backgroundRect(option.rect);

	const int margin = 0.05*backgroundRect.height();
	const int radius = 0.3*backgroundRect.height();

	// determine which side we are painting
	bool drawLeft = false;
	bool drawRight = false;

	if (index.column() == 0) {
		drawLeft = true;
	} else if (index.column() == columns - 1) {
		drawRight = true;
	}

	// calculate the background gradient
	QColor highlightColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
	highlightColor.setAlphaF(0.3);

	QColor strokeColor = highlightColor;
	strokeColor.setAlphaF(0.5);

	QColor topBgColor = highlightColor.lighter(170);
	QColor bottomBgColor = highlightColor.darker(130);
	topBgColor.setAlphaF(0.2);
	bottomBgColor.setAlphaF(0.5);

	QLinearGradient bgGradient(backgroundRect.topLeft(), backgroundRect.bottomLeft());
	bgGradient.setColorAt(0, topBgColor);
	bgGradient.setColorAt(1, bottomBgColor);

	// abjust the size of the highlight marker
	QRect highlightRect(backgroundRect);
	highlightRect.setTop(highlightRect.top() + margin);
	highlightRect.setBottom(highlightRect.bottom() - margin);

	// add margin (for the stroke) to borders to be rounded
	// and extend borders that are are not going to be rounded
	if(drawLeft) {
		highlightRect.setLeft(highlightRect.left() + margin);
	} else {
		highlightRect.setLeft(highlightRect.left() - radius);
	}
	if(drawRight) {
		highlightRect.setRight(highlightRect.right() - margin);
	} else {
		highlightRect.setRight(highlightRect.right() + radius);
	}

	// round highlight rect
	QPainterPath roundedHightlightPath = Plasma::PaintUtils::roundedRectangle(highlightRect, radius);

	// clip extended and rounded highlight rect with original beackground rect
	QPainterPath backgroundClipPath;
	backgroundClipPath.addRect(backgroundRect);
	QPainterPath path = roundedHightlightPath.intersected(backgroundClipPath);

	// set up painter
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(Qt::NoPen);

	// paint the hightlight background
	painter->setBrush(bgGradient);
	painter->drawPath(path);

	// create highlight stroke from rounded highlight rect
	QPainterPathStroker stroker;
	stroker.setWidth(margin);
	QPainterPath strokePath = stroker.createStroke(roundedHightlightPath);

	// clip highlight stroke path with original beackground rect
	strokePath = strokePath.intersected(backgroundClipPath);

	// paint highlight stroke
	painter->setBrush(strokeColor);
	painter->drawPath(strokePath);
}

void PrayerTimeDelegate::paintText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QString titleText = index.data(Qt::DisplayRole).value<QString>();
	const QFont titleFont = index.data(Qt::FontRole).value<QFont>().resolve(option.font);
	const QRect titleRect = QStyle::alignedRect(option.direction
		, option.decorationPosition == QStyleOptionViewItem::Left ? Qt::AlignRight : Qt::AlignLeft
		, option.rect.size()
		, option.rect);
	painter->setPen(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
	painter->setFont(titleFont);
	painter->drawText(titleRect, Qt::AlignCenter, titleText);
}

#include <prayertimedelegate.moc>
