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

#include "compassgraphicswidget.h"

#include <QPainter>
#include <QPointF>
#include <QRectF>

#include <Plasma/Svg>

CompassGraphicsWidget::CompassGraphicsWidget(QGraphicsItem* parent, Qt::WindowFlags wFlags)
	: QGraphicsWidget(parent, wFlags),
	m_needle(0),
	m_compassSvg(0)
{
	m_compassSvg = new Plasma::Svg(this);
	m_compassSvg->setImagePath("widgets/compass");
	m_compassSvg->setContainsMultipleImages(true);
}

void CompassGraphicsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setRenderHint(QPainter::SmoothPixmapTransform);
	painter->setRenderHint(QPainter::Antialiasing);

	double widgetSide = qMin(size().width(), size().height());
	m_compassSvg->resize(widgetSide, widgetSide);

	QRectF layerRect = m_compassSvg->elementRect("layer1");
	QRectF backgroundRect = m_compassSvg->elementRect("background");
	QRectF outerRingRect = m_compassSvg->elementRect("outer_ring");
	QRectF shadingRect = m_compassSvg->elementRect("shading");
	QRectF degreesRect = m_compassSvg->elementRect("degrees");
	QRectF orientationRect = m_compassSvg->elementRect("orientation");
	QRectF needleRect = m_compassSvg->elementRect("needle");
	QRectF glossRect = m_compassSvg->elementRect("gloss");
	QPointF compassCenterPoint = backgroundRect.topLeft() + QPointF(backgroundRect.width(), backgroundRect.height())/2;

	painter->translate((size().width()-widgetSide)/2, (size().height()-widgetSide)/2);

	painter->save();
	{
		painter->translate(-2.5*layerRect.topLeft());

		m_compassSvg->paint(painter, backgroundRect, "background");
		m_compassSvg->paint(painter, outerRingRect, "outer_ring");
		m_compassSvg->paint(painter, shadingRect, "shading");
		m_compassSvg->paint(painter, degreesRect, "degrees");

		painter->save();
		{
			painter->translate(compassCenterPoint);
			painter->rotate(0); // possibility to rotate orientation ring
			m_compassSvg->paint(painter, orientationRect.translated(-compassCenterPoint), "orientation");
		}
		painter->restore();

		painter->save();
		{
			painter->translate(compassCenterPoint);
			painter->rotate(needle());
			m_compassSvg->paint(painter, needleRect.translated(-compassCenterPoint), "needle");
		}
		painter->restore();

		m_compassSvg->paint(painter, glossRect, "gloss");
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

#include "compassgraphicswidget.moc"
