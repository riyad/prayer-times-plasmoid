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

#ifndef QIBLAGRAPHICSWIDGET_H
#define QIBLAGRAPHICSWIDGET_H

// Qt
#include <QGraphicsWidget>

// Plasma
namespace Plasma {
	class Svg;
}

class CompassGraphicsWidget : public QGraphicsWidget
{
	Q_OBJECT

	double m_needle;

	Plasma::Svg *m_compassSvg;

	public:
		CompassGraphicsWidget(QGraphicsItem *parent=0, Qt::WindowFlags wFlags=0);

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0);

		double needle() const;
		const QString& needleOrientation() const;
		void setNeedle(const double degrees);
};

#endif // QIBLAGRAPHICSWIDGET_H
