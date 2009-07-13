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
	QRect backgroundRect(option.rect);

	const int rows = index.model()->rowCount();
	const int columns = index.model()->columnCount();
	const int radius = 0.1*backgroundRect.width();
	bool drawLeft = false;
	bool drawRight = false;
	bool drawTop = false;
	bool drawBottom = false;

	QColor backgroundColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));

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
}

void PrayerTimeDelegate::paintHighlight(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const int columns = index.model()->columnCount();
	const QRect backgroundRect(option.rect);

	if (index.row() == d->currentPrayer) {
		const int lineHeight = 0.02*backgroundRect.height();
		QRect topLineRect(backgroundRect);
		topLineRect.setBottom(topLineRect.top() + lineHeight);
		QRect bottomLineRect(backgroundRect);
		bottomLineRect.setTop(bottomLineRect.bottom() - lineHeight);

		painter->setPen(Qt::NoPen);

		QColor outerLineColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::HighlightColor);
		QColor innerLineColor = outerLineColor;
		outerLineColor.setAlphaF(0);
		innerLineColor.setAlphaF(0.3);

		QColor outerBgColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::HighlightColor);
		QColor innerBgColor = outerBgColor;
		outerBgColor.setAlphaF(0);
		innerBgColor.setAlphaF(0.5);

		QLinearGradient topLineGradient(topLineRect.topLeft(), topLineRect.bottomRight());
		QLinearGradient bgGradient(backgroundRect.topLeft(), backgroundRect.bottomRight());
		QLinearGradient bottomLineGradient(bottomLineRect.topLeft(), bottomLineRect.bottomRight());
		if (index.column() == 0) {
			topLineGradient.setColorAt(0, outerLineColor);
			topLineGradient.setColorAt(0.5, innerLineColor);
			bgGradient.setColorAt(0, outerBgColor);
			bgGradient.setColorAt(0.5, innerBgColor);
			bottomLineGradient.setColorAt(0, outerLineColor);
			bottomLineGradient.setColorAt(0.5, innerLineColor);
		} else if (index.column() == columns - 1) {
			topLineGradient.setColorAt(0.5, innerLineColor);
			topLineGradient.setColorAt(1, outerLineColor);
			bgGradient.setColorAt(0.5, innerBgColor);
			bgGradient.setColorAt(1, outerBgColor);
			bottomLineGradient.setColorAt(0.5, innerLineColor);
			bottomLineGradient.setColorAt(1, outerLineColor);
		}

			painter->setBrush(bgGradient);
			painter->drawRect(backgroundRect);

			if(d->currentPrayer != Fajr) {
				painter->setBrush(topLineGradient);
				painter->drawRect(topLineRect);
			}
			if(d->currentPrayer != Ishaa) {
				painter->setBrush(bottomLineGradient);
				painter->drawRect(bottomLineRect);
			}
	}
}

void PrayerTimeDelegate::paintText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QString titleText = index.data(Qt::DisplayRole).value<QString>();
	const QFont titleFont = index.data(Qt::FontRole).value<QFont>().resolve(option.font);
	QColor titleColor = index.data(Qt::ForegroundRole).value<QColor>();
	const QRect titleRect = QStyle::alignedRect(option.direction,
																				option.decorationPosition == QStyleOptionViewItem::Left ?
																				Qt::AlignRight : Qt::AlignLeft,
																				option.rect.size(),
																				option.rect);

	if (!titleColor.isValid()) {
		titleColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
	}
	painter->setPen(titleColor);
	painter->setFont(titleFont);
	painter->drawText(titleRect, Qt::AlignCenter, titleText);
}

#include <prayertimedelegate.moc>
