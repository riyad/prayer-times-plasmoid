/*
 * compasswidget.cpp
 *
 * Copyright (C) 2008 %{AUTHOR} <%{EMAIL}>
 */
#include "compasswidget.h"

#include <math.h>

#include <QColor>
#include <QPainter>
#include <QPaintEvent>
#include <QPointF>
#include <QRectF>
#include <QSvgRenderer>

CompassWidget::CompassWidget(QWidget* parent, Qt::WindowFlags wFlags)
	: QWidget(parent, wFlags),
	m_compassSvg(0),
	m_needle(0)
{
	m_compassSvg = new QSvgRenderer(this);
	m_compassSvg->load(QString("/home/riyad/src/CompassWidget/compass.svg"));
}

void CompassWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);

	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::Antialiasing);

	QRectF viewBox = m_compassSvg->viewBox();
	QRectF layerRect = m_compassSvg->boundsOnElement("layer1");
	QRectF backgroundRect = m_compassSvg->boundsOnElement("background");
	QRectF outerRingRect = m_compassSvg->boundsOnElement("outer_ring");
	QRectF shadingRect = m_compassSvg->boundsOnElement("shading");
	QRectF degreesRect = m_compassSvg->boundsOnElement("degrees");
	QRectF orientationRect = m_compassSvg->boundsOnElement("orientation");
	QRectF needleRect = m_compassSvg->boundsOnElement("needle");
	QRectF glossRect = m_compassSvg->boundsOnElement("gloss");
			QPointF compassCenterPoint = backgroundRect.topLeft() + QPointF(backgroundRect.width(), backgroundRect.height())/2;

	double widgetSide = qMin(size().width(), size().height());
	double imageSide = qMin(viewBox.width(), viewBox.height());

	painter.scale(widgetSide/imageSide, widgetSide/imageSide);

	painter.save();
	{
		painter.translate(-2.5*layerRect.topLeft());

		m_compassSvg->render(&painter, "background", backgroundRect);
		m_compassSvg->render(&painter, "outer_ring", outerRingRect);
		m_compassSvg->render(&painter, "shading", shadingRect);
		m_compassSvg->render(&painter, "degrees", degreesRect);
		m_compassSvg->render(&painter, "orientation", orientationRect);

		painter.save();
		{
			painter.translate(compassCenterPoint);
			painter.rotate(needle());
			m_compassSvg->render(&painter, "needle", needleRect.translated(-compassCenterPoint));
		}
		painter.restore();

		m_compassSvg->render(&painter, "gloss", glossRect);
	}
	painter.restore();
}

double CompassWidget::needle() const
{
	return m_needle;
}

void CompassWidget::setNeedle(const double degrees)
{
	m_needle = degrees;
}

#include "compasswidget.moc"
