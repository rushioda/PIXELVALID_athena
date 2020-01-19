/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMPARAMETRIZATION_CALOGEOMETRY_H
#define ISF_FASTCALOSIMPARAMETRIZATION_CALOGEOMETRY_H

#include <TMath.h>

#include <vector>
#include <map>
#include <iostream>

#include "ISF_FastCaloSimEvent/ICaloGeometry.h"
#include "ISF_FastCaloSimParametrization/CaloGeometryLookup.h"
#include "ISF_FastCaloSimParametrization/MeanAndRMS.h"
#include "ISF_FastCaloSimParametrization/FSmap.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"

class CaloDetDescrElement;
class TCanvas;
class TGraph;
class TGraphErrors;

class CaloGeometry : virtual public ICaloGeometry {
  public :
    static const int MAX_SAMPLING;

    static Identifier m_debug_identify;
    static bool m_debug;

    CaloGeometry();
    virtual ~CaloGeometry();

    virtual bool PostProcessGeometry();

    virtual void Validate(int nrnd=100);

    virtual const CaloDetDescrElement* getDDE(Identifier identify);
    virtual const CaloDetDescrElement* getDDE(int sampling, Identifier identify);

    virtual const CaloDetDescrElement* getDDE(int sampling,float eta,float phi,float* distance=0,int* steps=0);
    virtual const CaloDetDescrElement* getFCalDDE(int sampling,float x,float y,float z,float* distance=0,int* steps=0);
    bool getClosestFCalCellIndex(int sampling,float x,float y,int& ieta, int& iphi,int* steps=0);

    double deta(int sample,double eta) const;
    void   minmaxeta(int sample,double eta,double& mineta,double& maxeta) const;
    double rzmid(int sample,double eta) const;
    double rzent(int sample,double eta) const;
    double rzext(int sample,double eta) const;
    double rmid(int sample,double eta) const;
    double rent(int sample,double eta) const;
    double rext(int sample,double eta) const;
    double zmid(int sample,double eta) const;
    double zent(int sample,double eta) const;
    double zext(int sample,double eta) const;
    double rpos(int sample,double eta,int subpos = CaloSubPos::SUBPOS_MID) const;
    double zpos(int sample,double eta,int subpos = CaloSubPos::SUBPOS_MID) const;
    double rzpos(int sample,double eta,int subpos = CaloSubPos::SUBPOS_MID) const;
    bool   isCaloBarrel(int sample) const {return m_isCaloBarrel[sample];};
    static std::string SamplingName(int sample);

    TGraphErrors* GetGraph(unsigned int sample) const {return m_graph_layers[sample];};
    void SetDoGraphs(bool dographs=true) {m_dographs=dographs;};
    bool DoGraphs() const {return m_dographs;};

    TGraph*  DrawGeoSampleForPhi0(int sample, int calocol, bool print=false);
    TCanvas* DrawGeoForPhi0();
    FCAL_ChannelMap* GetFCAL_ChannelMap(){return &m_FCal_ChannelMap;}
    virtual void LoadFCalGeometryFromFiles(TString filename1,TString filename2,TString filename3); // Initialize m_FCal_ChannelMap
    virtual void PrintMapInfo(int i, int j);

  protected:
    virtual void addcell(const CaloDetDescrElement* cell);

    virtual void post_process(int layer);

    

    virtual void InitRZmaps();

    t_cellmap m_cells;
    std::vector< t_cellmap > m_cells_in_sampling;
    std::vector< t_eta_cellmap > m_cells_in_sampling_for_phi0;
    std::vector< std::vector< CaloGeometryLookup* > > m_cells_in_regions;

    std::vector< bool > m_isCaloBarrel;
    std::vector< double > m_min_eta_sample[2]; //[side][calosample]
    std::vector< double > m_max_eta_sample[2]; //[side][calosample]
    std::vector< FSmap< double , double > > m_rmid_map[2]; //[side][calosample]
    std::vector< FSmap< double , double > > m_zmid_map[2]; //[side][calosample]
    std::vector< FSmap< double , double > > m_rent_map[2]; //[side][calosample]
    std::vector< FSmap< double , double > > m_zent_map[2]; //[side][calosample]
    std::vector< FSmap< double , double > > m_rext_map[2]; //[side][calosample]
    std::vector< FSmap< double , double > > m_zext_map[2]; //[side][calosample]

    bool m_dographs;
    std::vector< TGraphErrors* > m_graph_layers;
    FCAL_ChannelMap m_FCal_ChannelMap; // for hit-to-cell assignment in FCal
    std::vector<double> m_FCal_rmin,m_FCal_rmax;
    
    
    /*
       double  m_min_eta_sample[2][MAX_SAMPLING]; //[side][calosample]
       double  m_max_eta_sample[2][MAX_SAMPLING]; //[side][calosample]
       FSmap< double , double > m_rmid_map[2][MAX_SAMPLING]; //[side][calosample]
       FSmap< double , double > m_zmid_map[2][MAX_SAMPLING]; //[side][calosample]
       FSmap< double , double > m_rent_map[2][MAX_SAMPLING]; //[side][calosample]
       FSmap< double , double > m_zent_map[2][MAX_SAMPLING]; //[side][calosample]
       FSmap< double , double > m_rext_map[2][MAX_SAMPLING]; //[side][calosample]
       FSmap< double , double > m_zext_map[2][MAX_SAMPLING]; //[side][calosample]
       */
};

#endif
