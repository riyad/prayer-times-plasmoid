
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

	private:
		PrayerDelegate *m_delegate;
};

#endif // PRAYERTIMESVIEW_H
