/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_DBLoader
#define TrigConf_DBLoader

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/ILoader.h"
#include "RelationalAccess/ISessionProxy.h"
#include "TrigConfBase/TrigConfMessaging.h"

namespace TrigConf {

   /**@brief Base class for loaders of configurations from the TriggerDB*/
   class DBLoader : public TrigConfMessaging,
                    virtual public ILoader
   {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       *
       * @param session reference to the database session
       */
      explicit DBLoader( StorageMgr& sm, coral::ISessionProxy& session );

      explicit DBLoader( const std::string& name, StorageMgr& sm, coral::ISessionProxy& session );

      /**@brief destructor*/
      virtual ~DBLoader(){};

      /**@brief access to output stream*/
      //std::ostream & msg() { return m_storageMgr.msg(); }

      virtual void setLevel(MSGTC::Level lvl);
      virtual MSGTC::Level outputLevel() const { return msg().level(); }


      /** @brief start session if not already active */
      void startSession();

      /** @brief commit session if not already done */
      void commitSession() const;

      /** @brief get l1 master from super master */
      bool loadL1MasterKey(int SuperMasterKey, int& Lvl1MasterKey); 

      /** @brief get l1 menu id from super master */
      bool loadL1MenuKey(int SuperMasterKey, int& Lvl1MenuKey); 

      unsigned int triggerDBSchemaVersion() { if(s_triggerDBSchemaVersion==0) loadSchemaVersion(); return s_triggerDBSchemaVersion; }

      virtual int verbose() const { return m_verbose; }
      virtual void setVerbose(int v) { m_verbose=v; }

      bool isRun1() { return ! isRun2(); }
      bool isRun2();

   private:
      int m_verbose;

   public:
      typedef enum  {ALL, CTP, CTPOnl, HLT, COOLL1, COOLHLT, L1Simu} ENV;
      static        ENV getEnv()    { return m_env; }
      static void   setEnv(ENV env) { m_env = env;  }

   protected:
      StorageMgr&                    m_storageMgr; ///< reference to the storage manager
      coral::ISessionProxy&          m_session;    ///< CORAL interface to database session
      bool                           m_sessionOwner; ///< remember if the loader started the session in the first place
      static unsigned int            s_triggerDBSchemaVersion; ///< the version of the TriggerDB schema
      static unsigned int            s_run; ///< database schema for run s_run
      static unsigned int            s_ctpVersion;
      static unsigned int            s_l1Version;

   private:
      void loadSchemaVersion() const;

   private:
      static ENV m_env;

   };

}

#endif

