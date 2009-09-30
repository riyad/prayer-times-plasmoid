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

#include "prayertimesview.h"

//Own
#include "prayertimedelegate.h"

// Qt
#include <QHeaderView>
#include <QTreeView>

// KDE
#include <KIconLoader>

PrayerTimesView::PrayerTimesView(QGraphicsWidget* parent)
	: Plasma::TreeView(parent)
{
	QTreeView *native = nativeWidget();
	native->viewport()->setAutoFillBackground(false);
	native->header()->hide();
	native->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	native->setIconSize(QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium));
	native->setRootIsDecorated(false);

	m_delegate = new PrayerTimeDelegate(this);
	native->setItemDelegate(m_delegate);
}

PrayerTimesView::~PrayerTimesView()
{
}

PrayerTime PrayerTimesView::currentPrayer() const {
	return m_delegate->currentPrayer();
}

void PrayerTimesView::setCurrentPrayer(PrayerTime prayer) {
	m_delegate->setCurrentPrayer(prayer);
}

void PrayerTimesView::resizeEvent(QGraphicsSceneResizeEvent* event)
{
	QGraphicsProxyWidget::resizeEvent(event);

	const int newHeaderSectionWidth = size().width() / nativeWidget()->header()->count();

	for (int i = 0; i < nativeWidget()->header()->count(); ++i) {
		nativeWidget()->header()->resizeSection(i, newHeaderSectionWidth);
	}

	int newIconSize = int(KIconLoader::SizeSmall);
	if (model()) {
		newIconSize = qMax(newIconSize, int(size().height()/model()->rowCount()));
	}
	nativeWidget()->setIconSize(QSize(newIconSize, newIconSize));
}

#include <prayertimesview.moc>
