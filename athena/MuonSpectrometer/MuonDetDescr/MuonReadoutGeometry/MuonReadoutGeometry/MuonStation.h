/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonStation_H
#define MuonStation_H

#include "MuonReadoutGeometry/GlobalUtilities.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include <map>

class BLinePar;
class MdtAsBuiltPar;
class MsgStream;


namespace MuonGM {
    
/**
   A MuonStation corresponds to a physical set of detectors in the
   MuonSpectrometer denoted as STATION.  In AMDB the structure of a 
   station is described in the corresponding D-lines; there are as 
   many MuonStations as replicas in the MS of such set of subdetectors.
   Therefore, it is identified by StationType (BML) ZI location 
   (a la amdb) FI location (a la amdb) and it holds an alignable 
   transform (from P line, plus A line). Notice that it is not 
   possible to identify a MuonStation via StationType, StationEta and
   StationPhi, because in the case of TGC these fields of the offline
   identifiers specify a subdetector belonging to a TGC MuonStation.

   The MuonDetectorManager provides access to MuonStations via 
   StationType (BML) ZI location (a la amdb) FI location (a la amdb) 
   [the map access key is built by the MuonDetectorManager].  

   MuonStations are the readout-geometry elements in a 1-1 
   correspondence with the MuonChambers. Every MuonChamber instatiates 
   the corresponding MuonStation and records it in the DetectorManager.
   Any MuonReadoutElement will hold a pointer to the parent MuonStation and 
   the MuonStation will hold a map of the ReadoutElements with key = the integer 
   index (a la amdb) called Job [number of order of the component in the station 
   definition]   
*/    

class MuonReadoutElement;

    
class MuonStation {
public:

   MuonStation(std::string stName,
               double m_Ssize, double m_Rsize, double m_Zsize,
               double m_LongSsize, double m_LongRsize, double m_LongZsize, int zi, int fi, 
               bool m_descratzneg);
   ~MuonStation();

  //Declaring the Message method for further use
  //MsgStream& msg( MSG::Level lvl ) const ;
  //Declaring the Method providing Verbosity Level
  //bool msgLevel( MSG::Level lvl ) ;

   inline int getPhiIndex() const; //!< a la AMDB
    
   inline int getEtaIndex() const;   //!< a la AMDB
    
   inline std::string getKey();
    
   inline std::string getStationType() const; //!< like BMS, T1F, CSL
    
   inline std::string getStationName() const; //!< like BMS5, T1F1, CSL1 
    
   inline void setEtaPhiIndices(int eta, int phi);  //!< a la AMDB

   inline double Rsize() const;    
   inline double Ssize() const;
   inline double Zsize() const;
   inline double LongRsize() const;
   inline double LongSsize() const;
   inline double LongZsize() const;
   double RsizeMdtStation() const;    
   double ZsizeMdtStation() const;    
   bool endcap() const;
   bool barrel() const;


   inline double xAmdbCRO() const;
   inline void setxAmdbCRO(double xpos);
    
   inline void setTransform(GeoAlignableTransform* xf);
   void setBlineFixedPointInAmdbLRS(double s0, double z0, double t0);
   HepGeom::Point3D<double>  getBlineFixedPointInAmdbLRS() const;
   HepGeom::Point3D<double>  getUpdatedBlineFixedPointInAmdbLRS() const;
   inline void setNativeToAmdbLRS(HepGeom::Transform3D xf);
   void setNominalAmdbLRSToGlobal(HepGeom::Transform3D xf);
   void setDeltaAmdbLRS(HepGeom::Transform3D xf);
   //!< set the delta transform in the amdb frame and update the geoModel Delta
 
   void setDelta_fromAline_forComp(int, double, double, double, double, double, double); 
   void setDelta_fromAline(double, double, double, double, double, double); 
   //!< set the delta transform in the amdb frame and update the geoModel Delta 

   //inline void addMuonReadoutElement(const MuonReadoutElement* a);
   //inline void addMuonReadoutElement(const MuonReadoutElement* a, int jobIndex);
   void addMuonReadoutElementWithAlTransf(const MuonReadoutElement* a, GeoAlignableTransform* ptrsf, int jobIndex);
   const MuonReadoutElement* getMuonReadoutElement(int jobIndex) const;
   GeoAlignableTransform*    getComponentAlTransf(int jobIndex) const;
   inline int nMuonReadoutElements() const;
   void clearCache() const;
   void refreshCache() const;
   void fillCache() const;
   void clearBLineCache() const;
   void fillBLineCache() const;
   void setBline(BLinePar * bline);
   inline GeoAlignableTransform* getGeoTransform() const;
   inline HepGeom::Transform3D  getTransform() const;
   inline HepGeom::Transform3D* getNativeToAmdbLRS() const;
   inline HepGeom::Transform3D  getAmdbLRSToGlobal() const;
   inline HepGeom::Transform3D* getNominalAmdbLRSToGlobal() const;
   inline double getALine_tras() const;
   inline double getALine_traz() const;
   inline double getALine_trat() const;
   inline double getALine_rots() const;
   inline double getALine_rotz() const;
   inline double getALine_rott() const;
   inline bool hasALines() const;	
   inline bool hasBLines() const;
   inline bool hasMdtAsBuiltParams() const;
   MdtAsBuiltPar* getMdtAsBuiltParams() const;
   void setMdtAsBuiltParams(MdtAsBuiltPar* xtomo);
 
private:

  //Declaring private message stream member.
   mutable bool m_firstRequestBlineFixedP;
   MsgStream* m_MsgStream;
   inline MsgStream& reLog() const;

   std::string m_statname;
   double m_Ssize, m_Rsize, m_Zsize, m_LongSsize, m_LongRsize, m_LongZsize, m_xAmdbCRO;
   bool m_descratzneg;
   int m_statPhiIndex;
   int m_statEtaIndex;
   std::string m_key;
   GeoAlignableTransform* m_transform;

   HepGeom::Transform3D* m_delta_amdb_frame;
   HepGeom::Transform3D* m_native_to_amdbl;
   HepGeom::Transform3D* m_amdbl_to_global; //nominal
   double m_rots;
   double m_rotz;
   double m_rott;  
   bool m_hasALines;
   bool m_hasBLines;
   mutable HepGeom::Point3D<double> m_BlineFixedPointInAmdbLRS;
   MdtAsBuiltPar* m_XTomoData;


   //std::map< int, const MuonReadoutElement* > *m_REinStation;  //!< keep track of the REs in this station 
   typedef std::pair<const MuonReadoutElement*, GeoAlignableTransform*>  pairRE_AlignTransf;
   std::map< int, pairRE_AlignTransf > *m_REwithAlTransfInStation;  //!< keep track of the REs in this station  
   //std::vector<const MuonReadoutElement *> * m_REinStation;  //!< keep track of the REs in this station  

   MuonStation & operator=(const MuonStation &right);
   MuonStation(const MuonStation&);
};

int MuonStation::getPhiIndex() const
  {return m_statPhiIndex;}
int MuonStation::getEtaIndex() const
  {return m_statEtaIndex;}
std::string MuonStation::getStationType() const
  {return m_statname.substr(0,3);}
std::string MuonStation::getStationName() const
  {return m_statname;}
void MuonStation::setEtaPhiIndices(int eta, int phi)
  {
    m_statEtaIndex = eta;
    m_statPhiIndex = phi;
  }

void MuonStation::setTransform(GeoAlignableTransform* xf)
  {
    m_transform = xf;
  }

GeoAlignableTransform*
MuonStation::getGeoTransform() const
  {return m_transform;}

HepGeom::Transform3D
MuonStation::getTransform() const
  {return m_transform->getTransform();}

std::string MuonStation::getKey() 
  {
    return m_key;
  }

double MuonStation::Rsize() const{return m_Rsize;}    
double MuonStation::Ssize() const{return m_Ssize;}
double MuonStation::Zsize() const{return m_Zsize;}
double MuonStation::LongRsize() const{return m_LongRsize;}
double MuonStation::LongSsize() const{return m_LongSsize;}
double MuonStation::LongZsize() const{return m_LongZsize;}

double MuonStation::xAmdbCRO() const
  {return m_xAmdbCRO;}

void MuonStation::setxAmdbCRO(double xpos)
  {m_xAmdbCRO = xpos;}

void MuonStation::setNativeToAmdbLRS(HepGeom::Transform3D xf)
  {
    if (m_native_to_amdbl == NULL) m_native_to_amdbl = new HepGeom::Transform3D(xf);
    else  *m_native_to_amdbl = xf; 
  }

HepGeom::Transform3D*
MuonStation::getNativeToAmdbLRS() const
  {return m_native_to_amdbl;}

HepGeom::Transform3D*
MuonStation::getNominalAmdbLRSToGlobal() const
  {return m_amdbl_to_global;}

HepGeom::Transform3D 
MuonStation::getAmdbLRSToGlobal() const
  {return (*m_amdbl_to_global)*(*m_delta_amdb_frame);}

/* void MuonStation::addMuonReadoutElement(const MuonReadoutElement* a, int jobIndex) */
/* { */
/* //     std::cout<<"MuonStation with indices: "<<getStationType() */
/* // 	     <<" Jzz/Jff = "<<getEtaIndex()<<"/"<<getPhiIndex() */
/* // 	     <<" adding RE with IobIndex = "<<jobIndex<<std::endl; */
/*     (*m_REinStation)[jobIndex]=a; */
/* } */

MsgStream& MuonStation::reLog() const {return *m_MsgStream;} 

int MuonStation::nMuonReadoutElements() const
  {return m_REwithAlTransfInStation->size();}

double MuonStation::getALine_tras() const
  {return (*m_delta_amdb_frame)[0][3];}
double MuonStation::getALine_traz() const
  {return (*m_delta_amdb_frame)[1][3];}
double MuonStation::getALine_trat() const
  {return (*m_delta_amdb_frame)[2][3];}
double MuonStation::getALine_rots() const
  {return m_rots;/*return (*_delta_amdb_frame)[2][1];*/}
double MuonStation::getALine_rotz() const
  {return m_rotz;/*return (*_delta_amdb_frame)[0][2];*/}
double MuonStation::getALine_rott() const
  {return m_rott;/*return (*_delta_amdb_frame)[1][0];*/}
bool MuonStation::hasALines() const
  {return m_hasALines;}
bool MuonStation::hasBLines() const
  {return m_hasBLines;}
bool MuonStation::hasMdtAsBuiltParams() const
  {
    if (m_XTomoData==NULL) return false;
    return true;
  }
    
               
} // namespace MuonGM

#endif
