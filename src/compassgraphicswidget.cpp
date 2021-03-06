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

#include "compassgraphicswidget.h"

// Qt
#include <QPainter>

// KDE
#include <KLocalizedString>

// Plasma
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

const QString& CompassGraphicsWidget::needleOrientation() const
{
	#define ORIENTATIONS 16
	static const QString orientations[ORIENTATIONS] = {i18nc("North", "N"),
		i18nc("North North East", "NNE"),
		i18nc("North East", "NE"),
		i18nc("East North East", "ENE"),
		i18nc("East", "E"),
		i18nc("East South East", "ESE"),
		i18nc("South East", "SE"),
		i18nc("South South East", "SSE"),
		i18nc("South", "S"),
		i18nc("South South West", "SSW"),
		i18nc("South West", "SW"),
		i18nc("West South West", "WSW"),
		i18nc("West", "W"),
		i18nc("West North West", "WNW"),
		i18nc("North West", "NW"),
		i18nc("North North West", "NNW")};

	static const double degreePerOrientation = 360/ORIENTATIONS;
	static const double degreeOffset = -degreePerOrientation/2;
	return orientations[(int)((needle()-degreeOffset)/degreePerOrientation)];
}

void CompassGraphicsWidget::setNeedle(const double degrees)
{
	m_needle = degrees;
}

#include "compassgraphicswidget.moc"
