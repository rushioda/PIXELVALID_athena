/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ResPullValidation_h
#define ResPullValidation_h


class string;
template <class ht> class MultiHisto;
class RMSProfile;
class TH1D;
class TDirectory;

namespace PixelValid{

class ResPullValidation{

public:
	ResPullValidation(std::string etaORphi = "phi", std::string pullORres = "res",
			std::string cosmicORbeam = "beam");
	virtual ~ResPullValidation(){};
	bool Fill(int Layer, double GeVTrkPt, double Angle,
			double ClusterSize, double Residual);
	int Analyze(TDirectory *ref_file = 0);
	int Write();
	int Read();
	
	RMSProfile *GetPtProfile(int i);
	RMSProfile *GetAngleProfile(int i);
	TH1D *GetHistogram(int i);
	
private:

	std::string datatype;
	std::string respullname;
	std::string anglename;
	MultiHisto<RMSProfile> *PtProfile;
	MultiHisto<RMSProfile> *AngleProfile;
	MultiHisto<TH1D> *Histogram;

	std::string globaldirname;
	


};

}


#endif // #ifdef ResPullValidation_h
