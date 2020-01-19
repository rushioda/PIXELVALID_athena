/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP12DStandardChannelWidget    //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP12DStandardChannelWidget.h"
#include "VP1Base/IVP12DSystem.h"
#include "VP1Base/IVP12DSystemSimple.h"
#include "VP1Base/VP1GraphicsItemCollection.h"
#include "VP1Base/VP1GraphicsView.h"
#include "VP1Base/VP12DExaminerViewer.h"
#include "VP1Base/VP1TabWidget.h"
#include "VP1Base/VP1ControllerHelper.h"
#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1Base/VP1Msg.h"

#include <QtGui/QCheckBox>
#include <QtGui/QGraphicsScene>
#include <QtCore/QMap>
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <cassert>

#include <QtGui/QVBoxLayout>

class IVP12DStandardChannelWidget::Imp {
public:
  IVP12DStandardChannelWidget*channel;
  VP1GraphicsView * view;
  bool first;
  QMap<IVP12DSystem*,QSet<VP1GraphicsItemCollection*> > system2itemcols;

  QMap<QCheckBox*,IVP1System*> checkbox2system;
  QList<IVP1System*> systemsAllowedControllers;
  QList<QPair<IVP1System*,bool> > system2switchable;
  QMap<IVP1System*,bool> system2startdisabled;

  void updateSystemState(QCheckBox*);

  QMap<IVP1System*,QWidget*> sys2tabpage;
  VP1TabWidget*tabwidget;

  VP1ColorSelectButton * colorselectbutton;

};

//___________________________________________________________________________
IVP12DStandardChannelWidget::IVP12DStandardChannelWidget(const QString & name, const QString & information, const QString & contact_info)
  : IVP1ChannelWidget(name,information,contact_info), d(new Imp())
{
  d->channel=this;
  d->first=true;
  setMinimumSize(150,240);//minimum y size is to avoid absurd squeezing of buttons.

  QVBoxLayout * vboxLayout = new QVBoxLayout(this);
  vboxLayout->setSpacing(0);
  vboxLayout->setMargin(0);
  VP12DExaminerViewer * examiner = new VP12DExaminerViewer(this);
  vboxLayout->addWidget(examiner);
  d->colorselectbutton = 0;

  d->view = examiner->view();

  d->tabwidget = 0;
}

//___________________________________________________________________________
IVP12DStandardChannelWidget::~IVP12DStandardChannelWidget()
{
  delete d;
}

//___________________________________________________________________________
void IVP12DStandardChannelWidget::addSystem( IVP12DSystem*system, const SystemOptions& options ) {

  bool handleSelections = ! (options & DisallowSelections);
  bool switchable = ! (options & DisallowSwitchable);
  bool allowController = ! (options & DisallowController);
  bool allowMovable = ! (options & DisallowMovable);
  bool startDisabled = (options & StartDisabled);

  registerSystem(system);

  assert(!d->system2itemcols.contains(system));
  d->system2itemcols.insert(system,system->getItemCollections());
  foreach (VP1GraphicsItemCollection*ic,system->getItemCollections()) {
    d->view->addItemCollection(ic);
  }

  foreach (VP1GraphicsItemCollection* ic,system->getItemCollections()){
    d->view->setDisallowInteractions(ic, !handleSelections );
  }
  foreach (VP1GraphicsItemCollection* ic,system->getItemCollections()){
    d->view->setDisallowMovable(ic, !allowMovable );
  }

  d->system2switchable << QPair<IVP1System*,bool>(system,switchable);

  assert(!d->system2startdisabled.contains(system));
  d->system2startdisabled.insert(system,startDisabled);
  assert(d->system2startdisabled.contains(system));

  if (allowController) {
    d->systemsAllowedControllers << system;
    connect(system,SIGNAL(itemFromSystemSelected()),this,SLOT(showControlsForSystem()));
  }
}

//___________________________________________________________________________
void IVP12DStandardChannelWidget::create() {
  //Fixme: Less code should be replicated here and in IVP13DStandardChannelWidget+IVP12DDetViewsChannelWidget

  //Set up the controller.
  registerController(VP1ControllerHelper::compositionController( d->systemsAllowedControllers,
								 d->sys2tabpage,d->tabwidget,
								 d->system2switchable,
								 d->checkbox2system,
								 d->colorselectbutton ));
  connect(d->colorselectbutton,SIGNAL(colorChanged(const QColor&)),this,SLOT(setBackgroundColor(const QColor&)));
  d->system2switchable.clear();

  QMapIterator<QCheckBox*,IVP1System*> it(d->checkbox2system);
  while (it.hasNext()) {
    it.next();

    assert(d->system2startdisabled.contains(it.value()));
    if (d->system2startdisabled[it.value()]) {
      it.key()->setChecked(false);
      d->updateSystemState(it.key());
    }

    connect(it.key(),SIGNAL(toggled(bool)),this,SLOT(toggleSystemActive()));
  }
}


//___________________________________________________________________________
void IVP12DStandardChannelWidget::systemRefreshed(IVP1System*) {
  if (d->first) {
    d->view->fitViewToContents();
    d->first=false;
  }
}

//___________________________________________________________________________
void IVP12DStandardChannelWidget::systemErased(IVP1System*) {
  d->view->clearSelections();
}

//___________________________________________________________________________
void IVP12DStandardChannelWidget::toggleSystemActive()
{
  QCheckBox * cb = static_cast<QCheckBox*>(sender()); assert(cb);
  d->updateSystemState(cb);
}

//___________________________________________________________________________
void IVP12DStandardChannelWidget::Imp::updateSystemState(QCheckBox* cb)
{
  assert(checkbox2system.contains(cb));
  IVP12DSystem*sys = static_cast<IVP12DSystem*>(checkbox2system.value(cb)); assert(sys);

  assert(system2itemcols.contains(sys));
  if (cb->isChecked()) {
    channel->turnOn(sys);
    foreach(VP1GraphicsItemCollection*ic,system2itemcols.value(sys)) {
      ic->reattachToView();
    }
    if (tabwidget&&sys2tabpage.contains(sys)) {
      int sysindex = systemsAllowedControllers.indexOf(sys);
      if (sysindex>=0) {
	IVP1System*nextsystem(0);
	for (int i=sysindex+1;i<systemsAllowedControllers.count();++i) {
	  //Loop through latter systems with controllers in order and
	  //find the first of those which currently has an active
	  //controller:
	  IVP1System* testsys = systemsAllowedControllers.at(i);
	  if (sys2tabpage.contains(testsys)&&tabwidget->indexOf(sys2tabpage[testsys])>-1) {
	    nextsystem=testsys;
	    break;
	  }
	}
	//put before tab of "nextsystem" (or at the end if no nextsystem):
	int index = (nextsystem?tabwidget->indexOf(sys2tabpage[nextsystem]):99999);
	tabwidget->insertTab(index,sys2tabpage[sys],sys->name());
	int index2 = tabwidget->indexOf(sys2tabpage[sys]);
	if (index2!=-1) {
	  tabwidget->setTabEnabled(index2,true);
	}
      }
    }
  } else {
    //Fixme: if system being turned off has selections, we should deselect!!
    channel->turnOff(sys,false);
    foreach(VP1GraphicsItemCollection*ic,system2itemcols.value(sys)) {
      ic->detachFromView();
    }
    if (tabwidget&&sys2tabpage.contains(sys)) {
      int index = tabwidget->indexOf(sys2tabpage[sys]);
      if (index!=-1) {
	tabwidget->setTabEnabled(index,false);
	tabwidget->removeTab(index);
      }
    }
  }

  view->scene()->update();

}

//___________________________________________________________________________
void IVP12DStandardChannelWidget::addSystem(IVP12DSystemSimple* sys, const SystemOptions& options )
{
  addSystem((IVP12DSystem*)sys,options);
}

//___________________________________________________________________________
void IVP12DStandardChannelWidget::setBackgroundColor(const QColor & col)
{
  if (!col.isValid())
    return;
  d->view->setBackgroundBrush(col);
}

//___________________________________________________________________________
void IVP12DStandardChannelWidget::showControlsForSystem(  )
{
  if (!d->tabwidget)
    return;
  IVP1System * sys = static_cast<IVP1System*>(sender());
  if (!sys) {
    message("showControlsForSystem Error: Unable to determine system identity.");
    return;
  }
  if (!d->sys2tabpage.contains(sys)) {
    //Dont send warning here. The system in question might simply not have a controller!
    return;
  }

  int index = d->tabwidget->indexOf(d->sys2tabpage[sys]);
  if (index<0||!d->tabwidget->isTabEnabled(index)) {
    message("Warning: Asked to show controller for a disabled system. Surely you jest?");
    return;
  }

  d->tabwidget->setCurrentIndex(index);
}


//___________________________________________________________________________
QByteArray IVP12DStandardChannelWidget::saveState()
{
  VP1Msg::messageVerbose("IVP12DStandardChannelWidget::saveState");
  // ===> Setup stream writing to a byteArray:
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);
  QDataStream out(&buffer);

  // ===> Write Data:

  //Version & base state:
  out << (qint32)0; //version
  out << IVP1ChannelWidget::saveState();//Always include state info from the base class.

  //Background color:
  out << d->colorselectbutton->color();

  //Systems turned on/off:
  //Fixme: Make sure that if you have two copies of the same system,
  //that the text in the checkbox gets appended some stuff like [1],
  //[2], etc., so that the strings used here will be unique.
  QMap<QString, bool> sysname2turnedon;
  QMap<QCheckBox*,IVP1System*>::const_iterator it = d->checkbox2system.constBegin();
  while (it != d->checkbox2system.constEnd()) {
    sysname2turnedon.insert(it.key()->text(),it.key()->isChecked());
     ++it;
  }

  out << sysname2turnedon;

  //Current system tab:
  if (d->tabwidget)
    out << d->tabwidget->tabText(d->tabwidget->currentIndex());
  else
    out << QString("");

  // ===> Finish up:
  buffer.close();
  return byteArray;
}

//___________________________________________________________________________
void IVP12DStandardChannelWidget::restoreFromState(QByteArray ba)
{
  VP1Msg::messageVerbose("IVP12DStandardChannelWidget::restoreFromState");

  // ===> Setup stream for getting the contents of the byteArray:
  QBuffer buffer(&ba);
  buffer.open(QIODevice::ReadOnly);
  QDataStream state(&buffer);
  // ===> Check version and pass on state info to base class:
  qint32 version;
  state >> version;
  if (version!=0) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  QByteArray basestate;
  state >> basestate;
  IVP1ChannelWidget::restoreFromState(basestate);
  // ===> Decode the state info:

  QColor bgdcol;
  state >> bgdcol;
  if (bgdcol!=d->colorselectbutton->color())
    d->colorselectbutton->setColor(bgdcol);
  setBackgroundColor(bgdcol);

  //Switch systems on/off:
  QMap<QString, bool> sysname2turnedon;
  state >> sysname2turnedon;
  QMap<QCheckBox*,IVP1System*>::const_iterator it = d->checkbox2system.constBegin();
  while (it != d->checkbox2system.constEnd()) {
    if (sysname2turnedon.contains(it.key()->text())) {
      if (sysname2turnedon[it.key()->text()]!=it.key()->isChecked())
	it.key()->setChecked(sysname2turnedon[it.key()->text()]);
    } else {
      message("Warning: Config data does not contain information about switched state of subsystem '"+it.key()->text()+"'");
    }
     ++it;
  }

  //Current system tab
  QString tabname;
  state >> tabname;
  if (d->tabwidget) {
    for (int i = 0; i < d->tabwidget->count(); ++i) {
      if (d->tabwidget->tabText(i) == tabname) {
	d->tabwidget->setCurrentIndex(i);
	break;
      }
    }
  }

  // ===> Finish up:
  buffer.close();
}
