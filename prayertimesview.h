
#ifndef PRAYERTIMESVIEW_H
#define PRAYERTIMESVIEW_H

#include <Plasma/TreeView>

#include "dataengine/prayertimesengine.h"
class PrayerDelegate;

class QGraphicsSceneResizeEvent;
class QGraphicsWidget;

class PrayerTimesView : public Plasma::TreeView
{
	Q_OBJECT

	public:
		explicit PrayerTimesView(QGraphicsWidget *parent = 0);
		~PrayerTimesView();

		PrayerTime currentPrayer() const;
		void setCurrentPrayer(PrayerTime prayer);

	protected:
		void resizeEvent(QGraphicsSceneResizeEvent *event);
		virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint) const;

	private:
		PrayerDelegate *m_delegate;
};

#endif // PRAYERTIMESVIEW_H
