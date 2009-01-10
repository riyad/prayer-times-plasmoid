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

#ifndef QIBLAGRAPHICSWIDGET_H
#define QIBLAGRAPHICSWIDGET_H

#include <QGraphicsWidget>

class QPainter;
class QWidget;
class QStyleOptionGraphicsItem;

namespace Plasma {
	class Svg;
}

class CompassGraphicsWidget : public QGraphicsWidget
{
	Q_OBJECT

	double m_needle;

	Plasma::Svg *m_comassSvg;

	public:
		CompassGraphicsWidget(QGraphicsItem *parent=0, Qt::WindowFlags wFlags=0);
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

		double needle() const;
		void setNeedle(const double degrees);
};

#endif // QIBLAGRAPHICSWIDGET_H
