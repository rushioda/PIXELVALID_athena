/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CscCalibMonToolBase.cxx
// PACKAGE:  MuonCalibMonitoring  
//
// AUTHOR:  Caleb Parnell-Lampen <lampen@physics.arizona.edu>
//
// ********************************************************************

#include <sstream>


#include "CscCalibMonToolBase.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ToolHandle.h"

using namespace std;

    CscCalibMonToolBase::CscCalibMonToolBase(const std::string & type, const std::string & name, const IInterface* parent)
: ManagedMonitorToolBase(type, name, parent), 
    m_detStore(NULL),
    m_storeGate(NULL),
    m_debuglevel(false),  
    m_verboselevel(false),
    m_log( msgSvc(), name ),
    m_maxHashId(0),
    m_maxChamId(0),
    m_numBad(0), 
    m_expectedChamberLayer(2),
    m_detailedHashIds(NULL),
    m_onlyExpectPrecisionHashIds(false),
    m_hashName("hash_overview"), 
    m_hashTitle(""), 
    m_allChan1dName("all_chan_1d"),
    m_allChan1dTitle("(View of All Channels)"),
    m_allChan2dName("all_chan_2d"),
    m_allChan2dTitle("(View of All Channels)"),
    m_chamProfName("profile_all_sectors"),
    m_chamProfTitle(""), 
    m_layHistName("overview"), 
    m_layHistTitle(""), 
    m_chamSummHistName("spectrum"),
    m_chamSummHistTitle(""), 
    m_chamHistName("overview"),
    m_chamHistTitle(""), 
    m_laySummHistName("spectrum"), 
    m_laySummHistTitle(""), 
    m_histCol(kAzure +1),
    //m_histCol(kOrange -9),
    m_histColAlert(kRed),
    m_monGroupVec(NULL),
    m_muon_mgr(NULL),
    m_cscIdHelper(NULL),
    m_cscCoolSvc(NULL),
    m_statDbColl(NULL)
{
   
    declareProperty("MakeAllChanHashOverviewHists",m_makeHashHists=true);  //Histograms showing a parameter for allchannels
    declareProperty("MakeAllChan1dHists",m_makeAllChan1dHists=true);  //Histograms showing a parameter for allchannels
    declareProperty("MakeAllChan2dHists",m_makeAllChan2dHists=true);  //Histograms showing a parameter for allchannels
    declareProperty("MakeLayerOverviewHists",m_makeLayHists = true);
    declareProperty("MakeLayerValueViewHists", m_makeLaySummHists = true);
    declareProperty("MakeChamberProfiles", m_makeChamProfs = true);
    declareProperty("MakeChamberValueViewHists", m_makeChamSummHists =true);
    declareProperty("MakeChamberOverviewHists", m_makeChamHists =true);
    declareProperty("MaxDetailedChannels",m_maxDetailedChannels = -1); //-1 = all that show problems
    declareProperty("CscCalibResultKey", m_calibResultKey = "");        //TDS key, defaulted in derived class
    declareProperty("GetDebugForAllChannels", m_doAllDetailed = false);
    declareProperty("DoBadDetailed", m_doBadDetailed = false, "Store histograms for channels marked as bad");
    declareProperty("DoStatDb", m_doStatDb = true);
}

CscCalibMonToolBase::~CscCalibMonToolBase()
{
    m_log << MSG::INFO << " deleting CscCalibMonToolBase " << endreq;
}
/*-----------------------------------------------------*/
StatusCode CscCalibMonToolBase::initialize()
{
  // init message stream -  Part 1: Get the messaging service, print where you are
  m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
  m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached
  m_verboselevel = (m_log.level() <= MSG::VERBOSE); // save if threshold for verbose printout reached

  m_log << MSG::INFO << "CscCalibMonToolBase : in initialize()" << endreq;
  StatusCode sc;

  // get cscIdHelper through this convoluted process
  sc = service("DetectorStore", m_detStore);
  if ( !sc.isSuccess() ) {
    m_log << MSG::FATAL << "DetectorStore Service not found !" << endreq;
    return StatusCode::FAILURE;
  }

  sc = service("StoreGateSvc", m_storeGate);
  if ( !sc.isSuccess() ) {
    m_log << MSG::FATAL << "StoreGate Service not found !" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_detStore->retrieve(m_muon_mgr);
  if (!sc.isSuccess()) {
    m_log << MSG::FATAL << "Could not find the MuonGeoModel Manager" << endreq;
    return StatusCode::FAILURE;
  }
  else {if (m_debuglevel) m_log << MSG::DEBUG << "Found the MuonGeoModel Manager " << endreq;} 

  sc = m_detStore->retrieve(m_cscIdHelper,"CSCIDHELPER");
  if (!sc.isSuccess()) { m_log << MSG::FATAL << "Cannot get CscIdHelper" << endreq; return StatusCode::FAILURE; }
  else { if (m_debuglevel) m_log << MSG::DEBUG << " Found the CscIdHelper. " << endreq; }

  if (StatusCode::SUCCESS!=service("MuonCalib::CscCoolStrSvc",m_cscCoolSvc))
  {
    m_log << MSG::FATAL<< "Cannot get CscCoolStrSvc" << endreq;
    return StatusCode::FAILURE;
  }

  //m_generic_path_csccalibmonitoring = "Muon/MuonCalibrationMonitoring/CSC";
  m_generic_path_csccalibmonitoring = "MUON_CSC";

      
  //Find valid hashes
  m_log << MSG::DEBUG << "Expected chamber layer " 
    << m_expectedChamberLayer << endreq;
  m_log << MSG::DEBUG << "Constructing list of expected chamber layers" << endreq;

  //Loop through ids to find out what hash range we're working on, and to 
  //initialize histograms.
  IdContext chanContext = m_cscIdHelper->channel_context();
  vector<Identifier> ids = m_cscIdHelper->idVector();
  vector<Identifier>::const_iterator chamItr = ids.begin();
  vector<Identifier>::const_iterator chamEnd = ids.end();
  m_maxHashId = 0;
  m_maxChamId = 0;
  for(; chamItr != chamEnd; chamItr++)
  {
    IdentifierHash chamberHash;
    m_cscIdHelper->get_module_hash(*chamItr,chamberHash);
    if(chamberHash > m_maxChamId)
      m_maxChamId = chamberHash;

    vector<Identifier> stripVect;
    m_cscIdHelper->idChannels(*chamItr,stripVect);
    vector<Identifier>::const_iterator stripItr = stripVect.begin();
    vector<Identifier>::const_iterator stripEnd = stripVect.end();
    for(;stripItr != stripEnd; stripItr++)
    {
      IdentifierHash stripHash;
      m_cscIdHelper->get_channel_hash(*stripItr,stripHash);
      bool measuresPhi = m_cscIdHelper->measuresPhi(*stripItr);

      //Find maximum hash   
      if((unsigned int)stripHash > m_maxHashId)
        m_maxHashId = (int)stripHash;

      if(m_expectedChamberLayer 
          == (unsigned int)m_cscIdHelper->chamberLayer(*stripItr) 
        )
      {
        m_log << MSG::VERBOSE << "hash " << (int)stripHash << " is expected"<< endreq; 
        m_expectedHashIdsAll.insert(stripHash);
        if(!measuresPhi){
          m_log << MSG::VERBOSE << "hash " << (int)stripHash << " is prec and expected"<< endreq; 
          m_expectedHashIdsPrec.insert(stripHash);
        }
      }
      else
        m_log << MSG::VERBOSE << "hash " << (int)stripHash << " is NOT expected (Not a bug, just populating list)."<< endreq; 
    }//End strip loop
  }//End chamber loop     

  //m_detailedHashIds determines which hashes we're going to look at detailed histograms for.

  m_detailedHashIds = new bool[m_maxHashId+1];
  for(unsigned int chanItr =0; chanItr <= m_maxHashId; chanItr++)
    m_detailedHashIds[chanItr] = false;

  //This needs to be initialized before we book any hist Collecitons 
  m_log <<MSG::INFO << "Initializing ManagedMonitorToolBase" << endreq;
  ManagedMonitorToolBase::initialize().ignore();  // Ignore the checking code;


  return StatusCode::SUCCESS;
}//end initialize()

StatusCode CscCalibMonToolBase::finalize()
{
  delete [] m_detailedHashIds;
  delete m_statDbColl;

  ManagedMonitorToolBase::finalize();    
  return StatusCode::SUCCESS;
}//end finalize()


StatusCode CscCalibMonToolBase::bookHistCollection(HistCollection * histCollection, 
    string dataTypeName, string dataTypeTitle, string categoryName, string categoryTitle, 
    string axisLabel, int numBins, float lowBound, float highBound, string parDir,  
    uint16_t toSkip)
{        
  //toSkip bitmask:
  //00000000 do all
  //00000001 skip all channels hash view 
  //00000010 skip chamberAverage
  //00000100 skip layer overview
  //00001000 skip sector summary 
  //00010000 skip layer summary
  //00100000 skip AllChannels 1d view
  //00100000 skip AllChannels 2d view
  //01000000 skip csc spectra
  //10000000 skip sector overview
  StatusCode sc = StatusCode::SUCCESS;

  //Make sure last character isn't a "/"
  std::string endDir = "";
  if(parDir != "")
    endDir = "/" + parDir;

  //Some universal directories
  std::string allChambersDirectory = "/GeneralCscHists";

  //Set directory structure used by CscCalibMonToolBase
  /*
     m_hashPath          = m_generic_path_csccalibmonitoring + allChambersDirectory + "/AllChanHash" + endDir;
     m_allChan2dPath    = m_generic_path_csccalibmonitoring + allChambersDirectory + "/AllChan2d" + endDir;
     m_chamProfPath       = m_generic_path_csccalibmonitoring + allChambersDirectory + "/SectorProfiles" + endDir;
     m_layHistPath       = m_generic_path_csccalibmonitoring + "/EXPERT/" + parDir + "Layers";
     m_chamSummHistPath  = m_generic_path_csccalibmonitoring +  "/EXPERT/" + parDir + "ChamberValueSpectrum";
     m_laySummHistPath   = m_generic_path_csccalibmonitoring +  "/EXPERT/" + parDir + "LayerValueSpectrum";
   */
  bool allGood = true;


  string nameStart =  dataTypeName ;
  if(categoryName != "" )
    nameStart += "_" + categoryName;

  string titleStart = categoryTitle + " " + dataTypeTitle;
  string yaxis= "", xaxis = "";

  if(m_debuglevel) m_log << MSG::DEBUG << "In bookHistCollection for " << nameStart
    << " series." << endreq;

  if(!((toSkip>>6) &0x1)){
    histCollection->cscSpec.resize(2,NULL);
    for(int measuresPhi =0; measuresPhi <=1; measuresPhi++) {
      string name = "h_" + nameStart+ "_" + (measuresPhi ? "phi" : "eta") + "_spectrum" ;
      string title = titleStart +" " + (measuresPhi ? "Phi Strips" : "Eta Strips") + " spectrum";
      TH1F * specHist = new TH1F(name.c_str(), title.c_str(), numBins, lowBound, highBound);
      specHist->GetYaxis()->SetTitle("Counts");
      specHist->GetXaxis()->SetTitle(axisLabel.c_str());
      string specPath = getFullPath( getGeoPath(), "OverviewSpectra", parDir);
      MonGroup monGroup( this, specPath, run, ATTRIB_MANAGED);
      sc = monGroup.regHist(specHist);

      specHist->SetFillColor(m_histCol);

      if(!sc.isSuccess())
      {
        m_log << MSG::ERROR << "failed to register " << name << endreq;
        allGood = false;
      }       
      else
        histCollection->cscSpec[measuresPhi] = specHist; 
    }


  }

  if(m_makeHashHists && !(toSkip & 0x1))
  {
    string name = "h_" + nameStart + "_" + m_hashName;
    string title = categoryTitle + " " + dataTypeTitle + " "  + m_hashTitle;
    yaxis = axisLabel;
    xaxis = "Channel Hash ID";
    TH1F * hashHist = new TH1F(name.c_str(), title.c_str(), m_maxHashId+1,0,m_maxHashId+1);
    hashHist->GetXaxis()->SetTitle(xaxis.c_str());
    hashHist->GetYaxis()->SetTitle(yaxis.c_str());
    hashHist->SetFillColor(m_histCol);

    string hashPath = getFullPath( getGeoPath(), "FullViewHash", parDir);
    MonGroup monGroup( this, hashPath, run, ATTRIB_MANAGED);
    if(m_debuglevel) m_log << MSG::DEBUG << "Registering " << name << endreq;
    sc = monGroup.regHist(hashHist);
    if(!sc.isSuccess())
    {
      m_log << MSG::ERROR << "failed to register " << name << endreq;
      allGood = false;
    }       
    else
      histCollection->hashHist = hashHist;

  }
  //All channels 1d view
  if(m_makeAllChan1dHists && !((toSkip>>6) & 0x1))
  {

    string name = "h_" + nameStart + "_" + m_allChan1dName;
    string title = categoryTitle + " " + dataTypeTitle + " "  + m_allChan1dTitle;
    yaxis = axisLabel;
    xaxis = "Chamber";
    int nxbins  = 26146;// 4 layers, 192 channels each, with 32 chambers + 2 "extra" chambers" 
    float nxmin = -16; // -1  (for EC)
    float nxmax = 16; // 1 (for EA)

    TH1F * allChan1dHistX = new TH1F((name+"X").c_str(), (title+ " - Precision strips").c_str(),
        nxbins,nxmin,nxmax);
    allChan1dHistX->GetXaxis()->SetTitle(xaxis.c_str());
    allChan1dHistX->GetYaxis()->SetTitle(yaxis.c_str());

    string allChan1dPath = getFullPath( getGeoPath(), "FullView1d", parDir);
    MonGroup monGroup( this, allChan1dPath, run, ATTRIB_MANAGED );
    if(m_debuglevel) m_log << MSG::DEBUG << "Registering " << name << endreq;
    sc = monGroup.regHist(allChan1dHistX);
    if(!sc.isSuccess())
    {
      m_log << MSG::ERROR << "failed to register " << name << endreq;
      allGood = false;
    }       
    else
      histCollection->allChan1dHistX = allChan1dHistX;

    nxbins = 6562;

    TH1F * allChan1dHistY = new TH1F((name+"Y").c_str(), (title + " - Transverse strips").c_str(),
        nxbins,nxmin,nxmax);
    allChan1dHistY->GetXaxis()->SetTitle(xaxis.c_str());
    allChan1dHistY->GetYaxis()->SetTitle(yaxis.c_str());

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering " << name << endreq;
    sc = monGroup.regHist(allChan1dHistY);
    if(!sc.isSuccess())
    {
      m_log << MSG::ERROR << "failed to register " << name << endreq;
      allGood = false;
    }       
    else
      histCollection->allChan1dHistY = allChan1dHistY;


  }

  //All channels 2d view
  if(m_makeAllChan2dHists && !((toSkip>>6) & 0x1))
  {

    string name = "h_" + nameStart + "_" + m_allChan2dName;
    string title = categoryTitle + " " + dataTypeTitle + " " + m_allChan2dTitle;
    //yaxis = "sector  + 0.2 * (layer - 1) + 0.1";     
    yaxis = "Sector/Layer";
    xaxis = "Strip Number (Negative for Transverse Strips)";

    int nxbins ;
    float nxmin;
    float nxmax;
    int nybins ;
    float nymin;
    float nymax;
    if(histCollection->ignoreY){
      nxbins  = 193;  // 192 bins for precision + 1 extra
      nxmin = 0.; // -1 -> -48 (for transverse)
      nxmax = 193.; // 1 -> 192 (for precision)
      nybins  = 175;  // 32 chambers (16 per side x 5 layers per chamber) + 5 extra
      nymin = -17.; //
      nymax = 18.;  //
    }
    else{
      nxbins  = 242;  // 192 bins for precision, 48 for transverse strips + 2 extra
      nxmin = -49.; // -1 -> -48 (for transverse)
      nxmax = 193.; // 1 -> 192 (for precision)
      nybins  = 175;  // 32 chambers (16 per side x 5 layers per chamber) + 5 extra
      nymin = -17.; //
      nymax = 18.;  //
    }

    TH2F * allChan2dHist = new TH2F(name.c_str(), title.c_str(),
        nxbins,nxmin,nxmax,nybins,nymin,nymax);
    allChan2dHist->GetXaxis()->SetTitle(xaxis.c_str());
    allChan2dHist->GetYaxis()->SetTitle(yaxis.c_str());

    string allChan2dPath = getFullPath( getGeoPath(), "FullView2d", parDir);
    MonGroup monGroup( this, allChan2dPath, run, ATTRIB_MANAGED);
    if(m_debuglevel) m_log << MSG::DEBUG << "Registering " << name << endreq;
    sc = monGroup.regHist(allChan2dHist);
    if(!sc.isSuccess())
    {
      m_log << MSG::ERROR << "failed to register " << name << endreq;
      allGood = false;
    }       
    else
      histCollection->allChan2dHist = allChan2dHist;

  }

  if(m_makeChamProfs && !((toSkip >> 1) & 0x1) )
  {
    string name = "h_" + nameStart + "_" + m_chamProfName;
    string title = titleStart + " " + m_chamProfTitle;
    yaxis = "Average " + axisLabel;
    xaxis = "Sector * eta";
    int numSectors = 16;
    TProfile * chamProf= new TProfile(name.c_str(), title.c_str(), 
        numSectors + 1, -16, 17); //hard coding these because its annoying otherwise
    chamProf->GetXaxis()->SetTitle(xaxis.c_str());
    chamProf->GetYaxis()->SetTitle(yaxis.c_str());

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering " << name << endreq;
    string path = getFullPath( getGeoPath(), "Profiles", parDir);
    MonGroup monGroup( this, path, run, ATTRIB_MANAGED);
    sc = monGroup.regHist(chamProf);
    if(!sc.isSuccess())
    {
      m_log << MSG::ERROR << "failed to register " << name << endreq;
      allGood = false;
    }
    else 
      histCollection->chamProf = chamProf;
  }
  if(m_makeLayHists && !((toSkip >> 2) & 0x1) )
  {
    string namePrefix = nameStart + "_" + m_layHistName;
    string titlePrefix = titleStart + " " + m_layHistTitle;
    yaxis = axisLabel;
    xaxis = "Channel";

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering set with prefix" 
      << namePrefix << endreq;
    histCollection->layHistVect = new std::vector<TH1F*>();
    sc = bookLayHists("LayerView", parDir, *histCollection->layHistVect,namePrefix, titlePrefix, "Channel", xaxis
        ,true, histCollection->ignoreY);
    if(!sc.isSuccess())
    {
      m_log << MSG::ERROR << "failed to register " << namePrefix << " (layer histograms) " 
        << endreq;
      allGood = false;
    }
  }

  if(m_makeChamSummHists && !((toSkip >> 3) & 0x1) )
  {
    string namePrefix = nameStart + "_" + m_chamSummHistName;
    string titlePrefix = titleStart + " " + m_chamSummHistTitle;
    yaxis = "";
    xaxis = axisLabel;

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering set with prefix" 
      << namePrefix << endreq;
    histCollection->chamSummVect = new std::vector<TH1F*>();
    sc = bookChamHists("SecSpectrum", parDir,*histCollection->chamSummVect,namePrefix, titlePrefix, xaxis,yaxis,
        false,histCollection->ignoreY,numBins,lowBound,highBound);
    if(!sc.isSuccess())
    {
      m_log << MSG::ERROR << "failed to register " << namePrefix 
        << " (chamber summary histograms) " << endreq;
      allGood = false;
    }
  }

  if(m_makeChamHists && !((toSkip >> 7) & 0x1) )
  {
    string namePrefix = nameStart + "_" + m_chamHistName;
    string titlePrefix = titleStart + " " + m_chamHistTitle;
    yaxis = "";
    xaxis = axisLabel;

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering set with prefix" 
      << namePrefix << endreq;
    histCollection->chamHistVect = new std::vector<TH1F*>();
    sc = bookChamHists("SectorView", parDir,*histCollection->chamHistVect,namePrefix, titlePrefix, xaxis,yaxis,
        true,histCollection->ignoreY);
    if(!sc.isSuccess())
    {
      m_log << MSG::ERROR << "failed to register " << namePrefix 
        << " (chamber summary histograms) " << endreq;
      allGood = false;
    }
  }

  if(m_makeLaySummHists && !((toSkip >> 4) & 0x1) )
  {
    string namePrefix = nameStart + "_" + m_laySummHistName;
    string titlePrefix = titleStart + " " + m_laySummHistTitle;
    yaxis = "";
    xaxis = axisLabel;

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering set with prefix" 
      << namePrefix << endreq;
    histCollection->laySummVect = new std::vector<TH1F*>();
    sc = bookLayHists("LaySpectrum", parDir, *histCollection->laySummVect,
        namePrefix, titlePrefix, xaxis,yaxis,
        false,histCollection->ignoreY,numBins,lowBound,highBound);
    //cout << "Back from bookLayHists, histCollection->laySummVect is " 
    //    << ((int)(histCollection->laySummVect != NULL)) << endl;

    if(!sc.isSuccess())
    {
      m_log << MSG::ERROR << " Failed to register " << namePrefix << " (Layer summary hists) "
        << endreq;
      allGood = false;
    }
  }
  if(allGood)
    return StatusCode::SUCCESS;
  else 
    return StatusCode::FAILURE;
}//end bookHistCollection

StatusCode CscCalibMonToolBase::bookLayHists(std::string histTypeDir, std::string parDir,/* MonGroup &monGroup,*/ std::vector<TH1F*> & histVector, string namePrefix, string titlePrefix,
    string xaxis, string yaxis, 
    bool chanView, bool ignoreY,  unsigned int numBins,
    float lowBound, float highBound)
{
  //Book a set of histograms, one for each layer.
  //chanView = true ignores numBins and bounds and makes x-axis based on channel ids
  //ignore Y = true skips transverse direction strips 

  //unsigned int numStrips;
  int stationSize, stationPhi, stationEta, sector;
  string stationName;
  string orientationName = "prec";
  string orientationTitle = "Precision Direction";

  int numHists = 32 * ( (ignoreY) ? 4 : 8);
  histVector.resize(numHists,NULL); 

  if(m_debuglevel) m_log << MSG::DEBUG << "Allocated space for " << numHists << " histograms"
    << endreq;

  vector<Identifier> ids = m_cscIdHelper->idVector();
  vector<Identifier>::const_iterator chamItr = ids.begin();
  vector<Identifier>::const_iterator chamEnd = ids.end();
  for(; chamItr != chamEnd; chamItr++)
  {
    IdentifierHash chamHash;
    m_cscIdHelper->get_module_hash(*chamItr,chamHash);
    if(m_debuglevel) m_log << MSG::DEBUG << "Booking histograms for chamber with hash " 
      << (int)chamHash << endreq;

    stationSize = m_cscIdHelper->stationName(*chamItr); 
    stationName = m_cscIdHelper->stationNameString(stationSize);
    stationPhi = m_cscIdHelper->stationPhi(*chamItr);
    stationEta = m_cscIdHelper->stationEta(*chamItr); 
    sector = getSector(stationPhi,stationSize);
    for(unsigned int orientationItr = 0; orientationItr < 2; orientationItr++)
    {
      if(orientationItr ==1)
      {
        if(ignoreY)  //skip non precision strips
          continue; 
        if(chanView) //Base bin labels on orientation
        {
          numBins = 48;
          highBound = 49;
          lowBound = 1;
        }
        orientationName = "trans";
        orientationTitle = "Transverse Direction";
      }
      else
      {
        if(chanView)
        {
          numBins = 192;
          highBound = 193;
          lowBound = 1;
        }
        orientationName = "prec";
        orientationTitle = "Precision Direction";
      }

      for(unsigned int layItr = 1; layItr <= 4; layItr++)
      {
        if(m_verboselevel) 
          m_log << MSG::VERBOSE << "Storing a hist on orientation " << orientationItr 
            << " With highbound/lowbound/nbins "  
            << highBound << "/" << lowBound << "/" << numBins << endreq;

        stringstream nameStream;
        nameStream.setf(ios::right, ios::adjustfield);
        nameStream << namePrefix; 
        nameStream << "_" << orientationName <<  "_eta_";
        nameStream << ((stationEta == 1) ? "1" : "0");
        nameStream << "_sector_" << setw(2) << setfill('0') <<  sector;
        nameStream << "_layer_"  << layItr; 

        stringstream titleStream;
        titleStream << titlePrefix << ", " << orientationTitle; 
        titleStream << ", Sector " << sector;
        titleStream <<", Eta " << stationEta;
        titleStream << ", Layer " << layItr;

        if(m_verboselevel) 
          m_log << MSG::VERBOSE << "Storing " << nameStream.str() << " with title "
            << titleStream.str() << "On orientation " << orientationItr << " With highbound/lowbound/nbins "  << highBound << "/" << lowBound << "/" << numBins << endreq;

        TH1F* hist = new TH1F(nameStream.str().c_str(), titleStream.str().c_str(), 
            numBins, lowBound, highBound);
        hist->GetXaxis()->SetTitle(xaxis.c_str());
        hist->GetYaxis()->SetTitle(yaxis.c_str());
        hist->SetFillColor(m_histCol);

        int layIndex = getLayIndex(orientationItr,stationEta,sector,layItr);

        //Lets 
        std::string geoPath = getGeoPath(stationEta, sector,  layItr, orientationItr);
        std::string path = getFullPath(geoPath, histTypeDir, parDir);
        MonGroup monGroup( this, path ,run, ATTRIB_MANAGED);


        if(!monGroup.regHist(hist).isSuccess())
        { 
          m_log << MSG::FATAL << "Failed to register " << nameStream.str() 
            << " with layIndex" << layIndex << endreq; 
          return StatusCode::FAILURE;    
        }
        else
        {
          if(m_debuglevel) m_log << MSG::DEBUG 
            << "Succesfully registered histogram with layIndex " 
              << layIndex  <<" and name " << hist->GetName() << endreq;
          histVector[layIndex] = hist;
        }
      }//end layer loop
    }//End orientation loop        
  }//End chamber loop
  return StatusCode::SUCCESS;
}//end bookLayHists

//NOTE: Chanview mode isn't implemented!
StatusCode CscCalibMonToolBase::bookChamHists(std::string histTypeDir, std::string parTypeDir, std::vector<TH1F*>&  histVector, 
    string namePrefix, string titlePrefix,
    string xaxis, string yaxis, bool chanView, 
    bool ignoreY, unsigned int numBins, 
    float lowBound, float highBound)
{
  //When doing channel view (i.e. one value per channel, also refered to as SectorView)
  //we use pre-defined nbins, lower bound, and upper bound for histograms for obvious reasons
  static const int chanViewNBinsX = 800;
  static const double chanViewLowBoundX = .5;
  static const double chanViewHighBoundX = 800.5;

  static const int chanViewNBinsY = 200;
  static const double chanViewLowBoundY = .5;
  static const double chanViewHighBoundY = 200.5;


  //Book a set of channel view histograms.
  //unsigned int numStrips;
  int stationSize, stationPhi, stationEta, sector;
  string stationName;
  string orientationName = "prec";
  string orientationTitle = "Precision Direction";

  //histVector = new std::vector<TH1F*>; 
  int numHists = (ignoreY) ? 32 : 64; //32 chambers, 2 orientations
  histVector.resize(numHists,NULL);

  vector<Identifier> ids = m_cscIdHelper->idVector();
  vector<Identifier>::const_iterator chamItr = ids.begin();
  vector<Identifier>::const_iterator chamEnd = ids.end();
  for(; chamItr != chamEnd; chamItr++)
  {
    IdentifierHash chamHash;
    m_cscIdHelper->get_module_hash(*chamItr,chamHash);
    m_log << MSG::DEBUG << "Booking histograms for chamber with hash " 
      << (int)chamHash << endreq;

    stationSize = m_cscIdHelper->stationName(*chamItr); //50
    stationPhi = m_cscIdHelper->stationPhi(*chamItr);
    stationEta = m_cscIdHelper->stationEta(*chamItr); 
    sector = getSector(stationPhi,stationSize);

    for(unsigned int orientationItr = 0; orientationItr < 2; orientationItr++)
    {
      if(orientationItr ==1)
      {
        if(ignoreY)
          continue; //skip non precision strips.
        orientationName = "trans";
        orientationTitle = "Transverse Direction";
        if(chanView){
          numBins = chanViewNBinsY;
          lowBound = chanViewLowBoundY;
          highBound = chanViewHighBoundY;
        }
      }
      else
      {
        orientationName = "prec";
        orientationTitle = "Precision Direction";
        if(chanView){
          numBins = chanViewNBinsX;
          lowBound = chanViewLowBoundX;
          highBound = chanViewHighBoundX;
        }
      }

      stringstream nameStream;
      nameStream.setf(ios::right, ios::adjustfield);
      nameStream << namePrefix << "_" << orientationName <<  "_eta_" 
        << ((stationEta == 1) ? "1" : "0") << "_sector_" 
        << setw(2) << setfill('0')
        << sector ; 

      stringstream titleStream;

      titleStream << titlePrefix << ", " << orientationTitle 
        << ", Eta " << stationEta
        << ", Sector " << sector; 

      TH1F* hist = new TH1F(nameStream.str().c_str(), titleStream.str().c_str()
          ,numBins, lowBound, highBound);
      hist->GetXaxis()->SetTitle(xaxis.c_str());
      hist->GetYaxis()->SetTitle(yaxis.c_str());
      hist->SetFillColor(m_histCol);

      int chamIndex = getChamIndex(orientationItr,stationEta,sector);

      if(m_debuglevel) m_log << MSG::DEBUG << "Registering histogram with name " 
        << hist->GetName() << " and chamIndex " << chamIndex << endreq;

      histVector[chamIndex] = hist;

      string path = getFullPath(getGeoPath(stationEta,sector), histTypeDir, parTypeDir);

      MonGroup monGroup( this, path ,run, ATTRIB_MANAGED);
      if (!monGroup.regHist(hist).isSuccess())
      {
        m_log << MSG::WARNING <<"monGroup.regHist(hist) failed for "<< path <<endreq;
      }
    }//End orientation loop        
  }//End chamber loop
  if(m_debuglevel) m_log << MSG::DEBUG << "Exiting bookChamHists" << endreq;
  return StatusCode::SUCCESS;
}//end bookChamHists

StatusCode CscCalibMonToolBase::bookHistograms()
{
  if (m_debuglevel) m_log << MSG::DEBUG << "CscalibMonTool : in bookHistograms()" << endreq;
  if (newEventsBlock){}
  if (newLumiBlock){}
  if (newRun)
  {

    //Plot status words. This could be useful to include in all output files, so we'll put
    //it in the base class here 
    if(m_doStatDb){
      //This is a histogram collection both derived classes will probably like

      m_statDbColl = new HistCollection(m_maxHashId +1);
      string statDbName = "stat_cool";
      string statDbTitle = "Status Word Value From COOL";
      string statDbAxisLabel = "Stat Word Value";
      unsigned int statDbNumBins = 8;
      float statDbMin = -0.5;
      float statDbMax = 7.5;
      string statDbSubDir = "StatCool";

      StatusCode sc = bookHistCollection(m_statDbColl, statDbName, statDbTitle, "", "",
          statDbAxisLabel, statDbNumBins, statDbMin, statDbMax, statDbSubDir);
      if(!sc.isSuccess()){
        m_log << MSG::WARNING << " Failed to register db stat hists!" << endreq;
      }

      //Loop through channels retrieving status words
      for(unsigned int chanItr = 0; chanItr <= m_maxHashId; chanItr++){
        if(m_expectedHashIdsAll.count(chanItr)) {
          uint8_t statWord;
          if(!(m_cscCoolSvc->getStatus(statWord, chanItr)).isSuccess()){
            m_log << MSG::WARNING << "Failed to retrieve statword for hashId " 
              << chanItr << endreq;
          }
          m_statDbColl->data[chanItr] = (float)statWord;
        }
      }//end chanItr llop
      if(!copyDataToHists(m_statDbColl).isSuccess())
      {
        m_log << MSG::WARNING << "copyDataToHists failed in bookHistograms.";
      }
    }
  }
  return StatusCode::SUCCESS;
}//end bookHistograms

StatusCode CscCalibMonToolBase::fillHistograms()
{

  if (m_debuglevel) m_log << MSG::DEBUG << "CscCalibMonToolBase :: in fillHistograms()" << endreq;

  return StatusCode::SUCCESS;
}//end fillHistograms

/*---------------------------------------------------------------------------------------------------------*/
StatusCode CscCalibMonToolBase::procHistograms()
{

  if (m_debuglevel) m_log << MSG::DEBUG << "CscCalibMonToolBase : in procHistograms()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  if(endOfEventsBlock){}
  if(endOfLumiBlock){}
  if(endOfRun)
  {
    const DataHandle<CscCalibResultContainer> calibContainer;
    sc = m_storeGate->retrieve(calibContainer, m_calibResultKey);
    if( !sc.isSuccess())
    {
      m_log << MSG::ERROR << " Cannot retrieve container with name " << m_calibResultKey <<  endreq;
      return StatusCode::RECOVERABLE;
    }

    if(m_debuglevel) m_log << MSG::DEBUG << "There are " << calibContainer->size() << " parameters to monitor" << endreq; 

    CscCalibResultContainer::const_iterator parItr = calibContainer->begin();
    CscCalibResultContainer::const_iterator parEnd = calibContainer->end();
    for(;parItr != parEnd ; parItr++)
      handleParameter(*parItr);

    postProc();
  }
  return StatusCode::SUCCESS;
}//end procHistograms  


/*--retrieve histos: Overloaded in derived classes to collect interesting histograms from calibration alg--------*/
StatusCode CscCalibMonToolBase::postProc()
{
  if (m_debuglevel) m_log << MSG::DEBUG << "CscCalibMonToolBase : in postProc()" << endreq;

  //This method should be overloaded in a derived class

  return StatusCode::SUCCESS;
}// end postProc

/*---------------------------------------------------------*/
StatusCode CscCalibMonToolBase::checkHists(bool /* fromFinalize */)
{

  if (m_debuglevel) m_log << MSG::DEBUG << "CscCalibMonToolBase : in checkHists()" << endreq;

  return StatusCode::SUCCESS;
}

/*--preProcParameter : should be re-defined in derived class to select what histograms and parameters
  should go into procParameter*/
StatusCode CscCalibMonToolBase::handleParameter(const CscCalibResultCollection*)
{
  if (m_debuglevel) m_log << MSG::DEBUG << "CscCalibMonToolBase : in preProcParameter" << endreq;     
  return StatusCode::SUCCESS;
}


/*--procParameter : processes parameter  based upon the "set" structs which will be 
  defined in the user's preProcParameter.

  The "processing" done here is simply to retrieve all the new new parameter values, retrieve their
  corresponding last values from COOL, and find their difference. The value, the old value, and the
  difference are all put into arrays. These arrays should be passed by the user in handleParameter to copyDataToHists.

  Also inside procParameter, we fill the badHist if we see any problems. We also report any channels missing from the input values.
 */
StatusCode CscCalibMonToolBase::procParameter(const CscCalibResultCollection *parVals, 
    ProcSetupInfo *procParameterInput)
{
  if(m_debuglevel)
  {
    m_log << MSG::DEBUG << "Entering proc parameter for " << parVals->parName() << ". Max diff " << procParameterInput->maxDiff << ".";
    if (procParameterInput->doChi2)
      m_log << " Will retrieve chi2 with expected max of " << procParameterInput->chi2Max;
    m_log << endreq;
  }

  //Start with expected channels, remove whenever one is found, leftover are
  //missing channels
  set<int> missingChannels = procParameterInput->expectedChannels;

  //--Cycle through values and fill histograms
  int numFailures = 0, maxFailures = 10;
  CscCalibResultCollection::const_iterator chanItr = parVals->begin();
  CscCalibResultCollection::const_iterator chanEnd = parVals->end();
  for(;chanItr != chanEnd; chanItr++)
  {
    const CscCalibResult * chan = *chanItr;

    const int hashId = chan->hashId();
    const float val = chan->value();
    const  float error = chan->error();
    float oldVal = 0;

    //This channel isn't missing
    missingChannels.erase(hashId);

    //--Compare measured value with an expected value---------------------
    if(procParameterInput->dbName != "")
    {
      //Get expected value from database
      if(!(m_cscCoolSvc->getParameter(oldVal, procParameterInput->dbName, hashId)).isSuccess())
      {
        numFailures++;
        m_log << MSG::WARNING << "CscCalibMonToolBase :  Failed to retrieve parameter"
          << " for channel " << hashId  
          << " from COOL database. Continuing with COOL value = 0" 
          << endreq;
        if(numFailures==maxFailures)
        {
          m_log << MSG::FATAL << "CscCalibMonToolBase : "
            << maxFailures << " failed retrievals. Quiting. " << endreq;
          return StatusCode::FAILURE;
        }
      }
    }
    else
    {
      //No value in database, compare to provided expected value
      oldVal = procParameterInput->expectedVal;
    }

    float diff = val - oldVal;

    bool isBad = false;
    if( abs(diff) > procParameterInput->maxDiff)
    {
      m_log << MSG::INFO << "CscCalibMonToolBase : Possible problem! " << parVals->parName()
        << " measurement of " << val << " varies by " << diff
        << " from expected value of " << oldVal << " on channel with hash Id "
        << hashId << ". Specified maximum variance is " << procParameterInput->maxDiff << endreq;
      isBad = true;
      procParameterInput->badHist->Fill(procParameterInput->badBin); //Too high a difference
    }

    float chi2_ndf = 0;
    if(procParameterInput->doChi2)
    {
      chi2_ndf = chan->chi2()/chan->ndf();
      if(chi2_ndf > procParameterInput->chi2Max)
      {
        procParameterInput->badHist->Fill(procParameterInput->chi2BadBin);
        isBad = true;
      }
    }

    //If there is a bad channel, specify that we want details about it.
    if( isBad && ( (int)m_numBad  <= m_maxDetailedChannels || m_maxDetailedChannels < 0 ) ) 
    {
      if(!m_detailedHashIds[hashId])
      {    
        m_numBad++;
        m_detailedHashIds[hashId] = true; //Look closer at this hash id (used in derived class)
      }
    }

    (*procParameterInput->vals)[hashId] = val;
    if((procParameterInput->errors->size()))
      (*procParameterInput->errors)[hashId] = error;
    if(procParameterInput->oldVals)
      (*procParameterInput->oldVals)[hashId] = oldVal;
    if(procParameterInput->diffs)
      (*procParameterInput->diffs)[hashId] = diff;
    if(procParameterInput->doChi2)
      (*procParameterInput->chi2s)[hashId] = chi2_ndf;
    //--Print out values channel-by channel sorted by Hash Id-------------------
  }//End chanItr loop

  if(missingChannels.size() !=0)
  {
    set<int>::const_iterator chanItr = missingChannels.begin();
    set<int>::const_iterator chanEnd = missingChannels.end();
    for(;chanItr != chanEnd; chanItr++)
    {
      if(procParameterInput->missingChans != NULL)
      {
        //If we haven't already noticed that this channel is missing, 
        //increment missing bad bin in badHist.
        if(!procParameterInput->missingChans->GetBinContent(*chanItr+1))
          procParameterInput->badHist->Fill(procParameterInput->missingBadBin);
        //Always fill missingChans histogram 
        procParameterInput->missingChans->Fill(*chanItr);

      }
    }
  }
  return StatusCode::SUCCESS;
}


/*--makeLayHists : Takes data vector filled in procParameter and fills layer histograms
 */
StatusCode CscCalibMonToolBase::copyDataToHists(HistCollection * histCollection)
{

  //Determine what histograms we'll make:
  bool doSpec         = true; //(histCollection->cscSpec != NULL);
  bool doHash         = (histCollection->hashHist != NULL         && m_makeHashHists);
  bool doAllChan1d    = (histCollection->allChan1dHistX != NULL  && histCollection->allChan1dHistY != NULL   && m_makeAllChan1dHists);
  bool doAllChan2d    = (histCollection->allChan2dHist != NULL    && m_makeAllChan2dHists);
  bool doLayChan      = (histCollection->layHistVect != NULL      && m_makeLayHists);
  bool doLaySummary   = (histCollection->laySummVect != NULL      && m_makeLaySummHists);
  bool doChamAvg      = (histCollection->chamProf != NULL         && m_makeChamProfs);
  bool doChamChan     = (histCollection->chamSummVect != NULL     && m_makeChamHists);
  bool doChamSummary  = (histCollection->chamSummVect != NULL     && m_makeChamSummHists);
  bool doErrors       = (histCollection->errors.size() != 0);    //Changed to a vector, so this
  //                                                                is best way to check at moment

  if(m_debuglevel) 
    m_log << MSG::DEBUG 
      << "Copying data to hist collection, doing: All Channels (hash view):" 
        << (int)doHash  
            << "All Channels (1d view) " << (int)doHash  
            << "All Channels (2d view) " << (int)doHash  
            << " layer overview " << (int)doLayChan
            << " layer spectrum " << (int)doLaySummary
            << " sector prof " << (int)doChamAvg
            << " chamber overview " << (int)doChamChan
            << " chamber spectrum " << (int)doChamSummary
            << endreq;

  //cout << "TEST! hash " << (int)(histCollection->hashHist != NULL) << endl;
  //cout << "TEST! layHist " << (int)(histCollection->layHistVect != NULL) << endl;
  //cout << "TEST! laySumm " << ((histCollection->laySummVect != NULL) ? 1 : 0 )<< endl;

  //For shorter lines:
  std::vector<float> & data = histCollection->data; 
  std::vector<float> & errors = histCollection->errors;

  //Loop through all channels, and copy relevant data from channel to histogram.
  vector<Identifier> ids = m_cscIdHelper->idVector();
  vector<Identifier>::const_iterator chamItr = ids.begin();
  vector<Identifier>::const_iterator chamEnd = ids.end();
  for(; chamItr != chamEnd; chamItr++)
  {
    IdentifierHash chamHash;
    m_cscIdHelper->get_module_hash(*chamItr,chamHash);
    if(m_debuglevel) m_log << MSG::DEBUG
      << "Copying data to histograms for chamber with hash" << (int)chamHash 
        << endreq;

    unsigned int stationSize = m_cscIdHelper->stationName(*chamItr); //51 = large, 50 = small

    unsigned int stationPhi = m_cscIdHelper->stationPhi(*chamItr);
    int stationEta = m_cscIdHelper->stationEta(*chamItr); 
    unsigned int sector = getSector(stationPhi,stationSize);
    int sectorIndex = sector * stationEta; //Histogram will go from -16 to +16. Bin 0 ignored.



    vector<Identifier> stripVect;
    m_cscIdHelper->idChannels(*chamItr,stripVect);
    vector<Identifier>::const_iterator stripItr = stripVect.begin();
    vector<Identifier>::const_iterator stripEnd = stripVect.end();
    for(;stripItr != stripEnd; stripItr++)
    { 
      unsigned int chamberLayer = m_cscIdHelper->chamberLayer(*stripItr);
      if(chamberLayer != 2)
        continue;
      int measuresPhi = m_cscIdHelper->measuresPhi(*stripItr);
      if(histCollection->ignoreY && measuresPhi)
        continue;

      IdentifierHash stripHash;
      m_cscIdHelper->get_channel_hash(*stripItr,stripHash);
      unsigned int layer = m_cscIdHelper->wireLayer(*stripItr);    
      unsigned int strip = m_cscIdHelper->strip(*stripItr);
      float secLayer = (((float)stationEta*sector) + 0.2 * ((float)layer - 1) + 0.1);     
      float datum = data.at(stripHash);

      if(m_verboselevel) m_log << MSG::VERBOSE 
        << "Filling all chan for sector: " << sector 
          << "\tlayer: " << layer << "\tstationEta: " << stationEta
          << "\tsecLayer: " << secLayer 
          << "\tdata: " << datum << endreq;

      if(doSpec) {
        histCollection->cscSpec[measuresPhi]->Fill(datum);
      }
      if(doHash)
      {
        //copy data array directly to hash histogram with all channels
        histCollection->hashHist->Fill(stripHash,datum);
      }

      if(doAllChan1d)
      {
        if(m_verboselevel) m_log << MSG::VERBOSE 
          << "Filling 1d chan for sector: " << sector 
            << "\tlayer: " << layer << "\tstationEta: " << stationEta;
        if(m_verboselevel) m_log << MSG::VERBOSE << "\tsecLayer: " << secLayer 
          << "\tdata: " << datum << endreq;
        if(measuresPhi){

          double modifiedStripNum = 
            (stationEta *sector)
            + (layer-1)*.25
            + ((strip-1) * .25 /(48))
            ;

          if(m_verboselevel) m_log << MSG::VERBOSE  << "Modified strip num: " 
            << modifiedStripNum << endreq;
          //copy data array directly to 1d histogram with all channels
          histCollection->allChan1dHistY->Fill(
              modifiedStripNum, datum);
        } else {
          double modifiedStripNum = 
            (stationEta *sector)
            + (layer-1)*.25
            + ((strip-1) * .25 /(192 ))
            ;

          //copy data array directly to 1d histogram with all channels
          histCollection->allChan1dHistX->Fill(
              modifiedStripNum, datum);
        }
      }

      if(doAllChan2d)
      {
        if(m_verboselevel) m_log << MSG::VERBOSE 
          << "Filling 2d chan for sector: " << sector 
            << "\tlayer: " << layer << "\tstationEta: " << stationEta;
        if(m_verboselevel) m_log << MSG::VERBOSE << "\tsecLayer: " << secLayer 
          << "\tdata: " << datum << endreq;

        //copy data array directly to 2d histogram with all channels
        int modifiedStripNum = strip * (measuresPhi ? -1 : 1);
        histCollection->allChan2dHist->Fill(
            modifiedStripNum, secLayer, abs(datum));
      }

      int layIndex = getLayIndex(measuresPhi,stationEta,sector,layer);
      int chamIndex = getChamIndex(measuresPhi,stationEta,sector);
      if(doLayChan) 
      {
        //Copy values from data array for this layer to layer histogram
        (*(histCollection->layHistVect))[layIndex]->SetBinContent(strip,datum);
        if(doErrors)
          (*(histCollection->layHistVect))[layIndex]->SetBinError(strip,errors.at(stripHash));
      }
      if(doLaySummary)
      {
        //Histogram values for this layer
        (*(histCollection->laySummVect))[layIndex]->Fill(datum);

      } 
      if(doChamAvg)
      {  
        //Keeps track of average value for each chamber.
        histCollection->chamProf->Fill(sectorIndex,datum);
      }
      if(doChamSummary)
      {
        //histogram values for this chamber
        (*(histCollection->chamSummVect))[chamIndex]->Fill(datum);
      }
      if(doChamChan)
      {
        int shiftedStrip = strip + (layer-1)*( measuresPhi ? 50 : 200);
        //Copy values from data array for this chamber to chamber histogram
        (*(histCollection->chamHistVect))[chamIndex]->SetBinContent(shiftedStrip,datum);
        if(doErrors)
          (*(histCollection->chamHistVect))[chamIndex]->SetBinError(shiftedStrip,errors.at(stripHash));
      }
    }//end strip loop        
  }//end chamber loop

  //probably should put this in its own function at some point

  return StatusCode::SUCCESS;
}

int CscCalibMonToolBase::getLayIndex(int measuresPhi, int stationEta, int sector, int layer)
{
  int layIndex = measuresPhi*32*4
    + ((stationEta ==1) ? 16*4 : 0)
    + (sector-1)*4 
    + layer -1;
  return layIndex;
}

int CscCalibMonToolBase::getChamIndex(int measuresPhi, int stationEta, int sector)
{
  int chamIndex = measuresPhi*32
    + ((stationEta ==1) ? 16 : 0)
    + (sector-1); 
  return( chamIndex);
}

int CscCalibMonToolBase::getSector(int stationPhi, int stationSize)
{
  //stationSize : 50 = CSS, 51 = CSL
  //Sector 1 : large with athena's stationPhi = 1 (onlinePhi = 0)
  //Sector 2 : small with athenas stationPhi = 1 (onlinePhi = 0)
  //Sector 3 : large with athenas stationPhi = 2 (onlinePhi = 1)
  //etc...
  return(2*stationPhi + 50 - stationSize);
  //return(2*stationPhi + stationSize - 51);
}


std::string CscCalibMonToolBase::getEndCap(int eta)
{
  if(eta ==1)
    return "A";
  if(eta == -1)
    return "C";   
  return "ERROR";
}

//Geo path creates a directory structure for a histogram based upon where it is in the CSC geometry.
//The user fills the data down as far as they like. For example, just filling in eta and sector will provide
//a path for a sector-wide histogram.
//This provides a centralized location to change the layout of the root output file.
std::string CscCalibMonToolBase::getGeoPath( int eta, int sector,  int wireLayer, int measuresPhi, int channel )
{
  stringstream ss;

  static string histStr = "/_hists";
  static string errorDir = "/ERROR";

  ss << setfill('0');//so we can have uniform numbers


  if( eta == -9999 ) //CSC Wide
  {
    ss << "/CscOverview";
    return ss.str(); 
  }

  string endCap = getEndCap(eta);
  if(endCap == "ERROR")
  {
    m_log << MSG::ERROR << "Eta " << eta << " is invalid. " << endreq;
    return errorDir;
  }

  ss << "/EndCap" << endCap;

  if(sector == -9999) //Endcap Histogram
  {
    ss << histStr;
    return ss.str();
  }

  if(sector <1 || sector > 16)
  {
    m_log <<MSG::ERROR << "Sector " << sector << " is invalid. " << endreq;
    return errorDir;
  }

  ss << "/Sector" << setw(2) << sector;

  if(wireLayer == -9999)//Sector (chamber) wide histrogram
  {
    ss << histStr;
    return ss.str();
  }

  if(wireLayer < 1 || wireLayer > 4)
  {
    m_log << MSG::ERROR << "WireLayer " << wireLayer << " is invalid." << endreq;
    return errorDir;
  }

  ss << "/Layer" << wireLayer;

  if(measuresPhi == -9999)//Layer wide histrogram
  {
    ss << histStr;
    return ss.str();
  }

  if(measuresPhi < 0 || measuresPhi > 1)
  {
    m_log << MSG::ERROR << "MeasuresPhi " << measuresPhi << " is invalid." << endreq;
    return errorDir; 
  }

  ss << (measuresPhi ? "/Phi" : "/Eta");
  if(channel == -9999) //LayerDirection wide histogram
  {
    /* Channel histograms don't work (too much memory used by meta data tree), so layer is now our most detailed channel. No need for histStr.
       ss << histStr;*/
    return ss.str();
  }

  if( channel < 1 || (measuresPhi && channel > 48)  || (!measuresPhi && channel > 192)) 
  {
    m_log << MSG::ERROR << "Channel " << channel << " is invalid for a " << (measuresPhi ? "phi layers" : "eta layer");
    return errorDir;
  }


  ss << "/Channel" << setw( measuresPhi ? 2 : 3 ) << channel;

  return ss.str();
}//end getGeoPath 


//Produces a full path for a histogram to be placed.
std::string CscCalibMonToolBase::getFullPath(std::string geoPath, std::string histTypeDir, std::string parTypeDir)
{
  stringstream ss; 

  ss << m_generic_path_csccalibmonitoring << geoPath;

  if(histTypeDir != "") 
    ss << "/" << histTypeDir;

  if(parTypeDir != "")
    ss <<  "/" << parTypeDir;

  return ss.str();
}
