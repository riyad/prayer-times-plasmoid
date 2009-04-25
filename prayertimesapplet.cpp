// Own
#include "prayertimesapplet.h"

#include "compassgraphicswidget.h"
#include "prayertimesview.h"

// Qt
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QList>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTextOption>
#include <QTime>
#include <QTimer>

// KDE
#include <KConfigDialog>
#include <KIconLoader>

// Plasma
#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include <Plasma/Frame>
#include <Plasma/IconWidget>
#include <Plasma/Label>
#include <Plasma/Svg>
#include <Plasma/TabBar>
#include <Plasma/Theme>

// Marble
#include <marble/MarbleModel.h>
#include <marble/MarbleWidget.h>

PrayerTimesApplet::PrayerTimesApplet(QObject *parent, const QVariantList &args)
	: Plasma::Applet(parent, args),
	m_locationName(""),
	m_latitude(21.431536), m_longitude(39.819145), // Makkah
	m_calculationMethod(5), // Muslim World League
	m_prayerTimesModel(0)
{
	// this will get us the standard applet background, for free!
	setBackgroundHints(DefaultBackground);

	m_prayertimesSvg = new Plasma::Svg(this);
	m_prayertimesSvg->setImagePath("prayertimes/prayertimes");
	m_prayertimesSvg->setContainsMultipleImages(true);

	setHasConfigurationInterface(true);

	setAspectRatioMode(Plasma::IgnoreAspectRatio);
	setMinimumSize(280, 220);

	m_locationLabel = new Plasma::Label(this);
	QFont titleFont = font();
	titleFont.setPointSize(titleFont.pointSize() * 1.6);
	titleFont.setBold(true);
	m_locationLabel->nativeWidget()->setFont(titleFont);
	m_locationLabel->nativeWidget()->setWordWrap(false);
	m_locationLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	QGraphicsLinearLayout *titleLayout = new QGraphicsLinearLayout();
	titleLayout->addItem(m_locationLabel);

	Plasma::Frame *titleFrame = new Plasma::Frame(this);
	titleFrame->setLayout(titleLayout);

	Plasma::TabBar* tabbar = new Plasma::TabBar(this);
	tabbar->addTab("Times", createPrayerTimesWidget());
	tabbar->addTab("Qibla", createQiblaWidget());

	QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);
	layout->addItem(titleFrame);
	layout->addItem(tabbar);
	setLayout(layout);

	resize(350, 220);
}


PrayerTimesApplet::~PrayerTimesApplet()
{
	if (hasFailedToLaunch()) {
		// Do some cleanup here
	} else {
		// Save settings
	}

	m_updateTimer->stop();
}

void PrayerTimesApplet::init()
{
	KConfigGroup cg = config();
	m_locationName = cg.readEntry("locationName", m_locationName);
	m_latitude = cg.readEntry("latitude", m_latitude);
	m_longitude = cg.readEntry("longitude", m_longitude);
	m_calculationMethod = cg.readEntry("calculationMethod", m_calculationMethod);

	connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(updateColors()));

	m_updateTimer = new QTimer(this);
	connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateInterface()));
	m_updateTimer->start(60*1000);

	connectSources();
}

void PrayerTimesApplet::dataUpdated(const QString &source, const Plasma::DataEngine::Data &data)
{
	Q_UNUSED(source);

	m_fajr = data["Fajr"].toTime();
	m_shorooq = data["Shorooq"].toTime();
	m_dhuhr = data["Dhuhr"].toTime();
	m_asr = data["Asr"].toTime();
	m_maghrib = data["Maghrib"].toTime();
	m_ishaa = data["Ishaa"].toTime();
	m_nextFajr = data["NextFajr"].toTime();

	m_qibla = data["Qibla"].toDouble();

	updateInterface();
}

void PrayerTimesApplet::createConfigurationInterface(KConfigDialog* parent)
{
	QWidget *locationWidget = new QWidget(parent);
	locationConfigUi.setupUi(locationWidget);

	parent->addPage(locationWidget, i18n("Location"), "marble");
	locationConfigUi.locationNameLineEdit->setText(m_locationName);

	Marble::MarbleWidget* map = locationConfigUi.mapWidget;
	map->setProjection(Marble::Spherical);
	//Set how we want the map to look
	map->centerOn(m_longitude, m_latitude);
	map->zoomView(map->maximumZoom());
	map->setMapThemeId( "earth/atlas/atlas.dgml" );
	map->setShowGrid       ( true );
	map->setShowPlaces     ( true );
	map->setShowOtherPlaces( false );
	//map->setShowBorders    ( true );
	map->setShowCities     ( true );
	map->setShowCompass    ( false );
	map->setShowCrosshairs ( true );
	map->setShowScaleBar   ( false );
	map->setShowClouds     ( false );

	connect(locationConfigUi.mapWidget, SIGNAL(mouseMoveGeoPosition(QString)), this, SLOT(configMouseGeoPositionChanged()));
	connect(locationConfigUi.mapWidget, SIGNAL(zoomChanged(int)), this, SLOT(configMouseGeoPositionChanged()));

	QWidget *methodWidget = new QWidget(parent);
	calculationMethodConfigUi.setupUi(methodWidget);

	parent->addPage(methodWidget, i18n("Calculation Method"), "prayertimes");
	calculationMethodConfigUi.methodComboBox->setCurrentIndex(m_calculationMethod);

	connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
	connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
}

void PrayerTimesApplet::configAccepted()
{
	disconnectSources();

	KConfigGroup cg = config();

	QString locationName = locationConfigUi.locationNameLineEdit->text();
	if(m_locationName != locationName) {
		m_locationName = locationName;
		cg.writeEntry("locationName", m_locationName);
	}

	double latitude = locationConfigUi.mapWidget->centerLatitude();
	if(m_latitude != latitude) {
		m_latitude = latitude;
		cg.writeEntry("latitude", m_latitude);
	}

	double longitude = locationConfigUi.mapWidget->centerLongitude();
	if(m_longitude != longitude) {
		m_longitude = longitude;
		cg.writeEntry("longitude", m_longitude);
	}

	double method = calculationMethodConfigUi.methodComboBox->currentIndex();
	if(m_calculationMethod != method) {
		m_calculationMethod = method;
		cg.writeEntry("calculationMethod", m_calculationMethod);
	}

	connectSources();

	updateInterface();

	emit configNeedsSaving();
}

void PrayerTimesApplet::configMouseGeoPositionChanged()
{
	Marble::MarbleWidget* map = locationConfigUi.mapWidget;
	qreal lon = locationConfigUi.mapWidget->centerLongitude();
	qreal lat = locationConfigUi.mapWidget->centerLatitude();
	qreal x, y;
	map->screenCoordinates(lon, lat, x, y);
	QVector<QModelIndex> featureList = map->model()->whichFeatureAt(QPoint(x, y));
	if(!featureList.isEmpty()) {
		locationConfigUi.locationNameLineEdit->setText(featureList.at(0).data().toString());
	}
}

void PrayerTimesApplet::updateInterface()
{
	QFont normalFont(font());

	QFont boldFont(font());
	boldFont.setBold(true);

	if(!m_prayerTimesModel) {
		m_prayerTimesModel = new QStandardItemModel(this);
	} else {
		m_prayerTimesModel->clear();
	}

	QList<QStandardItem *> prayerNameItems;
	QList<QStandardItem *> prayerTimeItems;

	for(PrayerTime prayer = Fajr; prayer <= Ishaa; prayer = PrayerTime(prayer+1)) {
		QStandardItem *prayerName = new QStandardItem();
		prayerName->setForeground(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
		prayerName->setFont(boldFont);
		prayerName->setText(labelFor(prayer));
		prayerNameItems.append(prayerName);

		QStandardItem *prayerTime = new QStandardItem();
		prayerTime->setFont(normalFont);
		prayerTime->setText(prayerTimeFor(prayer).toString("hh:mm"));
		prayerTimeItems.append(prayerTime);
	}

	m_prayerTimesModel->appendColumn(prayerNameItems);
	m_prayerTimesModel->appendColumn(prayerTimeItems);

	if (!m_prayerTimesView->model()) {
		m_prayerTimesView->setModel(m_prayerTimesModel);
	}
	m_prayerTimesView->setCurrentPrayer(currentPrayer());

	int diffMSecs = QTime::currentTime().msecsTo(prayerTimeFor(nextPrayer()));
	QTime nextPrayerTime = QTime().addMSecs(diffMSecs);
	m_nextPrayerLabel->setText(i18nc("<time> to <prayer>", "%1 to %2", nextPrayerTime.toString("hh:mm"), labelFor(nextPrayer() == NextFajr ? Fajr : nextPrayer())));
	m_locationLabel->setText(m_locationName);
	m_dateLabel->setText(QDate::currentDate().toString());

	m_qiblaWidget->setNeedle(m_qibla);
	m_qiblaOrientationLabel->setText(i18nc("Qibla direction is <orientation>", "Qibla direction is %1", m_qiblaWidget->needleOrientation()));
}

QGraphicsWidget* PrayerTimesApplet::createPrayerTimesWidget()
{
	QFont titleFont = font();
	titleFont.setPointSize(titleFont.pointSize() * 1.1);

	QGraphicsGridLayout* layout = new QGraphicsGridLayout();

	layout->setRowStretchFactor(1, 4);

	Plasma::IconWidget* kaabaIconWidget = new Plasma::IconWidget(this);
	kaabaIconWidget->setSvg(m_prayertimesSvg->imagePath(), "kaaba");
	layout->addItem(kaabaIconWidget, 0, 0, 2, 1);

	m_dateLabel = new Plasma::Label(this);
	m_dateLabel->setAlignment(Qt::AlignCenter);
	m_dateLabel->nativeWidget()->setFont(titleFont);
	layout->addItem(m_dateLabel, 0, 1);

	m_prayerTimesView = new PrayerTimesView(this);
	m_prayerTimesView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	layout->addItem(m_prayerTimesView, 1, 1, 2, 1);

	m_nextPrayerLabel = new Plasma::Label(this);
	m_nextPrayerLabel->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	m_nextPrayerLabel->nativeWidget()->setFont(titleFont);
	layout->addItem(m_nextPrayerLabel, 2, 0);

	QGraphicsWidget* widget = new QGraphicsWidget(this);
	widget->setLayout(layout);
	return widget;
}

QGraphicsWidget* PrayerTimesApplet::createQiblaWidget()
{
	QGraphicsGridLayout* layout = new QGraphicsGridLayout();
	layout->setRowStretchFactor(0, 1);
	layout->setRowStretchFactor(1, 6);

	m_qiblaOrientationLabel = new Plasma::Label(this);
	m_qiblaOrientationLabel->setAlignment(Qt::AlignCenter);
	m_qiblaOrientationLabel->setText("");
	layout->addItem(m_qiblaOrientationLabel, 0, 0, 1, 2);

	Plasma::IconWidget* kaabaIconWidget = new Plasma::IconWidget(this);
	kaabaIconWidget->setSvg(m_prayertimesSvg->imagePath(), "kaaba");
	layout->addItem(kaabaIconWidget, 1, 0);

	m_qiblaWidget = new CompassGraphicsWidget(this);
	layout->addItem(m_qiblaWidget, 1, 1);

	QGraphicsWidget* widget = new QGraphicsWidget(this);
	widget->setLayout(layout);
	return widget;
}


void PrayerTimesApplet::connectSources()
{
	if(m_locationName.isEmpty()) {
		setConfigurationRequired(true, i18n("Please setup your location."));
	} else {
		Plasma::DataEngine* prayerTimesEngine;
		prayerTimesEngine = dataEngine("prayertimes");
		prayerTimesEngine->connectSource(sourceName(), this, 1000*60, Plasma::AlignToMinute);
		setConfigurationRequired(false);
	}
}

void PrayerTimesApplet::disconnectSources()
{
	Plasma::DataEngine* prayerTimesEngine;
	prayerTimesEngine = dataEngine("prayertimes");
	prayerTimesEngine->disconnectSource(sourceName(), this);
}

const QString PrayerTimesApplet::sourceName() const
{
	return QString("%1/%2,%3").arg(calculationMethodName[m_calculationMethod]).arg(m_latitude).arg(m_longitude);
}

PrayerTime PrayerTimesApplet::currentPrayer() const
{
	for(PrayerTime prayer=Fajr; prayer <= Ishaa; prayer = PrayerTime(prayer+1)) {
		if(prayerTimeFor(prayer) <= QTime::currentTime() && QTime::currentTime() < prayerTimeFor(PrayerTime(prayer+1))) {
			return prayer;
		}
	}
	return Ishaa;
}

const QString& PrayerTimesApplet::labelFor(PrayerTime prayer)
{
	static const QString labels[PRAYER_TIMES] = {i18n("Fajr"),
		i18n("Shuruq"),
		i18n("Dhuhr"),
		i18n("Asr"),
		i18n("Maghrib"),
		i18n("Ishaa"),
		i18n("Next Fajr")};

	return labels[prayer];
}

PrayerTime PrayerTimesApplet::nextPrayer() const
{
	return PrayerTime(currentPrayer()+1);
}

const QTime& PrayerTimesApplet::prayerTimeFor(PrayerTime prayer) const
{
	const QTime* times[PRAYER_TIMES] = {&m_fajr,
		&m_shorooq,
		&m_dhuhr,
		&m_asr,
		&m_maghrib,
		&m_ishaa,
		&m_nextFajr};

	return *times[prayer];
}

#include "prayertimesapplet.moc"
