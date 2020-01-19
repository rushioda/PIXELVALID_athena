/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Reader_h
#define Reader_h

#include <vector>

#include <TROOT.h>
#include <TChain.h>
#include <TRandom3.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH2.h>
#include <TStyle.h>

#include "MuonCalibrationFit/ProgressBar.h"
#include "MuonCalibrationFit/GRLReader.h"
#include "MuonCalibrationFit/Tools.h"

class Reader : public TSelector {
public :
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  //::: Declaration of leaf types
  UInt_t          RunNumber;
  ULong64_t       EvtNumber;
  UInt_t          LumiBlock;
  UInt_t          ChannelNumber;
  Float_t         EventWeight;
  Int_t           Trig_HLT_mu20_iloose_L1MU15;
  Int_t           Trig_HLT_mu24_iloose_L1MU15;
  Int_t           Trig_HLT_mu24_imedium;
  Int_t           Trig_HLT_mu26_imedium;
  Int_t           Trig_HLT_mu50;
  Int_t           Trig_HLT_mu22_mu8noL1;
  Int_t           Trig_HLT_mu24_mu8noL1;
  Int_t           Trig_HLT_2mu14;
  Int_t           Trig_HLT_2mu4;
  Int_t           Trig_HLT_2mu4_bJpsimumu;
  Int_t           Trig_HLT_2mu4_bUpsimumu;
  Float_t         Pos_Pt;
  Float_t         Pos_Eta;
  Float_t         Pos_Phi;
  Float_t         Pos_Charge;
  Float_t         Pos_ID_Pt;
  Float_t         Pos_ME_Pt;
  Float_t         Pos_ME_Eta;
  Float_t         Pos_ME_Phi;
  Float_t         Pos_d0sig;
  Float_t         Pos_z0;
  Float_t         Pos_PtCone20;
  Float_t         Pos_PtCone30;
  Float_t         Pos_PtCone40;
  Float_t         Pos_PtVarCone20;
  Float_t         Pos_PtVarCone30;
  Float_t         Pos_PtVarCone40;
  Int_t           Pos_Quality;
  Int_t           Pos_Author;
  Int_t           Pos_EnergyLossType;
  Int_t           Pos_PrimarySector;
  Int_t           Pos_SecondarySector;
  Float_t         Neg_Pt;
  Float_t         Neg_Eta;
  Float_t         Neg_Phi;
  Float_t         Neg_Charge;
  Float_t         Neg_ID_Pt;
  Float_t         Neg_ME_Pt;
  Float_t         Neg_ME_Eta;
  Float_t         Neg_ME_Phi;
  Float_t         Neg_d0sig;
  Float_t         Neg_z0;
  Float_t         Neg_PtCone20;
  Float_t         Neg_PtCone30;
  Float_t         Neg_PtCone40;
  Float_t         Neg_PtVarCone20;
  Float_t         Neg_PtVarCone30;
  Float_t         Neg_PtVarCone40;
  Int_t           Neg_Quality;
  Int_t           Neg_Author;
  Int_t           Neg_EnergyLossType;
  Int_t           Neg_PrimarySector;
  Int_t           Neg_SecondarySector;

  //::: List of branches
  TBranch        *b_RunNumber;   //!
  TBranch        *b_EvtNumber;   //!
  TBranch        *b_LumiBlock;   //!
  TBranch        *b_ChannelNumber;   //!
  TBranch        *b_EventWeight;   //!
  TBranch        *b_Trig_HLT_mu20_iloose_L1MU15;   //!
  TBranch        *b_Trig_HLT_mu24_iloose_L1MU15;   //!
  TBranch        *b_Trig_HLT_mu24_imedium;   //!
  TBranch        *b_Trig_HLT_mu26_imedium;   //!
  TBranch        *b_Trig_HLT_mu50;   //!
  TBranch        *b_Trig_HLT_mu22_mu8noL1;   //!
  TBranch        *b_Trig_HLT_mu24_mu8noL1;   //!
  TBranch        *b_Trig_HLT_2mu14;   //!
  TBranch        *b_Trig_HLT_2mu4;   //!
  TBranch        *b_Trig_HLT_2mu4_bJpsimumu;   //!
  TBranch        *b_Trig_HLT_2mu4_bUpsimumu;   //!
  TBranch        *b_Pos_Pt;   //!
  TBranch        *b_Pos_Eta;   //!
  TBranch        *b_Pos_Phi;   //!
  TBranch        *b_Pos_Charge;   //!
  TBranch        *b_Pos_ID_Pt;   //!
  TBranch        *b_Pos_ME_Pt;   //!
  TBranch        *b_Pos_ME_Eta;   //!
  TBranch        *b_Pos_ME_Phi;   //!
  TBranch        *b_Pos_d0sig;   //!
  TBranch        *b_Pos_z0;   //!
  TBranch        *b_Pos_PtCone20;   //!
  TBranch        *b_Pos_PtCone30;   //!
  TBranch        *b_Pos_PtCone40;   //!
  TBranch        *b_Pos_PtVarCone20;   //!
  TBranch        *b_Pos_PtVarCone30;   //!
  TBranch        *b_Pos_PtVarCone40;   //!
  TBranch        *b_Pos_Quality;   //!
  TBranch        *b_Pos_Author;   //!
  TBranch        *b_Pos_EnergyLossType;   //!
  TBranch        *b_Pos_PrimarySector;   //!
  TBranch        *b_Pos_SecondarySector;   //!
  TBranch        *b_Neg_Pt;   //!
  TBranch        *b_Neg_Eta;   //!
  TBranch        *b_Neg_Phi;   //!
  TBranch        *b_Neg_Charge;   //!
  TBranch        *b_Neg_ID_Pt;   //!
  TBranch        *b_Neg_ME_Pt;   //!
  TBranch        *b_Neg_ME_Eta;   //!
  TBranch        *b_Neg_ME_Phi;   //!
  TBranch        *b_Neg_d0sig;   //!
  TBranch        *b_Neg_z0;   //!
  TBranch        *b_Neg_PtCone20;   //!
  TBranch        *b_Neg_PtCone30;   //!
  TBranch        *b_Neg_PtCone40;   //!
  TBranch        *b_Neg_PtVarCone20;   //!
  TBranch        *b_Neg_PtVarCone30;   //!
  TBranch        *b_Neg_PtVarCone40;   //!
  TBranch        *b_Neg_Quality;   //!
  TBranch        *b_Neg_Author;   //!
  TBranch        *b_Neg_EnergyLossType;   //!
  TBranch        *b_Neg_PrimarySector;   //!
  TBranch        *b_Neg_SecondarySector;   //!
  //:::
  int          m_type;
  TString      m_GRL_Name;
  Long_t       m_N_events;
  Bool_t       m_detector;
  Bool_t       m_usePB;
  Float_t      m_MinMuonsPtForJpsi;
  Float_t      m_MaxMuonsPtForJpsi;
  Float_t      m_MinLeadingMuonPtForZ;
  Float_t      m_MinSubLeadingMuonPtForZ;
  std::vector< Tools::RegionInfo* > m_AllRegInfo;
  std::vector< Tools::RegionInfo* > m_IDRegInfo;
  Tools::RegionInfo* m_RegInfo;
  Tools::RegionInfo* m_RegInfoForPos;
  Tools::RegionInfo* m_RegInfoForNeg;
  Tools::RegionInfo* m_IDRegInfoForPos;
  Tools::RegionInfo* m_IDRegInfoForNeg;
  std::vector< Tools::Info >* m_SingleInfo; 
  std::vector< Tools::Info >* m_BothInfo; 
  Long_t       m_entry;
  GRLReader*   m_GRL;
  ProgressBar* m_prog;
  TRandom3*    m_random;
  TString      m_JpsiRewFileName;
  TString      m_ZRewFileName;
  Bool_t       m_PreviousCorrectionsForAll;
  TFile*       m_JpsiRewFile;
  //TH2F*        m_JpsiRewHist;
  //TH2F*        m_JpsiRewHistDummy;
  TH1F*        m_JpsiRewHist;
  TH1F*        m_JpsiRewHistDummy;
  TFile*       m_ZRewFile;
  //TH2F*        m_ZRewHist;
  //TH2F*        m_ZRewHistDummy;
  TH1F*        m_ZRewHist;
  TH1F*        m_ZRewHistDummy;
  Bool_t       m_Print;
  //:::
  std::map< int, int > m_Jpsi_Counter;
  std::map< int, int > m_Z_Counter;
  //:::
  Reader( TTree * /*tree*/ = 0 ) : fChain( 0 ) { m_SingleInfo = NULL; m_BothInfo = NULL; }
  Reader( int type, TString grl_name, Long_t n, Bool_t detector, Bool_t pb, Float_t min_pt_cut_jpsi, Float_t max_pt_cut_jpsi, Float_t min_pt_cut_leadZ, Float_t min_pt_cut_subleadZ, std::vector< Tools::RegionInfo* > vec_reg, std::vector< Tools::RegionInfo* > vec_reg_ID, Tools::RegionInfo* reg, std::vector< Tools::Info >* vec_info_single, std::vector< Tools::Info >* vec_info_double, TString jpsi_rew_file_name, TString z_rew_file_name, Bool_t correct_all ) : fChain( 0 ), m_type( type ), m_GRL_Name( grl_name ), m_N_events( n ), m_detector( detector ), m_usePB( pb ), m_MinMuonsPtForJpsi( min_pt_cut_jpsi ), m_MaxMuonsPtForJpsi( max_pt_cut_jpsi ), m_MinLeadingMuonPtForZ( min_pt_cut_leadZ ), m_MinSubLeadingMuonPtForZ( min_pt_cut_subleadZ ), m_AllRegInfo( vec_reg ), m_IDRegInfo( vec_reg_ID ), m_RegInfo( reg ), m_SingleInfo( vec_info_single ), m_BothInfo( vec_info_double ), m_JpsiRewFileName( jpsi_rew_file_name ), m_ZRewFileName( z_rew_file_name ), m_PreviousCorrectionsForAll( correct_all ) { }
  virtual ~Reader() { }
  virtual Int_t   Version() const { return 2; }
  virtual void    Begin( TTree *tree );
  virtual void    SlaveBegin( TTree *tree );
  virtual void    Init( TTree *tree );
  virtual Bool_t  Notify();
  virtual Bool_t  Process( Long64_t entry );
  virtual Int_t   GetEntry( Long64_t entry, Int_t getall = 0 ) { return fChain ? fChain->GetTree()->GetEntry( entry, getall ) : 0; }
  virtual void    SetOption( const char *option ) { fOption = option; }
  virtual void    SetObject( TObject *obj ) { fObject = obj; }
  virtual void    SetInputList( TList *input ) { fInput = input; }
  virtual TList  *GetOutputList() const { return fOutput; }
  virtual void    SlaveTerminate();
  virtual void    Terminate();
  //:::
  void BeginFirstEvent();
  void SetType( int val ) { m_type = val; }
  void SetAllRegionInfo( std::vector< Tools::RegionInfo* > r ) { m_AllRegInfo = r; }
  void SetIDRegionInfo( std::vector< Tools::RegionInfo* > r ) { m_IDRegInfo = r; }
  void SetRegionInfo( Tools::RegionInfo* r ) { m_RegInfo = r; }
  void SetSingleInfo( std::vector< Tools::Info >* v ) { m_SingleInfo = v; }
  void SetBothInfo( std::vector< Tools::Info >* v ) { m_BothInfo = v; }
  void SetNEvents( Long_t n ) { m_N_events = n; }
  void UseProgressBar( Bool_t val ) { m_usePB = val; }
  //:::
  std::vector< Tools::RegionInfo* > GetAllRegionInfo() { return m_AllRegInfo; }
  //:::
  Float_t Correct( Float_t pt, Float_t r0, Float_t add, Tools::RegionInfo* rI );
  //:::
  //Float_t GetWeight( Float_t pair_mass, Float_t pair_pt, Float_t pair_y );
  Float_t GetWeight( Float_t pair_mass, Float_t eta1, Float_t eta2 );
  //:::
  void Add( std::vector< Tools::Info >* info_vec, Tools::Info info );
  //:::
  void FillCounter( int Key, std::map< int, int >& Map );
  void PrintCounter( TString name, std::map< int, int > Map );
  //ClassDef( Reader, 0 );
};
#endif

#ifdef Reader_cxx
void Reader::Init( TTree *tree ) {
  if ( !tree ) return;
  fChain = tree;
  fChain->SetMakeClass( 1 );
  fChain->SetBranchAddress( "RunNumber", &RunNumber, &b_RunNumber );
  fChain->SetBranchAddress( "EvtNumber", &EvtNumber, &b_EvtNumber );
  fChain->SetBranchAddress( "LumiBlock", &LumiBlock, &b_LumiBlock );
  fChain->SetBranchAddress( "ChannelNumber", &ChannelNumber, &b_ChannelNumber );
  fChain->SetBranchAddress( "EventWeight", &EventWeight, &b_EventWeight );
  fChain->SetBranchAddress( "Trig_HLT_mu20_iloose_L1MU15", &Trig_HLT_mu20_iloose_L1MU15, &b_Trig_HLT_mu20_iloose_L1MU15 );
  fChain->SetBranchAddress( "Trig_HLT_mu24_iloose_L1MU15", &Trig_HLT_mu24_iloose_L1MU15, &b_Trig_HLT_mu24_iloose_L1MU15 );
  fChain->SetBranchAddress( "Trig_HLT_mu24_imedium", &Trig_HLT_mu24_imedium, &b_Trig_HLT_mu24_imedium );
  fChain->SetBranchAddress( "Trig_HLT_mu26_imedium", &Trig_HLT_mu26_imedium, &b_Trig_HLT_mu26_imedium );
  fChain->SetBranchAddress( "Trig_HLT_mu50", &Trig_HLT_mu50, &b_Trig_HLT_mu50 );
  fChain->SetBranchAddress( "Trig_HLT_mu22_mu8noL1", &Trig_HLT_mu22_mu8noL1, &b_Trig_HLT_mu22_mu8noL1 );
  fChain->SetBranchAddress( "Trig_HLT_mu24_mu8noL1", &Trig_HLT_mu24_mu8noL1, &b_Trig_HLT_mu24_mu8noL1 );
  fChain->SetBranchAddress( "Trig_HLT_2mu14", &Trig_HLT_2mu14, &b_Trig_HLT_2mu14 );
  fChain->SetBranchAddress( "Trig_HLT_2mu4", &Trig_HLT_2mu4, &b_Trig_HLT_2mu4 );
  fChain->SetBranchAddress( "Trig_HLT_2mu4_bJpsimumu", &Trig_HLT_2mu4_bJpsimumu, &b_Trig_HLT_2mu4_bJpsimumu );
  fChain->SetBranchAddress( "Trig_HLT_2mu4_bUpsimumu", &Trig_HLT_2mu4_bUpsimumu, &b_Trig_HLT_2mu4_bUpsimumu );
  fChain->SetBranchAddress( "Pos_Pt", &Pos_Pt, &b_Pos_Pt );
  fChain->SetBranchAddress( "Pos_Eta", &Pos_Eta, &b_Pos_Eta );
  fChain->SetBranchAddress( "Pos_Phi", &Pos_Phi, &b_Pos_Phi );
  fChain->SetBranchAddress( "Pos_Charge", &Pos_Charge, &b_Pos_Charge );
  fChain->SetBranchAddress( "Pos_ID_Pt", &Pos_ID_Pt, &b_Pos_ID_Pt );
  fChain->SetBranchAddress( "Pos_ME_Pt", &Pos_ME_Pt, &b_Pos_ME_Pt );
  fChain->SetBranchAddress( "Pos_ME_Eta", &Pos_ME_Eta, &b_Pos_ME_Eta );
  fChain->SetBranchAddress( "Pos_ME_Phi", &Pos_ME_Phi, &b_Pos_ME_Phi );
  fChain->SetBranchAddress( "Pos_d0sig", &Pos_d0sig, &b_Pos_d0sig );
  fChain->SetBranchAddress( "Pos_z0", &Pos_z0, &b_Pos_z0 );
  fChain->SetBranchAddress( "Pos_PtCone20", &Pos_PtCone20, &b_Pos_PtCone20 );
  fChain->SetBranchAddress( "Pos_PtCone30", &Pos_PtCone30, &b_Pos_PtCone30 );
  fChain->SetBranchAddress( "Pos_PtCone40", &Pos_PtCone40, &b_Pos_PtCone40 );
  fChain->SetBranchAddress( "Pos_PtVarCone20", &Pos_PtVarCone20, &b_Pos_PtVarCone20 );
  fChain->SetBranchAddress( "Pos_PtVarCone30", &Pos_PtVarCone30, &b_Pos_PtVarCone30 );
  fChain->SetBranchAddress( "Pos_PtVarCone40", &Pos_PtVarCone40, &b_Pos_PtVarCone40 );
  fChain->SetBranchAddress( "Pos_Quality", &Pos_Quality, &b_Pos_Quality );
  fChain->SetBranchAddress( "Pos_Author", &Pos_Author, &b_Pos_Author );
  fChain->SetBranchAddress( "Pos_EnergyLossType", &Pos_EnergyLossType, &b_Pos_EnergyLossType );
  fChain->SetBranchAddress( "Pos_PrimarySector", &Pos_PrimarySector, &b_Pos_PrimarySector );
  fChain->SetBranchAddress( "Pos_SecondarySector", &Pos_SecondarySector, &b_Pos_SecondarySector );
  fChain->SetBranchAddress( "Neg_Pt", &Neg_Pt, &b_Neg_Pt );
  fChain->SetBranchAddress( "Neg_Eta", &Neg_Eta, &b_Neg_Eta );
  fChain->SetBranchAddress( "Neg_Phi", &Neg_Phi, &b_Neg_Phi );
  fChain->SetBranchAddress( "Neg_Charge", &Neg_Charge, &b_Neg_Charge );
  fChain->SetBranchAddress( "Neg_ID_Pt", &Neg_ID_Pt, &b_Neg_ID_Pt );
  fChain->SetBranchAddress( "Neg_ME_Pt", &Neg_ME_Pt, &b_Neg_ME_Pt );
  fChain->SetBranchAddress( "Neg_ME_Eta", &Neg_ME_Eta, &b_Neg_ME_Eta );
  fChain->SetBranchAddress( "Neg_ME_Phi", &Neg_ME_Phi, &b_Neg_ME_Phi );
  fChain->SetBranchAddress( "Neg_d0sig", &Neg_d0sig, &b_Neg_d0sig );
  fChain->SetBranchAddress( "Neg_z0", &Neg_z0, &b_Neg_z0 );
  fChain->SetBranchAddress( "Neg_PtCone20", &Neg_PtCone20, &b_Neg_PtCone20 );
  fChain->SetBranchAddress( "Neg_PtCone30", &Neg_PtCone30, &b_Neg_PtCone30 );
  fChain->SetBranchAddress( "Neg_PtCone40", &Neg_PtCone40, &b_Neg_PtCone40 );
  fChain->SetBranchAddress( "Neg_PtVarCone20", &Neg_PtVarCone20, &b_Neg_PtVarCone20 );
  fChain->SetBranchAddress( "Neg_PtVarCone30", &Neg_PtVarCone30, &b_Neg_PtVarCone30 );
  fChain->SetBranchAddress( "Neg_PtVarCone40", &Neg_PtVarCone40, &b_Neg_PtVarCone40 );
  fChain->SetBranchAddress( "Neg_Quality", &Neg_Quality, &b_Neg_Quality );
  fChain->SetBranchAddress( "Neg_Author", &Neg_Author, &b_Neg_Author);
  fChain->SetBranchAddress( "Neg_EnergyLossType", &Neg_EnergyLossType, &b_Neg_EnergyLossType );
  fChain->SetBranchAddress( "Neg_PrimarySector", &Neg_PrimarySector, &b_Neg_PrimarySector );
  fChain->SetBranchAddress( "Neg_SecondarySector", &Neg_SecondarySector, &b_Neg_SecondarySector );
}

Bool_t Reader::Notify( ) {
  return kTRUE;
}

#endif // #ifdef Reader_cxx
