/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "QiblaGraphicsWidget.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

#include <Plasma/Svg>

QiblaGraphicsWidget::QiblaGraphicsWidget(QGraphicsItem* parent, Qt::WindowFlags wFlags)
	: QGraphicsWidget(parent, wFlags),
	m_prayertimesSvg(0)
{
	m_prayertimesSvg = new Plasma::Svg(this);
	m_prayertimesSvg->setImagePath("widgets/prayertimes");
	m_prayertimesSvg->setContainsMultipleImages(true);
}

void QiblaGraphicsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  QGraphicsWidget::paint(painter, option, widget);

	double compassSize = qMin(size().width(), size().height());
	m_prayertimesSvg->resize(QSizeF(compassSize, compassSize));

	QRectF compassElementRect = m_prayertimesSvg->elementRect("compass");
	QPointF compassTopleft((size().width()-compassSize)/2.0, (size().height()-compassSize)/2.0);
	m_prayertimesSvg->paint(painter, compassTopleft, "compass");

	QRectF pointElementRect = m_prayertimesSvg->elementRect("compass_point");
	QPointF pointTopleft((compassElementRect.width() - pointElementRect.width())/2.0,
		(compassElementRect.height() - pointElementRect.height())/2.0);
	m_prayertimesSvg->paint(painter, compassTopleft + pointTopleft, "compass_point");

	QRectF needleElementRect = m_prayertimesSvg->elementRect("compass_needle");
	m_prayertimesSvg->paint(painter, 0, 0, "compass_needle");
}

#include "QiblaGraphicsWidget.moc"
