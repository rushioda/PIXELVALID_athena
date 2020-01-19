/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  ClusterTOB.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef __TopoCore__ClusterTOB__
#define __TopoCore__ClusterTOB__

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class ClusterTOB : public BaseTOB {
   public:
      
      static unsigned int nBitsEt() { return g_nBitsEt; }
      static unsigned int nBitsIsolation() { return g_nBitsIsolation; }
      static unsigned int nBitsEta() { return g_nBitsEta; }
      static unsigned int nBitsPhi() { return g_nBitsPhi; }


      // default constructor
      ClusterTOB(uint32_t roiWord = 0);
      
      // constructor with individual values
      ClusterTOB(unsigned int et, unsigned int isolation, int eta, int phi, inputTOBType_t tobType = NONE, uint32_t roiWord = 0);

      // constructor with initial values
      ClusterTOB(const ClusterTOB & cluster);

      // destructor
      virtual ~ClusterTOB();

      // accessors
      unsigned int Et() const { return m_Et; }
      unsigned int isolation() const { return m_isolation; }
      int eta() const { return m_eta; }
      int phi() const { return m_phi; }

      double EtDouble() const { return m_EtDouble; }
      double etaDouble() const { return m_etaDouble; }
      double phiDouble() const { return m_phiDouble; }
      
      // setters
      void setEt(unsigned int et) { m_Et = sizeCheck(et, nBitsEt()); }
      void setIsolation(unsigned int et) { m_isolation = sizeCheck(et, nBitsIsolation()); }
      void setEta(int eta) { m_eta = sizeCheck(eta, nBitsEta()); }
      void setPhi(int phi) { m_phi = sizeCheck(phi, nBitsPhi()); }
      
      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }
      
      // memory management
      static ClusterTOB* createOnHeap(const ClusterTOB& cl);
      static void clearHeap();
      static const Heap<TCS::ClusterTOB>& heap() { return fg_heap; }

      static unsigned int instances() { return fg_instances; }

      virtual void print(std::ostream &o) const;

      void setTobType(inputTOBType_t tobType) { m_tobType = tobType; }

      inputTOBType_t tobType() const { return m_tobType; }

   private:
      static unsigned int g_nBitsEt;
      static unsigned int g_nBitsIsolation;
      static unsigned int g_nBitsEta;
      static unsigned int g_nBitsPhi;
      
      unsigned int m_Et {0};
      unsigned int m_isolation {0};
      int m_eta {0};
      int m_phi {0};

      double m_EtDouble {0};
      double m_etaDouble {0};
      double m_phiDouble {0};

      inputTOBType_t  m_tobType { NONE };

      static unsigned int fg_instances;

      static Heap<TCS::ClusterTOB> fg_heap;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__ClusterTOB__) */
