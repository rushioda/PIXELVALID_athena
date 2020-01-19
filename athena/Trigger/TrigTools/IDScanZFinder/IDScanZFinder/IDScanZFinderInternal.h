// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: IDScanZFinder.h
// 
// author: Nikos Konstantinidis <n.konstantinidis@ucl.ac.uk>
//         
//		 
// Description: NON ATHENA Internals for the ZFinder primary z vertex finding AlgTool 
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#ifndef __IDSCANZFINDERINTERNAL_H__
#define __IDSCANZFINDERINTERNAL_H__

#include <cmath>
#include <vector>
#include <string>

#include "IDScanZFinder/ZFinderConstants.h"
#include "IDScanZFinder/PhiSlice.h"
#include "IRegionSelector/IRoiDescriptor.h"

static const std::string mZFIVER("$Id: IDScanZFinderInternal.h 700483 2015-10-14 09:37:16Z sutt $");




namespace Run1 {

template<class SpacePoint> class IDScanZFinderInternal
{
public: 
  
  struct vertex { 
    vertex( double z, double weight ) : _z(z), _weight(weight) { } 
    double _z;
    double _weight;
  };
  
public:
   
  IDScanZFinderInternal( const std::string&, const std::string& );
  virtual ~IDScanZFinderInternal(){};
  //  void initializeInternal(long maxLayers=20, long lastBarrel=7); 
  void initializeInternal(long maxLayers, long lastBarrel); 
  
  std::vector<vertex>* findZInternal( const std::vector<const SpacePoint *>& spVec, const IRoiDescriptor& roi);
  
  const std::vector< std::vector<long> >*   GetnHisto() { return m_nHisto; }
  const std::vector< std::vector<double> >* GetzHisto() { return m_zHisto; }
  
  long  GetNMax() { return m_NMax; } 
  
  //  void setLayers(long maxLayers=20, long lastBarrelLayer=7) {
  void setLayers(long maxLayers, long lastBarrelLayer) {
    m_IdScan_MaxNumLayers = maxLayers;        // dphiEC depends on this value
    m_IdScan_LastBrlLayer = lastBarrelLayer;  // dphiBrl depends on this value
  } 
  
protected:  // member functions
  
  // fills phi, rho, z, layer of spacepoints to simple vectors
  
  long fillVectors( const std::vector<const SpacePoint *>& spVec, const IRoiDescriptor& roi,
  		    std::vector<double>& phi, std::vector<double>& rho, std::vector<double>& zed, 
  		    std::vector<long>&   lyr, std::vector<long>&   filledLayers);
  
  std::string getType() const { return mType; }
  std::string getName() const { return mName; }
  std::string getVersion() const { return mZFIVER; }
  
  int GetInternalStatus() const { return m_Status; }
  int SetInternalStatus(int s)  { m_Status = s; return m_Status; }
  
  double computeZV(double r1, double z1, double r2, double z2) const;
  double computeZV(double r1, double p1, double z1, double r2, double p2, double z2) const;
  
  void   SetReturnValue(double d) { mreturnval=d; }
  double GetReturnValue() const   { return mreturnval; }
  
  
protected:  // data members
  
  /// maximum number of layers and last barrel layer 
  long   m_IdScan_MaxNumLayers; 
  long   m_IdScan_LastBrlLayer; 
  
  
  // To be read in from jobOptions by IdScanMain
  
  double m_invPhiSliceSize;     // the inverse size of the phi slices 
  long   m_NumPhiSlices;        // the number of phi slices, given the width of the RoI 
  
  double m_phiBinSize;          // the size of the phi slices 
  double m_usedphiBinSize;      // the size of the phi slices 
  double m_ROIphiWidth;         // the phi width of the ROI 
  double m_usedROIphiWidth;     // the phi width of the ROI 
  double m_minZBinSize;         // z-histo bin size: m_minZBinSize+|etaRoI|*m_zBinSizeEtaCoeff (to account for worse resolution in high eta)
  double m_zBinSizeEtaCoeff;    // z-histo bin size: m_minZBinSize+|etaRoI|*m_zBinSizeEtaCoeff (to account for worse resolution in high eta)
  
  long   m_numberOfPeaks;       // how many z-positions to return in findZ
  
  bool   m_pixOnly;             // use only Pixel space points
  
  std::string mType;  // type information for internal book keeping
  std::string mName;  // name information for the same
  
  int    m_Status;   // return status of the algorithm: 0=ok, -1=error
    
  bool m_chargeAware ;  // maintain separate sets of z histos for + & - tracks
  bool m_zHistoPerPhi;  // maintain one set of z histos per each phi slice
  
  double m_dphideta; // how, as a function of eta, the number of phi neighbours decreases
  double m_neighborMultiplier; // extra factor to manually increase the number of phi neighbors
  //  long extraPhi[IdScan_MaxNumLayers][IdScan_MaxNumLayers]; // number of phi neighbours to look at
  std::vector< std::vector<long> > extraPhi; // ( IdScan_MaxNumLayers, std::vector<long>(IdScan_MaxNumLayers) ); // number of phi neighbours to look at
  
  // access the ZFinder histogram from outside the findZInternal method
  
  std::vector < std::vector<long>   >  m_nHisto[2]; // the actual z histogram count of pairs
  std::vector < std::vector<double> >  m_zHisto[2]; // summed z position histograms
  
  long m_NMax; // maximum number of z histogram entries
  
  int m_nFirstLayers;    // When the pairs of SPs are made, the inner SP comes from up to this "filled layer"
  
  double m_vrtxDistCut;  // The minimum fractional distance between two output vertices
  double m_vrtxMixing ;  // If two vertices are found to be too close, "mix" the second into first
  int m_nvrtxSeparation; // The minimum number of zbins that any two output vertices should by separated by
  bool m_preferCentralZ; // Among peaks of same height, should precedence go to the one with smaller |z|
  
  bool m_trustSPprovider; // Should we re-extract the RoI phi range from the phis of the SPs from the SPP
  
  double mreturnval; // return value for algorithm
  
  bool   m_fullScanMode;
  
  int m_tripletMode;
  double m_tripletDZ;
  double m_tripletDK;
  double m_tripletDP;
  
  /// to apply a hreshold to the found vertex candidates
  
  double  m_weightThreshold;
  
};
  
  
  
// template<typename T> long IDScanZFinderInternal<T>::IdScan_MaxNumLayers = 20; // dphiEC depends on this value
// template<typename T> long IDScanZFinderInternal<T>::IdScan_LastBrlLayer = 7;
  
  
////////////////////////////////////////////////////////////////////////////////
////    IDScanZFinderInternal template implementation
////////////////////////////////////////////////////////////////////////////////
  
  
template<class SpacePoint> 
IDScanZFinderInternal<SpacePoint>::IDScanZFinderInternal( const std::string& type, 
  							  const std::string& name)  
{
  mType = type;
  mName = name;
  
  m_phiBinSize       = 0.2   ;
  m_usedphiBinSize   = m_phiBinSize   ;
  m_pixOnly          = false ;
  m_ROIphiWidth      = 0.2   ;
  m_usedROIphiWidth  = m_ROIphiWidth ;
  m_minZBinSize      = 0.2   ;
  m_zBinSizeEtaCoeff = 0.1   ;
  m_dphideta         = -0.02 ;
  m_neighborMultiplier = 1.  ;
  m_numberOfPeaks    = 1     ;
  m_chargeAware      = true  ;
  m_zHistoPerPhi     = true  ;
  m_nFirstLayers     = 3     ;
  m_vrtxDistCut      = 0.    ;
  m_vrtxMixing       = 0.    ;
  m_nvrtxSeparation  = 0     ;
  m_preferCentralZ   = false ;
  m_trustSPprovider  = true  ;
  m_fullScanMode     = false ;
  m_tripletMode      = 0     ;
  m_tripletDZ        = 25.   ;
  m_tripletDK        = 0.005 ;  
  m_tripletDP        = 0.05  ;
  
  //  m_applyWeightThreshold = false;
  m_weightThreshold      = 0;
  
  
  /// to allow valiable size layers
  m_IdScan_MaxNumLayers     = 20; // dphiEC depends on this value !!! 19 without IBL, 20 with IBL!!
  m_IdScan_LastBrlLayer     = 7;  // dphiBrl depends on this value
  
  //  std::cout << "m_nFirstLayers  " << m_nFirstLayers  << std::endl;
  
  /// why is this called from the constructor ???? it is called again during initialise?
  /// do not call that here
  ///  initializeInternal( m_IdScan_MaxNumLayers, m_IdScan_LastBrlLayer );
  
  m_Status = 0;
}
  
  
  
template<class SpacePoint>
void IDScanZFinderInternal<SpacePoint>::initializeInternal(long maxLayers, long lastBarrel )
{
  m_IdScan_MaxNumLayers = maxLayers; 
  m_IdScan_LastBrlLayer = lastBarrel; 
  
  //  std::cout << "m_IdScan_MaxNumLayers "    <<  m_IdScan_MaxNumLayers
  //	    << "\tm_IdScan_LastBrlLayer "  <<  m_IdScan_LastBrlLayer << std::endl;
  
  // number of phi neighbours to look at
  //  if ( extraPhi.size()==0 ) 
  extraPhi = std::vector< std::vector<long> >( m_IdScan_MaxNumLayers, std::vector<long>(m_IdScan_MaxNumLayers) ); 
  
  if ( m_fullScanMode ) m_usedROIphiWidth = 2*M_PI;
  else                  m_usedROIphiWidth = m_ROIphiWidth;
  
  // from IDScanZFinder::initialize
  m_usedphiBinSize = m_phiBinSize;
  if ( m_usedphiBinSize < ZFinder_MinPhiSliceSize ) m_usedphiBinSize = ZFinder_MinPhiSliceSize;
  if ( m_dphideta > 0 )                             m_dphideta *= -m_dphideta;
  
  m_invPhiSliceSize = 180./(M_PI*m_usedphiBinSize);
  /// this has to be computed event by event !!!
  ///  m_NumPhiSlices = long (ceil( m_usedROIphiWidth*m_invPhiSliceSize ));  
  
  for (long l1=0; l1<m_IdScan_MaxNumLayers-1; ++l1) {
    for (long l2=l1+1; l2<m_IdScan_MaxNumLayers; ++l2) {
      extraPhi[l1][l2]=1; // look at a single phi neighbour
    }
  }
  
  /// barrel
  
  long first_layer  = 0;
  long offset_layer = 1;
  if ( m_IdScan_MaxNumLayers<20 ) { 
    first_layer  = 1; 
    offset_layer = 0; 
  }
  
  long lyrcnt = 0;
  for (long l1=0; l1<m_IdScan_LastBrlLayer; ++l1) {
    for (long l2=l1+1; l2<=m_IdScan_LastBrlLayer; ++l2) {
      double dphi = ZF_dphiBrl[lyrcnt + 7*first_layer];
      dphi *= m_neighborMultiplier;
      extraPhi[l1][l2]=static_cast<long>( ceil(  sqrt(dphi*dphi+ZF_phiRes*ZF_phiRes*2) * m_invPhiSliceSize ) );
  					      
      //      std::cout << "test 1 " << l1 << " " << l2 << "\tmax : " <<  m_IdScan_MaxNumLayers << std::endl;
  						   
      if (extraPhi[l1][l2]<1) extraPhi[l1][l2]=1;
      //      std::cout << "Delta Phi between layers " << l1 << " and " << l2
      //		<< " = "<< ZF_dphiBrl[lyrcnt] 
      //		<< " rads ( " << extraPhi[l1][l2] << " bins including phi resln.)\n";
      lyrcnt++;
    }
  }
  
  
  /// standard Endcap
  
    
  
  for ( long lyrpair=12*first_layer ;  lyrpair<117; ++lyrpair ) {
  
    double dphi = ZF_dphiEC[lyrpair*4+2];
    /// increment all the layer ids by one because of the IBL
    /// IF and ONLY IF the IBL is included
    long     l1 = (long)ZF_dphiEC[lyrpair*4] + offset_layer; 
    long     l2 = (long)ZF_dphiEC[lyrpair*4+1] + offset_layer; 
    double  eta = ZF_dphiEC[lyrpair*4+3];
    //   std::cout << "Delta Phi between layers " << l1 << " and " << l2 
    //	      << " = " << dphi << " rads @ eta=" << eta
    //	      << ". Extrapolate it to eta=0.9 to get ";
    dphi = dphi + m_dphideta * ( 0.9 - eta );
    dphi *= m_neighborMultiplier;
    extraPhi[l1][l2]=static_cast<long>(ceil(sqrt(dphi*dphi+ZF_phiRes*ZF_phiRes*2)*m_invPhiSliceSize));
  						 
    if (extraPhi[l1][l2]<1) extraPhi[l1][l2]=1;
  
    //    std::cout << "test 2 " << l1 << " " << l2 << "\tmax : " <<  m_IdScan_MaxNumLayers << std::endl;
  
    // std::cout << dphi << " rads ( " << extraPhi[l1][l2] << " bins including phi resln.)\n";
  }
  
}
  
template<class SpacePoint>
double IDScanZFinderInternal<SpacePoint>::computeZV(double r1, double z1, double r2, double z2) const {
  return (z2*r1-z1*r2)/(r1-r2);
}
  
template<class SpacePoint>
double IDScanZFinderInternal<SpacePoint>::computeZV(double r1, double p1, double z1,
  						    double r2, double p2, double z2) const {
  double x1, y1, x2, y2;
  //sincos( p1, &y1, &x1 );  x1 *= r1;  y1 *= r1;
  //sincos( p2, &y2, &x2 );  x2 *= r2;  y2 *= r2;
  x1 = r1 * cos(p1);
  x2 = r2 * cos(p2);
  y1 = r1 * sin(p1);
  y2 = r2 * sin(p2);
  
#define _COMPUTEX0_
#ifdef _COMPUTEX0_
  double slope = (y2-y1)/(x2-x1);
  double invslope = 1./slope;
  double x0 = (slope*x1-y1)/(slope+invslope);
  //double y0 = -1*x0*invslope;
  double d0sqr = x0*x0*(1+invslope*invslope);
  // s1 and s2 are the track lengths from the point of closest approach
  double s1 = sqrt(r1*r1-d0sqr);
  double s2 = sqrt(r2*r2-d0sqr); // or s1*(x1-x0)/(x2-x0)
#else
  double inv_dels = 1./sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
  double dotrr = x1*x2 + y1*y2;
  double s1 = ( dotrr - r1*r1 ) * inv_dels;
  double s2 = ( r2*r2 - dotrr ) * inv_dels;
#endif
  
  return (z2*s1-z1*s2)/(s1-s2);
}
  
  
template<class SpacePoint> long IDScanZFinderInternal<SpacePoint>::fillVectors (const std::vector<const SpacePoint *>& spVec, 
  										const IRoiDescriptor& roi,
  										std::vector<double>& phi, 
  										std::vector<double>& rho, 
  										std::vector<double>& zed, 
  										std::vector<long>& lyr, 
  										std::vector<long>& filledLayers)
{
  std::vector<bool> lcount( m_IdScan_MaxNumLayers, false );
  
  // full scan check
  typename std::vector<const SpacePoint *>::const_iterator SpItr( spVec.begin() );
  
  long nSPs = spVec.size();
  
  // to shift the phi of space points as if the RoI starts at phi~0
  // assumes that RoI->phi0() and the SPs are in range [-pi,+pi]
  //
  double roiPhiMin, roiPhiMax;
  
  if ( m_fullScanMode || roi.isFullscan() ) { 
    roiPhiMin = -M_PI;
    roiPhiMax =  M_PI;
  }
  else { 
    // If we trust that all the SPs are properly input, we determine the RoI phi width
    //  using the SPs themselves.
    //  If the RoI phi range is wider than pi) we keep everything as usual.
    if ( m_trustSPprovider  &&  m_usedROIphiWidth < M_PI )
      {
  	double roiPhiPosMin( 9.9), roiPhiPosMax(0);
  	double roiPhiNegMin(-9.9), roiPhiNegMax(0);  // least negative and most negative
  	for(long i=0; i<nSPs; ++i, ++SpItr)
  	  {
  	    double spphi = (*SpItr)->phi();
  	    if ( spphi>0  &&  spphi>roiPhiPosMax )  roiPhiPosMax = spphi;
  	    if ( spphi>0  &&  spphi<roiPhiPosMin )  roiPhiPosMin = spphi;
  	    
  	    if ( spphi<0  &&  spphi<roiPhiNegMax )  roiPhiNegMax = spphi;
  	    if ( spphi<0  &&  spphi>roiPhiNegMin )  roiPhiNegMin = spphi;
  	  }
  	
  	if ( roiPhiNegMax > roiPhiNegMin )  { 
  	  // if all SPs are in (0, pi):
  	  roiPhiMin = roiPhiPosMin; 
  	  roiPhiMax = roiPhiPosMax;
  	}
  	else if ( roiPhiPosMax < roiPhiPosMin )  { 
  	  // if all SPs are in (-pi, 0):
  	  roiPhiMin = roiPhiNegMax; 
  	  roiPhiMax = roiPhiNegMin; 
  	}
  	else if ( roiPhiPosMin - roiPhiNegMin < M_PI )  { 
  	  // if we have an RoI that covers phi=0 axis
  	  roiPhiMin = roiPhiNegMax; 
  	  roiPhiMax = roiPhiPosMax;
  	}
  	else  { 
	  // if we have an RoI that covers phi=pi axis
  	  roiPhiMin = roiPhiPosMin; 
  	  roiPhiMax = roiPhiNegMin; 
  	}
  	
  	roiPhiMin -= 1e-10;
  	roiPhiMax += 1e-10;
  	
  	SpItr = spVec.begin();  // rewind the iterator 
      }
    else  {
  
      /// get from roi now
      if ( roi.phiMinus()==roi.phiPlus() ) { 
  	roiPhiMin = roi.phi()-0.5*m_usedROIphiWidth;
  	roiPhiMax = roi.phi()+0.5*m_usedROIphiWidth;
  	if(roiPhiMin<-M_PI) roiPhiMin+=2*M_PI;
  	if(roiPhiMax>M_PI)  roiPhiMax-=2*M_PI;
      }
      else {
  	// already wrapped by RoiDescriptor
  	roiPhiMin = roi.phiMinus();
  	roiPhiMax = roi.phiPlus();
      }
  
    }
      
  }
  
  
  double dphi = roiPhiMax-roiPhiMin;
  if ( dphi<0 ) dphi+=2*M_PI;
  
  m_usedROIphiWidth = dphi;
  
  //  std::cout << "m_usedROIphiWidth: " << m_usedROIphiWidth << std::endl;
  m_NumPhiSlices = long (ceil( m_usedROIphiWidth*m_invPhiSliceSize ));
  
  
  bool piBound=(roiPhiMin>roiPhiMax);
    
  int icount = 0;
  
  if(!piBound)
    {
      /// DOES NOT span the phi=pi boundary
      for(long i=0; i<nSPs; ++i, ++SpItr) 
  	{
  	  double _phi = (*SpItr)->phi() - roiPhiMin;
  
  	  if ( _phi>=0 && _phi<dphi ) { 
  	    phi[i] = _phi;
  	    rho[i] = (*SpItr)->r();
  	    zed[i] = (*SpItr)->z();
  	    lyr[i] = (*SpItr)->layer();
  	    lcount[lyr[i]]=true;
  	    ++icount;
  	  }
  	}
    }
  else
    {
      /// DOES span the phi=pi boundary
      for(long i=0; i<nSPs; ++i, ++SpItr) 
  	{
  	  double _phi = (*SpItr)->phi() - roiPhiMin;
  	  if( _phi<0.0) _phi+=2*M_PI;
  
  	  if ( _phi>=0 && _phi<dphi ) { 
  	    phi[i] = _phi;
  	    rho[i] = (*SpItr)->r();
  	    zed[i] = (*SpItr)->z();
  	    lyr[i] = (*SpItr)->layer();
  	    lcount[lyr[i]]=true;
  	    ++icount;
  	  }
  	}
    }
    
  if ( icount<nSPs ) { 
  
    //    std::cout << "IDScanZFinderInternal::fillVectors() filtered some spacepoints " << nSPs 
    //	      << " -> " << icount << std::endl; 
    /// resize excluding points outside the RoI    
    phi.resize(icount);
    rho.resize(icount);
    zed.resize(icount);
    lyr.resize(icount);
  
    nSPs = icount;
  }
    
  
  //   Store in filledLayers the layerNumber of those layers that contain hits.
  //   So, if there are hits in layers 1,3,8 filledLayers[0]=1, filledLayers[1]=3
  //   and filledLayers[2]=8 
  //
  long filled = 0;
  for ( long i=0; i<m_IdScan_MaxNumLayers; ++i ) {
    if ( lcount[i] ) {
      filledLayers[filled] = i;
      ++filled;
    }
  }
  
  //  std::cout << "SUTT NSP : " << phi.size() << " " << spVec.size() << std::endl;
  //  for ( unsigned i=0 ; i<phi.size() ; i++ ) { 
  //    std::cout << "SUTT SP : " << i << "\tphi " << phi[i] << "\tz " << zed[i] << "\tr " << rho[i] << std::endl; 
  //  }
  
  return filled;
}
  
template<class SpacePoint> 
std::vector<typename IDScanZFinderInternal<SpacePoint>::vertex>* IDScanZFinderInternal<SpacePoint>::findZInternal( const std::vector<const SpacePoint *>& spVec, 
  														   const IRoiDescriptor& roi)
{
  std::vector<vertex>* output = new std::vector<vertex>();
  
  long nsp = spVec.size();
  if ( !nsp ) return output; //No points - return nothing
  
  SetInternalStatus(0);
  
  //   Creating vectors of doubles/longs for storing phi,rho,z and layer of space points.
  //   filledLayers is used to know which of all layers contain space points
  //   and fill with relevant info...
  //
  std::vector<double> phi(nsp), rho(nsp), zed(nsp);
  std::vector<long>   lyr(nsp), filledLayers(m_IdScan_MaxNumLayers);
  
  long filled = this->fillVectors( spVec, roi, phi, rho, zed, lyr, filledLayers);
  
  nsp = phi.size();
  
  //  std::cout << "SUTT roi " << roi << "nsp: " << nsp << std::endl;
  
  double zMin = roi.zedMinus();
  double zMax = roi.zedPlus();
  
  //   The bin size of the z-histo -and hence the number of bins- 
  //   depends on the RoI's |eta| (to account for worsening z-resolution)
  //
  const double ZBinSize = m_minZBinSize + m_zBinSizeEtaCoeff*fabs(roi.eta());
  const double invZBinSize = 1./ZBinSize;
  
  
  const long HalfZhistoBins = long ( ceil( 0.5*(zMax-zMin)*invZBinSize ) );
  const long NumZhistoBins = 2*HalfZhistoBins;
  
  // number of phi bins to look at will get fewer as eta increases
  const long extraPhiNeg = static_cast<long> ( floor( (0.9 - fabs(roi.eta()))*m_dphideta*m_invPhiSliceSize*m_neighborMultiplier ) );
  
  
  
  //   These are the z-Histograms
  //   Two sets are defined: {n/z}Histo[0][phi][z] will be for positively bending tracks
  //                         {n/z}Histo[1][phi][z] will be for negatively bending tracks
  //
  
  long numZhistos = m_zHistoPerPhi ? m_NumPhiSlices : 1 ;
  
  //  std::vector < std::vector < std::vector<long>   > >  nHisto( 2, std::vector < std::vector<long>   > (numZhistos, std::vector<long>  () ) );
  //  std::vector < std::vector < std::vector<double> > >  zHisto( 2, std::vector < std::vector<double> > (numZhistos, std::vector<double>() ) );
  
  for ( int i=2 ; i-- ;  ) { m_nHisto[i].clear();   m_nHisto[i].resize(numZhistos); } 
  for ( int i=2 ; i-- ;  ) { m_zHisto[i].clear();   m_zHisto[i].resize(numZhistos); } 
  
  std::vector< std::vector<long> >*   nHisto = m_nHisto;
  std::vector< std::vector<double> >* zHisto = m_zHisto;
  
  m_NMax = 0;
  
  
  
  
  //Make a vector of all the PhiSlice instances we need
  std::vector< PhiSlice* > allSlices( m_NumPhiSlices );
  for ( unsigned int sliceIndex = 0; sliceIndex < m_NumPhiSlices; sliceIndex++ )
    {
      allSlices[ sliceIndex ] = new PhiSlice( sliceIndex, ZBinSize, m_invPhiSliceSize,
					      m_tripletDZ, m_tripletDK, m_tripletDP, zMin, zMax,
					      m_IdScan_MaxNumLayers, m_IdScan_LastBrlLayer );
    }
    
  int allSlicesSize = allSlices.size();
  //Populate the slices
  for ( long pointIndex = 0; pointIndex < nsp; pointIndex++ )
    {
      int phiIndex = floor( phi[ pointIndex ] * m_invPhiSliceSize );
      if (phiIndex > allSlicesSize) {
        continue;
      }
      allSlices[ phiIndex ]->AddPoint( rho[ pointIndex ], zed[ pointIndex ], phi[ pointIndex ], lyr[ pointIndex ] );
    }
  
  //Read out the slices into flat structures for the whole layers
  std::vector< std::vector< double > > allLayerRhos, allLayerZs, allLayerPhis;
  allLayerRhos.resize( m_IdScan_MaxNumLayers );
  allLayerZs.resize( m_IdScan_MaxNumLayers );
  allLayerPhis.resize( m_IdScan_MaxNumLayers );
  std::vector< std::vector< int > > allSliceWidths( m_IdScan_MaxNumLayers, std::vector< int >( m_NumPhiSlices + 1, 0 ) );
  for ( unsigned int sliceIndex = 0; sliceIndex < m_NumPhiSlices; sliceIndex++ )
    {
      allSlices[ sliceIndex ]->MakeWideLayers( &allLayerRhos, &allLayerZs, &allLayerPhis, &allSliceWidths, filled, &filledLayers );
    }
  
  //One histogram per phi slice?
  if ( m_zHistoPerPhi )
    {
      //Allocate all the histograms
      for ( unsigned int sliceIndex = 0; sliceIndex < m_NumPhiSlices; sliceIndex++ )
	{
	  nHisto[0][sliceIndex].resize( NumZhistoBins + 1 );
	  zHisto[0][sliceIndex].resize( NumZhistoBins + 1 );
	}
      if ( m_chargeAware )
	{
	  for ( unsigned int sliceIndex = 0; sliceIndex < m_NumPhiSlices; sliceIndex++ )
	    {
	      nHisto[1][sliceIndex].resize( NumZhistoBins + 1 );
	      zHisto[1][sliceIndex].resize( NumZhistoBins + 1 );
	    }
	}
  
      //Populate the histograms
      for ( unsigned int sliceIndex = 0; sliceIndex < m_NumPhiSlices; sliceIndex++ )
	{
	  allSlices[ sliceIndex ]->GetHistogram( &( nHisto[0][sliceIndex] ), &( zHisto[0][sliceIndex] ),
						 &( nHisto[1][sliceIndex] ), &( zHisto[1][sliceIndex] ),
						 extraPhi, extraPhiNeg, m_nFirstLayers, m_tripletMode, m_chargeAware, nHisto, zHisto );
	  //Note the extra arguments here - pointers to the whole histogram collection
	  //This allows the filling of neighbouring slice histograms as required, but breaks thread safety
  
	  delete allSlices[ sliceIndex ];
	}
    }
  else
    {
      //Allocate the z-axis histograms
      nHisto[0][0].resize( NumZhistoBins + 1 );
      zHisto[0][0].resize( NumZhistoBins + 1 );
      if ( m_chargeAware )
	{
	  nHisto[1][0].resize( NumZhistoBins + 1 );
	  zHisto[1][0].resize( NumZhistoBins + 1 );
	}
  
      //Populate the histogram - fast and memory-efficient, but not thread safe (use MakeHistogram for thread safety)
      for ( unsigned int sliceIndex = 0; sliceIndex < m_NumPhiSlices; sliceIndex++ )
	{
	  allSlices[ sliceIndex ]->GetHistogram( &( nHisto[0][0] ), &( zHisto[0][0] ),
						 &( nHisto[1][0] ), &( zHisto[1][0] ),
						 extraPhi, extraPhiNeg, m_nFirstLayers, m_tripletMode, m_chargeAware );
	  delete allSlices[ sliceIndex ];
	}
    }
  
  /*  
      printf("etaRoI: %f\n", etaRoI);  
      printf("ZBinSize: %f\n", ZBinSize);
      printf("m_minZBinSize %f\n", m_minZBinSize);
      printf("NumZhistoBins: %d\n",NumZhistoBins);
  */
  
  
  
  /// First calculate the pedestal to be subtracted (only used in the HI 
  /// case at the moment)
    
  double pedestal = 0;
  
  if ( m_weightThreshold>0 ) { 
  
    int count = 0; 
  
    for ( long zpm=0; zpm<1 || ( m_chargeAware && zpm<2 ) ; ++zpm ) {
  
      for(std::vector< std::vector<long> >::iterator nfiter = nHisto[zpm].begin(); nfiter!=nHisto[zpm].end(); ++nfiter) {
  
  	if((*nfiter).empty()) continue; // only check the filled zHistos
  	if((*nfiter).size() <= 2 ) continue;// this is only a protection : with proper inputs to zfinder, it is always satisfied
  	
  	for(std::vector<long>::iterator niter=nfiter->begin()+2; niter!=nfiter->end(); ++niter ) {
  	  /// skip bins used for the vertex candidates 
  	  if ( *niter>=0 ) { 
  	    count++;
  	    pedestal += *(niter) + *(niter-1) + *(niter-2);
  	  }
  	}
      }
    }
  
    if ( count>0 ) pedestal /= count;
        
  }
  
  
  
    
  //   Now the nHisto's are filled; find the 3 consecutive bins with the highest number of entries...
  //
    
  std::vector<double>  zoutput;
  std::vector<double>  woutput;
  
    
  
  for(long b = 0; (int)zoutput.size() < m_numberOfPeaks; ++b) {
    
    long maxh=0;  // was 1 before triplets were introduced
    long binMax=0;
    long bending=0, bestPhi=0;
    long ztest;
  
    for ( long zpm=0; zpm<1 || ( m_chargeAware && zpm<2 ) ; ++zpm ) {
  
      for(std::vector< std::vector<long> >::iterator nfiter = nHisto[zpm].begin(); nfiter!=nHisto[zpm].end(); ++nfiter) {
  
  	if((*nfiter).empty()) continue; // only check the filled zHistos
  	if((*nfiter).size() <= 2 ) continue;// this is only a protection : with proper inputs to zfinder, it is always satisfied
  
  	for(std::vector<long>::iterator niter=(*nfiter).begin()+2; niter!=(*nfiter).end(); ++niter ) {
  
  	  ztest = *(niter-2) + *(niter-1) + *(niter);
  	  if ( ztest <= 0 || ztest < maxh ) continue;
  	  ///	  if ( ztest >= maxh && ( ( m_applyWeightThreshold && ztest>m_weightThreshold ) || !m_applyWeightThreshold ) ) {
  	  /// apply threshold later - should we wish it
  	  if ( ztest >= maxh ) { // && ztest>m_weightThreshold ) {
  	    long bintest = niter-(*nfiter).begin()-1;
  	    if ( ztest > maxh ||
  		 // for two candidates at same "height", prefer the more central one
  		 (m_preferCentralZ && std::abs( bintest - HalfZhistoBins ) < std::abs( binMax - HalfZhistoBins ) ) ) {
  	      maxh = ztest;
  	      binMax = bintest;
  	      bestPhi = nfiter-nHisto[zpm].begin();
  	      bending = zpm;
  	    }
  	  }
  	}// end of niter loop
      }
    }
    m_NMax = maxh;
    /// if we are in triplet mode, even a single pair means 3 consistent hits
    /// also bomb out if no maximum (above threshold) is found
    if ( maxh==0 || ( m_tripletMode==0 && maxh<2 ) ) {
      break;
    }
  
    //   ...and compute the "entries-weighted" average bin position
  
    double weightedMax = ( zHisto[bending][bestPhi][binMax] +
  			   zHisto[bending][bestPhi][binMax+1] +
  			   zHisto[bending][bestPhi][binMax-1] ) /maxh;
  
    /// if found a vertex flag the bins so we don't use them again 
    if ( m_numberOfPeaks>1 ) { 
      nHisto[bending][bestPhi][binMax]   = -1;
      nHisto[bending][bestPhi][binMax-1] = -1;
      nHisto[bending][bestPhi][binMax+1] = -1;
      zHisto[bending][bestPhi][binMax]   = 0;
      zHisto[bending][bestPhi][binMax-1] = 0;
      zHisto[bending][bestPhi][binMax+1] = 0;
    }
  
    int closestVtx = -1; // find the closest vertex already put into the output list
    float dist2closestVtx = 1000; // should be larger than m_ZFinder_MaxZ*2
    for ( size_t iv = 0; iv < zoutput.size(); ++iv )
      if ( fabs(weightedMax-zoutput[iv]) < dist2closestVtx ) {
  	closestVtx = iv;
  	dist2closestVtx = fabs(weightedMax-zoutput[iv]); 
      }
  
    if ( dist2closestVtx < m_nvrtxSeparation * ZBinSize ||
  	 dist2closestVtx < fabs(weightedMax) * m_vrtxDistCut ) {
      zoutput[closestVtx] = m_vrtxMixing * weightedMax + (1.0 - m_vrtxMixing) * zoutput[closestVtx] ;
      woutput[closestVtx] = m_vrtxMixing * maxh        + (1.0 - m_vrtxMixing) * woutput[closestVtx] ;
    }  else  {
      zoutput.push_back( weightedMax );
      woutput.push_back( maxh );
    }
  
  } // end of "b" loop, the loop over m_numberOfPeaks
  
  
    /// at this point we have the histogram with the highest N vertices removed
    /// so we can find the "non vertex" pedestal from these, although it will be 
    /// somewhat lower than perhaps it should be, in case some of the "vertices"
    /// we are removing are just random upwards fluctuations 
    
    /// NB: have moved pedestal calculation to before the extraction of the vertices
    ///     if we calculate it after, then we have too low a pedestal if some vertices 
    ///     are really random fluctuations. If we calculate it before then we 
    ///     overestimate the pedestal, really we should try to decide how many *real* 
    ///     vertices we have, and then only exclude them, but that level of detail is 
    ///     probably not justified by the correlation with the offline track multiplicity
    ///     on the vertex  
  
    /// copy vertices to output vector - this is so we can first impose cuts on the vertices we 
    /// have found should we wish to
  
  
#if 0
  if ( m_weightThreshold>0 ) { 
      
    /// NB: This is a VERY, VERY, bad thing to do. If we have the "weightThreshold" set, then 
    ///     this is the HI run, ZFinder only mode, and we want to select hight track multiplicity 
    ///     vertices
    ///     In this case we do not actually care anout the z position of the vertices (well we 
    ///     should probably) so to allow us to select on the vertex "track multiplicity" (ie
    ///     the weight of these vertices) we write the *weight* to the TrigVertex instead
    ///     then in the TrigHIL2VtxMultHypo we can cut on theweight, multiplicity etc 
      
    for ( unsigned i=0 ; i<zoutput.size() ; i++ ) { 
      /// write the *weight* not the *z position* in this *special* case
      /// but first subtract the pedestal
      output->push_back( woutput[i] - pedestal );
    }
      
  }    
  else { 
    for ( unsigned i=0 ; i<zoutput.size() ; i++ ) output->push_back( zoutput[i] ); 
  }
  
  
#else
  
  /// NB: if m_weightThreshold==0 then pedestal == 0 also   
  /// AAAAAAAAARRRGHH!!! This is ridiculous, passing parameters about differently because we don't have a 
  /// proper interface defined 
  if ( m_weightThreshold>0 ) { 
    for ( unsigned i=0 ; i<zoutput.size() ; i++ ) { 
      output->push_back( vertex( woutput[i] - pedestal, zoutput[i] ) ); 
    }
  }
  else {
    for ( unsigned i=0 ; i<zoutput.size() ; i++ ) { 
      output->push_back( vertex( zoutput[i], woutput[i] - pedestal ) ); 
    }
  }
  
  
#endif
  
  //  std::cout << "SUTT zoutput size " << zoutput.size() << "\t" << roi << std::endl;
  //  for ( unsigned i=0 ; i<zoutput.size() ; i++ ) std::cout << "SUTT zoutput        " << i << "\t" << zoutput[i] << std::endl;
  
  return output;
}
}

#endif

