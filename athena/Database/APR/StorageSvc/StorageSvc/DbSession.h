/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbSession.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbSession class definition
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//
//  Description: Definition of a Database session and related classes
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBSESSION_H
#define POOL_DBSESSION_H

// Framework include files
#include "StorageSvc/DbStatus.h"
#include "StorageSvc/DbHandleBase.h"

/*
 *   POOL namespace declaration
 */
namespace pool  {

  // Forward declarations
  class DbType;
  class DbDomainObj;
  class DbSessionObj;
  class IOODatabase;

  /** Db objects: class DbSession

      Description:
      Handle managing a DbSessionObj, which allows to access
      domains and Databasees of a given storage type.

      @author  M.Frank
      @version 1.0
  */
  class DbSession : public DbHandleBase<DbSessionObj> {
    /// Friend declarations
    friend class DbSessionObj;
  private:
    /// Assign transient object properly (including reference counting)
    void switchPtr(const DbSessionObj* obj)  const;
  public:
    /// Constructor
    DbSession();
    /// Copy constructor
    DbSession(const DbSession& copy);
    /// Object constructor
    DbSession(const DbSessionObj* session);
    /// Standard destructor
    virtual ~DbSession();
    /// Assignment operator   
    DbSession& operator=(const DbSession& copy)    {
      if ( &copy != this )  {
        switchPtr(copy.m_ptr);
      }
      return *this;
    }
    /// Assignment operator
    DbSession& operator=(const int /* null_obj */ )    {
      switchPtr(0);
      return *this;
    }
    /// Access reference counter
    int refCount() const;
    /// Find domain object in session (by technology type)
    DbDomainObj* find(const DbType& type) const;
    /// Add domain to session
    DbStatus add(DbDomainObj* dom) const;
    /// Find domain in session
    DbStatus remove(const DbDomainObj* dom) const;
    /// Open the session in a given mode
    DbStatus open(void* context);
    /// Close the session
    DbStatus close() const;
    /// Allow access to the Database implementation
    IOODatabase* db(const DbType& typ)   const;
    /// Inject new Database implementation
    DbStatus addDb(IOODatabase* impl)   const;
  };
}       // End namespace pool
#endif  // POOL_DBSESSION_H
