/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSEnergyBinParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "TMath.h"
#include "TRandom.h"

//=============================================
//======= TFCSSelectEnergyBin =========
//=============================================

TFCSEnergyBinParametrization::TFCSEnergyBinParametrization(const char* name, const char* title):TFCSEnergyParametrization(name,title)
{
  m_number_of_Ekin_bins=1;
}

bool TFCSEnergyBinParametrization::is_match_Ekin_bin(int Ekin_bin) const 
{
  if(Ekin_bin>=1 && Ekin_bin<=n_bins()) return true;
  return false;
}

void TFCSEnergyBinParametrization::resize()
{
  for (std::set<int>::iterator itid=pdgid().begin(); itid!=pdgid().end(); ++itid) {
	  ATH_MSG_VERBOSE("PDGid="<<*itid<<" resize to "<<n_bins()+1);
    m_pdgid_Ebin_probability[*itid].resize(n_bins()+1);
  }
  for (auto it=m_pdgid_Ebin_probability.begin(); it!=m_pdgid_Ebin_probability.end(); ++it) {
    if(!is_match_pdgid(it->first)) {
      m_pdgid_Ebin_probability.erase(it);
      it=m_pdgid_Ebin_probability.begin();
    }
  }    
}

void TFCSEnergyBinParametrization::set_pdgid(int id)
{
  TFCSEnergyParametrization::set_pdgid(id);
  resize();
}

void TFCSEnergyBinParametrization::set_pdgid(const std::set< int > &ids)
{
  TFCSEnergyParametrization::set_pdgid(ids);
  resize();
}

void TFCSEnergyBinParametrization::add_pdgid(int id)
{
  TFCSEnergyParametrization::add_pdgid(id);
  resize();
}

void TFCSEnergyBinParametrization::clear_pdgid()
{
  TFCSEnergyParametrization::clear_pdgid();
  resize();
}

void TFCSEnergyBinParametrization::set_pdgid_Ekin_bin_probability(int id,std::vector< float > prob)
{
  add_pdgid(id);
  if(prob.size()!=m_pdgid_Ebin_probability[id].size()) {
    ATH_MSG_ERROR("TFCSEnergyBinParametrization::set_pdgid_Ekin_bin_probability(): size of vectors does not match! in.size()="<<prob.size()<<" instance="<<m_pdgid_Ebin_probability[id].size());
    return;
  }
  float ptot=0;
  for(int iEbin=0;iEbin<=n_bins();++iEbin) ptot+=prob[iEbin];
  float p=0;
  for(int iEbin=0;iEbin<=n_bins();++iEbin) {
    p+=prob[iEbin]/ptot;
    m_pdgid_Ebin_probability[id][iEbin]=p;
  }
}
  
void TFCSEnergyBinParametrization::Print(Option_t *option) const
{
  TString opt(option);
  bool shortprint=opt.Index("short")>=0;
  bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  TString optprint=opt;optprint.ReplaceAll("short","");
  TFCSEnergyParametrization::Print(option);
  if(longprint) {
    for (std::set<int>::iterator it=pdgid().begin(); it!=pdgid().end(); ++it) {
      ATH_MSG(INFO) <<optprint<< "  PDGID="<< *it<<" : ";
      float p=0;
      for(int iEbin=0;iEbin<=n_bins();++iEbin) {
        if(iEbin>0) std::cout <<", ";
        auto mapit=m_pdgid_Ebin_probability.find(*it);
        msg() <<"b"<<iEbin<<"="<<(mapit->second[iEbin]-p)/mapit->second.back()*100<<"%";
        p=mapit->second[iEbin];
      }
      msg() <<endmsg;
    }  
  }  
}

FCSReturnCode TFCSEnergyBinParametrization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* /*extrapol*/)
{
  int pdgid=truth->pdgid();
  if(!is_match_pdgid(pdgid)) {
    ATH_MSG_ERROR("TFCSEnergyBinParametrization::simulate(): cannot simulate pdgid="<<pdgid);
    return FCSFatal;
  }
  float searchRand=gRandom->Rndm();
  int chosenBin=TMath::BinarySearch(n_bins()+1, m_pdgid_Ebin_probability[pdgid].data(), searchRand)+1;
  if(chosenBin<1 || chosenBin>n_bins()) {
    ATH_MSG_ERROR("TFCSEnergyBinParametrization::simulate(): cannot simulate bin="<<chosenBin);
    ATH_MSG_ERROR("  This error could probably be retried.");
    if(msgLvl(MSG::ERROR)) {
      ATH_MSG(ERROR)<<"in "<<GetName()<<": E="<<simulstate.E()<<" Ebin="<<chosenBin<<" rnd="<<searchRand<<" array=";
      for(int iEbin=0;iEbin<=n_bins();++iEbin) msg()<<m_pdgid_Ebin_probability[pdgid][iEbin]<<" ";
      msg()<<std::endl;
    }  
    return FCSFatal;
  }
  simulstate.set_Ebin(chosenBin);
  ATH_MSG_DEBUG("Ebin="<<chosenBin);

  return FCSSuccess;
}
