/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1MainWindow                  //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Gui/VP1MainWindow.h"
#include "VP1Gui/VP1ChannelManager.h"
#include "VP1Gui/VP1TabManager.h"
#include "VP1Gui/VP1ExecutionScheduler.h"
#include "VP1Gui/VP1IncomingMessageDialog.h"
#include "VP1Gui/VP1PluginDialog.h"
#include "VP1Gui/VP1DockWidget.h"
#include "VP1Gui/VP1AvailEvents.h"
#include "VP1Gui/VP1AvailEvtsLocalDir.h"
#include "VP1UtilsBase/VP1FileUtilities.h"
#include "VP1Base/IVP1ChannelWidget.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1Settings.h"
#include "VP1StreamMenuUpdater.h"

#include "VP1Base/VP1QtUtils.h"
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include <QtGui/QLabel>
#include <QtGui/QCloseEvent>
#include <QtGui/QFileDialog>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <QtGui/QComboBox>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QProgressBar>
#include <QtGui/QStyleFactory>
#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QGraphicsView>
#include <QGraphicsScene>

#include <QtGui>
#include <QtWebKit>


#include <cassert>
#include <iostream>


//_________________________________________________________________________________
VP1MainWindow::VP1MainWindow(VP1ExecutionScheduler*sched,VP1AvailEvents * ae,QWidget * parent)
: QMainWindow(parent),
  runnumber(-1),
  eventnumber(-1),
  betweenevents(true),
  mustquit(false),
  dummyemptycontroller(new QWidget(0)),
  scheduler(sched),
  availEvents(ae),
  settingsfile(QDir::homePath()+QDir::separator()+".atlasvp1"),
  m_userRequestedExit(false),
  m_streamMenuUpdater(0),
  m_mutex(new QMutex()),
  m_view(new QWebView(0)),
  edEditor(0)
{
	setupUi(this); // this sets up the GUI

	setupStatusBar();
	//
	if (!VP1QtUtils::environmentVariableIsOn("VP1_DEVEL_SHOW_ALL_CRUISE_AND_EVENT_CONTROLS")) {
		pushButton_eventseek->setVisible(false);
		groupBox_cruise->setVisible(false);
	}

	if (availEvents) {
		connect(availEvents,SIGNAL(message(const QString&)),this,SLOT(helperAddToMessageBox(const QString&)));
		connect(availEvents,SIGNAL(freshEventsChanged()),this,SLOT(updateEventControls()));
	}

	// File menu
	if(!availEvents) {
		QMenu* menu_file = new QMenu(menubar);
		menu_file->setObjectName("menu_file");
		menu_file->setTitle("&File");
		menubar->addAction(menu_file->menuAction());
		action_addEventFile = menu_file->addAction("&Add event file ...");

		connect(action_addEventFile,SIGNAL(triggered(bool)),this,SLOT(addEventFile()));
	}


	////////////////////////////////////////////////////
	//Do we need a menu for multiple input directories?

	VP1AvailEvtsLocalDir* availLocal = dynamic_cast<VP1AvailEvtsLocalDir*>(availEvents);

	QStringList inputdirs;
	if (availLocal)
		inputdirs = availLocal->availableSourceDirectories();

	if (availLocal&&!inputdirs.isEmpty()) {

		QString currentdir = availLocal->currentSourceDir();
		if (currentdir.endsWith("/"))
			currentdir.chop(1);
		currentStream = QDir(currentdir).dirName();
		QMenu * menu_inputdir = new QMenu(menubar);
		menu_inputdir->setObjectName("menu_inputdir");
		menu_inputdir->setTitle("&Stream");

		menubar->addAction(menu_inputdir->menuAction());

		QActionGroup * inputdir_actiongroup = new QActionGroup(menu_inputdir);
		QAction*action_inputdir_current(0);
		foreach (QString inputdir, inputdirs) {
			if (inputdir.endsWith("/"))
				inputdir.chop(1);
			QString dirname = QDir(inputdir).dirName();
			QAction * action_inputdir = new QAction(this);
			action_inputdir->setObjectName("action_inputdir_"+dirname);
			action_inputdir->setData(inputdir);
			action_inputdir->setStatusTip("Get input files from: "+inputdir);
			action_inputdir->setCheckable(true);

			if (currentdir==inputdir)
				action_inputdir_current = action_inputdir;
			menu_inputdir->addAction(action_inputdir);
			inputdir_actiongroup->addAction(action_inputdir);
			inputdiractions << action_inputdir;
			connect(action_inputdir,SIGNAL(triggered(bool)),this,SLOT(inputDirectoryActionTriggered()));
		}
		if (action_inputdir_current) {
			action_inputdir_current->blockSignals(true);
			action_inputdir_current->setChecked(true);
			action_inputdir_current->blockSignals(false);
		}

		// Populate inputdirstatuses
		foreach(QAction* action, inputdiractions)
		inputdirstatuses[action] = VP1DirStatusData(action->data().toString(),
				QString(),
				true,
				false);

		m_streamMenuUpdater = new VP1StreamMenuUpdater(inputdirstatuses,m_mutex);
		m_streamMenuUpdater->start();

		//Fixme: start timer which every minute checks the status of these directories
		QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(updateInputDirectoriesStatus()));
		timer->start(5000);//update this menu rather often (this is only for P1 anyway)
	}

	//.......

	channelmanager = new VP1ChannelManager(scheduler,this);
	tabmanager = new VP1TabManager(this,tabWidget_central,channelmanager);
	stackedWidget_customcontrols->addWidget(dummyemptycontroller);

	//Final touches to instructions page:

	//  Remove any margins added by the layouts in the stacked
	//  widget. This really ought to be the default for stacked widgets!
	for (int i=0;i<stackedWidget_central->count();++i)
		stackedWidget_central->widget(i)->layout()->setContentsMargins(0,0,0,0);

	frame_instructions->setStyleSheet("QFrame#frame_instructions { "+VP1DockWidget::highlightStyle()+" } ");
	frame_instructions->setFrameShape(QFrame::StyledPanel);
	//   textBrowser_intro1->setStyleSheet("QTextBrowser#textBrowser_intro1 { background-color: rgba(0, 0, 0, 0%) } ");
	//   textBrowser_intro2->setStyleSheet("QTextBrowser#textBrowser_intro2 { background-color: rgba(0, 0, 0, 0%) } ");
	connect(pushButton_quicksetup_3dcocktail,SIGNAL(     clicked()),this,SLOT(quickSetupTriggered()));
	connect(pushButton_quicksetup_trackingstudies,SIGNAL(clicked()),this,SLOT(quickSetupTriggered()));
	connect(pushButton_quicksetup_calostudies,SIGNAL(    clicked()),this,SLOT(quickSetupTriggered()));
	connect(pushButton_quicksetup_geometrystudies,SIGNAL(clicked()),this,SLOT(quickSetupTriggered()));
	connect(pushButton_quicksetup_analysisstudies,SIGNAL(clicked()),this,SLOT(quickSetupTriggered()));

	//Default application font:
	m_defaultfont = QApplication::font();
	m_defaultfont_pointsize = m_defaultfont.pointSizeF();
	if (m_defaultfont_pointsize<0.0)
		m_defaultfont_pixelsize = m_defaultfont.pixelSize();
	else
		m_defaultfont_pixelsize = -1;

	//Tabs:
	connect(tabmanager,SIGNAL(tabListChanged(QStringList)),this,SLOT(tabListChanged(QStringList)));
	connect(tabWidget_central,SIGNAL(currentChanged(int)),this,SLOT(updateCentralStackWidget()));
	updateCentralStackWidget();

	//Channels:

	connect(pushButton_saveAllChannels,SIGNAL(clicked()),this,SLOT(saveAllCurrentChannels()));
	connect(pushButton_makeEventDisplay,SIGNAL(clicked()),this,SLOT(makeAllChannelsEventDisplay()));

	connect(pushButton_3D,SIGNAL(clicked()),this,SLOT(launch3DstereoEditor()));

	connect(pushButton_channelfullscreen,SIGNAL(clicked()),tabmanager,SLOT(showCurrentChannelFullScreen()));
	connect(pushButton_channelinformation,SIGNAL(clicked()),this,SLOT(request_channelInformation()));
	connect(pushButton_printchannel,SIGNAL(clicked()),this,SLOT(request_printChannel()));
	connect(pushButton_savesnapshotchannel,SIGNAL(clicked()),this,SLOT(request_saveChannelSnapshot()));
	connect(tabmanager,SIGNAL(selectedChannelChanged(IVP1ChannelWidget*)),this,SLOT(selectedChannelChanged(IVP1ChannelWidget*)));

	//Menu:
	//Quick start:
	connect(action_quicklaunch_Tracking_studies,SIGNAL(triggered(bool)),this,SLOT(quickSetupTriggered()));
	connect(action_quicklaunch_Calo_studies,SIGNAL(triggered(bool)),this,SLOT(quickSetupTriggered()));
	connect(action_quicklaunch_Geometry_studies,SIGNAL(triggered(bool)),this,SLOT(quickSetupTriggered()));
	connect(action_quicklaunch_Storegate_browser,SIGNAL(triggered(bool)),this,SLOT(quickSetupTriggered()));
	connect(action_quicklaunch_3dcocktail,SIGNAL(triggered(bool)),this,SLOT(quickSetupTriggered()));
	connect(action_quicklaunch_trackcalo_commis,SIGNAL(triggered(bool)),this,SLOT(quickSetupTriggered()));
	connect(action_exit_VP1,SIGNAL(triggered(bool)),this,SLOT(close()));

	//Configuration
	menu_loadConfFile = menuConfiguration->addMenu ( "&Load tab configuration from file" );
	menu_loadConfFile->setStatusTip("Select .vp1 config file to load");
	connect(menu_loadConfFile,SIGNAL(aboutToShow()),this,SLOT(showMenu_loadConfFile()));
	menuConfiguration->addSeparator();
	//plugins:
	menu_loadPlugin = menuConfiguration->addMenu ( "&Load plugin" );
	menu_loadPlugin->setStatusTip("Select plugin to load");
	//   action_infoAboutLoadedPlugins = menuPlugins->addAction ( "&Info about loaded plugins" );
	//   action_infoAboutLoadedPlugins->setStatusTip("Get information about the presently loaded plugins");
	//   action_infoAboutLoadedPlugins->setEnabled(false);
	connect(menu_loadPlugin,SIGNAL(aboutToShow()),this,SLOT(showMenu_loadPlugin()));
	//Style
	menu_changeStyle = menu_Style->addMenu ( "&Style" );
	QActionGroup * styleGroup = new QActionGroup(this);
	QAction * laststyleact(0);
	bool foundplastique=false;
	QSettings s(settingsfile,QSettings::IniFormat);
	QString defaultstyle=s.value("style/defaultstyle", "Plastique").toString();
	foreach (QString style, QStyleFactory::keys() ) {
		QAction * act = menu_changeStyle->addAction(style);
		act->setStatusTip("Change application style to "+style);
		connect(act,SIGNAL(triggered(bool)),this,SLOT(changeStyleActionTriggered()));
		act->setCheckable(true);
		styleGroup->addAction(act);
		laststyleact=act;
		if (act->text()==defaultstyle) {
			act->setChecked(true);
			QApplication::setStyle(QStyleFactory::create(act->text()));
			foundplastique=true;
		}
	}
	if (!foundplastique) {
		laststyleact->setChecked(true);
		QApplication::setStyle(QStyleFactory::create(laststyleact->text()));
	}
	//Fonts:
	int savedgoal = 0;
	if (m_defaultfont_pointsize<0.0)
		savedgoal = s.value("font/pixelsize_diff", 0).toInt();
	else
		savedgoal = s.value("font/pointsize_relativepercent", 0).toInt();

	menu_changeFontSize = menu_Style->addMenu ( "&Font size" );;
	QList<int> fontoptions;
	if (m_defaultfont_pointsize<0.0)
		fontoptions <<15<<10<<+5<<+3<<+2<<+1<<0<<-1<<-2<<-3<<-5<<-10<<-20<<-30;
	else
		fontoptions <<150<<100<<50<<30<<20<<10<<5<<0<<-5<<-10<<-20<<-30<<-50;

	QActionGroup * fontGroup = new QActionGroup(this);
	bool foundsavedgoal(false);
	QAction * normalfontact(0);
	foreach (int fontopt,fontoptions) {
		if (m_defaultfont_pixelsize>0&&m_defaultfont_pixelsize+fontopt<=0)
			continue;
		QString text = (fontopt==0?"normal": (fontopt>0?"+":"")+QString::number(fontopt)+(m_defaultfont_pointsize < 0.0? " pixels" : "%"));
		QAction * act = menu_changeFontSize->addAction(text);
		act->setStatusTip("Change overall font size of application to "+text);
		act->setData(fontopt);
		act->setCheckable(true);
		fontGroup->addAction(act);
		connect(act,SIGNAL(triggered(bool)),this,SLOT(changeFontSizeActionTriggered()));
		if (fontopt==0)
			normalfontact=act;
		if (savedgoal==fontopt) {
			act->setChecked(true);
			changeFontSize(fontopt);
			foundsavedgoal=true;
		}
	}
	if (!foundsavedgoal) {
		normalfontact->setChecked(true);
	}

	//
	actionSave_current_tabs = menuConfiguration->addAction ( "&Save current tab configuration to file" );
	actionSave_current_tabs->setStatusTip("Save current tab/channel layout to .vp1 config file");
	menuConfiguration->addSeparator();
	actionAdd_empty_tab = menuConfiguration->addAction ( "&Add empty tab" );
	actionAdd_empty_tab->setStatusTip("Add empty tab to the current tab list");
	connect(actionAdd_empty_tab,SIGNAL(triggered(bool)),this,SLOT(request_addEmptyTab()));
	connect(actionSave_current_tabs,SIGNAL(triggered(bool)),this,SLOT(request_saveasConfig()));

	//Event navigation:
	connect(pushButton_nextevent,SIGNAL(clicked()),this,SLOT(goToNextEvent()));

	//Listen for external requests:
	connect(&tcpserver,SIGNAL(receivedExternalRequest(VP1ExternalRequest)),this,SLOT(receivedExternalRequest(VP1ExternalRequest)));
	listenOnTcp();
	//  updateTcpIcon();
	connect(&tcpserver,SIGNAL(listenStateChanged(bool)),this,SLOT(updateTcpIcon()));
	updateTcpIcon();
	currentincomingdialog=0;
	blockallmessages=false;
	plugindialog=0;

	//Cruise:
	connect(pushButton_cruise,SIGNAL(clicked()),this,SLOT(request_cruisemodechange()));
	connect(radioButton_cruise_event,SIGNAL(clicked()),this,SLOT(request_cruisemodechange()));
	connect(radioButton_cruise_tab,SIGNAL(clicked()),this,SLOT(request_cruisemodechange()));
	connect(radioButton_cruise_both,SIGNAL(clicked()),this,SLOT(request_cruisemodechange()));

	// Help menu
  QMenu* menu_help = new QMenu(menubar);
  menu_help->setObjectName("menu_help");
  menu_help->setTitle("&Help");
  menubar->addAction(menu_help->menuAction());
  action_openVP1Site = menu_help->addAction("VP1 &Web Site");
  action_openUsersGuide = menu_help->addAction("VP1 &User's Guide");
  action_openUsersSupport = menu_help->addAction("VP1 User's &Support (in the system browser)");
  menu_help->addSeparator();
  action_openAbout = menu_help->addAction("&About VP1");

	QTimer::singleShot(0, this, SLOT(postInitUpdates()));
	currentsaveimagepath = VP1Settings::defaultFileSelectDirectory();
	currentloadpluginpath = VP1Settings::defaultFileSelectDirectory();

	connect(action_openUsersGuide,SIGNAL(triggered(bool)),this,SLOT(help_openUserGuide()));
  connect(action_openVP1Site,SIGNAL(triggered(bool)),this,SLOT(help_openVP1WebSite()));
  connect(action_openUsersSupport,SIGNAL(triggered(bool)),this,SLOT(help_openUserSupport()));
  connect(action_openAbout,SIGNAL(triggered(bool)),this,SLOT(help_openAbout()));

  // FIXME: enabling menubar again. It's part of a quickfix, described here: https://its.cern.ch/jira/browse/ATLASVPONE-120
  menubar->setEnabled(false);
}

//_________________________________________________________________________________
void VP1MainWindow::launch3DstereoEditor()
{
	VP1Msg::messageDebug("VP1MainWindow::launch3DstereoEditor()");
	tabmanager->launchStereoEditorCurrentTab();

}

//_________________________________________________________________________________
void VP1MainWindow::help_openUserGuide() {
	/*
	 * Open the online help from a web url location
	 * with the default system web browser
	 */
	VP1Msg::messageDebug("VP1MainWindow::help_openUserGuide()");

	// we use the Qt Web Browser to show the VP1 documentation
	QWebView *old = m_view;
	m_view = new QWebView(0);
	m_view->load(QUrl("http://atlas-vp1.web.cern.ch/atlas-vp1/doc/"));
	m_view->show();
	delete old; old = 0;

	return;
}

//_________________________________________________________________________________
void VP1MainWindow::help_openUserSupport() {
	/*
	 * Open the online page with links to the JIRA issue collectors
	 * with the default system web browser
	 */
	VP1Msg::messageDebug("VP1MainWindow::help_openUserSupport()");

	// fixme: apparently javascript does not work in the embedded browser, so I use the default system one

	// we use here the default system browser
	QDesktopServices::openUrl(QUrl("http://atlas-vp1.web.cern.ch/atlas-vp1/vp1_users_support/"));

//	// we use the Qt Web Browser to show the VP1 user's support page
//	QWebView *old = m_view;
//	m_view = new QWebView(0);
//	m_view->load(QUrl("http://atlas-vp1.web.cern.ch/atlas-vp1/vp1_users_support_em/"));
//	m_view->show();
//	delete old; old = 0;

	return;
}

//_________________________________________________________________________________
void VP1MainWindow::help_openVP1WebSite() {
	/*
	 * Open the online help from a web url location
	 * with the default system web browser
	 */
	VP1Msg::messageDebug("VP1MainWindow::help_openVP1WebSite()");

	// we use the Qt Web Browser to show the VP1 web site
	QWebView *old = m_view;
	m_view = new QWebView(0);
	m_view->resize(1000, 800);
	m_view->load(QUrl("http://atlas-vp1.web.cern.ch/atlas-vp1/"));
	m_view->show();
	delete old; old = 0;
	return;
}


//_________________________________________________________________________________
void VP1MainWindow::help_openAbout() {
	/*
	 * open the online help with the internal web browser
	 */
	VP1Msg::messageDebug("VP1MainWindow::help_openAbout()");

	// we use the Qt Web Browser to show the VP1 "About" page
	QWebView *old = m_view;
	m_view = new QWebView(0);
	m_view->load(QUrl("https://atlas-vp1.web.cern.ch/atlas-vp1/doc_new/about_vp1/CREDITS.html"));
	m_view->show();
	delete old; old = 0;

	return;
}


//_________________________________________________________________________________
void VP1MainWindow::postInitUpdates(){

	//Make sure that the splitters give maximum space for the tab area.

	int sum1 = splitter_tabwidget_messagebox->sizes().at(0) + splitter_tabwidget_messagebox->sizes().at(1);
	QList<int> sizes1; sizes1 << sum1 << 0;
	splitter_tabwidget_messagebox->setSizes ( sizes1 );
	int bottom = 1;
	while (splitter_tabwidget_messagebox->sizes().at(1)==0&&bottom<50000) {
		QList<int> sizes2; sizes2 << sum1-bottom << bottom;
		splitter_tabwidget_messagebox->setSizes ( sizes2 );
		++bottom;
	}

	int sum2 = splitter_leftright->sizes().at(0) + splitter_leftright->sizes().at(1);
	int left = 300;
	QList<int> sizes3; sizes3 << left << sum2-left;
	splitter_leftright->setSizes ( sizes3 );
	while (splitter_leftright->sizes().at(0)==0&&left<50000) {
		QList<int> sizes4; sizes4 << left << sum2-left;
		splitter_leftright->setSizes ( sizes4 );
		++left;
	}

	int prefwidths = std::max<int>(left,widget_controlsContainer->sizeHint().width());
	prefwidths = std::max<int>(prefwidths,groupBox_channelcontrols->sizeHint().width());
	prefwidths = std::max<int>(prefwidths,groupBox_cruise->sizeHint().width());
	prefwidths = std::max<int>(prefwidths,groupBox_event->sizeHint().width());
	prefwidths = std::max<int>(prefwidths,stackedWidget_customcontrols->sizeHint().width());
	widget_controlsContainer->setMaximumWidth(50+prefwidths);
	//   int h1(textBrowser_intro1->viewport()->sizeHint().height());
	//   textBrowser_intro1->setMaximumHeight(h1+2);
	if (availEvents)
		availEvents->init();
	updateEventControls();
}

//_________________________________________________________________________________
void VP1MainWindow::setupStatusBar()  {
	progressbar = new QProgressBar();
	progressbar->setMinimum(0);
	progressbar->reset();
	progressbar->setOrientation(Qt::Horizontal);
	statusBar()->addPermanentWidget(progressbar);
	statusbarlabel = new QLabel();
	//Turn off ugly box around items in statusbar:
	statusBar()->setStyleSheet("QStatusBar::item { border-width: 0 }");
	statusBar()->addPermanentWidget(statusbarlabel);
	progressbar->hide();
}

//_________________________________________________________________________________
VP1MainWindow::~VP1MainWindow() 
{ 
	if (edEditor) {
		VP1Msg::messageDebug("deleting the editor");
		delete edEditor;
	}
	VP1Msg::messageDebug("deleting the tab manager");
	delete tabmanager;
	VP1Msg::messageDebug("deleting the channel manager");
	delete channelmanager;
	VP1Msg::messageDebug("deleting the events");
	delete availEvents;

	if(m_streamMenuUpdater) {
		VP1Msg::messageDebug("deleting the streamupdater");
		m_streamMenuUpdater->quit();
		m_streamMenuUpdater->deleteLater();
	}

	VP1Msg::messageDebug("deleting the mutex");
	delete m_mutex;

	VP1Msg::messageDebug("deleting the view");
	delete m_view;
	m_view = 0;
}

//_________________________________________________________________________________
bool VP1MainWindow::mustQuit() const {
	return mustquit;
}


//_________________________________________________________________________________
void VP1MainWindow::updateTcpIcon()
{
	bool l = tcpserver.isListening();
	statusbarlabel->setPixmap(QIcon(l?":/vp1/icons/icons/network_64x64.png":":/vp1/icons/icons/network_disconnect_64x64.png")
			.pixmap(progressbar->height(),progressbar->height(),QIcon::Normal,QIcon::On));
	statusbarlabel->setToolTip(l?"Listening on port "+QString::number(tcpserver.port())+" for incoming messsages"
			:"VP1 is presently NOT listening for incoming messages");

}

//_________________________________________________________________________________
void VP1MainWindow::loadPluginFile(QString filename)
{
	VP1Msg::messageDebug("loadPluginFile()");

	QString err = channelmanager->loadPluginFile(filename);
	if (!err.isEmpty()) {
		QMessageBox::critical(0, "Error - could not load plugin file: "+filename,
				"Could not load plugin file: "
				+filename+"\n\nReason: "+err,QMessageBox::Ok,QMessageBox::Ok);
		VP1Msg::message("Could not load plugin file: "+filename+"\n\nReason: "+err);
	}
	//Fixme: Error message here is hardcoded to be the same as in loadPluginFile method!!
}

//_________________________________________________________________________________
void VP1MainWindow::request_addEmptyTab() {
	bool ok;
	QString newtabname = QInputDialog::getText( 0, "New Tab Name","New tab name:",
			QLineEdit::Normal, tabmanager->suggestNewTabName("My Tab"), &ok );
	if (!ok||newtabname.isEmpty())
		return;
	tabmanager->addNewTab(newtabname);
}

//_________________________________________________________________________________
void VP1MainWindow::tabListChanged(QStringList l) {
	updateCentralStackWidget();
	if (l.count()) {
		actionSave_current_tabs->setEnabled(true);
		groupBox_cruise->setEnabled(true);
	} else {
		actionSave_current_tabs->setEnabled(false);
		groupBox_cruise->setEnabled(false);
	}
	if (l.count()>1) {
		radioButton_cruise_tab->setEnabled(true);
		radioButton_cruise_both->setEnabled(true);
	} else {
		if (radioButton_cruise_tab->isChecked()||radioButton_cruise_both->isChecked())
			radioButton_cruise_event->setChecked(true);
		radioButton_cruise_tab->setEnabled(false);
		radioButton_cruise_both->setEnabled(false);
	}
}

//_________________________________________________________________________________
void VP1MainWindow::addChannelIconsToComboBox(QComboBox* cb, const bool& isbasenames) {
	int n= cb->count();
	for (int i = 0; i<n; ++i) {
		QString icontext = channelmanager->getIconLocation(cb->itemText(i), isbasenames);
		if (!icontext.isEmpty())
			cb->setItemIcon ( i, QIcon(icontext) );
	}
}

//_________________________________________________________________________________
void VP1MainWindow::selectedChannelChanged(IVP1ChannelWidget* cw)
{
	VP1Msg::messageDebug("VP1MainWindow::selectedChannelChanged()");

	//Controls box:
	if (cw) {
		groupBox_channelcontrols->setTitle("Controls: "+cw->unique_name());
		groupBox_channelcontrols->setEnabled(true);
		QWidget* controller = channelmanager->getController(cw);
		if (!controller) {
			stackedWidget_customcontrols->setCurrentWidget(dummyemptycontroller);
		} else {
			if (stackedWidget_customcontrols->indexOf(controller)==-1)
				stackedWidget_customcontrols->addWidget(controller);
			stackedWidget_customcontrols->setCurrentWidget(controller);
		}
	} else {
		groupBox_channelcontrols->setTitle("Controls: no channel selected");
		groupBox_channelcontrols->setEnabled(false);
		stackedWidget_customcontrols->setCurrentWidget(dummyemptycontroller);
	}

	// FIXME: enabling menubar again. It's part of a quickfix, described here: https://its.cern.ch/jira/browse/ATLASVPONE-120
	menubar->setEnabled(true);

  // FIXME: enabling menubar again. It's part of a quickfix, described here: https://its.cern.ch/jira/browse/ATLASVPONE-120
  menubar->setEnabled(true);
}

//_________________________________________________________________________________
void VP1MainWindow::request_saveasConfig() {

	QString filename = QFileDialog::getSaveFileName(this, "Select configuration file to save",
			(currentconfigfile.isEmpty()?VP1Settings::defaultFileSelectDirectory():currentconfigfile),
			"VP1 Configuration files (*.vp1)",0,QFileDialog::DontResolveSymlinks);
	if(filename.isEmpty())
		return;

	if (!filename.endsWith(".vp1"))
		filename += ".vp1";

	tabmanager->saveConfigurationToFile(filename,false/*Since the filedialog already asks*/);
	currentconfigfile=filename;
}

//_________________________________________________________________________________
void VP1MainWindow::request_saveConfig()
{
	if (currentconfigfile.isEmpty()) {
		request_saveasConfig();
		return;
	}
	tabmanager->saveConfigurationToFile(currentconfigfile,false);
}

//_________________________________________________________________________________
void VP1MainWindow::request_loadConfig()
{
	QString filename = QFileDialog::getOpenFileName(this, "Select configuration file to load",
			(currentconfigfile.isEmpty()?VP1Settings::defaultFileSelectDirectory():currentconfigfile),
			"VP1 configuration files (*.vp1)",0,QFileDialog::DontResolveSymlinks);
	if(filename.isEmpty())
		return;
	tabmanager->loadConfigurationFromFile(filename,availablePluginFiles());
	currentconfigfile=filename;
}

//_________________________________________________________________________________
void VP1MainWindow::request_loadPlugin()
{
#ifdef __APPLE__
	QString sharedlibsuffix = "dylib";
#else
	QString sharedlibsuffix = "so";
#endif

	QString filename = QFileDialog::getOpenFileName(this, "Select plugin file to load",
			currentloadpluginpath,
			"VP1 plugin files (*."+sharedlibsuffix+")",0,QFileDialog::DontResolveSymlinks);
	if(filename.isEmpty())
		return;
	currentloadpluginpath = QFileInfo(filename).dir().absolutePath();
	loadPluginFile(filename);
}

//_________________________________________________________________________________
QMap<QString,QString> VP1MainWindow::availableFiles(const QString& extension,
		const QString& pathvar,//LD_LIBRARY_PATH or DATAPATH
		const QString& instareasubdir,
		const QString& extradirenvvar,
		bool currentdir ) const
{

	//Add directories from extradirenvvar (e.g. $VP1PlUGINPATH)
	QStringList vp1pluginpath = extradirenvvar.isEmpty() ? QStringList() : QString(::getenv(extradirenvvar.toStdString().c_str())).split(":",QString::SkipEmptyParts);

	//Currentdir:
	if (currentdir) {
		vp1pluginpath<<QDir::currentPath();
		if (QDir::currentPath()!=VP1Settings::defaultFileSelectDirectory())
			vp1pluginpath<<VP1Settings::defaultFileSelectDirectory();
	}



	//Add directories from pathvar (looking in subdir instareasubdir):
	QString varStr = QString(::getenv(pathvar.toStdString().c_str()));
	VP1Msg::messageDebug("Add directories from pathvar... " + pathvar + " - " + varStr);
	QString path = QString(::getenv(pathvar.toStdString().c_str()));
	if (!path.isEmpty()) {
		//!instareasubdir.isEmpty()&&
		QStringList tmp = path.split(":",QString::SkipEmptyParts);//This 'tmp' is for SLC3 compilation.
		foreach (QString dir,tmp) {
			vp1pluginpath << ( instareasubdir.isEmpty() ? dir : dir+QDir::separator()+QDir::separator()+instareasubdir );
		}
	}

	//Remove all nonexisting directories:
	foreach (QString plugindir, vp1pluginpath) {
		QFileInfo fi(plugindir);
		if (!fi.exists()||!fi.isDir()) {
			vp1pluginpath.removeAll(plugindir);
		}
	}

	//Find all files with required extension in the directories (in case of duplicates - the ones appearing first are used):
	QMap<QString,QString> plugins2fullpath;
	foreach (QString plugindir, vp1pluginpath) {
		QStringList plugins = QDir(plugindir).entryList((QStringList()<<("*"+extension)),QDir::CaseSensitive | QDir::Files | QDir::Readable,QDir::Name);
		foreach (QString plugin, plugins) {
			plugin = QFileInfo(plugin).fileName();
			if (!plugins2fullpath.contains(plugin)) {
				QString fullpath = plugindir+QDir::separator()+plugin;
				plugins2fullpath[plugin]=fullpath;
			}
		}
	}
	return plugins2fullpath;
}

//_________________________________________________________________________________
QMap<QString,QString> VP1MainWindow::availablePluginFiles() const
{
#ifdef __APPLE__
	QString sharedlibsuffix = "dylib";
#else
	QString sharedlibsuffix = "so";
#endif

	return availableFiles( "."+sharedlibsuffix, "LD_LIBRARY_PATH", "vp1plugins", "VP1PlUGINPATH" );

}

//_________________________________________________________________________________
void VP1MainWindow::pluginDialogClosed() {
	if (!plugindialog)
		return;

	int res = plugindialog->result();
	QString filename = plugindialog->unloadfile();
	disconnect(plugindialog,SIGNAL(finished(int)),this,SLOT(pluginDialogClosed()));
	plugindialog->deleteLater();
	plugindialog = 0;

	if (res!=QDialog::Accepted||filename.isEmpty())
		return;

	//How many channels would be affected by such unloading?
	QStringList bns = channelmanager->channelsInPluginFile(filename);
	int naffected(0);
	foreach (QString bn, bns)
	naffected += channelmanager->nActive(bn);

	foreach (QString bn, bns) {
		while(channelmanager->basename2UniqueNames(bn).count()>0)
			tabmanager->removeChannel(channelmanager->basename2UniqueNames(bn).value(0));
	}

	currentunloadpluginfiles << filename;
	QTimer::singleShot(0, this, SLOT(unloadPlugin_continue()));

}

//_________________________________________________________________________________
void VP1MainWindow::unloadPlugin_continue()
{
	foreach (QString filename, currentunloadpluginfiles) {
		bool success = channelmanager->unloadPluginFile(filename);
		if (!success)
			QMessageBox::critical(0, "Error - problems unloading plugin file: "+filename,
					"Problems encountered while attempting to unload plugin file: "+filename,QMessageBox::Ok,QMessageBox::Ok);
	}
	currentunloadpluginfiles.clear();
}

//_________________________________________________________________________________
bool VP1MainWindow::okToProceedToNextEvent() const
{
	return ! (betweenevents || (availEvents&&availEvents->freshEvents().isEmpty()));
}


//_________________________________________________________________________________
void VP1MainWindow::nextEvent() {
	betweenevents=true;
	if (availEvents) {
		QList<VP1EventFile>  evts = availEvents->freshEvents();
		if (evts.empty()) {
			addToMessageBox("ERROR: Going to next event, but one is not available!");
			scheduler->setNextRequestedEventFile("");
		} else {
			scheduler->setNextRequestedEventFile(evts.front().fileName());
		}
	}
	updateEventControls();
}

//_________________________________________________________________________________
void VP1MainWindow::goToNextEvent() {
	nextEvent();
	qApp->quit();
}

//_________________________________________________________________________________
void VP1MainWindow::closeEvent(QCloseEvent * event)
{
	VP1Msg::messageDebug("VP1MainWindow::closeEvent()");

	if (VP1QtUtils::environmentVariableIsOn("VP1_ENABLE_ASK_ON_CLOSE")) {
		int ret = QMessageBox::warning(this,
				"Close VP1?",
				"You are about to close VP1 and end the job.\nProceed?",
				QMessageBox::Ok| QMessageBox::Cancel,
				QMessageBox::Cancel );
		if (ret!=QMessageBox::Ok) {
			event->ignore();
			return;
		}
		m_userRequestedExit = true;
	}

	hide();

	VP1Msg::messageDebug("calling tabmanager->setSelectedDockWidget(0)...");
	tabmanager->setSelectedDockWidget(0);
	VP1Msg::messageDebug("tabmanager->setSelectedDockWidget(0) called.");

	mustquit=true; // this will inform VP1Alg that we want to quit VP1 (then we'll quit the Athena algorithm)
	VP1Msg::messageDebug("calling qApp->quit()...");
	qApp->quit();
}

//_________________________________________________________________________________
void VP1MainWindow::setRunEvtNumber(const int& r, const unsigned long long& e, const unsigned& triggerType, const unsigned& time, const bool& printmessage ) {

	scheduler->setNextRequestedEventFile("");
	const bool sameasold(runnumber==r&&eventnumber==e);

	runnumber=r;
	eventnumber=e;
	timestamp=time;

	betweenevents = false;
	if (availEvents)
		availEvents->setCurrentEvent(r,e);

	foreach(IVP1ChannelWidget* channel,tabmanager->allChannels()) {
		channel->setRunEvtNumber(r,e);
		channel->setEvtTimestamp(time);
	}

	if(printmessage) {
		qulonglong evNumber = eventnumber;
		QString evtstr = "run# "+QString::number(runnumber)+", event# "+QString::number(evNumber)+(sameasold?" (reused)":"");
		QString trighex = triggerType > 0 ? "0x"+QString::number(triggerType, 16).toUpper().rightJustified(sizeof(triggerType),'0') : "";
		QString expandedevtstr = evtstr
				+ QString(trighex.isEmpty()?QString(""):", triggerType: "+trighex)
				+ QString(time>0 ? ", time: "+QDateTime::fromTime_t(time).toString(Qt::ISODate).replace('T',' ') : "")
				+ QString(currentStream.isEmpty()?"":", "+currentStream);
		setWindowTitle("Virtual Point 1 ["+expandedevtstr+"]");
		groupBox_event->setTitle("Event ["+evtstr+"]");

		addToMessageBox("New event: "+expandedevtstr,"color:#ff0000");

	}
	updateEventControls();
}

//_________________________________________________________________________________
void VP1MainWindow::addToMessageBox( const QString& m, const QString& extrastyleopts,
		const QString& title, const QString& titleextrastyleopts )
{

	if (title.isEmpty())
		std::cout<<VP1Msg::prefix_msg()<<": "<<m.toStdString()<<std::endl;
	else
		std::cout<<VP1Msg::prefix_msg()<<": "<<title.toStdString() << ": " <<m.toStdString()<<std::endl;


	QString titlepart = ( title.isEmpty() ? "" : ( titleextrastyleopts.isEmpty() ? title
			: "<span style=\"font-style:italic;"+titleextrastyleopts+";\">["+title+"]</span> "));

	textBrowser_channelmessages->append(titlepart
			+ ( extrastyleopts.isEmpty() ? m
					: "<span style=\"font-style:italic;"+extrastyleopts+";\">"+m+"</span>"));
}

//_________________________________________________________________________________
void VP1MainWindow::systemAddToMessageBox( const QString& m )
{
	IVP1System*sys = static_cast<IVP1System*>(sender());
	if (!sys) {
		addToMessageBox("VP1MainWindow::systemAddToMessageBox Error: Only prints system messages!");
		return;
	}
	if (!sys->channel()) {
		addToMessageBox("VP1MainWindow::systemAddToMessageBox Error: System does not know its channel!");
		return;
	}
	addToMessageBox( m, "color:#000000",sys->channel()->unique_name()+"/"+sys->name(),"color:#0000ff" );
}

//_________________________________________________________________________________
void VP1MainWindow::channelAddToMessageBox( const QString& m )
{
	IVP1ChannelWidget*cw = static_cast<IVP1ChannelWidget*>(sender());
	if (!cw) {
		addToMessageBox("VP1MainWindow::channelAddToMessageBox Error: Only prints channelwidget messages!");
		return;
	}
	addToMessageBox(m,"color:#000000", cw->unique_name(),"color:#0000ff");
}

//_________________________________________________________________________________
void VP1MainWindow::helperAddToMessageBox( const QString& m )
{
	addToMessageBox(m);
}

//_________________________________________________________________________________
void VP1MainWindow::request_channelInformation() {
	if(!tabmanager->selectedChannelWidget())
		return;

	QString out = "Information about channel: "+tabmanager->selectedChannelWidget()->name() + "\n\n";
	out += "Contact: "+tabmanager->selectedChannelWidget()->contact_info()+"\n";
	out += "Information: "+tabmanager->selectedChannelWidget()->information()+"\n";
	out += "Systems:\n\n";
	std::set<IVP1System *>::iterator itsys, itsysE = tabmanager->selectedChannelWidget()->systems().end();
	for (itsys = tabmanager->selectedChannelWidget()->systems().begin();itsys!=itsysE;++itsys) {
		out += "  ==> System "+(*itsys)->name()+"\n";
		out += "      Contact: "+(*itsys)->contact_info()+"\n";
		out += "      Information: "+(*itsys)->information()+"\n";
		out += "\n";
	}

	QMessageBox::information(0, "Information about channel: "+tabmanager->selectedChannelWidget()->name(),Qt::convertFromPlainText(out),QMessageBox::Ok,QMessageBox::Ok);
}


void VP1MainWindow::makeAllChannelsEventDisplay()
{
	VP1Msg::messageVerbose("VP1MainWindow::makeAllChannelsEventDisplay()");

	QList<QPixmap> list;
	QStringList listNames;
	QList<unsigned long long> listRunEventNumberTimestamp;

	getAllChannelsIntoSnapshots(list, listNames);

	listRunEventNumberTimestamp << runnumber;
	listRunEventNumberTimestamp << eventnumber;
	listRunEventNumberTimestamp << timestamp;

	// create a new editor window
	edEditor = new VP1EventDisplayEditor(this, listRunEventNumberTimestamp);

	edEditor->addPixmapList(list, listNames);

	// pass the lists of all tabs and their names to the editor
	edEditor->setTabsList( listNames);

	edEditor->show();

}





void VP1MainWindow::getAllChannelsIntoSnapshots(QList<QPixmap>& list, QStringList& listNames)
{
	VP1Msg::messageDebug("VP1MainWindow::getAllChannelsIntoSnapshots()");

//	int nTabs = tabmanager->nTabs();
	QList<IVP1ChannelWidget*> allTabs = tabmanager->allChannels();

	if (allTabs.isEmpty()) {
		VP1Msg::message("WARNING - No tabs to save.");
		return;
	}

	int nT = 0;

	// loop over all tabs/channels
	foreach(IVP1ChannelWidget* widg, allTabs) {

		// increase tab number
		++nT;

		// get channel name (e.g. Geometry, 3DCocktail)
//		QString channelname = widg->unique_name().toLower();
		QString channelname = tabmanager->channelToTab(widg);
		channelname.replace(' ','_');
		VP1Msg::messageDebug("tab: " + channelname);

//		// get channel info, only for debug
//		QString info = widg->information();
//		VP1Msg::messageDebug("channel info: " + info);


		QPixmap snap = getSingleChannelCustomSnapshot(widg);

		list << snap;
		listNames << channelname;

	}

	VP1Msg::messageVerbose(QString::number(nT)+" tabs/channels saved in the QList.");

}


QPixmap VP1MainWindow::getSingleChannelCustomSnapshot(IVP1ChannelWidget* tab, int width)
{
	VP1Msg::messageDebug("VP1MainWindow::getSingleChannelCustomSnapshot()");

	std::cout << "tab: " << tab << std::endl;

	// save the anti-aliasing status, set by the user
	bool antialias_original = tab->isAntiAliasing();

	// for the final event display images we want the anti-aliasing turned ON,
	tab->setAntiAliasing(true);

	// get the snapshot
	QPixmap snap;
	if (width)
		snap = tab->getSnapshot(true, width); // 'true' means 'transparent background'
	else
		snap = tab->getSnapshot(true); // 'true' means 'transparent background'

	if (snap.isNull()) {
		VP1Msg::message("ERROR! - No snapshot produced!");
		return QPixmap();
	}

	// go back to the original anti-aliasing status, set by the user for the tab
	tab->setAntiAliasing(antialias_original);

	return snap;
}

QPixmap VP1MainWindow::getSingleChannelCustomSnapshot(QString tabName, int width)
{
	QList<IVP1ChannelWidget*> allTabs = tabmanager->allChannels();

	if (allTabs.isEmpty()) {
		VP1Msg::message("WARNING - No tabs to get snapshots from.");
	}

	// loop over all tabs/channels
	foreach(IVP1ChannelWidget* widg, allTabs) {

		// get channel name (e.g. Geometry, 3DCocktail)
		QString channelname = tabmanager->channelToTab(widg);
		channelname.replace(' ','_');

		if (channelname == tabName) {
			VP1Msg::messageDebug("found tab: " + channelname);

			if (width)
				return getSingleChannelCustomSnapshot(widg, width);
			else
				return getSingleChannelCustomSnapshot(widg);

		}
	}
	return QPixmap();
}


void VP1MainWindow::saveAllCurrentChannels()
{
	VP1Msg::messageDebug("VP1MainWindow::saveAllCurrentChannels()");

	int nTabs = tabmanager->nTabs();

	VP1Msg::messageDebug("# of tabs: " + QString::number(nTabs));

	QList<IVP1ChannelWidget*> allTabs = tabmanager->allChannels();

	if (allTabs.isEmpty()) {
		VP1Msg::message("WARNING - No tabs to save.");
		return;
	}


//	QString guess;
//	QString chnlname = tabmanager->selectedChannelWidget()->name().toLower();
//	chnlname.replace(' ','_');

	QString base=currentsaveimagepath+QDir::separator()+"vp1"
			+ "_run"+QString::number(runnumber)+"_evt"+QString::number(eventnumber)
			+ QString(timestamp>0 ? "_"+QDateTime::fromTime_t(timestamp).toString(Qt::ISODate).replace(':','-') : "");


	// check for existing files
	int i(2);
	while (QFile::exists(base+".png")) {
		base = base+"_"+QString::number(i++);
	}

	// let the user choose the base filename
	QString filename = QFileDialog::getSaveFileName(0, "Select target image file", base,
			"Image (*.png *.bmp)",
			0,QFileDialog::DontResolveSymlinks);
	if(filename.isEmpty()) {
		VP1Msg::message("WARNING - No filename selected.");
		return;
	}

	currentsaveimagepath = QFileInfo(filename).dir().absolutePath ();


	QStringList tab_save_files;

	int nT = 0;
	foreach(IVP1ChannelWidget* widg, allTabs) {

		// increase tab number
		++nT;

		// get channel name (e.g. Geometry, 3DCocktail)
		QString channelname = widg->name().toLower();
		channelname.replace(' ','_');
		VP1Msg::messageDebug("tab: " + channelname);

//		// get channel info, only for debug
//		QString info = widg->information();
//		VP1Msg::messageDebug("channel info: " + info);

		QPixmap snap = widg->getSnapshot();
		if (snap.isNull()) {
			VP1Msg::message("ERROR! - No snapshot produced!");
			return;
		}

		QString tabfilename = filename + "_" + QString::number(nT) + "_" + channelname + ".png";

		if (!(tabfilename.endsWith(".png",Qt::CaseInsensitive)||tabfilename.endsWith(".bmp",Qt::CaseInsensitive)))
			tabfilename += ".png";


		snap.save(tabfilename);

		tab_save_files << tabfilename;
	}

	VP1Msg::message("Tabs saved as: " + tab_save_files.join(" - "));


    QMessageBox::information(this, tr("Snapshots saved."),
                         "All tabs have been saved as snapshots in the following files:\n\n"+tab_save_files.join("\n"),
	                     QMessageBox::Ok,
	                     QMessageBox::Ok);


//	// TODO: test to create a multilayered PSD or TIFF file
//	// but doesn't work with the ImageMagick version shipped with SLC5...
//	// So I give it up now...
//	QProcess *proc = new QProcess();
//	QString program = "convert";
//	QStringList arguments;
//	arguments = tab_save_files;
//	arguments << " " + currentsaveimagepath + QDir::separator() + "out.psd";
//	VP1Msg::messageDebug("running: " + program + " " + arguments.join(" "));
//
//	// start the process
//	proc->start(program, arguments );
//
//	if (!proc->waitForStarted())
//		return;
//
//	if (!proc->waitForFinished())
//		return;

}

//_________________________________________________________________________________
/*
 * save a snaphot of the currently selected tab.
 *
 * nsnap is an optional parameter: it's an extra label being added
 * to the output filename.
 */
QString VP1MainWindow::request_saveChannelSnapshot(QString xLabel)
{

	VP1Msg::messageDebug("VP1MainWindow::request_saveChannelSnapshot()");

	if(!tabmanager->selectedChannelWidget()) {
		return QString();
	}

	QString guess;
	QString chnlname = tabmanager->selectedChannelWidget()->name().toLower();
	chnlname.replace(' ','_');

	QString base=currentsaveimagepath+QDir::separator()+"vp1_"+chnlname
			+"_run"+QString::number(runnumber)+"_evt"+QString::number(eventnumber)
			+ QString(timestamp>0 ? "_"+QDateTime::fromTime_t(timestamp).toString(Qt::ISODate).replace(':','-') : "");


	// check for existing files
	guess=base;
	int i(2);
	while (QFile::exists(guess+".png")) {
		guess=base+"_"+QString::number(i++);
	}
	guess+=".png";


	// adding the extra label xLabel, if provided
	if (!xLabel.isEmpty()) {
		guess += "_" + xLabel;
	}


	QString filename = QFileDialog::getSaveFileName(0, "Select target image file", guess,
			"Image (*.png *.bmp)",
			0,QFileDialog::DontResolveSymlinks);
	if(filename.isEmpty())
		return QString();

	currentsaveimagepath = QFileInfo(filename).dir().absolutePath ();

	if (!(filename.endsWith(".png",Qt::CaseInsensitive)||filename.endsWith(".bmp",Qt::CaseInsensitive)))
		filename += ".png";

	VP1Msg::messageVerbose("calling snapshot");
	QPixmap pm = tabmanager->selectedChannelWidget()->getSnapshot();

	if (pm.isNull())
		return QString();

	pm.save(filename);

	return filename;
}

//_________________________________________________________________________________
void VP1MainWindow::request_printChannel() {
	if(!tabmanager->selectedChannelWidget())
		return;

	//The following will paint the widget onto a paper and bring up the print dialog:
	QPrinter printer;
	QPrintDialog dialog(&printer, this);
	if (dialog.exec() == QDialog::Accepted) {
		QPixmap pm = tabmanager->selectedChannelWidget()->getSnapshot();
		if (pm.isNull())
			return;
		QPainter painter;
		painter.begin(&printer);
		painter.drawPixmap(0,0,pm);
		painter.end();
	}
}


//_________________________________________________________________________________
void VP1MainWindow::loadConfigurationFromFile(QString file) {
	tabmanager->loadConfigurationFromFile(file,availablePluginFiles());
}

//_________________________________________________________________________________
void VP1MainWindow::listenOnTcp()
{
	QString err;
	if (!tcpserver.listen(err)) {
		qDebug(err.toStdString().c_str());
	}
}


//_________________________________________________________________________________
void VP1MainWindow::finishedIncomingDialog()
{
	currentincomingdialog=0;
	if (!requestqueue.empty())
		QTimer::singleShot(0, this, SLOT(processEnqueuedRequests()));
}


//_________________________________________________________________________________
void VP1MainWindow::processEnqueuedRequests()
{
	if (!requestqueue.empty())
		receivedExternalRequest(requestqueue.dequeue());
}

//_________________________________________________________________________________
void VP1MainWindow::receivedExternalRequest(VP1ExternalRequest request)
{
	if (blockallmessages)
		return;
	if (messages_blockedsenders.contains(request.sender()))
		return;
	if (messages_blockedexactmessages.contains(request))
		return;
	if (currentincomingdialog) {
		//Fixme: store TIME of incoming request (to show the user).
		if (requestqueue.count()<999) {
			requestqueue.enqueue(request);
			currentincomingdialog->updatependinginfo();
		}
		return;
	}
	VP1IncomingMessageDialog * md = new VP1IncomingMessageDialog(request,&requestqueue,&blockallmessages,
			&messages_blockedsenders,&messages_blockedexactmessages,
			tabmanager,channelmanager,this);
	currentincomingdialog=md;
	connect(md,SIGNAL(finished(int)),this,SLOT(finishedIncomingDialog()));
	tabmanager->dropOutOfFullScreen();
	md->show();
}

//_________________________________________________________________________________
void VP1MainWindow::request_cruisemodechange()
{
	if (pushButton_cruise->isChecked()) {
		if (radioButton_cruise_event->isChecked()) {
			scheduler->setCruiseMode(VP1ExecutionScheduler::EVENT);
			groupBox_cruise->setTitle("Cruise Mode [event]");
		} else if (radioButton_cruise_tab->isChecked()) {
			scheduler->setCruiseMode(VP1ExecutionScheduler::TAB);
			groupBox_cruise->setTitle("Cruise Mode [tab]");
		} else if (radioButton_cruise_both->isChecked()) {
			scheduler->setCruiseMode(VP1ExecutionScheduler::BOTH);
			groupBox_cruise->setTitle("Cruise Mode [event && tab]");
		} else { assert(0); }
	} else {
		scheduler->setCruiseMode(VP1ExecutionScheduler::NONE);
		groupBox_cruise->setTitle("Cruise Mode [off]");
	}
}

//_________________________________________________________________________________
void VP1MainWindow::showMenu_loadPlugin()
{
	menu_loadPlugin->clear();

	QMap<QString,QString> plugins2fullpath = availablePluginFiles();

	if (plugins2fullpath.empty()) {
		menu_loadPlugin->addAction("No plugins found")->setEnabled(false);
		return;
	}

	QStringList pluglist(plugins2fullpath.keys());
	pluglist.sort();

	QStringList currentpluginfiles = channelmanager->currentPluginFiles();
	foreach(QString plug,pluglist) {
		QAction * act = menu_loadPlugin->addAction(plug);
		assert(plugins2fullpath.contains(plug));
		QString fullpath = plugins2fullpath[plug];
		if (currentpluginfiles.contains(fullpath)) {
			act->setEnabled(false);
		} else {
			act->setData(fullpath);
			act->setStatusTip(fullpath);
			connect(act,SIGNAL(triggered(bool)),this,SLOT(showMenu_loadPluginItemSelected()));
		}
	}

	menu_loadPlugin->addSeparator();
	QAction * act_browse = menu_loadPlugin->addAction("&Browse...");
	act_browse->setStatusTip("Browse filesystem for VP1 plugin files");
	connect(act_browse,SIGNAL(triggered(bool)),this,SLOT(request_loadPlugin()));

}

//_________________________________________________________________________________
void VP1MainWindow::showMenu_loadPluginItemSelected()
{
	QAction * act = static_cast<QAction*>(sender());
	assert(act);
	if (!act)
		return;
	loadPluginFile(act->data().toString());
}


//_________________________________________________________________________________
void VP1MainWindow::showMenu_loadConfFile()
{
	menu_loadConfFile->clear();

	QMap<QString,QString> conffile2fullpath =  availableFiles( ".vp1", "DATAPATH", "", "VP1CONFIGFILEPATH", true );

	if (conffile2fullpath.empty()) {
		menu_loadConfFile->addAction("No .vp1 config files found")->setEnabled(false);
		return;
	}

	QStringList filelist(conffile2fullpath.keys());
	filelist.sort();

	foreach(QString file,filelist) {
		QAction * act = menu_loadConfFile->addAction(file);
		assert(conffile2fullpath.contains(file));
		QString fullpath = conffile2fullpath[file];
		act->setData(fullpath);
		act->setStatusTip(fullpath);
		connect(act,SIGNAL(triggered(bool)),this,SLOT(showMenu_loadConfFileItemSelected()));
	}

	menu_loadConfFile->addSeparator();
	QAction * act_browse = menu_loadConfFile->addAction("&Browse...");
	act_browse->setStatusTip("Browse filesystem for .vp1 config file");
	connect(act_browse,SIGNAL(triggered(bool)),this,SLOT(request_loadConfig()));

}


//_________________________________________________________________________________
void VP1MainWindow::showMenu_loadConfFileItemSelected()
{
	QAction * act = static_cast<QAction*>(sender());
	assert(act);
	if (!act)
		return;
	loadConfigurationFromFile(act->data().toString());
}

//_________________________________________________________________________________
void VP1MainWindow::changeStyleActionTriggered()
{
	QAction * act = static_cast<QAction*>(sender());
	assert(act);
	if (!act)
		return;
	assert(QStyleFactory::keys().contains(act->text()));
	if (!QStyleFactory::keys().contains(act->text()))
		return;
	QApplication::setStyle(QStyleFactory::create(act->text()));
	QSettings s(settingsfile,QSettings::IniFormat);
	s.setValue("style/defaultstyle",act->text());

}

//_________________________________________________________________________________
void VP1MainWindow::changeFontSizeActionTriggered()
{
	QAction * act = static_cast<QAction*>(sender());
	assert(act);
	if (!act)
		return;
	bool ok;
	int goal = act->data().toInt(&ok);
	assert(ok);
	if (!ok)
		return;
	changeFontSize(goal);
}

//_________________________________________________________________________________
void VP1MainWindow::changeFontSize(int goal)
{
	QFont newfont = m_defaultfont;
	QSettings s(settingsfile,QSettings::IniFormat);
	if (m_defaultfont_pointsize<0.0) {
		//pixels
		assert(m_defaultfont_pixelsize+goal>0);
		if (m_defaultfont_pixelsize+goal<=0)
			return;
		if (m_defaultfont_pixelsize+goal>200)
			return;
		newfont.setPixelSize(m_defaultfont_pixelsize+goal);
		s.setValue("font/pixelsize_diff",goal);
	} else {
		//pointsize
		double fact = (goal+100.0)/100.0;
		if (fact<0.01||fact>50)
			return;
		double newpointsize=m_defaultfont_pointsize*fact;
		if (newpointsize<0)
			return;
		newfont.setPointSizeF(newpointsize);
		s.setValue("font/pointsize_relativepercent",goal);
	}
	QApplication::setFont(newfont);
}


//_________________________________________________________________________________
void VP1MainWindow::updateCentralStackWidget()
{
	QWidget * targetpage = tabWidget_central->count() ? page_tabwidget : page_instructions;
	if (stackedWidget_central->currentWidget() != targetpage)
		stackedWidget_central->setCurrentWidget(targetpage);
}

//_________________________________________________________________________________
void VP1MainWindow::quickSetupTriggered()
{
	VP1Msg::messageVerbose("VP1MainWindow::quickSetupTriggered()");

	QString plugfile, channelname, tabname;

	if (sender()==pushButton_quicksetup_geometrystudies||sender()==action_quicklaunch_Geometry_studies) {
		plugfile="libVP1GeometryPlugin.so";
		channelname="Geometry";
		tabname = "Geometry";
	} else if (sender()==pushButton_quicksetup_trackingstudies||sender()==action_quicklaunch_Tracking_studies) {
		plugfile="libVP1TrackPlugin.so";
		channelname="Tracking";
		tabname = "Tracking";
	} else if (sender()==pushButton_quicksetup_calostudies||sender()==action_quicklaunch_Calo_studies) {
		plugfile="libVP1CaloPlugin.so";
		channelname="Calo Cells";
		tabname = "Calorimeter";
	} else if (sender()==action_quicklaunch_Storegate_browser) {
		plugfile="libVP1BanksPlugin.so";
		channelname="Banks";
		tabname = "StoreGate";
	} else if (sender()==pushButton_quicksetup_3dcocktail||sender()==action_quicklaunch_3dcocktail) {
		plugfile="libVP13DCocktailPlugin.so";
		channelname="3DCocktail";
		tabname = "3D Cocktail";
	} else if (sender()==action_quicklaunch_trackcalo_commis) {
		plugfile="libVP13DCocktailPlugin.so";
		channelname="TrackCalo";
		tabname = "Track/Calo";
	} else if (sender()==pushButton_quicksetup_analysisstudies) {
		plugfile="libVP1AODPlugin.so";
		channelname="AOD";
		tabname = "Analysis";
	} else {
		addToMessageBox("quickSetupTriggered() Error: Unknown sender");
		return;
	}

#ifdef __APPLE__
	if (plugfile.endsWith(".so"))
		plugfile = plugfile.left(plugfile.count()-3)+".dylib";
#endif


	//Check that the plugin is available:
	QMap<QString,QString> plugins2fullpath = availablePluginFiles();
	if (!plugins2fullpath.contains(plugfile)) {
		QMessageBox::critical(0, "Error - could not locate plugin file: "+plugfile,
				"could not locate plugin file: "
				+plugfile,QMessageBox::Ok,QMessageBox::Ok);
		return;
	}
	QString plugfile_fullpath = plugins2fullpath[plugfile];


	//Load plugin
	if (!channelmanager->currentPluginFiles().contains(plugfile_fullpath)) {
		QString err = channelmanager->loadPluginFile(plugfile_fullpath);
		if (!err.isEmpty()||!channelmanager->currentPluginFiles().contains(plugfile_fullpath)) {
			QMessageBox::critical(0, "Error - could not load plugin file: "+plugfile_fullpath,//Fixme: Error message here is hardcoded to be the same as in loadPluginFile method!!
					"Could not load plugin file: "
					+plugfile_fullpath+"\n\nReason: "+err,QMessageBox::Ok,QMessageBox::Ok);
			return;
		}
	}


	//Check that plugin contains necessary channel:
	if (!channelmanager->channelsInPluginFile(plugfile_fullpath).contains(channelname)) {
		QMessageBox::critical(0, "Error - did not find necessary channel: "+channelname,
				"Could not find channel: "+channelname+" in loaded plugin "+plugfile_fullpath,
				QMessageBox::Ok,QMessageBox::Ok);
		return;
	}

	bool save = updatesEnabled();
	setUpdatesEnabled(false);


	//Add tab:
	QString newtabname = tabmanager->suggestNewTabName(tabname);
	tabmanager->addNewTab(newtabname);
	if (!tabmanager->hasTab(newtabname)) {
		QMessageBox::critical(0, "Error - could not create tab: "+newtabname,
				"Could not create tab: "+newtabname,
				QMessageBox::Ok,QMessageBox::Ok);
		setUpdatesEnabled(save);
		return;
	}


	//Finally, add channel:

	if (!tabmanager->addChannelToTab( channelname, newtabname )) {
		QMessageBox::critical(0, "Error - problems launching channel: "+channelname,
				"Problems launching channel: "+channelname,
				QMessageBox::Ok,QMessageBox::Ok);
		setUpdatesEnabled(save);
		return;
	}


	tabmanager->showTab(newtabname);


	setUpdatesEnabled(save);


}

//_________________________________________________________________________________
void VP1MainWindow::updateEventControls()
{
	pushButton_nextevent->setEnabled(okToProceedToNextEvent());
}

//_________________________________________________________________________________
void VP1MainWindow::updateInputDirectoriesStatus()
{
	if(!m_mutex->tryLock()) return;

	QFont f;
	QFont fb;
	fb.setBold(true);

	foreach (QAction * act,inputdiractions) {
		VP1DirStatusData& dirstatus = inputdirstatuses[act];
		QString inputdir(act->data().toString());
		QString dirname = QDir(inputdir).dirName();
		act->setEnabled(dirstatus.enabled);
		act->setFont(dirstatus.bold?fb:f);
		act->setText(dirname+" ["+dirstatus.dirStatus+"]");
	}

	m_mutex->unlock();
}

//_________________________________________________________________________________
void VP1MainWindow::inputDirectoryActionTriggered()
{
	QAction * act = dynamic_cast<QAction*>(sender());
	if (!act)
		return;
	VP1AvailEvtsLocalDir* availLocal = dynamic_cast<VP1AvailEvtsLocalDir*>(availEvents);
	if (!availLocal)
		return;
	QString inputdir(act->data().toString());
	QFileInfo fi(inputdir);
	if (fi.exists()&&fi.isDir()) {
		availLocal->setSourceDir(inputdir);
		std::cout<<VP1Msg::prefix_msg()<<": "
				<<"VP1Message: inputdirectory changed to "
				<<availLocal->currentSourceDir().toStdString()<<std::endl;
		currentStream = QDir(availLocal->currentSourceDir()).dirName();
	}


}

//________________________________________________________________________________
QStringList VP1MainWindow::userRequestedFiles()
{
	QStringList returnval(m_userRequestedFiles);
	m_userRequestedFiles.clear();
	return returnval;
}

//_________________________________________________________________________________
void VP1MainWindow::addEventFile()
{
	QString newEventFile = QFileDialog::getOpenFileName(NULL
			,tr("Open Event File")
			,VP1Settings::defaultFileSelectDirectory()
	,tr("All files (*.*)"));
	if(newEventFile.isEmpty()) return;
	if(VP1FileUtilities::fileExistsAndReadable(newEventFile.toStdString())) {
		m_userRequestedFiles.append(newEventFile);
		addToMessageBox(newEventFile + " will be appended to the list of input files");
	}
	else
		QMessageBox::critical(0, newEventFile + " either does not exist or is not readable",
				newEventFile + " either does not exist or is not readable",
				QMessageBox::Ok,QMessageBox::Ok);
}
