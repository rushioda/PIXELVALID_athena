/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1ExecutionScheduler          //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
//  History:                                               //
//  R.M.Bianchi <rbianchi@cern.ch> - Feb 2013              //
//                                                         //
/////////////////////////////////////////////////////////////

//TODO:
//
//  * Save sys2time data in .vp1 file
//  * Disable custom controllers between events!
//  * Integrate with cruise modes (tab and event)
//  * In tab cruise mode, dont go to next tab until it is ready. Thus,
//    we need to be able to ask the scheduler if a given tab is ready.
//  * "Soon to be visible" priority
//  * General performance optimisation (inline internal methods +
//    cache iterators).
//  * Take care of accumulators.
//  * Accumulate cruise mode (with ETA if it is a given number of events).

/* this to fix the 'ptrdiff_t' does not name a type error with Qt:
 * refs:
 * - http://qt-project.org/forums/viewthread/16992
 * - http://stackoverflow.com/questions/6727193/g-4-6-complains-about-iterator-difference-type-g-4-4-and-visual-studio-don
 */
// #include <stddef.h>


#include "VP1Gui/VP1ExecutionScheduler.h"
#include "VP1Gui/VP1AvailEvents.h"
#include "VP1Gui/VP1AvailEvtsHttp.h"
#include "VP1Gui/VP1AvailEvtsHttps.h"
#include "VP1Gui/VP1AvailEvtsLocalDir.h"
#include "VP1Gui/VP1Authenticator.h"
#include "VP1Gui/VP1Prioritiser.h"
#include "VP1Gui/VP1MainWindow.h"
#include "VP1Gui/VP1ChannelManager.h"
#include "VP1Gui/VP1TabManager.h"

#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/IVP1ChannelWidget.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1HelperClassBase.h"
#include "VP1Base/VP1AthenaPtrs.h"
#include "VP1Base/VP1Settings.h"

#include <QtGui/QApplication>
#include <QtGui/QProgressBar>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QCursor>
#include <QtCore/QTimer>
#include <QtCore/QSet>
#include <QtGui/QMessageBox>

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/Qt/SoQt.h>

#include <set>
#include <cassert>
#include <iostream>     //For "widgetcount" output
#include <sstream>      // std::ostringstream
#include <ctime>
#include <stdexcept>


//___________________________________________________________________
class VP1ExecutionScheduler::Imp {
public:
	class GlobalEventFilter;//Used to animate mouse clicks (for screencasts);
	GlobalEventFilter * globalEventFilter;

	VP1ExecutionScheduler * scheduler;
	VP1Prioritiser * prioritiser;
	VP1MainWindow* mainwindow;

	bool batchMode;
	bool batchModeAllEvents;

	VP1AvailEvents * availEvents;

	QTimer * refreshtimer;
	IVP1System* currentsystemrefreshing;
	bool allSystemsRefreshed;
	bool goingtonextevent;

	//Statusbar:
	QProgressBar * pb;
	double calctimethisevent;
	double currentrefreshsystemestimate;
	void updateProgressBar();
	QTimer * pbtimer;

	//When receiving erase requests for a system that is currently refreshing, we use this:
	bool eraseJustAfterRefresh;
	IVP1ChannelWidget* postponedUncreateAndDeleteCW;

	CruiseMode cruisemode;
	QTimer * cruisetimer;
	bool allVisibleRefreshed() const;
	bool allSoonVisibleRefreshed() const;
	void initCruise();
	void performPostRefreshCruiseActions(IVP1ChannelWidget*cw);
	bool cruisetab_waitingtoproceed;

	static void warnIfWidgetsAlive();

	QString nextRequestedEvent;

	bool skipEvent;
};

//___________________________________________________________________
class VP1ExecutionScheduler::Imp::GlobalEventFilter : public QObject {
public:
	GlobalEventFilter():m_lastPopup(QTime::currentTime()),
	m_lastPopupWasQMenu(false){}
	bool eventFilter ( QObject * watched, QEvent * event ) {
		if (event->type()==QEvent::MouseButtonPress
				||event->type()==QEvent::MouseButtonDblClick
				||event->type()==QEvent::Wheel) {
			QTime t = QTime::currentTime();
			int timediff(abs(t.msecsTo(m_lastPopup)));
			int timecut(m_lastPopupWasQMenu?300:100);
			if (timediff>timecut) {
				m_lastPopup = t;
				QString txt;
				QMouseEvent*mouseEvent = dynamic_cast<QMouseEvent*>(event);
				if (mouseEvent) {
					txt = event->type()==QEvent::MouseButtonDblClick?"Dbl-click"
							:(mouseEvent->button()&Qt::LeftButton?"Left-click"
									:(mouseEvent->button()&Qt::RightButton?"Right-click":"Middle-click"));
				} else {
					QWheelEvent * wheelEvent = dynamic_cast<QWheelEvent*>(event);
					if (wheelEvent)
						txt = "wheel";
					else
						txt = "Unknown event";
				}
				// 	std::cout<<"Popup (dt="<<timediff<<") "<<txt.toStdString()<<". watched = "<<watched
				// 		 <<" (on="<<watched->objectName().toStdString()<<")"
				// 		 <<" (cn="<<watched->metaObject()->className()<<")"
				// 		 <<std::endl;
				QLabel * label = new QLabel(txt,0,Qt::Tool|Qt::FramelessWindowHint
						|Qt::X11BypassWindowManagerHint|Qt::WindowStaysOnTopHint);
				label->setStyleSheet("background-color: yellow;color: black;"
						"font: bold 140%;border: 2px solid black");
				//"border-radius: 3px;"
				label->setFrameStyle(QFrame::StyledPanel);
				label->setAttribute(Qt::WA_ShowWithoutActivating);
				label->setFocusPolicy(Qt::NoFocus);
				QPoint p(QCursor::pos().x()-label->sizeHint().width()/2,QCursor::pos().y()-label->sizeHint().height());
				label->move(p);
				QTimer::singleShot(0,label,SLOT(show()));
				QTimer::singleShot(500, label, SLOT(deleteLater()));
				m_lastPopupWasQMenu = (dynamic_cast<QMenu*>(watched)!=0);
			}
		}
		return false;
	}
private:
	QTime m_lastPopup;
	bool m_lastPopupWasQMenu;
};

//___________________________________________________________________
VP1ExecutionScheduler::VP1ExecutionScheduler( QObject * parent,
		StoreGateSvc* eventStore,StoreGateSvc* detStore,
		ISvcLocator* svcLocator,IToolSvc*toolSvc,
		VP1AvailEvents * availEvents)
: QObject(parent), d(new Imp)
{
	d->availEvents = availEvents;
	VP1AthenaPtrs::setPointers(eventStore,detStore,svcLocator,toolSvc);

	d->scheduler = this;
	d->prioritiser = new VP1Prioritiser(this);
	d->mainwindow = new VP1MainWindow(this,availEvents);//mainwindow takes ownership of available events
	d->batchMode = false;
	d->batchModeAllEvents = false;
	d->allSystemsRefreshed = false;
	d->goingtonextevent=true;
	d->currentsystemrefreshing=0;
	d->eraseJustAfterRefresh=false;
	d->postponedUncreateAndDeleteCW=0;
	d->refreshtimer = new QTimer(this);
	connect(d->refreshtimer, SIGNAL(timeout()), this, SLOT(processSystemForRefresh()));

	//Connect signals to ensure that prioritiser knows about present channels and their visibility:
	connect(d->mainwindow->channelManager(),SIGNAL(newChannelCreated(IVP1ChannelWidget*)),d->prioritiser, SLOT(channelCreated(IVP1ChannelWidget*)));
	connect(d->mainwindow->channelManager(),SIGNAL(channelUncreated(IVP1ChannelWidget*)),d->prioritiser, SLOT(channelUncreated(IVP1ChannelWidget*)));
	connect(d->mainwindow->channelManager(),SIGNAL(newChannelCreated(IVP1ChannelWidget*)),this, SLOT(channelCreated(IVP1ChannelWidget*)));
	connect(d->mainwindow->channelManager(),SIGNAL(channelUncreated(IVP1ChannelWidget*)),this, SLOT(channelUncreated(IVP1ChannelWidget*)));

	connect(d->mainwindow->tabManager(),SIGNAL(visibleChannelsChanged(const QSet<IVP1ChannelWidget*>&,const QSet<IVP1ChannelWidget*>&,const double&)),
			d->prioritiser,SLOT(visibleChannelsChanged(const QSet<IVP1ChannelWidget*>&,const QSet<IVP1ChannelWidget*>&,const double&)));


	// Init and show the main window of VP1
	SoQt::init( d->mainwindow );// SoQt::init( "VP1" );
	bool batchMode = VP1QtUtils::environmentVariableIsSet("VP1_BATCHMODE"); // ::getenv("VP1_BATCHMODE");
	qDebug() << "VP1ExecutionScheduler:: Do we run in 'batch' mode?" << batchMode;
	if (batchMode) {
		VP1Msg::messageWarningAllRed("User has run VP1 in 'batch-mode', so the main window of the program will not be shown.");
		d->batchMode = true;
	}
	else {
		d->mainwindow->show();
	}

    // If in batch mode, let's see if the user set the "all events" options as well
	d->batchModeAllEvents = VP1QtUtils::environmentVariableIsSet("VP1_BATCHMODE_ALLEVENTS"); // ::getenv("VP1_BATCHMODE");


	d->pb = d->mainwindow->progressbar;
	d->pbtimer = new QTimer(this);
	connect(d->pbtimer, SIGNAL(timeout()), this, SLOT(updateProgressBarDuringRefresh()));
	d->calctimethisevent=0;
	d->currentrefreshsystemestimate=0;

	d->cruisemode = NONE;
	d->cruisetimer = new QTimer(this);
	connect(d->cruisetimer, SIGNAL(timeout()), this, SLOT(performCruise()));
	d->cruisetab_waitingtoproceed=false;

	if (VP1QtUtils::environmentVariableIsOn("VP1_DISPLAY_MOUSE_CLICKS")) {
		d->globalEventFilter = new Imp::GlobalEventFilter;
		qApp->installEventFilter(d->globalEventFilter);
	} else {
		d->globalEventFilter = 0;
	}

	VP1AvailEvtsHttps* availEvtsHttps = dynamic_cast<VP1AvailEvtsHttps*>(availEvents);
	if(availEvtsHttps) {
		d->skipEvent = true;
		VP1Authenticator* auth = new VP1Authenticator(d->mainwindow,availEvtsHttps->fileinfoLocation());

		connect(auth,SIGNAL(authenticationSuccessful(QNetworkAccessManager*)),
				availEvtsHttps,SLOT(start(QNetworkAccessManager*)));
		connect(availEvtsHttps,SIGNAL(freshEventsChanged()),
				auth,SLOT(accept()));

		SoQt::init(auth);
		auth->exec();
		delete auth;
	}
	else
		d->skipEvent = false;
}

//___________________________________________________________________
VP1ExecutionScheduler::~VP1ExecutionScheduler()
{
	d->refreshtimer->stop();
	delete d->mainwindow;
	delete d->prioritiser;
	delete d->globalEventFilter;
	delete d;
}

//___________________________________________________________________
VP1ExecutionScheduler* VP1ExecutionScheduler::init( StoreGateSvc* eventStore, StoreGateSvc* detStore,
		ISvcLocator* svcLocator, IToolSvc*toolSvc,
		QStringList joboptions,
		QString initialCruiseMode,
		unsigned initialCruiseSeconds,
		QString singleEventSource,
		QString singleEventLocalTmpDir,
		unsigned localFileCacheLimit,
		QStringList availableLocalInputDirectories )
{
	//First we make sure the DISPLAY variable is set (importing ROOT in
	//athena.py might cause it to be unset!).
	//
	//NB: The following might only be relevant in X11 build:
	if (VP1QtUtils::environmentVariableValue("DISPLAY").isEmpty()) {
		const bool unset(!VP1QtUtils::environmentVariableIsSet("DISPLAY"));
		QString alternative = VP1QtUtils::environmentVariableValue("DISPLAY_ORIG");
		if (alternative.isEmpty()) {
			VP1Msg::message("ERROR: The DISPLAY environment variable is "+QString(unset?"not set":"empty")+".");
			VP1Msg::message("This is likely due to perfmon being turned on (cf. https://savannah.cern.ch/bugs/?35461 ).");
			VP1Msg::message("You can work around this problem by either disabling perfmon, or by setting "
					"the DISPLAY_ORIG environment variable to the contents of DISPLAY before launching your job.");
			VP1Msg::message("E.g., in bash do:");
			VP1Msg::message("  export DISPLAY_ORIG=$DISPLAY");
			VP1Msg::message("");
			VP1Msg::message("For the current job, I will try with DISPLAY=\":0.0\", which is the correct value when running locally.");
			alternative=":0.0";
		} else {
			VP1Msg::message("WARNING: The DISPLAY environment variable is "+QString(unset?"not set":"empty")+". Setting to value found in DISPLAY_ORIG");
		}
		VP1Msg::message("WARNING: Setting DISPLAY variable to '"+alternative+"'");
		VP1QtUtils::setEnvironmentVariable("DISPLAY",alternative);
	}

	// here we check if the main (and unique!) Qt application has been initialized already. If not we initialize it.
	if (qApp) {
		VP1Msg::message("VP1ExecutionScheduler::init ERROR: QApplication already initialized. Expect problems!!!");
	} else {
		//NB: Static to avoid scope-problems:
		static int argc=1;
		static char execpath[] = "/some/fake/executable/vp1";
		static char *argv[2];
		//    VP1Msg::messageDebug("setting argv[0]...");
		argv[0] = execpath;
		//    VP1Msg::messageDebug("setting argv[1]...");
		argv[1] = NULL;
		// instead of using the default Qt QApplication class,
		// we now use our custom inherited class where we
		// reimplemented the notify() method, in order to catch
		// C++ exceptions, especially while running it inside Athena.
		//new QApplication(argc, argv);
		new VP1QtApplication(argc, argv);
	}
	QCoreApplication::setOrganizationName("ATLAS");
	QCoreApplication::setApplicationName("VP1");

	VP1AvailEvents * availEvents(0);
	if (!singleEventSource.isEmpty()&&!singleEventLocalTmpDir.isEmpty()) {
		const bool httpmode = singleEventSource.startsWith("http://");
		const bool httpsmode = singleEventSource.startsWith("https://");
		//Create appropriate instance:
		if (httpmode) {
			availEvents = new VP1AvailEvtsHttp(singleEventSource, 60/*update interval*/, 30*60/*time cut for new*/, singleEventLocalTmpDir,localFileCacheLimit);
		} else if(httpsmode) {
			availEvents = new VP1AvailEvtsHttps(singleEventSource, 1000/*update interval*/, 30*60/*time cut for new*/, singleEventLocalTmpDir,localFileCacheLimit);
		} else {
			availEvents = new VP1AvailEvtsLocalDir(5*60/*time cut for new*/, singleEventSource,
					singleEventLocalTmpDir,localFileCacheLimit);
			static_cast<VP1AvailEvtsLocalDir*>(availEvents)->setAvailableSourceDirectories(availableLocalInputDirectories);

		}

	}

	VP1ExecutionScheduler*scheduler = new VP1ExecutionScheduler(0,eventStore,detStore,svcLocator,toolSvc,availEvents);

	//Pass on "joboptions"
	if (joboptions.empty()) {
		//scheduler->d->mainwindow->tabManager()->addNewTab("My Tab");
	} else {
		foreach(QString opt,joboptions)
    		  scheduler->d->mainwindow->loadConfigurationFromFile(opt);
	}
	if (scheduler->d->mainwindow->tabWidget_central->count()<=1) {
		if (initialCruiseMode=="TAB") {
			VP1Msg::message("ERROR: Can not start in cruisemode TAB unless there are at least 2 tabs loaded from initial .vp1 files. Reverting to cruise mode NONE.");
			initialCruiseMode="NONE";
		} else if (initialCruiseMode=="BOTH") {
			VP1Msg::message("ERROR: Can not start in cruisemode BOTH unless there are at least 2 tabs loaded from initial .vp1 files. Reverting to cruise mode EVENT.");
			initialCruiseMode="EVENT";
		}
	}

	//Set default value of cruisemode (dont use setCruiseMode() since it starts timers):
	if (initialCruiseMode=="EVENT") {
		scheduler->d->cruisemode=EVENT;
		scheduler->d->mainwindow->radioButton_cruise_event->setChecked(true);
		scheduler->d->mainwindow->pushButton_cruise->setChecked(true);

	} else if (initialCruiseMode=="TAB") {
		scheduler->d->cruisemode=TAB;
		scheduler->d->mainwindow->radioButton_cruise_tab->setChecked(true);
		scheduler->d->mainwindow->pushButton_cruise->setChecked(true);
	} else if (initialCruiseMode=="BOTH") {
		scheduler->d->cruisemode=BOTH;
		scheduler->d->mainwindow->radioButton_cruise_both->setChecked(true);
		scheduler->d->mainwindow->pushButton_cruise->setChecked(true);
	} else {
		if (initialCruiseMode!="NONE")
			VP1Msg::message("ERROR: unknown initial cruise mode "+initialCruiseMode+" (valid are NONE/EVENT/TAB/BOTH). Assuming NONE.");
		scheduler->d->cruisemode=NONE;
		scheduler->d->mainwindow->radioButton_cruise_event->setChecked(true);
		scheduler->d->mainwindow->pushButton_cruise->setChecked(false);
	}

	scheduler->d->mainwindow->request_cruisemodechange();

	int cruisesecs = ( initialCruiseSeconds == 0 ? 0 :
			std::max(scheduler->d->mainwindow->spinBox_cruise->minimum(),
					std::min(scheduler->d->mainwindow->spinBox_cruise->maximum(),
							static_cast<int>(initialCruiseSeconds))));
	if ( cruisesecs>0 )
		scheduler->d->mainwindow->spinBox_cruise->setValue(cruisesecs);

	return scheduler;
}

//___________________________________________________________________
void VP1ExecutionScheduler::cleanup(VP1ExecutionScheduler*scheduler)
{
	if (VP1QtUtils::environmentVariableIsOn("VP1_ENABLE_INFORM_ON_END_OF_JOB")
	&& ( !scheduler||!(scheduler->d->mainwindow->userRequestedExit()) ) )
		QMessageBox::information(0, "End of job reached",Qt::convertFromPlainText("Job about to end.\n\nThis is most"
				" likely since there are no more available events to process."),QMessageBox::Ok,QMessageBox::Ok);

	if (scheduler) {
		delete scheduler;
		qApp->processEvents(QEventLoop::DeferredDeletion);

		Imp::warnIfWidgetsAlive();
		VP1HelperClassBase::warnUndeletedInstances();

		delete qApp;
		SoQt::done();
	}

	const QString quickExitEnv("VP1_HARD_EXIT_AT_END");
	if (VP1QtUtils::environmentVariableIsOn(quickExitEnv)) {
		VP1Msg::message("Hard job exit (unset "+quickExitEnv+" to disable this behaviour).");
		exit(0);
	}

}

//___________________________________________________________________
void VP1ExecutionScheduler::Imp::updateProgressBar()
{
	double remaining = prioritiser->estimateRemainingCalcTime();
	if (remaining>0.0) {
		pb->setMaximum(static_cast<int>((calctimethisevent+remaining)*10.0));
		pb->setValue(static_cast<int>((calctimethisevent)*10.0));
		pb->show();
		if (!pbtimer->isActive())
			pbtimer->start(40);//25 "frames"/second. If it is good enough for TV, it is good enough for us.
	} else {
		calctimethisevent=0.0;
		pb->hide();
		pb->reset();
		pbtimer->stop();
	}
}
//___________________________________________________________________
void VP1ExecutionScheduler::updateProgressBarDuringRefresh()
{
	if (d->currentrefreshsystemestimate<1.0)
		return;
	double timing=(d->prioritiser->elapsedTiming_Refresh())*0.95;//The *0.95 is to give a smoother overall impression.
	if (timing>d->currentrefreshsystemestimate)
		return;
	d->pb->setValue(static_cast<int>((d->calctimethisevent+timing)*10.0));
}


//___________________________________________________________________
QString VP1ExecutionScheduler::nextRequestedEventFile() const
{
	return d->nextRequestedEvent;
}

//___________________________________________________________________
void VP1ExecutionScheduler::setNextRequestedEventFile(const QString& f)
{
	d->nextRequestedEvent = f;
}

//___________________________________________________________________
bool VP1ExecutionScheduler::executeNewEvent(const int& runnumber, const unsigned long long& eventnumber, const unsigned& triggerType, const unsigned& time)
{
	VP1Msg::messageDebug("VP1ExecutionScheduler::executeNewEvent()");

	d->nextRequestedEvent="";

	d->mainwindow->setRunEvtNumber(runnumber, eventnumber, triggerType, time, !d->skipEvent);

	d->goingtonextevent = false;
	d->calctimethisevent=0;
	d->currentrefreshsystemestimate=0;
	d->updateProgressBar();

	assert(!d->refreshtimer->isActive());//fixme: -> if.


	VP1Msg::messageDebug("calling refreshtimer->start()...");
	d->refreshtimer->start();

	VP1Msg::messageDebug("calling initCruise...");
	d->initCruise();
	VP1Msg::messageDebug("initCruise called.");

	//Flush event queue before reenabling controllers, etc.:
	qApp->processEvents();
	VP1Msg::messageDebug("qApp->processEvents() called.");

	//Enable various user input:
	d->mainwindow->groupBox_channelcontrols->setEnabled(true);
	d->mainwindow->groupBox_cruise->setEnabled(true);
	d->mainwindow->groupBox_event->setEnabled(true);


	VP1Msg::messageDebug("batch mode: " + QString::number(d->batchMode));
	if ( d->batchMode && d->allVisibleRefreshed() ) { // or d->allSystemsRefreshed ???
			VP1Msg::messageDebug("We're in batch mode, skipping...");
	} else {
		VP1Msg::messageDebug("skipEvent: " + QString::number(d->skipEvent));
		if(d->skipEvent) {
			VP1Msg::messageDebug("skipEvent");
			d->skipEvent=false;
			d->mainwindow->nextEvent();
		}
		else {
			VP1Msg::messageDebug("calling qApp->exec()...");
			qApp->exec();//NOTE!!! We then exit the exec() when someone pushes the "next event" button.
		}
	}

	VP1Msg::messageDebug("Disabling user inputs...");
	//Disable various user input:
	d->mainwindow->groupBox_channelcontrols->setEnabled(false);
	d->mainwindow->groupBox_cruise->setEnabled(false);
	d->mainwindow->groupBox_event->setEnabled(false);

	d->goingtonextevent = true;

	if (d->refreshtimer->isActive()) {
		d->refreshtimer->stop();
	}
	d->pb->hide();
	d->pb->reset();
	d->pbtimer->stop();
	//Fixme: Refresh all accumulators that still needs refresh (or just have the button disabled until now)

	//Fixme: wait here until end of any active refreshing...

	VP1Msg::messageDebug("Erasing systems...");
	assert(!d->currentsystemrefreshing);
	foreach(IVP1System*s,d->prioritiser->getSystemsToEraseByPriority()) {
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
		eraseSystem(s);
	}

	VP1Msg::messageDebug("event processed.");

	//Let channels know we are going to the next event now:
	foreach(IVP1ChannelWidget*cw, d->mainwindow->tabManager()->allChannels()) {
		cw->goingToNextEvent();
	}

	qApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);

	VP1Msg::messageDebug("mainwindow->mustQuit: " + QString::number(d->mainwindow->mustQuit()) );
	return !d->mainwindow->mustQuit();
}

//___________________________________________________________________
void VP1ExecutionScheduler::eraseSystem(IVP1System*s) {

	VP1Msg::messageDebug("VP1ExecutionScheduler::eraseSystem()");

	assert(s->state()==IVP1System::REFRESHED);
	assert(!s->isRefreshing());

	QString base =  QString(s->name())+" from channel "+s->channel()->unique_name();
	d->mainwindow->statusBar()->showMessage( "Erasing system ["+base+"]" );
	VP1Msg::messageDebug("ERASING - " + base);

	s->disallowUpdateGUI();
	s->erase();//fixme: time?
	s->setState(IVP1System::ERASED);
	d->mainwindow->statusBar()->showMessage( "Post-erase update to channel ["+base+"]" );
	s->channel()->systemErased(s);//fixme: time?
	d->mainwindow->statusBar()->clearMessage();
}

//___________________________________________________________________
void VP1ExecutionScheduler::systemNeedErase() {
	IVP1System*s = static_cast<IVP1System*>(sender());
	assert(s);
	if (d->currentsystemrefreshing!=s) {
		eraseSystem(s);
	} else {
		assert(s->isRefreshing());
		d->eraseJustAfterRefresh=true;
	}
}

//___________________________________________________________________
void VP1ExecutionScheduler::processSystemForRefresh()
{
	assert(!d->goingtonextevent);
	if (d->currentsystemrefreshing)
		return;

	IVP1System * s = d->prioritiser->nextErasedActiveSystemByPriority();
	if (s) {
		refreshSystem(s);
	} else {
		assert(d->refreshtimer->isActive());//fixme: -> if. ???
		//if (refreshtimer->isActive())
		d->refreshtimer->stop();
		d->pb->hide();
		d->pb->reset();
		d->pbtimer->stop();
	}
}

//___________________________________________________________________
void VP1ExecutionScheduler::channelCreated(IVP1ChannelWidget* cw)
{
	std::set<IVP1System*>::const_iterator it, itE = cw->systems().end();
	for (it=cw->systems().begin();it!=itE;++it) {
		connect(*it,SIGNAL(inactiveSystemTurnedActive()),this,SLOT(startRefreshQueueIfAppropriate()));
		connect(*it,SIGNAL(needErase()),this,SLOT(systemNeedErase()));
	}
	startRefreshQueueIfAppropriate();
}

//___________________________________________________________________
void VP1ExecutionScheduler::channelUncreated(IVP1ChannelWidget* cw)
{
	std::set<IVP1System*>::const_iterator it, itE = cw->systems().end();
	for (it=cw->systems().begin();it!=itE;++it)
		disconnect(*it,SIGNAL(inactiveSystemTurnedActive()),this,SLOT(startRefreshQueueIfAppropriate()));
}

//___________________________________________________________________
void VP1ExecutionScheduler::startRefreshQueueIfAppropriate()
{
	if (!d->goingtonextevent&&!d->refreshtimer->isActive())
		d->refreshtimer->start();
}

//___________________________________________________________________
bool VP1ExecutionScheduler::isRefreshing() const
{
	return d->currentsystemrefreshing;
}

//___________________________________________________________________
void VP1ExecutionScheduler::refreshSystem(IVP1System*s)
{
	QString sysname = s->name();
	VP1Msg::messageDebug("VP1ExecutionScheduler::refreshSystem() - system: " + sysname);

	d->updateProgressBar();

	assert(s->state()==IVP1System::ERASED);
	assert(s->activeState()==IVP1System::ON);
	assert(!d->currentsystemrefreshing);

	d->currentrefreshsystemestimate = d->prioritiser->beginTiming_Refresh(s);
	d->currentsystemrefreshing = s;

	QString base =  QString(s->name())+" from channel "+s->channel()->unique_name();
	d->mainwindow->statusBar()->showMessage( "Refreshing system ["+base+"]" );

	s->allowUpdateGUI();
	s->setRefreshing(true);
	refreshingStatusChanged(true);
	s->channel()->emitRefreshInfoChanged();
	s->refresh(VP1AthenaPtrs::eventStore());
	s->setState(IVP1System::REFRESHED);
	s->setRefreshing(false);
	s->disallowUpdateGUI();

	d->mainwindow->statusBar()->showMessage( "Post-refresh update to channel ["+base+"]" );
	s->channel()->systemRefreshed(s);//fixme: time independently?

	d->mainwindow->statusBar()->clearMessage();
	d->currentsystemrefreshing = 0;
	d->calctimethisevent += d->prioritiser->endTiming_Refresh();
	d->currentrefreshsystemestimate=0;
	d->updateProgressBar();

	refreshingStatusChanged(false);
	s->channel()->emitRefreshInfoChanged();

	VP1Msg::messageDebug("checking if all systems have refreshed...");
	if (hasAllActiveSystemsRefreshed(s->channel())) {

		VP1Msg::messageDebug("All systems refreshed! - last system refreshed! - system: " + sysname);
		s->channel()->lastOfActiveSystemsRefreshed();

		d->allSystemsRefreshed = true; // check if that is fine for multiple channels window: i.e. VP1 instances with a couple of 3DCkocktail windows, for example.

	} // end of hasAllActiveSystemsRefreshed()

	d->performPostRefreshCruiseActions(s->channel());

	if (d->eraseJustAfterRefresh) {
		//Someone asked to erase the system while it was refreshing!
		d->eraseJustAfterRefresh=false;
		//Check that it still needs to (maybe it was flicked back to ON by the impatient user)
		if (s->activeState()==IVP1System::OFF)
			eraseSystem(s);
	}
	if (d->postponedUncreateAndDeleteCW) {
		actualUncreateAndDelete(d->postponedUncreateAndDeleteCW);
		d->postponedUncreateAndDeleteCW=0;
	}

	VP1Msg::messageDebug("end of refreshing the system: " + sysname);

//	// if in "batch mode", now exit from VP1
//	if (d->batchMode) {
//
////		if (hasAllActiveSystemsRefreshed(s->channel())) {
////			VP1Msg::messageWarningRed("All systems refreshed!");
//
//		VP1Msg::messageWarningRed("'batch mode'. Now exiting VP1");
//
//		// N.B. calling QApplication::quit() here it makes move to the next event;
//		// NOTE!!!! It can be useful for a batch mode on all events of one single file!!
//		// TODO: An additional switch to the command line batch option should be implemented
//		//QApplication::quit();
//
//		//for the moment, we only quit VP1.
//		//To quit the application completely, we have to call "MainWindow::close()"
//		d->mainwindow->close();
////		}
//
//	}

	// ***************************
	// we now run all the things
	// that need to be done
	// after all tabs are drawn...
	// ***************************
	if ( hasAllActiveSystemsRefreshed(s->channel()) ) {


		if (d->batchMode) {

			VP1Msg::messageDebug("we are in 'batch-mode'...");

			// saving the snapshot of the 3D window to a file
			QString save_ok = saveSnaphsotToFile(s, true);
			if (save_ok == "") {
				VP1Msg::messageWarningAllRed("FAILED! The snapshot for the channel '" + s->channel()->name() + "' has not been saved!");
			} else {
				VP1Msg::message("OK! Snapshot saved to file: " + save_ok);
			}


			if (d->batchModeAllEvents) {
				VP1Msg::messageWarningAllRed("'batch mode' with 'all events' option. Moving to the next event...");
				VP1Msg::messageWarningAllRed("******************************************************************");
				QApplication::quit();
			} else {
				VP1Msg::messageWarningRed("'batch mode'. Now exiting VP1");

				// Here we want to close VP1 completely.
				// So, in order to do that, we have to call "MainWindow::close()"
				d->mainwindow->close();
			}
		}
	}
}

//___________________________________________________________________
QString VP1ExecutionScheduler::saveSnaphsotToFile(IVP1System* s, bool batch)
{
	QString chnlname = s->name().toLower();
	VP1Msg::messageDebug("VP1ExecutionScheduler::saveSnaphsotToFile() - taking the snapshot of the channel " + chnlname );

	QString currentsaveimagepath = VP1Settings::defaultFileSelectDirectory() + QDir::separator();


	if (batch) {

		VP1Msg::messageDebug("Note: 'batch-mode'");


		// A FOLDER CONTAINER
		QString folderName = "";

		// check if use specified an output folder for the batch job
		QString batchOutFolder = VP1QtUtils::environmentVariableValue("VP1_BATCHMODE_OUT_FOLDER"); // ::getenv("VP1_BATCHMODE");
		if ( ! (batchOutFolder == "") ) {
			VP1Msg::messageDebug("Setting 'batch' output folder from user's settings");
			folderName = batchOutFolder;
		}
		else
			folderName = "atlas_vp1_batch_images";

		// check if the folder already exists, if not make a new folder
		QDir dir(folderName);
		if (!dir.exists()) {
		    dir.mkpath(".");
		}

		QString folder;
		if (folderName.startsWith("/"))
			folder = folderName + QDir::separator();
		else
			folder = currentsaveimagepath + folderName + QDir::separator();

		VP1Msg::messageDebug("folder set: " + folder);


		// EVENT INFO AND TIMESTAMP

		QString runnumb = QString::number(d->mainwindow->getRunNumber());
		QString evnumb = QString::number(d->mainwindow->getEventNumber());
		QString evtimestamp = QString::number(d->mainwindow->getEventTimestamp());

		time_t t_evttimestamp = d->mainwindow->getEventTimestamp();
		tm * human_evtimestamp = localtime(&t_evttimestamp);

		std::ostringstream h_evtimestamp_ostri;
		h_evtimestamp_ostri  << 1900 + human_evtimestamp->tm_year
				       << "-" << 1 + human_evtimestamp->tm_mon  // tm_mon is in the range [0, 11], so 1 must be added to get real months
				       << "-" << human_evtimestamp->tm_mday
				       << "T" << human_evtimestamp->tm_hour << "-" << human_evtimestamp->tm_min << "-" << human_evtimestamp->tm_sec << "CEST";

		std::string h_evtimestamp_str = h_evtimestamp_ostri.str();
		QString h_evtimestamp = QString::fromStdString(h_evtimestamp_str);

		// IMAGE TIMESTAMP

		time_t t_timestamp = time(0);   // get time now;
		tm *ltm = localtime(&t_timestamp);

		// print various components of tm structure.
		VP1Msg::messageDebug(  "Year: "+ QString::number(1900 + ltm->tm_year)
		                + " - " + "Month: " + QString::number(1 + ltm->tm_mon) + " - "  // tm_mon is in the range [0, 11], so 1 must be added to get real months
		                + "Day: " +  QString::number(ltm->tm_mday)
		                + " - " "Time: " + QString::number(ltm->tm_hour) + ":" + QString::number(ltm->tm_min) + ":" + QString::number(ltm->tm_sec) + "CEST");

		std::ostringstream ostri_unix;
		ostri_unix << t_timestamp;

		std::ostringstream ostri;
		ostri  << 1900 + ltm->tm_year
		       << "-" << 1 + ltm->tm_mon  // tm_mon is in the range [0, 11], so 1 must be added to get real months
		       << "-" << ltm->tm_mday
		       << "T" << ltm->tm_hour << "-" << ltm->tm_min << "-" << ltm->tm_sec << "CEST";

		std::string unixTimestamp = ostri_unix.str();
		std::string humanTimestamp = ostri.str();
		QString q_unixTimestamp = QString::fromStdString(unixTimestamp);
		QString q_humanTimestamp = QString::fromStdString(humanTimestamp);
		VP1Msg::messageDebug("Unix timestamp: " + q_unixTimestamp );
		VP1Msg::messageDebug("'human readable' timestamp: " + q_humanTimestamp );

		/* channel's name is the name of the last channel updated by the executer,
		 * i.e. the name changes according to the order the channels are refreshed.
		 * Thus, we don't use the channel's name in the filename,
		 * because it could be misleading.
		 */
		//QString filename = currentsaveimagepath + "vp1_" + chnlname + "_snapshot_" + q_humanTimestamp + ".png";
		QString filename = folder + "vp1_batch_snapshot_r" + runnumb + "_ev" + evnumb + "_evtime_H" + h_evtimestamp + "_U" + evtimestamp + "___imgtime_H" + q_humanTimestamp + "_U" + q_unixTimestamp + ".png";

		// taking the actual snapshot
		QPixmap pm = s->channel()->getSnapshot();
		if (pm.isNull()) {
			VP1Msg::messageDebug("QPixmap is null! returning...");
			return QString();
		}

		// saving to file
		pm.save(filename);

		// save last-saved image filename to working dir
		QString latestImageFileName = currentsaveimagepath + QString("latest_vp1image");
		QFile latestImage(latestImageFileName);
		if(latestImage.exists() && !latestImage.remove())
			throw std::runtime_error("Unable to overwrite the existing latest image file");
		if(!latestImage.open(QIODevice::WriteOnly | QIODevice::Text))
		    throw std::runtime_error("Unable to create new latest image file");
		latestImage.write(filename.toStdString().c_str());
		latestImage.close();


		return filename;
	}

	// default exit
	return QString();//filename;

}



//___________________________________________________________________
void VP1ExecutionScheduler::bringFromConstructedToReady(IVP1ChannelWidget*cw)
{
	assert(cw->state()==IVP1ChannelWidget::CONSTRUCTED);

	connect(cw,SIGNAL(message(const QString&)),d->mainwindow,SLOT(channelAddToMessageBox(const QString&)));

	std::set<IVP1System *>::iterator itsys, itsysE = cw->systems().end();
	for (itsys = cw->systems().begin();itsys!=itsysE;++itsys) {
		assert((*itsys)->state()==IVP1System::CONSTRUCTED);
		connect((*itsys),SIGNAL(sysmessage(const QString&)),d->mainwindow,SLOT(systemAddToMessageBox(const QString&)));
	}
	itsysE = cw->systems().end();
	for (itsys = cw->systems().begin();itsys!=itsysE;++itsys) {
		(*itsys)->setCanRegisterController(true);
		(*itsys)->create(VP1AthenaPtrs::detectorStore());
		(*itsys)->setCanRegisterController(false);
		(*itsys)->setState(IVP1System::ERASED);
	}
	assert(cw->state()==IVP1ChannelWidget::CONSTRUCTED);
	cw->setCanRegister(true,false);//Ok to register controllers during create.
	cw->create();
	cw->setCanRegister(false,false);
	cw->setState(IVP1ChannelWidget::READY);

	assert(cw->state()==IVP1ChannelWidget::READY);
	//Fixme: statusbar!!!!!
}

//___________________________________________________________________
void VP1ExecutionScheduler::uncreateAndDelete(IVP1ChannelWidget*cw)
{
	assert(cw->state()==IVP1ChannelWidget::READY);
	if (d->currentsystemrefreshing&&cw->systems().find(d->currentsystemrefreshing)!=cw->systems().end()) {
		assert(!d->postponedUncreateAndDeleteCW);
		d->postponedUncreateAndDeleteCW=cw;
	} else {
		actualUncreateAndDelete(cw);
	}
}

//___________________________________________________________________
void VP1ExecutionScheduler::actualUncreateAndDelete(IVP1ChannelWidget*cw)
{
	assert(cw->state()==IVP1ChannelWidget::READY);
	cw->setUpdatesEnabled(false);//Just because...

	//From this point on, we are not interested in signals from the channel (and its systems) that could disturb us:
	std::set<IVP1System *>::iterator itsys, itsysE = cw->systems().end();
	for (itsys = cw->systems().begin();itsys!=itsysE;++itsys) {
		(*itsys)->blockSignals(true);
		disconnect(*itsys,0,0,0);
	}
	cw->blockSignals(true);
	disconnect(cw,0,0,0);

	//Make sure that all systems gets in the ERASED state. Throw assert if any is presently refreshing (BAD PROGRAMMER!!)
	for (itsys = cw->systems().begin();itsys!=itsysE;++itsys) {
		assert(d->currentsystemrefreshing!=(*itsys));
		if ((*itsys)->state()==IVP1System::REFRESHED)
			eraseSystem(*itsys);
	}

	//Uncreate systems:
	for (itsys = cw->systems().begin();itsys!=itsysE;++itsys) {
		assert((*itsys)->state()==IVP1System::ERASED);
		(*itsys)->uncreate();
		(*itsys)->setState(IVP1System::UNCREATED);
	}

	//Uncreate channel:
	assert(cw->state()==IVP1ChannelWidget::READY);
	cw->uncreate();//Fixme: Return codes!!
	cw->setState(IVP1ChannelWidget::UNCREATED);

	assert(cw->state()==IVP1ChannelWidget::UNCREATED);

	cw->deleteControllers();//Also deletes system controllers.

	//Delete:
	cw->deleteLater();

}

//___________________________________________________________________
void VP1ExecutionScheduler::Imp::warnIfWidgetsAlive()
{
	QSet<QWidget*> w_ignore, wl = QApplication::allWidgets().toSet();
	w_ignore<<qApp->desktop();
	foreach (QObject*o,qApp->children()) {
		if (o->isWidgetType())
			w_ignore << static_cast<QWidget*>(o);
	}
	foreach (QWidget * w, wl) {
		if (w->objectName().startsWith("internal clipboard"))
			w_ignore << w;
		if (w->objectName()=="empty_widget")
			w_ignore << w;
	}
	wl.subtract(w_ignore);
	if (!wl.empty()) {
		std::cout<<std::endl;
		std::cout<<"VP1 WARNING: "<<wl.count()<< " widget"<<(wl.count()>1?"s":"")<<" left at end of job:"<<std::endl;
		int i(0);
		foreach(QWidget*w,wl) {
			std::cout<<++i<<") Address="<<w<<", ObjectName="<<w->objectName().toStdString()<<", ClassName="<<w->metaObject()->className()<<std::endl;
		}
		std::cout<<std::endl;
	}
}

//___________________________________________________________________
bool VP1ExecutionScheduler::hasAllActiveSystemsRefreshed( IVP1ChannelWidget* cw ) const
{
	VP1Msg::messageDebug("VP1ExecutionScheduler::hasAllActiveSystemsRefreshed() - channelWidget: " + cw->name());

	std::set<IVP1System*>::const_iterator it, itE = cw->systems().end();
	int i=0;
	for (it=cw->systems().begin();it!=itE;++it) {

		/*
		 * enum State { CONSTRUCTED=0, REFRESHED, ERASED, UNCREATED };
  	  	 * enum ActiveState { ON=0, OFF };//Whether it is part of the erase/refresh cycle.
  	  	 */
		if (VP1Msg::verbose()) {
			QString name = QString((*it)->name());
			QString active = QString::number((*it)->activeState());
			QString state = QString::number((*it)->state());
			std::cout << i << " - name: " << name.toStdString() << " - active: " << active.toStdString() << " - refreshed: " << state.toStdString() << std::endl;
		}
		if ((*it)->activeState()==IVP1System::ON&&(*it)->state()!=IVP1System::REFRESHED)
			return false;

		i++;
	}
	return true;
}

//___________________________________________________________________
bool VP1ExecutionScheduler::Imp::allVisibleRefreshed() const
{
	foreach(IVP1ChannelWidget*cw,mainwindow->tabManager()->visibleChannels())
    		if (!scheduler->hasAllActiveSystemsRefreshed(cw))
    			return false;
	return true;
}

//___________________________________________________________________
bool VP1ExecutionScheduler::Imp::allSoonVisibleRefreshed() const
{
	foreach(IVP1ChannelWidget*cw,mainwindow->tabManager()->soonVisibleChannels())
    		if (!scheduler->hasAllActiveSystemsRefreshed(cw))
    			return false;
	return true;
}

//___________________________________________________________________
void VP1ExecutionScheduler::Imp::performPostRefreshCruiseActions(IVP1ChannelWidget*cw) {

	//Abort if not in cruise mode, or if the system just refreshed did
	//not make cw fully refreshed:
	if (cruisemode==NONE||!scheduler->hasAllActiveSystemsRefreshed(cw))
		return;

	if (cruisemode==EVENT) {
		//Abort if this refresh did not make all visible channels refreshed:
		if (!mainwindow->tabManager()->isVisible(cw)||!allVisibleRefreshed())
			return;
		//Start the countdown for the next event:
		assert(!cruisetimer->isActive());
		cruisetimer->start(mainwindow->spinBox_cruise->value()*1000);
		return;
	} else if (cruisemode==TAB) {
		if (cruisetab_waitingtoproceed) {
			//We are waiting for channels in the next tab to refresh before
			//we can move on, so we should check if this channel refresh
			//made all soonvisible channels refreshed. If so: move on.
			if (allSoonVisibleRefreshed()) {
				mainwindow->tabManager()->showNextTab();
				cruisetab_waitingtoproceed=false;
				//If now all visible are refreshed, we start the timer again.
				if (allVisibleRefreshed())
					cruisetimer->start(mainwindow->spinBox_cruise->value()*1000);
			}
		} else {
			//Same as in the EVENT case: Check if it is time to start the countdown:
			//Abort if this refresh did not make all visible channels refreshed:
			if (!mainwindow->tabManager()->isVisible(cw)||!allVisibleRefreshed())
				return;
			//Start the countdown for the next event:
			assert(!cruisetimer->isActive());
			cruisetimer->start(mainwindow->spinBox_cruise->value()*1000);
			return;
		}

		return;
	} else {
		assert(cruisemode==BOTH);
		assert(0&&"not implemented");
	}
}

//___________________________________________________________________
void VP1ExecutionScheduler::Imp::initCruise()
{
	//No matter what we stop the timer when changing mode or starting a new event.
	if (cruisetimer->isActive())
		cruisetimer->stop();
	cruisetab_waitingtoproceed=false;

	//FIXME: DO STUFF HERE

	switch (cruisemode) {
	case NONE:
		VP1Msg::messageVerbose("initCruise NONE");
		break;
	case TAB:
		if (allVisibleRefreshed())
			cruisetimer->start(mainwindow->spinBox_cruise->value()*1000);
		VP1Msg::messageVerbose("initCruise TAB");
		break;
	case EVENT:
		//Start cruise countdown if all visible refreshed:
		if (allVisibleRefreshed())
			cruisetimer->start(mainwindow->spinBox_cruise->value()*1000);
		VP1Msg::messageVerbose("initCruise EVENT");
		break;
	case BOTH:
		VP1Msg::messageVerbose("initCruise BOTH");
		break;
	default:
		assert(0&&"UNKNOWN CRUISE MODE");
		break;
	}
}

//___________________________________________________________________
void VP1ExecutionScheduler::setCruiseMode(const CruiseMode& m)
{
	if (d->cruisemode == m)
		return;
	d->cruisemode = m;

	d->mainwindow->tabManager()->setTabCruiseMode(m==TAB||m==BOTH);

	d->initCruise();

}

//Fixme: abort cruise when this and that... (or, sometimes just reset timer).

//___________________________________________________________________
void VP1ExecutionScheduler::performCruise()
{
	//In any case, we should stop the timer (fixme: What if there are 0 visible channels - when will the timer get started again?):
	d->cruisetimer->stop();

	if (!d->mainwindow->okToProceedToNextEvent()) {
		//Hmm. Would like to cruise, but that is not ok. Check back in a few seconds.
		d->cruisetimer->start( (d->mainwindow->spinBox_cruise->value() > 5 ? 3000 : 1000) );
		return;
	}

	assert(!d->goingtonextevent);//Otherwise it is a bit silly?

	switch (d->cruisemode) {
	case NONE:
		assert(0&&"should never happen");
		break;
	case TAB:
		assert(d->cruisetab_waitingtoproceed==false);
		if (d->allSoonVisibleRefreshed()) {
			d->mainwindow->tabManager()->showNextTab();
			//If now all visible are refreshed, we start the timer again.
			if (d->allVisibleRefreshed())
				d->cruisetimer->start(d->mainwindow->spinBox_cruise->value()*1000);
		} else {
			d->cruisetab_waitingtoproceed=true;
		}
		break;
	case EVENT:
		d->mainwindow->goToNextEvent();
		VP1Msg::messageDebug("Crusing to next event");
		break;
	case BOTH:
		assert(0&&"Not implemented");
		VP1Msg::messageDebug("ERROR: BOTH cruise mode not implemented");
		break;
	default:
		assert(0&&"UNKNOWN CRUISE MODE");
		VP1Msg::messageDebug("ERROR: Unknown cruise mode");
		break;
	}
}

//___________________________________________________________________
QStringList VP1ExecutionScheduler::userRequestedFiles()
{
	return d->mainwindow->userRequestedFiles();
}


//When in tabcruise mode:
//  1) We know that we are not in single-channel FS mode.
//  2) Soonvisible next

//FS tab mode: What about floating dock widgets? Rather high priority, but those in next tab should get next...

//Actually: Floating widgets from other tabs should get docked anyway when going FS-tab when in cruise-TAB mode...

