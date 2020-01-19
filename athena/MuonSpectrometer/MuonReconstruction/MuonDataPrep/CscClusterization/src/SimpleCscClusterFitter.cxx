/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SimpleCscClusterFitter.cxx
#include "SimpleCscClusterFitter.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "CscClusterization/ICscAlignmentTool.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include <sstream>
#include <iomanip>

using std::string;
using std::vector;
using Muon::CscClusterStatus;
using Muon::CscPrepData;
using Muon::CscStripPrepData;
using MuonGM::CscReadoutElement;

typedef ICscClusterFitter::Result Result;
typedef std::vector<Result> Results;

enum CscStation { UNKNOWN_STATION, CSS, CSL };
enum CscPlane { CSS_R, CSL_R, CSS_PHI, CSL_PHI, UNKNOWN_PLANE };

#if 0
namespace {
  string splane(CscPlane plane) {
    switch(plane) {
    case CSS_R: return "CSS r";
    case CSL_R: return "CSL r";
    case CSS_PHI: return "CSS phi";
    case CSL_PHI: return "CSL phi";
    case UNKNOWN_PLANE: return "no such plane";
    }
    return "no such plane";
  }

  CscStation findStation(double pitch){
    CscStation station = UNKNOWN_STATION;
    if ( pitch > 5.5 && pitch < 5.6 ) {
      station = CSS;
    } else if ( pitch > 5.3 && pitch < 5.4 ) {
      station = CSL;
    } else if ( pitch > 12.0 && pitch < 14.0 ) {
      station = CSS;
    } else if ( pitch > 20.0 && pitch < 22.0 ) {
      station = CSL;
    }
    return station;
  }
  
  CscPlane findPlane(double pitch){
    CscPlane   plane = UNKNOWN_PLANE;
    if ( pitch > 5.5 && pitch < 5.6 ) {
      plane = CSS_R;
    } else if ( pitch > 5.3 && pitch < 5.4 ) {
      plane = CSL_R;
    } else if ( pitch > 12.0 && pitch < 14.0 ) {
      plane = CSS_PHI;
    } else if ( pitch > 20.0 && pitch < 22.0 ) {
      plane = CSL_PHI;
    }
    return plane;
  }
}
#endif

//**********************************************************************
// Member functions.
//**********************************************************************

SimpleCscClusterFitter::
SimpleCscClusterFitter(string type, string aname, const IInterface* parent)
  : AthAlgTool(type, aname, parent), m_detMgr(nullptr), m_cscIdHelper(nullptr)
  , m_alignmentTool("CscAlignmentTool/CscAlignmentTool")
{
  declareInterface<ICscClusterFitter>(this);
  declareProperty("position_option", m_option = "MEAN",
                  "Cluster fitting option: MEAN, PEAK, CENTROID");
  declareProperty("intrinsic_cluster_width", m_intrinsic_cluster_width = 20.0,
                  "Intrinsic cluster width (mm)");
  declareProperty("use_peakthreshold", m_use_peakthreshold = false);
  declareProperty("defaultErrorScaler_eta", m_defaultErrorScaler_eta = 1.0);
  declareProperty("defaultErrorScaler_phi", m_defaultErrorScaler_phi = 1.0);

  declareProperty("CscAlignmentTool", m_alignmentTool );
}

//**********************************************************************

SimpleCscClusterFitter::~SimpleCscClusterFitter() { }

//**********************************************************************

StatusCode SimpleCscClusterFitter::initialize() {

  ATH_MSG_VERBOSE ( "Initializing " << name() );

  ATH_MSG_DEBUG ( "Properties for " << name() << ":" );
  ATH_MSG_DEBUG ( "  Position option: " << m_option );
  ATH_MSG_DEBUG ( "  Intrinsic width: " << m_intrinsic_cluster_width << " mm" );
  
  // Retrieve muon geometry.
  if ( detStore()->retrieve(m_detMgr,"Muon").isFailure() ) {
    ATH_MSG_FATAL ( "Could not find the MuonGeoModel Manager! " );
    return StatusCode::FAILURE;
  }
  // Fetch ID helper.
  m_cscIdHelper = m_detMgr->cscIdHelper();

  if ( m_alignmentTool.retrieve().isFailure() )   {
    ATH_MSG_WARNING ( name() << ": unable to retrieve cluster fitter " << m_alignmentTool );
  } else {
    ATH_MSG_DEBUG ( name() << ": retrieved " << m_alignmentTool );
  }
  return StatusCode::SUCCESS;
}


//**********************************************************************

Results SimpleCscClusterFitter::fit(const StripFitList& sfits) const {

  Results results;
  Result res;
  
  // Check the input lists.
  unsigned int nstrip = sfits.size();
  if ( nstrip == 0 ) {
    ATH_MSG_WARNING ( "Strip list is empty." );
    res.fitStatus = 1;
    results.push_back(res);
    return results;
  }
  if ( sfits.size() != nstrip ) {
    ATH_MSG_WARNING ( "Fit and strip lists have different sizes" );
    res.fitStatus = 2;
    results.push_back(res);
    return results;
  }

  // Check initialization.
  if ( ! m_cscIdHelper ) {
    ATH_MSG_WARNING ( "Initialization failed." );
     res.fitStatus = 3;
     results.push_back(res);
     return results;
  }

  const CscStripPrepData* pstrip = sfits[0].strip;
  if ( pstrip == 0 ) {
    ATH_MSG_WARNING ( "Strip pointer is null." );
     res.fitStatus = 4;
     results.push_back(res);
     return results;
  }
  Identifier idStrip0 = pstrip->identify();
  const CscReadoutElement* pro = m_detMgr->getCscReadoutElement(idStrip0);
  bool measphi = m_cscIdHelper->CscIdHelper::measuresPhi(idStrip0);
  double pitch = pro->cathodeReadoutPitch(0, measphi);
  int maxstrip = pro->maxNumberOfStrips(measphi);
  int strip0 = m_cscIdHelper->strip(idStrip0) - 1;

  int zsec    = m_cscIdHelper->stationEta(idStrip0);
  int station = m_cscIdHelper->stationName(idStrip0) - 49;    // 1=CSS, 2=CSL
  int phisec  = m_cscIdHelper->stationPhi(idStrip0);

  int sector  = zsec*(2*phisec - station + 1);
  int wlay    = m_cscIdHelper->wireLayer(idStrip0);
  
  // In SimpleCscClusterFitter  istrip_peak = strip0;
  int peak_count = 0;                  // # peaks in the cluster
  bool edge = strip0 == 0;             // is cluster on the edge of the chamber?
  //  int istrip_peak = strip0;            // Strip with the most charge and initialized as the first strip idx.
  int stripidx =0;                     // actual strip position [0-191] or [0-47]
  int countstrip =0;                   // counting strip in for loop
  double qsum=0;                       // charge sum of strips in cluster
  double xsum=0;                       // stripidx sum in cluster 
  double qxsum=0;                      // position weighted (stripidx) charge sum
  double qerravg=0;
  // istrip starts from 0.
  // stripidx is for actual strip position [0-191] or [0-47].
  // Out of for loop, stripidx will be the last strip of cluster.
  unsigned int istrip_peak = 0;
  double lastqpeak =0;
  for ( unsigned int istrip =0; istrip<nstrip; ++istrip ) {
    StripFit sfit = sfits[istrip];
    float qthis = sfit.charge;
    float qlast = 0.0;
    if ( istrip > 0 ) qlast = sfits[istrip-1].charge;
    float qnext = 0.0;
    if ( istrip < nstrip-1 ) qnext = sfits[istrip+1].charge;
    stripidx = strip0+istrip;
    qsum  += qthis;
    qerravg += sfit.dcharge;
    xsum  += stripidx;
    qxsum += qthis*stripidx;
    countstrip = istrip+1;
    if ( countstrip==2 && qthis<qlast ) ++peak_count;
    if ( countstrip>2 && qthis<qlast && qlast>=sfits[istrip-2].charge ) ++peak_count;

    bool ispeak = qthis > qlast  &&  qthis > qnext;
    // Special case: next strip has the same charge.
    // Require the previous strip has less charge and the next following
    // strip be absent or have less charge.
    if ( ! ispeak ) {
      if ( qthis == qnext ) {
        ispeak = ( qthis > qlast ) &&
                 ( istrip+2 == nstrip || sfits[istrip+2].charge < qthis );
      }
    }
    // Special case: first and second strips have the same charge.
    // Require the third strip has less charge.
    if ( ! ispeak ) {
      if ( istrip == 1 ) {
        if ( qthis == qlast ) {
          ispeak = qthis > qnext; // bug found 10/13/07
        }
      }
    }
    // Record if peak.
    if ( ispeak && qthis>lastqpeak ) {
        istrip_peak = istrip;
        lastqpeak = qthis;
    }
  }
  if ( stripidx == maxstrip-1 ) edge = true;
  // Update peak count and edge.
  if ( countstrip==1 ) ++peak_count;
  if ( countstrip>1 && sfits[nstrip-1].charge>=sfits[nstrip-2].charge ) ++peak_count;
  

  //Fix to avoid division-by-zero (W.L. 29 Jun 2012)
  if (qsum<=0) {
    //ATH_MSG_WARNING("Charge sum is not positive.");
    //ATH_MSG_WARNING("Charge sum : "<<qsum);
    res.fitStatus=0;
    double savg = strip0+istrip_peak; // peak position: this strip has Q>0
    res.position = pitch*(savg + 0.5 - 0.5*maxstrip);
    res.strip = istrip_peak; // relative to cluster start
    double errorScaler = measphi ? m_defaultErrorScaler_phi : m_defaultErrorScaler_eta;
    res.dposition = errorScaler*pitch/sqrt(12.0);
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++added 13/06/2014 to avoid index out of range in CscThreshholdClusterBuilder.cxx
    res.fstrip = 0;
    res.lstrip = nstrip-1;
    res.time = sfits[istrip_peak].time;
    res.timeStatus = sfits[istrip_peak].timeStatus;
    res.qpeak = sfits[istrip_peak].charge;

    res.charge = res.qpeak;
    res.charge_beforeBPCorr = sfits[res.strip].charge_beforeBPCorr;
  
    res.qleft = 0.;
    res.qright = 0.;

    if (istrip_peak>=1) {
      res.qleft= sfits[istrip_peak-1].charge;
      res.charge += res.qleft;
      res.charge_beforeBPCorr += sfits[istrip_peak-1].charge_beforeBPCorr;
    }
    if (istrip_peak+1<nstrip) {
      res.qright= sfits[istrip_peak+1].charge;
      res.charge += res.qright;
      res.charge_beforeBPCorr += sfits[istrip_peak+1].charge_beforeBPCorr;
    }
    res.time_beforeT0Corr = sfits[res.strip].time_beforeT0Corr;
    res.time_beforeBPCorr = sfits[res.strip].time_beforeBPCorr;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    results.push_back(res);
    return results;
  }


  // Calculate strip averages.
  double strip_mean = xsum/nstrip;    // Avg strip position
  double strip_qmean = qxsum/qsum;    // Avg strip position weighted by charge
  qerravg = qerravg/nstrip; // for centroid error calculation....
  // Assign cluster status.
  // Cluster is spoiled if it is on the edge or has multiple peaks.
  res.fitStatus = 0;
  res.clusterStatus = Muon::CscStatusSimple;
  if ( edge ) res.clusterStatus = Muon::CscStatusEdge;
  else if ( peak_count > 1 ) res.clusterStatus = Muon::CscStatusMultiPeak;

  double savg=-99.;
  if ( m_option == "MEAN" ) {
    savg = strip_mean;
  } else if ( m_option == "PEAK" ) {
    savg = strip0+istrip_peak;
  } else if ( m_option == "CENTROID" ) {
    savg = strip_qmean;
  } else {
    ATH_MSG_WARNING ( "Invalid position option: " << m_option );
    res.clusterStatus = Muon::CscStatusUndefined;
  } 

  //WP treats.... special centroid method....
  if (m_use_peakthreshold && nstrip>2 && peak_count==1)
    savg = strip_qmean;

  if ( measphi )  savg = strip_mean;
  
  // Assign cluster identifier and time using the center of the cluster.
  //  res.strip = (sfits.size()-1)/2;
  // Assign cluster identifier and time using peak strip  2/24/2009
  res.strip = istrip_peak;

  // Assign position.
  res.position = pitch*(savg + 0.5 - 0.5*maxstrip);

  // internal alignment ... 
  Identifier id = sfits[res.strip].strip->identify();
  double offset = m_alignmentTool->getAlignmentOffset(id);
  res.position -= offset;

  // Assign position error.
  double wmeas = pitch*nstrip;
  double weff = wmeas - m_intrinsic_cluster_width;
  double weffmin = 0.5*wmeas;
  if ( weff < weffmin ) weff = weffmin;
  double errorScaler = measphi ? m_defaultErrorScaler_phi : m_defaultErrorScaler_eta;
  res.dposition = errorScaler*weff/sqrt(12.0); // CENTROID doesn't make any effect on the case nstrip=1
  /*
  if (nstrip>1) {
    if ( m_option == "CENTROID" || ( m_use_peakthreshold && nstrip>2 && peak_count==1) ) {
      double scale_centroid = qerravg/qsum;
      double xxsum =0;
      for ( unsigned int istrip =0; istrip<nstrip; ++istrip ) {
        StripFit sfit = sfits[istrip];
        stripidx = strip0+istrip;
        xxsum  += (stripidx-strip_qmean)*(stripidx-strip_qmean);
      }
      
      res.dposition = pitch*scale_centroid*sqrt(xxsum);
      ATH_MSG_VERBOSE ("qerravg:qsum:scale_centroid:xxsum:res.dposition= "
                       << qerravg << ":" << qsum << ":" << scale_centroid << ":"<< xxsum << ":"<< res.dposition);
    }
  }
  */
  ATH_MSG_VERBOSE ( "    Simple Fit Result " << " nstr=" << nstrip
                    << "[sector:wlay:measphi]= " << sector << ":" <<wlay << ":" <<measphi
                    << " strip0=" << strip0
                    << " istrip_peak=" << istrip_peak
                    << " peaktime=" << sfits[istrip_peak].time
                    << " peakstatus=" << sfits[istrip_peak].status
                    << " peaktimeStatus=" << sfits[istrip_peak].timeStatus
                    << " pos=" << res.position << " dpos=" << res.dposition
                    << " chg=" << qsum
                    );

  // cluster charge should be qsum over three strip... 3/21/2011
  res.fstrip = 0;
  res.lstrip = nstrip-1;
  res.time = sfits[istrip_peak].time;
  res.timeStatus = sfits[istrip_peak].timeStatus;
  res.qpeak = sfits[istrip_peak].charge;

  res.charge = res.qpeak;
  res.charge_beforeBPCorr = sfits[res.strip].charge_beforeBPCorr;
  
  res.qleft = 0.;
  res.qright = 0.;

  if (istrip_peak>=1) {
    res.qleft= sfits[istrip_peak-1].charge;
    res.charge += res.qleft;
    res.charge_beforeBPCorr += sfits[istrip_peak-1].charge_beforeBPCorr;
  }
  if (istrip_peak+1<nstrip) {
    res.qright= sfits[istrip_peak+1].charge;
    res.charge += res.qright;
    res.charge_beforeBPCorr += sfits[istrip_peak+1].charge_beforeBPCorr;
  }
  res.time_beforeT0Corr = sfits[res.strip].time_beforeT0Corr;
  res.time_beforeBPCorr = sfits[res.strip].time_beforeBPCorr;

  //  res.charge = qsum;

  results.push_back(res);
  return results;
}

//**********************************************************************

Results SimpleCscClusterFitter::fit(const StripFitList& sfits, double) const {
  return fit(sfits);
}

//**********************************************************************

StatusCode SimpleCscClusterFitter::finalize(){
  ATH_MSG_VERBOSE ( "Finalizing " << name() );
  return StatusCode::SUCCESS;
}

//**********************************************************************
double SimpleCscClusterFitter::getCorrectedError(const CscPrepData* /*pclu*/, double /*slope*/) const {
  return 0;
}
