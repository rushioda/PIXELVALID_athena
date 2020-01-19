/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  PixelClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for PixelClusterOnTrack production
///////////////////////////////////////////////////////////////////
// started 1/05/2004 I.Gavrilenko - see ChangeLog for details
///////////////////////////////////////////////////////////////////
#ifndef PixelClusterOnTrackTool_H
#define PixelClusterOnTrackTool_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IRIO_OnTrackErrorScalingTool.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"

#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "TrkParameters/TrackParameters.h"
//#include "InDetIdentifier/PixelID.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkAmbiguityProcessor/dRMap.h"

//#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"
//#include "PixelConditionsTools/IModuleDistortionsTool.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
class PixelID;
class IPixelOfflineCalibSvc;
class IModuleDistortionsTool;
class IIncidentSvc;
class StoreGateSvc;
class IIBLParameterSvc;

namespace InDet {

  /** @brief creates PixelClusterOnTrack objects allowing to
      calibrate cluster position and error using a given track hypothesis. 

      See doxygen of Trk::RIO_OnTrackCreator for details.
      Different strategies to calibrate the cluster error can be chosen
      by job Option. Also the handle to the general hit-error scaling
      is implemented.

      Special strategies for correction can be invoked by calling the
      correct method with an additional argument from the 
      PixelClusterStrategy enumeration

  */

  class NnClusterizationFactory;

  enum PixelClusterStrategy {
    PIXELCLUSTER_DEFAULT=0,
    PIXELCLUSTER_OUTLIER=1,
    PIXELCLUSTER_SHARED =2,
    PIXELCLUSTER_SPLIT  =3
  };


  class PixelClusterOnTrackTool: 
        public AthAlgTool, virtual public Trk::IRIO_OnTrackCreator,  virtual public IIncidentListener
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

public:

  //! AlgTool constructor 
  PixelClusterOnTrackTool(const std::string&,const std::string&,
                          const IInterface*);
  virtual ~PixelClusterOnTrackTool ();
  //! AlgTool initialisation
  virtual StatusCode initialize();
  //! AlgTool termination
  virtual StatusCode finalize  ();
  
  /** handle for incident service */
  void handle(const Incident& inc) ;
 
  void correctBow(const Identifier&, Amg::Vector2D& locpos, const double tanphi, const double taneta) const;

  double splineIBLPullX(float x, int layer) const;

  /** @brief produces a PixelClusterOnTrack (object factory!).

      Depending on job options it changes the pixel cluster position
      and error according to the parameters (in particular, the angle)
      of the intersecting track.
  */
  virtual const InDet::PixelClusterOnTrack* correct(const Trk::PrepRawData&,
                                                    const Trk::TrackParameters&) const;

  virtual const InDet::PixelClusterOnTrack* correctDefault(const Trk::PrepRawData&, 
                                                           const Trk::TrackParameters&) const;

  virtual const InDet::PixelClusterOnTrack* correctNN(const Trk::PrepRawData&, const Trk::TrackParameters&) const;
	
  virtual bool getErrorsDefaultAmbi( const InDet::PixelCluster*, const Trk::TrackParameters&, 
                           Amg::Vector2D&,  Amg::MatrixX&) const;
													 
	virtual bool getErrorsTIDE_Ambi( const InDet::PixelCluster*, const Trk::TrackParameters&,  
                        Amg::Vector2D&,  Amg::MatrixX&) const;

  virtual const InDet::PixelClusterOnTrack* correct
    (const Trk::PrepRawData&, const Trk::TrackParameters&, 
     const InDet::PixelClusterStrategy) const;

     
  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
  
 private:

  /** @brief parametrizes the pixel cluster position error as a function of 
      the track angle alpha and the cluster width (number of rows) deltax */
  //  double getBarrelPhiError(double& alpha, int& deltax) const;
  //  double getBarrelEtaError(double eta, int deltax, int deltay) const;
  // double getEndcapPhiError(int etasize, int phisize) const;
  // double getEndcapEtaError(int etasize, int phisize) const;
  

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////

  ToolHandle<IModuleDistortionsTool>            m_pixDistoTool    ;
  ToolHandle<Trk::IRIO_OnTrackErrorScalingTool> m_errorScalingTool;
  ServiceHandle<IPixelOfflineCalibSvc>          m_calibSvc        ;
  StoreGateSvc*                                 m_detStore        ;
  /* ME: Test histos have nothing to do with production code, use a flag
    IHistogram1D* m_h_Resx;
    IHistogram1D* m_h_Resy;
    IHistogram1D* m_h_Locx;
    IHistogram1D* m_h_Locy;
    IHistogram1D* m_h_PhiTrack;
    IHistogram1D* m_h_ThetaTrack;
    IHistogram1D* m_h_Rad; 
    IHistogram1D* m_h_Slope;
  */

  //! toolhandle for central error scaling
  //! flag storing if errors need scaling or should be kept nominal
  bool                               m_scalePixelCov     ;
  bool                               m_disableDistortions;
  bool                               m_rel13like         ;
  int                                m_positionStrategy  ;
  mutable int                        m_errorStrategy     ;
  
  
  /** @brief Flag controlling how module distortions are taken into account:
      
  case 0 -----> No distorsions implemented;
  
  case 1 -----> Set curvature (in 1/meter) and twist (in radiant) equal for all modules;
  
  case 2 -----> Read curvatures and twists from textfile containing Survey data;
  
  case 3 -----> Set curvature and twist from Gaussian random generator with mean and RMS coming from Survey data;
  
  case 4 -----> Read curvatures and twists from database (not ready yet);
  */
  //! identifier-helper
  const PixelID*                     m_pixelid;
  
  /** Enable NN based calibration (do only if NN calibration is applied) **/
  mutable bool                      m_applyNNcorrection;
  mutable bool                      m_applydRcorrection;
  bool				    m_NNIBLcorrection;
  bool				    m_IBLAbsent;
  
  /** NN clusterizationi factory for NN based positions and errors **/
  ToolHandle<NnClusterizationFactory>                   m_NnClusterizationFactory;
  ServiceHandle<StoreGateSvc>                           m_storeGate;            //!< Event store
  ServiceHandle<IIncidentSvc>                           m_incidentSvc;          //!< IncidentSvc to catch begin of event and end of envent
  ServiceHandle<IIBLParameterSvc>                        m_IBLParameterSvc;
  mutable const InDet::PixelGangedClusterAmbiguities*   m_splitClusterMap;      //!< the actual split map         
  std::string                                           m_splitClusterMapName;  //!< split cluster ambiguity map
  mutable const InDet::DRMap*                           m_dRMap;      //!< the actual dR map         
  std::string                                           m_dRMapName;  //!< dR map
  bool                                                  m_doNotRecalibrateNN;
  bool                                                  m_noNNandBroadErrors;
	
	/** Enable different treatment of  cluster errors based on NN information (do only if TIDE ambi is run) **/
  bool                      m_usingTIDE_Ambi;
  
  std::vector< std::vector<float> > m_fX, m_fY, m_fB, m_fC, m_fD;
};

} // end of namespace InDet

#endif // PixelClusterOnTrackTool_H
