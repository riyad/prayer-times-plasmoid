
#ifndef PRAYERTIMESVIEW_H
#define PRAYERTIMESVIEW_H

#include <Plasma/TreeView>

#include "dataengine/prayertimesengine.h"
class PrayerTimeDelegate;

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
		PrayerTimeDelegate *m_delegate;
};

#endif // PRAYERTIMESVIEW_H
