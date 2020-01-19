// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_GETMESSAGESVC_H
#define ATHENAKERNEL_GETMESSAGESVC_H 1
/** @file getMessageSvc.h
 *  @brief singleton-like access to IMessageSvc via open function and helper
 *
 *  $Id: getMessageSvc.h,v 1.6 2008-07-14 22:10:14 calaf Exp $
 *  @author Paolo Calafiura - Atlas collaboration
   */
#include <cassert>
#include <string>

class IMessageSvc;
namespace Athena {
  namespace Options {
    enum CreateOptions {
      Eager,
      Lazy
    };
  }
  /** @fn IMessageSvc* getMessageSvc()
   *  @brief returns a pointer to the Gaudi IMessageSvc interface 
   *  @param o : if o is @c Athena::Options::Eager it will create a
   *             @c MessageSvc instance if it is not there already.
   *             The default is to be lazy and don't create anything.
   *  @param quiet : do not print warning if MessagesSvc cannot be found (default false)
   *
   *  Also used via weak-linking from DetCommon code (see e.g. TrigConf::MsgStream in DetCommon)
   *  DO NOT MODIFY THE SIGNATURE OF THESE METHODS WITHOUT UPDATING THE DETCOMMON SIDE !!!
   */
  IMessageSvc* getMessageSvc( bool quiet=false );
  IMessageSvc* getMessageSvc( const Options::CreateOptions o, bool quiet=false );

  /// Set this to force off the warning messages from getMessageSvc
  /// (in unit tests, for example).
  extern bool getMessageSvcQuiet;

  //@{
  /** Wrappers for some of the IMessageSvc methods
   *  These can be used from libraries without explicit Gaudi dependency via weak linking.
   *  (see e.g. TrigConf::MsgStream in DetCommon)
   *
   *  DO NOT MODIFY THE SIGNATURE OF THESE METHODS WITHOUT UPDATING THE DETCOMMON SIDE !!!
   */
  void reportMessage(IMessageSvc* ims, const std::string &source, int type, const std::string &message);
  int outputLevel(IMessageSvc* ims, const std::string &source);
  void setOutputLevel(IMessageSvc* ims, const std::string &source, int level);
  //@}

  /** @class IMessageSvcHolder
   *  @brief get a IMessageSvc* on 1st use (if not set) and release it on ~
   */
  class IMessageSvcHolder {
  public:
    /// will take the IMessageSvc* lazily from getMessageSvc()  in get()
    /// Slow, but useful for classes without direct access to IMessageSvc*
    IMessageSvcHolder() : m_ims() {}

    /// allows to get the IMessageSvc immediately
    /// @param o: if o is @c Athena::Options::Eager it will create a
    /// @c MessageSvc instance there and then.
    IMessageSvcHolder(const Options::CreateOptions o);

    /// Not terribly useful per se,  since one could simply use the
    /// IMessageSvc* directly, but helps implementing 
    /// @class Athena::MsgStreamMember
    /// Notice that when ~IMessageSvcHolder is called ims ref count will go
    /// back to what it was before passing it to this constructor
    IMessageSvcHolder(IMessageSvc *ims);

    /// behaves like the constructor taking an IMessageSvc *
    IMessageSvcHolder(const IMessageSvcHolder&);
    IMessageSvcHolder& operator=(const IMessageSvcHolder&);

    /// releases the IMessageSvc
    ~IMessageSvcHolder();
    /// upon first access sets m_ims as needed
    IMessageSvc* get() const;
    /// conversion to IMessageSvc*, same as get
    operator const IMessageSvc*() const { return this->get(); }
    /// conversion to IMessageSvc*, same as get
    operator IMessageSvc*() { return this->get(); }
  private:
    mutable IMessageSvc* m_ims; //mutable to have op() const
  };
}

#endif
