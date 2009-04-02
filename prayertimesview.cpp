//Own
#include "prayertimesview.h"

#include "dataengine/prayertimesengine.h"
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

PrayerTime PrayerTimesView::currentPrayer() const {
	m_delegate->currentPrayer();
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

QSizeF PrayerTimesView::sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const
{
	Q_UNUSED(which);
	Q_UNUSED(constraint);

	return QSizeF(0,0);
}

#include <prayertimesview.moc>
