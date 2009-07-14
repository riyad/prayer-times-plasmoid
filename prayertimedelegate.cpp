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

// Own
#include "prayertimedelegate.h"

#include "dataengine/prayertimesengine.h"

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
	const int radius = 0.1*backgroundRect.width();
	bool drawLeft = false;
	bool drawRight = false;
	bool drawTop = false;
	bool drawBottom = false;

	QColor backgroundColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::BackgroundColor));

	backgroundColor.setAlphaF(0.1);

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
		QRect roundedBackgroundRect(backgroundRect);

		if (!drawLeft) {
			roundedBackgroundRect.setLeft(roundedBackgroundRect.left()-radius);
		}
		if (!drawRight) {
			roundedBackgroundRect.setRight(roundedBackgroundRect.right()+radius);
		}
		if (!drawTop) {
			roundedBackgroundRect.setTop(roundedBackgroundRect.top()-radius);
		}
		if (!drawBottom) {
			roundedBackgroundRect.setBottom(roundedBackgroundRect.bottom()+radius);
		}

		QPainterPath roundedBackgroundPath = Plasma::PaintUtils::roundedRectangle(roundedBackgroundRect, radius);

		QPainterPath backgroundClipPath;
		backgroundClipPath.addRect(backgroundRect);
		QPainterPath path = roundedBackgroundPath.intersected(backgroundClipPath);

		painter->setRenderHint(QPainter::Antialiasing);
		painter->setPen(Qt::NoPen);
		painter->setBrush(backgroundColor);
		painter->drawPath(path);
	}
}

void PrayerTimeDelegate::paintHighlight(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.row() != d->currentPrayer) {
		return;
	}

	const int columns = index.model()->columnCount();
	const QRect backgroundRect(option.rect);

	const int margin = 0.05*backgroundRect.height();
	const int radius = 0.05*backgroundRect.width();

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

	QColor topBgColor = highlightColor;
	QColor bottomBgColor = highlightColor;
	topBgColor.setAlphaF(0.2);
	bottomBgColor.setAlphaF(0.7);

	QLinearGradient bgGradient(backgroundRect.topLeft(), backgroundRect.bottomLeft());
	bgGradient.setColorAt(0, topBgColor);
	bgGradient.setColorAt(1, bottomBgColor);

	// calculate the highlight rect
	QRect roundedHighlightRect(backgroundRect);
	roundedHighlightRect.setTop(roundedHighlightRect.top() + margin);
	roundedHighlightRect.setBottom(roundedHighlightRect.bottom() - margin);

	if(drawLeft) {
		roundedHighlightRect.setLeft(roundedHighlightRect.left() + margin);
	} else {
		roundedHighlightRect.setLeft(roundedHighlightRect.left() - radius);
	}
	if(drawRight) {
		roundedHighlightRect.setRight(roundedHighlightRect.right() - margin);
	} else {
		roundedHighlightRect.setRight(roundedHighlightRect.right() + radius);
	}

	QPainterPath roundedHightlightPath = Plasma::PaintUtils::roundedRectangle(roundedHighlightRect, radius);

	QPainterPath backgroundClipPath;
	backgroundClipPath.addRect(backgroundRect);
	QPainterPath path = roundedHightlightPath.intersected(backgroundClipPath);

	// paint at last
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(Qt::NoPen);

	// paint the hightlight background
	painter->setBrush(bgGradient);
	painter->drawPath(path);

	// paint stroke
	QPainterPathStroker stroker;
	stroker.setWidth(margin/2);
	QPainterPath strokePath = stroker.createStroke(roundedHightlightPath);

	strokePath = strokePath.intersected(backgroundClipPath);

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
	QColor titleColor = index.data(Qt::ForegroundRole).value<QColor>();
	if (!titleColor.isValid()) {
		titleColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
	}
	painter->setPen(titleColor);
	painter->setFont(titleFont);
	painter->drawText(titleRect, Qt::AlignCenter, titleText);
}

#include <prayertimedelegate.moc>
