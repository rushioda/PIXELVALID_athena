/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File:  Generators/FlowAfterburnber/CheckFlow.h
// Description:
//    This is a simple algorithm to histogram particle properties
//    for diagnosing of flow generation
//
// AuthorList:
// Andrzej Olszewski: Initial Code February 2006
// Andrzej Olszewski: Converted to ROOT histograms July 2007
#include "FlowAfterburner/CheckFlow.h"
#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

#include "GaudiKernel/ITHistSvc.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ObjectList.h"

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/GenAll.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include "GeneratorObjects/HijingEventParams.h"

using namespace TruthHelper;

typedef std::vector<const HepMC::GenParticle*>  MCparticleCollection ;

CheckFlow::CheckFlow(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_hgenerated ( NULL ),
  m_b ( NULL ),
  m_phi ( NULL ),
  m_phiR ( NULL ),
  m_phi_vs_phiR ( NULL ),
  m_phiv1reco_vs_phiR ( NULL ),
  m_phiv2reco_vs_phiR ( NULL ),
  m_phi_vs_phiR_etap ( NULL ),
  m_phi_vs_phiR_etan ( NULL ),
  m_v2betapth ( NULL ),
  m_ebetapth ( NULL ),
  m_sgSvc ( NULL ),
  m_thistSvc ( NULL ),
  m_tesIO ( NULL ) 
{
  //Declare the algorithm's properties
  declareProperty("McEventKey",     m_key="FLOW_EVENT");
  declareProperty("HistogramFlag", m_produceHistogram = true );
  declareProperty("ImpactCutMin", m_bcut_min = 0 );
  declareProperty("ImpactCutMax", m_bcut_max = 99 );
  declareProperty("PtCutMin", m_ptcut_min = 0 );
  declareProperty("PtCutMax", m_ptcut_max = 999999 );
  declareProperty("RapidityCutMin", m_rapcut_min = 0 );
  declareProperty("RapidityCutMax", m_rapcut_max = 5.5 );
}

StatusCode CheckFlow::initialize(){
  StatusCode result = StatusCode::SUCCESS;
  msg(MSG::INFO) << ">>> CheckFlow from Initialize" << endreq;

  StatusCode sc = service("StoreGateSvc", m_sgSvc);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not find StoreGateSvc" << endreq;
    return sc;
  }

  m_hgenerated = new TH1F("ngen","Generated",100,0,100000);
  m_b = new TH1F("b","Impact parameter",35,0.,35.0);
  m_phi = new TH1F("phi","Phi",100,-M_PI,M_PI);
  m_phiR = new TH1F("phiR","PhiR",100,0,2*M_PI);
  m_phi_vs_phiR = new TH1F("phi_vs_phiR","Phi - PhiR",100,-M_PI,M_PI);
  m_phiv1reco_vs_phiR = new TH2F("phiv1reco_vs_phiR",
			  "PhiV1Reco vs PhiR", 30,-M_PI,M_PI,30,-M_PI,M_PI);
  m_phiv2reco_vs_phiR = new TH2F("phiv2reco_vs_phiR",
                          "PhiV2Reco vs PhiR", 30,-M_PI/2,M_PI/2,30,-M_PI/2,M_PI/2);
  m_phi_vs_phiR_etap = new TH1F("phi_vs_phiR_etap",
					"Phi - PhiR positive eta",
					100,-M_PI,M_PI);
  m_phi_vs_phiR_etan = new TH1F("phi_vs_phiR_etan",
					"Phi - PhiR negative eta",
					100,-M_PI,M_PI);
  m_v2betapth = new TH3F("v2betapth",
				 "V2 vs b, eta, pt",
				 20,0,20, 30,-7.5,7.5, 25,0,5000);
  m_ebetapth = new TH3F("ebetapth",
				 "Tracks vs b, eta, pt",
				 20,0,20, 30,-7.5,7.5, 25,0,5000);

  ITHistSvc *rootHistSvc;
  if (!service("THistSvc", rootHistSvc, true).isSuccess()) {
    msg(MSG::ERROR) << "Unable to locate THistSvc" << endreq;
    return StatusCode::FAILURE;
  }

  std::string StreamAndPath="/FlowOutPut/";
  std::string histPath = StreamAndPath;
  if ( rootHistSvc->regHist(histPath+m_hgenerated->GetName(), 
			    m_hgenerated).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_hgenerated->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_b->GetName(), 
			    m_b).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_b->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_phi->GetName(), 
			    m_phi).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_phi->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_phiR->GetName(), 
			    m_phiR).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_phiR->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_phi_vs_phiR->GetName(), 
			    m_phi_vs_phiR).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_phi_vs_phiR->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_phiv1reco_vs_phiR->GetName(), 
			    m_phiv1reco_vs_phiR).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_phiv1reco_vs_phiR->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_phiv2reco_vs_phiR->GetName(), 
			    m_phiv2reco_vs_phiR).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_phiv2reco_vs_phiR->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_phi_vs_phiR_etap->GetName(), 
			    m_phi_vs_phiR_etap).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_phi_vs_phiR_etap->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_phi_vs_phiR_etan->GetName(), 
			    m_phi_vs_phiR_etan).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_phi_vs_phiR_etan->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_v2betapth->GetName(), 
			    m_v2betapth).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_v2betapth->GetName() << endreq;

  if ( rootHistSvc->regHist(histPath+m_ebetapth->GetName(), 
			    m_ebetapth).isFailure() )
    msg(MSG::WARNING) << "Can't book "
	    << histPath+m_ebetapth->GetName() << endreq;

  msg(MSG::DEBUG) << "Histograms have been booked " << endreq;

  m_tesIO = new GenAccessIO();

  // return StatusCode::SUCCESS;
  return result;
}

StatusCode CheckFlow::execute() {
  //   HepMC::IO_PDG_ParticleDataTable pdg_io("PDGTABLE");
  //   static HepMC::ParticleDataTable *pp = NULL;
  //   if (pp == NULL) pp = pdg_io.read_particle_data_table();
  msg(MSG::INFO) << ">>> CheckFlow from execute" << endreq;

  //
  // Event parameters
  //



//---------------------------------------------------------------------------------------------------
  const HijingEventParams *hijing_pars;
  //HijingEventParams *hijing_pars;
//---------------------------------------------------------------------------------------------------






  if ( m_sgSvc->retrieve(hijing_pars, "Hijing_event_params").isFailure() ) {
//  if ( evtStore()->retrieve(hijing_pars, "Hijing_event_params").isFailure() ) {
    msg(MSG::ERROR) << "Could not retrieve Hijing_event_params"
	   << endreq;
    return StatusCode::FAILURE;
  }
  float b = hijing_pars->get_b();
  float phiR = hijing_pars->get_bphi();

  //msglog << MSG::DEBUG<<"SOUMYA  "
  // <<hijing_pars->get_psi(1)
  // <<"   "<<hijing_pars->get_psi(2)
  // <<"  "<<hijing_pars->get_psi(3)
  // <<hijing_pars->get_psi(4)
  // <<"   "<<hijing_pars->get_psi(5)
  // <<"  "<<hijing_pars->get_psi(6)  << endreq;

  // Check cut on impact parameter b
  if(b<m_bcut_min || b>m_bcut_max) 
    return StatusCode::SUCCESS;

  m_b->Fill(b, 1.);
  m_phiR->Fill(phiR, 1.);

  float ngenerated = 0;
  double phiv1_recon, phiv1_recop;
  double phiv1_reco = 0, phiv2_reco = 0;
  double phi_reco_sin1phip = 0, phi_reco_cos1phip = 0;
  double phi_reco_sin1phin = 0, phi_reco_cos1phin = 0;
  double phi_reco_sin2phi = 0, phi_reco_cos2phi = 0;
  // Iterate over MC particles  We are using the IsGenStable predicate from
  // IsGenStable ifs;
  GenAll ifs;
  std::vector<const HepMC::GenParticle*> particles;
  StatusCode stat = m_tesIO->getMC(particles, &ifs, m_key);
  if (stat.isFailure()) {
    msg(MSG::ERROR) << "Could not find " << m_key << endreq;
    return stat;
  }

  for (std::vector<const HepMC::GenParticle*>::iterator pitr = particles.begin();
       pitr != particles.end(); pitr++) {
    int pid = (*pitr)->pdg_id();
    int p_stat = (*pitr)->status();
    double pt = (*pitr)->momentum().perp();
    double rapid = (*pitr)->momentum().pseudoRapidity();
    double phi = (*pitr)->momentum().phi();
    msg(MSG::DEBUG)
	   << " PID = " << pid << " Status = " << p_stat
	   << " Eta = " << rapid << "  Phi = " << phi 
	   << " PhiR = " << phiR << endreq;
    
    if( (fabs(rapid) >= m_rapcut_min) && (fabs(rapid) <= m_rapcut_max) &&
	(fabs(pt) >= m_ptcut_min) && (fabs(pt) <= m_ptcut_max) ) {
      ngenerated++;
      m_phi->Fill(phi, 1.);
      double phi_corr = phi - phiR;
      // v2 vs b,eta,pt histograms
      m_v2betapth->Fill(b,rapid,pt,cos(2*phi_corr));
      m_ebetapth->Fill(b,rapid,pt,1);
      // -----------------
      // convert to (-pi,pi) range
      int kpi = (int)(phi_corr/(2*M_PI));
      phi_corr -= kpi * 2*M_PI;
      if (phi_corr < -M_PI) phi_corr += 2*M_PI;
      if (phi_corr >  M_PI) phi_corr -= 2*M_PI;
      // --------------------------------------
      m_phi_vs_phiR->Fill(phi_corr, 1.);
      if(rapid>=0) m_phi_vs_phiR_etap->Fill(phi_corr, 1.);
      else         m_phi_vs_phiR_etan->Fill(phi_corr, 1.);
      // -------------------------------------------------
      if( rapid >= 0 ) { 
	phi_reco_sin1phip += sin(1*phi);
	phi_reco_cos1phip += cos(1*phi); 
      } else {
	phi_reco_sin1phin += sin(1*phi);
	phi_reco_cos1phin += cos(1*phi);
      }
      phi_reco_sin2phi += sin(2*phi);
      phi_reco_cos2phi += cos(2*phi);
    }
  }
  m_hgenerated->Fill(ngenerated, 1.);

  // calculate event plane position 
  phiv1_recop = atan2( phi_reco_sin1phip,phi_reco_cos1phip );
  phiv1_recon = atan2( phi_reco_sin1phin,phi_reco_cos1phin ) + M_PI; 
  if( phiv1_recon > M_PI ) phiv1_recon -= 2*M_PI; 
  // averaged v1 plane position (in pos and neg eta ranges)
  phiv1_reco = (phiv1_recop + phiv1_recon)/2;
  phiv2_reco = 0.5 * atan2( phi_reco_sin2phi,phi_reco_cos2phi );
  msg(MSG::INFO)
	 << " PhiR = " << phiR 
	 << " PhiV1Reco = " << phiv1_reco 
	 << " PhiV2Reco = " << phiv2_reco << endreq;

  // convert phiR (0, 2*pi) to (-pi,pi) range
  double phiR_v1corr = phiR;
  if( phiR > M_PI ) phiR_v1corr = phiR_v1corr - 2*M_PI;
  // convert phiR (0, 2*pi) to (-pi/2,pi/2) range
  double phiR_v2corr = phiR;
  if (phiR >  M_PI/2) phiR_v2corr -= M_PI;
  if (phiR >  3*M_PI/2) phiR_v2corr -= 2*M_PI;
  m_phiv1reco_vs_phiR->Fill(phiR_v1corr, phiv1_reco);
  m_phiv2reco_vs_phiR->Fill(phiR_v2corr, phiv2_reco);

  // End of execution for each event
  return StatusCode::SUCCESS;
}

StatusCode CheckFlow::finalize() {
  msg(MSG::INFO) << ">>> CheckFlow from finalize" << endreq;

  return StatusCode::SUCCESS;
}

