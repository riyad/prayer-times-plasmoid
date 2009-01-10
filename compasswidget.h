/*
 * compasswidget.h
 *
 * Copyright (C) 2008 %{AUTHOR} <%{EMAIL}>
 */
#ifndef COMPASSWIDGET_H
#define COMPASSWIDGET_H

#include <QWidget>

class QPaintEvent;
class QSvgRenderer;

class CompassWidget : public QWidget
{
	Q_OBJECT

	double m_needle;

	QSvgRenderer* m_compassSvg;

	public:
		CompassWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

		virtual void paintEvent(QPaintEvent* event);

		double needle() const;
		void setNeedle(const double degrees);
};

#endif // _COMPASSWIDGET_H_
