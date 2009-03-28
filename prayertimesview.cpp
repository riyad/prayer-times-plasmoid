//Own
#include "prayertimesview.h"

#include "prayerdelegate.h"

// Qt
#include <QGraphicsSceneResizeEvent>
#include <QGraphicsProxyWidget>
#include <QGraphicsWidget>
#include <QHeaderView>
#include <QTreeView>

// KDE
#include <KIconLoader>

// Plasma
#include <Plasma/TreeView>

PrayerTimesView::PrayerTimesView(QGraphicsWidget* parent)
	: Plasma::TreeView(parent)
{
	QTreeView *native = nativeWidget();
	native->viewport()->setAutoFillBackground(false);
	native->header()->hide();
	native->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	native->setIconSize(QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium));
	native->setRootIsDecorated(false);

	m_delegate = new PrayerDelegate(this);
	native->setItemDelegate(m_delegate);
}

PrayerTimesView::~PrayerTimesView()
{

}

void PrayerTimesView::resizeEvent(QGraphicsSceneResizeEvent* event)
{
	QGraphicsProxyWidget::resizeEvent(event);

	const int newWidth = size().width() / nativeWidget()->header()->count();

	for (int i = 0; i < nativeWidget()->header()->count(); ++i) {
		nativeWidget()->header()->resizeSection(i, newWidth);
	}

	int iconSize = int(KIconLoader::SizeSmall);
	if (model()) {
		iconSize = qMax(iconSize, int(size().height()/model()->rowCount()));
	}
	nativeWidget()->setIconSize(QSize(iconSize, iconSize));
}

#include <prayertimesview.moc>
