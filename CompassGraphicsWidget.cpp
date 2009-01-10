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

#include "CompassGraphicsWidget.h"

#include <math.h>

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

#include <Plasma/Svg>

CompassGraphicsWidget::CompassGraphicsWidget(QGraphicsItem* parent, Qt::WindowFlags wFlags)
	: QGraphicsWidget(parent, wFlags),
	m_prayertimesSvg(0)
{
	m_prayertimesSvg = new Plasma::Svg(this);
	m_prayertimesSvg->setImagePath("widgets/prayertimes");
	m_prayertimesSvg->setContainsMultipleImages(true);
}

void CompassGraphicsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QGraphicsWidget::paint(painter, option, widget);

	painter->setRenderHint(QPainter::SmoothPixmapTransform);
	painter->setRenderHint(QPainter::Antialiasing);

	double compassSize = qMin(size().width(), size().height());
	m_prayertimesSvg->resize(QSizeF(compassSize, compassSize));

	QRectF compassElementRect = m_prayertimesSvg->elementRect("compass");
	QRectF needleElementRect = m_prayertimesSvg->elementRect("compass_needle");
	QRectF pointElementRect = m_prayertimesSvg->elementRect("compass_point");

	QPointF compassTopleft((size().width()-compassSize)/2.0, (size().height()-compassSize)/2.0);
	QPointF pointTopleft((compassElementRect.width() - pointElementRect.width())/2.0,
		(compassElementRect.height() - pointElementRect.height())/2.0);
	QPointF needleTopleft((compassElementRect.width() - needleElementRect.width())/2.0, 0);

	double degrees = needle();
	double h = fabs(needleTopleft.y()-pointTopleft.y()-pointElementRect.height()/2.0);
	double x = needleTopleft.x()-(h*sin(-degrees*3.14/180.)+(needleElementRect.width()/2.0)*cos(-degrees*3.14/180.))+(needleElementRect.width()/2.0);
	double y = needleTopleft.y()+h-h*cos(-degrees*3.14/180.)+(needleElementRect.width()/2.0)*sin(-degrees*3.14/180.);

	painter->save();
	{
		painter->translate(compassTopleft);

		m_prayertimesSvg->paint(painter, 0, 0, "compass");

		painter->save();
		{
			painter->translate(x, y);
			painter->rotate(degrees);
			m_prayertimesSvg->paint(painter, QRectF(QPointF(0, 0), needleElementRect.size()), "compass_needle");
		}
		painter->restore();

		m_prayertimesSvg->paint(painter, pointTopleft, "compass_point");
	}
	painter->restore();
}

double CompassGraphicsWidget::needle() const
{
	return m_needle;
}

void CompassGraphicsWidget::setNeedle(const double degrees)
{
	m_needle = degrees;
}

#include "CompassGraphicsWidget.moc"
