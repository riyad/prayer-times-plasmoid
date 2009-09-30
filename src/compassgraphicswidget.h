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
/**
 * @file
 * Contains a graphis widget showing a themable compass with a movable needle.
 */

#ifndef QIBLAGRAPHICSWIDGET_H
#define QIBLAGRAPHICSWIDGET_H

// Qt
#include <QGraphicsWidget>

// Plasma
namespace Plasma {
	class Svg;
}

/**
 * @class CompassGraphicsWidget
 * A graphis widget showing a themable compass.
 * The needle can be set to show into the desired direction.
 */
class CompassGraphicsWidget : public QGraphicsWidget
{
	Q_OBJECT

	//! The direction the needle shoud be pointed to in degrees from north.
	double m_needle;

	//! The compass' SVG
	Plasma::Svg *m_compassSvg;

public:
	CompassGraphicsWidget(QGraphicsItem *parent=0, Qt::WindowFlags wFlags=0);

	/**
	 * Paints the layers of the compass SVG and orients the needle.
	 * @param painter The painter to use.
	 * @param option The style options to use.
	 * @param widget ???
	 */
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0);

	/**
		* Gives you the current needle direction.
		* @return The needle direction.
		*/
	double needle() const;

	/**
		* Gives you the current needle direction as a compass orientation (N for 0¡Æ or SE for 139¡Æ).
		* It support a 16 segment resolution (i.e. at most 3 characters like SSE or WNW).
		* North is defined to be from -11.25¡Æ to +11.25¡Æ (mod 360¡Æ).
		* @return The orientation.
		*/
	const QString& needleOrientation() const;

	/**
		* Sets the direction the needle shoud be pointed to in degrees from north.
		* @param degrees The direction.
		*/
	void setNeedle(const double degrees);
};

#endif // QIBLAGRAPHICSWIDGET_H
