/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TForwardElectronIsEMSelector.cxx
PACKAGE:  PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools

 *********************************************************************/

#include "TForwardElectronIsEMSelector.h"
#include <cmath>

Root::TForwardElectronIsEMSelector::TForwardElectronIsEMSelector(const char* name) :
	TSelectorToolBase(name),
	asg::AsgMessaging(std::string(name)), 
	isEMMask(0), //All will pass if not specified
	m_isEM(~0),
	/** @brief cluster eta range */
	m_cutPositionBinEta_ForwardElectron(0),    
	/** @brief matching to photon (not necessarily conversion--the name is historical) */
	//m_cutPositionVxp_ForwardElectron(0),    
	/** @brief cluster leakage o the hadronic calorimeter */
	m_cutPositionSECONDLAMBDA_ForwardElectron(0),
	/** @brief Et <0  */
	m_cutPositionLATERAL_ForwardElectron(0),
	/** @brief energy ratio in 2nd sampling (e.g E237/E277) */
	m_cutPositionLONGITUDINAL_ForwardElectron(0),
	/** @brief energy ratio in 2nd sampling (e.g E233/E237) */
	m_cutPositionCELLMAXFRAC_ForwardElectron(0),
	/** @brief width in the second sampling (e.g Weta2) */
	m_cutPositionSECONDR_ForwardElectron(0),
	/** @brief energy fraction in the third layer */
	m_cutPositionCENTERLAMBDA_ForwardElectron(0),
	/** @brief cluster eta range */
	m_cutNameBinEta_ForwardElectron("BinEta_ForwardElectron"),
	/** @brief matching to photon (not necessarily conversion--the name is historical) */
	//m_cutNameVxp_ForwardElectron("Vxp_ForwardElectron"),  
	/** @brief cluster leakage into the hadronic calorimeter */
	m_cutNameSECONDLAMBDA_ForwardElectron("SECONDLAMBDA_ForwardElectron"),
	/** @brief shower width weighted by distance from the maximum one */
	m_cutNameLATERAL_ForwardElectron("LATERAL_ForwardElectron"),  
	/** @brief difference between max and 2nd max in strips */
	m_cutNameLONGITUDINAL_ForwardElectron("LONGITUDINAL_ForwardElectron"),
	/** @brief B layer hit */
	m_cutNameCELLMAXFRAC_ForwardElectron("CELLMAXFRAC_ForwardElectron"),
	/** @brief number of Pixel hits */
	m_cutNameSECONDR_ForwardElectron("SECONDR_ForwardElectron"),
	/** @brief number of Pixel and SCT hits */
	m_cutNameCENTERLAMBDA_ForwardElectron("CENTERLAMBDA_ForwardElectron")
{
	m_cutPositionSECONDLAMBDA_ForwardElectron = 0;
	m_cutPositionLATERAL_ForwardElectron = 0;
	m_cutPositionLONGITUDINAL_ForwardElectron= 0;
	m_cutPositionCELLMAXFRAC_ForwardElectron = 0;
	m_cutPositionSECONDR_ForwardElectron = 0;
	m_cutPositionCENTERLAMBDA_ForwardElectron = 0;

}

// ====================================================================
Root::TForwardElectronIsEMSelector::~TForwardElectronIsEMSelector() 
{
	//
	// destructor
	//
}

//=============================================================================
// Initialize this selector tool
//=============================================================================
int Root::TForwardElectronIsEMSelector::initialize()
{

	int  sc(1);

	// --------------------------------------------------------------------------
	// Register the cuts and check that the registration worked:
	// NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...

	/** @brief cluster eta range, bit 0 */
	m_cutPositionBinEta_ForwardElectron = 
		m_accept.addCut(m_cutNameBinEta_ForwardElectron, "Electron within eta range");
	if (m_cutPositionBinEta_ForwardElectron < 0) sc = 0;

	/** @brief matching to photon (not necessarily conversion--the name is historical), but 1 */
	// m_cutPositionVxp_ForwardElectron = 
	//  m_accept.addCut(m_cutNameVxp_ForwardElectron, "Electron matches a photon with AR > LOOSE");
	// if (m_cutPositionVxp_ForwardElectron < 0) sc = 0;

	/** @brief cluster leakage into the hadronic calorimeter, bit 2 */
	m_cutPositionSECONDLAMBDA_ForwardElectron = 
		m_accept.addCut(m_cutNameSECONDLAMBDA_ForwardElectron, "secondLambda < Cut");
	if (m_cutPositionSECONDLAMBDA_ForwardElectron < 0) sc = 0;

	/** @brief et < 0 bit 3 */
	m_cutPositionLATERAL_ForwardElectron = 
		m_accept.addCut(m_cutNameLATERAL_ForwardElectron, "lateral <0  Cut"); 
	if (m_cutPositionLATERAL_ForwardElectron < 0) sc = 0;

	/** @brief energy ratio in 2nd sampling (i.e. E237/E277), bit 4 */
	m_cutPositionLONGITUDINAL_ForwardElectron = 
		m_accept.addCut(m_cutNameLONGITUDINAL_ForwardElectron, "longitudinal <Cut");
	if (m_cutPositionLONGITUDINAL_ForwardElectron < 0) sc = 0;

	/** @brief energy ratio in 2nd sampling (i.e. E233/E237), bit 5 */
	m_cutPositionCELLMAXFRAC_ForwardElectron = 
		m_accept.addCut(m_cutNameCELLMAXFRAC_ForwardElectron, "fracMax > Cut");
	if (m_cutPositionCELLMAXFRAC_ForwardElectron < 0) sc = 0;

	/** @brief width in the second sampling (i.e. Weta2), bit 6 */
	m_cutPositionSECONDR_ForwardElectron = 
		m_accept.addCut(m_cutNameSECONDR_ForwardElectron, "SecondR < Cut");
	if (m_cutPositionSECONDR_ForwardElectron < 0) sc = 0;

	/** @brief energy fraction in the third layer, bit 7 */
	m_cutPositionCENTERLAMBDA_ForwardElectron = 
		m_accept.addCut(m_cutNameCENTERLAMBDA_ForwardElectron, "CenterLambda < Cut");
	if (m_cutPositionCENTERLAMBDA_ForwardElectron < 0) sc = 0;

	if (sc == 0) {
		ATH_MSG_ERROR("Exceeded the number of allowed cuts in TForwardElectronIsEMSelector");
	}

	return sc;
}

const Root::TAccept& Root::TForwardElectronIsEMSelector::fillAccept() const
{ 
	for (int i = 0; i < 7; i++) {
		const unsigned int mask = (0x1 << i) & isEMMask;
		m_accept.setCutResult(i, (m_isEM & mask) == 0);
	}
	ATH_MSG_DEBUG("m_accept "<<m_accept);
	return m_accept;
}

//=============================================================================
// Calculate the actual accept of each cut individually.
//=============================================================================
const Root::TAccept& Root::TForwardElectronIsEMSelector::accept( 
		// eta position in second sampling
		float eta,
		// transverse energy in calorimeter (using eta position in second sampling)
		float nvtx,
		//////////////// - calo
		// E(3*3) in 2nd sampling   e233
		float secondLambda,
		// E(3*7) in 2nd sampling  e237
		float lateral,
		// transverse energy in 1st scintillator of hadronic calorimeter
		float longitudinal,
		// E(min) in strips
		float fracMax,
		// total shower width in 1st sampling
		float centerLambda,
		// fraction of energy reconstructed in the 3rd sampling
		float secondR) const 
{
	// Reset the cut result bits to zero (= fail cut)
	m_accept.clear();
	// -----------------------------------------------------------
	// Do the actual selection

	m_isEM = calcIsEm(eta,
			nvtx,
			secondLambda, // e233,
			lateral, //e237,
			longitudinal, ///ethad1,
			fracMax, //emin,
			centerLambda,
			secondR);

	return fillAccept();

}

// return the isem
//
unsigned int Root::TForwardElectronIsEMSelector::calcIsEm(
		// eta position in second sampling
		float eta,
		// transverse energy in calorimeter (using eta position in second sampling)
		float nvtx,
		//////////////// - calo
		// E(3*3) in 2nd sampling e233
		float secondLambda,
		// E(3*7) in 2nd sampling e237
		float lateral,
		// transverse energy in 1st scintillator of hadronic calorimeter
		float longitudinal,
		// E(min) in strips
		float fracMax, //emin,
		// total shower width in 1st sampling
		float centerLambda,
		// fraction of energy reconstructed in the 3rd sampling
		float secondR) const
{ 
	unsigned int iflag = calocuts_electrons(eta,
			nvtx,
			secondLambda, //e233,
			lateral, //e237,
			longitudinal, //ethad1,
			fracMax, //emin,
			centerLambda,
			secondR,
			0);

	return iflag;
}

// ======================================================================
unsigned int Root::TForwardElectronIsEMSelector::calocuts_electrons(
		// eta position in second sampling
		float eta,
		// transverse energy in calorimeter (using eta position in second sampling)
		float nvtx,
		// E(3*3) in 2nd sampling  233
		float secondLambda,
		// E(3*7) in 2nd sampling
		float lateral,
		// transverse energy in 1st scintillator of hadronic calorimeter
		float longitudinal,
		// E(min) in strips (emin)
		float fracMax,
		// total shower width in 1st sampling
		float centerLambda,
		// fraction of energy reconstructed in the 3rd sampling
		float secondR,
		// the starting isEM (should be 0 if new)
		unsigned int iflag
		) const
{
	// modifiy et when dealing with trigger
	// to be sure that it will take the correct bin (VD)
	std::vector<int> bins =FindNvtxEtaBin(nvtx,eta);
	int ibin_nvtx= bins.at(0);
	int ibin_eta= bins.at(1);
	int ibin_combined= bins.at(2);

	// check eta range 
	if (eta < 2.5) {  
		iflag |= (0x1 << egammaPID::BinEta_ForwardElectron);
		ATH_MSG_DEBUG("Fail eta"); 
	}

	// check if index is defined
	if (ibin_eta>=0 && ibin_nvtx>=0 &&  ibin_combined>=0) {

		// check secondR
		if (CheckVar(CutSECONDR_ForwardElectron,4)) {
			if (secondR >= CutSECONDR_ForwardElectron[ibin_combined]){
				iflag |= ( 0x1 << egammaPID::SECONDR_ForwardElectron);
				ATH_MSG_DEBUG("Fail SecondR "<<secondR<<" > "<<CutSECONDR_ForwardElectron[ibin_combined]); 
			} 
		}
		// check secondlambda
		if (CheckVar(CutSECONDLAMBDA_ForwardElectron,4)) {
			if (secondLambda >= CutSECONDLAMBDA_ForwardElectron[ibin_combined]){ 
				iflag |= ( 0x1 << egammaPID::SECONDLAMBDA_ForwardElectron);
				ATH_MSG_DEBUG("Fail SecondLambda "<<secondLambda<<" > "<<CutSECONDLAMBDA_ForwardElectron[ibin_combined]); 
			}
		}
		// ---------
		// check lateral
		if (CheckVar(CutLATERAL_ForwardElectron,4)) {
			if (lateral >= CutLATERAL_ForwardElectron[ibin_combined]){ 
				iflag |= ( 0x1 << egammaPID::LATERAL_ForwardElectron);
				ATH_MSG_DEBUG("Fail LATERAL "<<lateral<<" > "<<CutLATERAL_ForwardElectron[ibin_combined]); 
			}
		}
		// check longitudinal
		if (CheckVar(CutLONGITUDINAL_ForwardElectron,4)) {
			if (longitudinal >= CutLONGITUDINAL_ForwardElectron[ibin_combined]){ 
				iflag |= ( 0x1 << egammaPID::LONGITUDINAL_ForwardElectron);
				ATH_MSG_DEBUG("Fail Longitudinal "<<longitudinal<<" > "<<CutLONGITUDINAL_ForwardElectron[ibin_combined]); 
			}
		}
		// check fracMax
		if (CheckVar(CutCELLMAXFRAC_ForwardElectron,4)) {
			if (fracMax < CutCELLMAXFRAC_ForwardElectron[ibin_combined]){
				iflag |= ( 0x1 << egammaPID::CELLMAXFRAC_ForwardElectron);
				ATH_MSG_DEBUG("Fail fracMax "<<fracMax<<" < "<<CutCELLMAXFRAC_ForwardElectron[ibin_combined]); 
			}
		}
		// check CENTERLAMBDA
		if (CheckVar(CutCENTERLAMBDA_ForwardElectron,4)) {
			if (centerLambda >= CutCENTERLAMBDA_ForwardElectron[ibin_combined] ){
				iflag |= ( 0x1 << egammaPID::CENTERLAMBDA_ForwardElectron);
				ATH_MSG_DEBUG("Fail CENTERLAMBDA "<<centerLambda<<" > "<<CutCENTERLAMBDA_ForwardElectron[ibin_combined]); 
			}
		}
	}

	return iflag; 
}

std::vector<int> Root::TForwardElectronIsEMSelector::FindNvtxEtaBin(float nvtx, double eta) const 
{  
         //Try to figure out in which bin we belong
         int ibin_nvtx = -1;
	 // loop on nvtx range
	 if(CutVxp_ForwardElectron.size()>0){
	   for (unsigned int ibinNVTX=0;ibinNVTX<CutVxp_ForwardElectron.size();++ibinNVTX) {
	     if ( ibinNVTX == 0 ) {
	       if (nvtx < CutVxp_ForwardElectron[ibinNVTX] ) {
		 ibin_nvtx = ibinNVTX;
	       }
	     } 
	     else if ( ibinNVTX > 0 && ibinNVTX <CutVxp_ForwardElectron.size() ) {
	       if ( nvtx >= CutVxp_ForwardElectron[ibinNVTX-1] && 
		    nvtx < CutVxp_ForwardElectron[ibinNVTX] ) {
		 ibin_nvtx = ibinNVTX;
	       }
	     }
	     else if ( ibinNVTX == CutVxp_ForwardElectron.size() ) {
	       if ( nvtx >= CutVxp_ForwardElectron[ibinNVTX-1] ) {
		 ibin_nvtx = ibinNVTX;
	       }
	     }
	   }
	 }
	 int ibin_eta = -1;
	 // loop on eta range
	 for (unsigned int ibinEta=0;ibinEta<CutBinEta_ForwardElectron.size();++ibinEta) {
	   if ( ibinEta == 0 ){
	     if ( eta < CutBinEta_ForwardElectron[ibinEta] ) {
	       ibin_eta = ibinEta;
	     }
	   }
	   else if ( ibinEta > 0 && ibinEta <CutBinEta_ForwardElectron.size() ) {
	     if ( eta >= CutBinEta_ForwardElectron[ibinEta-1] && 
		  eta < CutBinEta_ForwardElectron[ibinEta] ) {
	       ibin_eta = ibinEta;
	     }
	   }
	   else if ( ibinEta == CutBinEta_ForwardElectron.size() ) {
	     if ( eta >= CutBinEta_ForwardElectron[ibinEta-1] ) {
	       ibin_eta = ibinEta;
	     }
	   }
	 }
	 
	 //int ibin_combined = ibin_nvtx*CutBinEta_ForwardElectron.size()+ibin_eta;
	 int ibin_combined = -1;
	 if(ibin_eta == 2 ){
	   ibin_combined = ibin_nvtx*ibin_eta-1;
	 }
	 else if(ibin_eta == 1 ){
	   ibin_combined = 2*(ibin_nvtx*ibin_eta-1);
	 }
	 else{
	   ATH_MSG_INFO(" Not posible "); 
	 }
	 //int ibin_combined = ibin_nvtx*ibin_eta-1;
	 // check the bin number
	 std::vector<int> bins{ibin_nvtx, ibin_eta,ibin_combined};
	 return bins;
}

// ==============================================================

template<typename T> 
bool Root::TForwardElectronIsEMSelector::CheckVar(const std::vector<T>& vec, int choice) const
{
	// check vector size
	// 0 : size should be 1
	// 1 : vs etaNB
	// 2 : vs etaTRTNB
	// 3 : vs etNB
	// 4 : vs combinedNB
	// 5 : vs combinedTRTNB

	// if size of vector is 0 it means cut is not defined
	if (vec.size() == 0) return false;

	unsigned int etaNB = CutBinEta_ForwardElectron.size();
	unsigned int etaTRTNB = 1;
	unsigned int etNB =  CutVxp_ForwardElectron.size();
	//unsigned int etTRTNB =  1;
	unsigned int combinedNB = 0;
	unsigned int combinedTRTNB = 0;

	if (etNB>1) {
		//combinedNB = etaNB * (etNB+1);
		combinedNB = (etaNB-1) * (etNB-1);
	}
	else {
		combinedNB = etaNB;
	}

	combinedTRTNB = etaTRTNB;
	
	// check if size is 1 (give choice 0)
	if (choice==0) {
		if ( vec.size() != 1) {
			ATH_MSG_ERROR("choice 0 vector size is " 
					<< vec.size() << " but needs 1"); 
			return false;      
		}
	}

	// check if size is etaNB
	if (choice==1) {
		if (vec.size() != etaNB ) {
			ATH_MSG_ERROR("choice 1 vector size is " 
					<< vec.size() << " but needs " 
					<< etaNB);
			return false;      
		}
	}
	// check if size is etaTRTNB
	if (choice==2) {
		if ( vec.size() != etaTRTNB ) {
			ATH_MSG_ERROR("choice 2 vector size is " 
					<< vec.size() << " but needs " 
					<< etaTRTNB);
			return false;      
		}
	}

	// check if size is etNB
	if (choice==3) {
		if ( vec.size() != etNB ) {
			ATH_MSG_ERROR("choice 3 vector size is " 
					<< vec.size() << " but needs " 
					<< etNB);
			return false;      
		}
	}

	// check if size is combinedNB 
	if (choice==4) {
		if ( vec.size() != combinedNB ) {
			ATH_MSG_ERROR("choice 4 vector size is " 
					<< vec.size() << " but needs " 
					<< combinedNB);
			return false;      
		}
	}

	// check if size is etaTRTNB
	if (choice==5) {
		if ( vec.size() != combinedTRTNB ) {
			ATH_MSG_ERROR("choice 5 vector size is " 
					<< vec.size() << " but needs " 
					<< combinedTRTNB);
			return false;      
		}
	}
	return true;
}

template bool Root::TForwardElectronIsEMSelector::CheckVar<float>  (const std::vector<float>& vec, int choice) const;
template bool Root::TForwardElectronIsEMSelector::CheckVar<int>  (const std::vector<int>& vec, int choice) const;
