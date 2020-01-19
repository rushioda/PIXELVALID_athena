/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EnergyCalculator.h
// Prepared 10-Apr-2002 Bill Seligman
// from code written by Jozsef Toth.
// 07-May-2003 AMS: now EnergyCalculator is not a singleton
// 02-July-2003 J.T. Charge collection added
// 20-July-2004 J.T. FieldMapVersion variable is added
// 25-May-2005 J.T. -calling sequence of GapAdj changed
//                  -new variables and function for reading and handling
//                    values of HV of power supplies
//                  - IonReco :for suppress signal because of ion recombination
//                  - DriftVelo: Walkowiak's formula for drift velocity
//  Sept-2006 J.T. - collect signal from the Barrette volume
//  Nov -2006 J.T. - fieldmap array structures changed,
//                 - lengths defined dinamically
//                 - field map for first/last fold and for Barrett volume are included
// May 2009 AMS move to namespace LArG4::EC
//              duplicated data members removed

#ifndef LArG4_EC_EnergyCalculator_H
#define LArG4_EC_EnergyCalculator_H

#include <string>
#include <stdexcept>

#include "CLHEP/Units/SystemOfUnits.h"

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArCalculatorSvcImp.h"
#include "LArG4Code/LArG4EnumDefs.h"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "GeoSpecialShapes/LArWheelCalculatorEnums.h"

#include "G4ThreeVector.hh"
#include "globals.hh"


class ILArCalibCalculatorSvc;
class LArG4BirksLaw;


namespace LArG4 {

  namespace EC {


    class EnergyCalculator : public LArCalculatorSvcImp
    {
    public:

      EnergyCalculator(const std::string& name, ISvcLocator *pSvcLocator);
      // Update handlers
      void CorrectionTypeHandler(Property&);
      void SolidTypeHandler(Property&);

      virtual StatusCode initialize() override final;
      virtual StatusCode finalize() override final;

      /////////////////////////////////////////////
      // The interface for LArVCalculator.
      virtual G4float OOTcut() const override final { return m_OOTcut; }
      virtual G4bool Process(const G4Step*, std::vector<LArHitData>&) const override final;

      // Check if the current hitTime is in-time
      virtual G4bool isInTime(G4double hitTime) const override final
      {
        return !(hitTime > m_OOTcut); //FIXME should we be checking the absolute value of hitTime here?
      }


    private:

      //G4int    m_compartment; -> made local for FindIdentifier functions

      G4bool (EnergyCalculator::*m_Process_type) (const G4Step*, std::vector<LArHitData>&) const;
      G4double (EnergyCalculator::*m_GetHV_Value_type) (const G4ThreeVector &p, G4double /*Barret_PhiStart*/) const;
      G4double (EnergyCalculator::*m_GetGapSize_type) (const G4ThreeVector &p) const;
      G4double (EnergyCalculator::*m_distance_to_the_nearest_electrode_type) (const G4ThreeVector &p, G4double /*Barret_PhiStart*/) const;

      G4bool Process_Default(const G4Step*, std::vector<LArHitData>&) const;
      G4bool Process_Barrett(const G4Step*, std::vector<LArHitData>&) const;
      G4bool FindIdentifier_Default(const G4Step *, std::vector<LArHitData>&, G4ThreeVector &, G4ThreeVector &) const;
      G4bool FindIdentifier_Barrett(const G4Step *, G4double, std::vector<LArHitData>&, G4ThreeVector &, G4ThreeVector &) const;
      G4bool FindDMIdentifier_Barrett(const G4Step* step, std::vector<LArHitData>&) const;
      G4bool GetCompartment_Barrett(G4ThreeVector,G4double,G4double,G4double,
                                    G4int &, G4int &) const;
      G4double GetHV_Value_Default(const G4ThreeVector& p, G4double /*Barret_PhiStart*/) const {
        return GetHV_Value(p);
      }
      G4double GetHV_Value_Barrett(const G4ThreeVector& p, G4double PhiStartOfPhiDiv) const;
      G4double GetGapSize_Default(const G4ThreeVector &p) const {
        return GetGapSize(p);
      }
      G4double GetGapSize_Barrett(const G4ThreeVector &p) const;
      G4int GetPhiGap_Barrett(const G4ThreeVector &p, G4double PhiStartOfPhiDiv) const;
      G4double distance_to_the_nearest_electrode_Default(const G4ThreeVector &p, G4double /*Barret_PhiStart*/) const {
        return distance_to_the_nearest_electrode(p);
      }
      G4double distance_to_the_nearest_electrode_Barrett(const G4ThreeVector &p, G4double Barret_PhiStart) const;

      ServiceHandle<ILArCalibCalculatorSvc> m_supportCalculator;

      static void SetConst_OuterBarrett(void); // used only for initialization
      static void SetConst_InnerBarrett(void); // used only for initialization
      G4bool GetVolumeIndex(const G4Step *, G4int &, G4int &) const;
      static       G4bool   s_SetConstOuterBarrett;   // used as const after init
      static       G4bool   s_SetConstInnerBarrett;   // used as const after init
      static const G4double s_LongBarThickness;//       =   20. *mm;
      static const G4double s_ColdCorrection;//         =1.0036256;
      static const G4double s_StripWidth;//             =3.*mm/ColdCorrection;
      static const G4double s_KapGap;//                 =1.*mm/ColdCorrection;
      static const G4double s_EdgeWidth;//              =1.*mm;
      static const G4double s_DistOfEndofCuFromBack;//  =22.77*mm/ColdCorrection;
      static const G4double s_DistOfStartofCuFromBack;//=31.*mm; // frontface of the barrette
      static const G4double s_ZmaxOfSignal;// DistOfStartofCuFromBack - DistOfEndofCuFromBack + EdgeWidth;
      static       G4double s_RefzDist; // = dElecFocaltoWRP+dWRPtoFrontFace+WheelThickness+  // used as const after
      // +dWRPtoFrontFace+ LongBarThickness                 // initialization
      // -DistOfEndofCuFromBack

      static const G4double s_S3_Etalim[21];
      static const G4double s_Rmeas_outer[50];
      static const G4double s_Zmeas_outer[2];
      static       G4double s_S3_Rlim[21]; // used as const after init
      static       G4double s_rlim[50];    // used as const after init
      static       G4double s_zlim[4];     // used as const after init

      UnsignedIntegerProperty m_corrProp;
      EnergyCorrection_t m_correction_type;

      G4double (EnergyCalculator::*m_ecorr_method) (G4double, const G4ThreeVector&, const G4ThreeVector&, G4double /*Barret_PhiStart*/) const;
      G4double dummy_correction_method(G4double e, const G4ThreeVector&, const G4ThreeVector&,
                                       G4double /*Barret_PhiStart*/) const {
        return e;
      }
      G4double GapAdjustment_old(G4double, const G4ThreeVector&, const G4ThreeVector&, G4double /*Barret_PhiStart*/) const;
      G4double GapAdjustment    (G4double, const G4ThreeVector&, const G4ThreeVector&, G4double /*Barret_PhiStart*/) const;
      G4double GapAdjustment_E  (G4double, const G4ThreeVector&, const G4ThreeVector&, G4double /*Barret_PhiStart*/) const;
      G4double GapAdjustment_s  (G4double, const G4ThreeVector&, const G4ThreeVector&, G4double /*Barret_PhiStart*/) const;
      G4double GapAdjustment__sE (G4double, const G4ThreeVector&, const G4ThreeVector&, G4double /*Barret_PhiStart*/) const;
      G4double CalculateChargeCollection(G4double, const G4ThreeVector&, const G4ThreeVector&, G4double /*Barret_PhiStart*/) const;
      G4double CalculateChargeCollection1(G4double, const G4ThreeVector&, const G4ThreeVector&, G4double /*Barret_PhiStart*/) const;

      G4double m_GApower;  // used as const after init
      inline G4double GApower() const { return m_GApower; };

      // **************************************************************************
      //Declaration of variables,functions  for charge collection
      //J.T
      // **************************************************************************


      //variables specific for wheel geometry
      struct WheelGeometry {
        G4int PhiGapNumber, PhiHalfGapNumber;
        G4int HalfWaveNumber, SignofZinHalfWave, SignofSlopeofHalfWave;
        G4double SinPhiGap, CosPhiGap, ZinHalfWave;
        G4double HalfEleThickness;
        WheelGeometry() :
          PhiGapNumber(0),
          PhiHalfGapNumber(0),
          HalfWaveNumber(0),
          SignofZinHalfWave(0),
          SignofSlopeofHalfWave(0),
          SinPhiGap(0),
          CosPhiGap(0),
          ZinHalfWave(0),
          HalfEleThickness(0)
        {}
      };


      G4double m_ElectrodeFanHalfThickness;   // used as const after init
      G4double m_FanEleThicknessOld;          // used as const after init
      G4double m_FanEleFoldRadiusOld;         // used as const after init
      G4double m_FanAbsThickness;             // used as const after init
      G4double m_FanEleThickness;             // used as const after init
      G4double m_WaveLength;                  // used as const after init

      G4int m_NofPhiSections;                 // used as const after init
      G4int m_NumberOfElectrodesInPhiSection; // used as const after init

      inline G4double ElectrodeFanHalfThickness() const { return m_ElectrodeFanHalfThickness; };
      inline G4double FanEleThicknessOld() const { return m_FanEleThicknessOld; };
      inline G4double FanEleFoldRadiusOld() const { return m_FanEleFoldRadiusOld; };
      inline G4double FanAbsThickness() const { return m_FanAbsThickness; };
      inline G4double FanEleThickness() const { return m_FanEleThickness; };
      inline G4double WaveLength() const { return m_WaveLength; };

      inline G4int NofPhiSections() const { return m_NofPhiSections; };
      inline G4int NumberOfElectrodesInPhiSection() const { return m_NumberOfElectrodesInPhiSection; };

      //variables specific for Efield calculation

      static       G4bool   s_FieldMapsRead;   // used as const after init
      static       G4String s_FieldMapVersion; // used as const after init

      static const G4double s_GridSize;
      static const G4double s_AverageGap;
      static const G4double s_inv_AverageGap;

      struct Fold_Efield_Map{
        G4bool    FieldMapPrepared;
        G4double*  FieldMap;           // [NumberOfRadialLayers][ZYWeight][MaxNofPoints];
        G4double* MinZofLayer;        //these are limits of the
        G4double* MaxZofLayer;        //area where  the FieldMap can
        G4double* MinYofLayer;        //be used for interpolation
        G4double* MaxYofLayer;
        G4int*    NofColofLayer;      // a column is parallel to y
        G4int*    NofRowofLayer;      // a row is  parallel to z
        G4int*    NofPointsinLayer;
        G4int*    pLayer;
      };

      struct Wheel_Efield_Map {G4bool          FieldMapPrepared;
        G4int           NumberOfRadialLayer;
        G4double*       RadiusOfLayers;
        G4double*       FoldinAngleOfLayers;
        G4double*       HalfLArGapSizeOfLayers;
        Fold_Efield_Map* Fold;
        Fold_Efield_Map Fold0;
        Fold_Efield_Map Fold1;
        G4double  GridShift;
      };

      static Wheel_Efield_Map s_ChCollInner,s_ChCollOuter; // used as const after init
      Wheel_Efield_Map* m_ChCollWheelType;             // used as const after init

      inline const Wheel_Efield_Map* ChCollWheelType() const { return m_ChCollWheelType; };

      struct FoldArea {
        const Fold_Efield_Map*  ChCollFoldType;
        G4int PointFoldMapArea;
        FoldArea() :
          ChCollFoldType(0),
          PointFoldMapArea(0)
        {}
      };

      //G4double PhiStartOfPhiDiv;

      void CreateArrays(Wheel_Efield_Map &, G4int);
      inline G4int Index(const Fold_Efield_Map* foldmap, G4int i, G4int j, G4int k ) const {
        return foldmap->pLayer[i]+j*foldmap->NofPointsinLayer[i]+k;
      };
      void SetFoldArea(G4double, FoldArea & ) const;

      //HV for current calculation

      static       G4bool   s_HVMapRead;    // used only for initialization
      std::string m_HVMapVersion; // used only for initialization
      static const G4double s_AverageHV;
      static const G4double s_AverageEfield;
      static const G4double s_AverageCurrent;
      static const G4String s_HVEMECMapFileName;  //{"HVEMECMap.dat"};

      static const G4int s_NofAtlasSide     = 2;
      static const G4int s_NofEtaSection    = 9;
      static const G4int s_NofElectrodeSide = 2;
      static const G4int s_NofElectrodesOut = 768;
      static const G4double s_HV_Etalim[s_NofEtaSection+1]; // = {1.375,1.5,1.6,1.8,2.,2.1,2.3,2.5,2.8,3.2};
      static G4int s_HV_Start_phi[s_NofAtlasSide][s_NofEtaSection][s_NofElectrodeSide];  // used as const after init
      static G4double s_HV_Values[s_NofAtlasSide][s_NofEtaSection][s_NofElectrodeSide][s_NofElectrodesOut]; // used as const after init

      static const G4double s_LArTemperature_ECC0;//={88.15}; //K
      static const G4double s_LArTemperature_ECC1;//={88.37};
      static const G4double s_LArTemperature_ECC5;//={87.97};
      static const G4double s_LArTemperature_av ;// ={88.16};

      void GetHVMap(const G4String);  // used only for initialization
      G4double GetHV_Value(const G4ThreeVector& p) const;
      G4double GetHV_Value_ChColl_Wheel( const G4ThreeVector& , G4int , G4int) const;

      //Efield in [kv/cm], driftvelo in [mm/microsec], Temperature in [K]

      inline static G4double IonReco(const G4double Efield) {
        if(Efield<=0.000001){return 0.;}
        if(Efield>2.)       {return (1./(1.  +0.36/Efield));}
        return (1./(1.04+0.28/Efield));
      }

      inline static G4double DriftVelo(const G4double T, const G4double Efield) {
        if( Efield <= 0.000001) {return 0.;}
        return ( (-0.01481*(T-90.371)+1.)*
                 ( 0.141*Efield*log(1.+12.4/Efield)+
                   1.627*pow(Efield,0.317) )
                 -0.0075*(T-90.371)
                 );
      }

      // functions specific for geometry

      void     SetHalfWave(G4double, WheelGeometry &) const;
      void     GetPhiGap(const G4double *, WheelGeometry &) const;
      void     SetYlimitsofPhigapinWheel(G4double, G4double, const WheelGeometry & wg, G4double * Ylimits) const;
      G4double YofSurface(G4double,G4double,G4double,const WheelGeometry &) const;
      inline   G4double YofNeutralFibre(G4double alpha,G4double rho, const WheelGeometry & wg) const {
        return YofSurface(alpha,rho,0., wg);
      }
      G4double FoldingAngle(G4double) const;
      G4double HalfLArGapSize(G4double, G4double) const;

      //  functions specific for charge coll.

      void     IniGeomforFieldMaps(void);      // called only at init phase
      void     LoadFieldMaps(const G4String);  // called only at init phase
      void     PrepareFieldMap(Wheel_Efield_Map* ChCollWheelType);  // called only at init phase
      G4double GetCurrent(G4double *,G4double *,G4double, G4double Barret_PhiStart) const;
      void     TransformWheeltoFieldMap(const G4double *,G4double *, const WheelGeometry & wg, const FoldArea & fa) const;
      void     SetYlimitsofPhigapinFieldMap(G4int, const WheelGeometry & wg, G4double * Ylimits) const;
      void     TransFromBarrtoWheel(const G4double*, G4double PhiStartOfPhiDiv, G4double*) const;
      G4double GetWeightfromFieldMap(G4int,G4double,G4double, const FoldArea & fa) const;
      G4double HalfLArGapSizeOld(G4double) const;

      G4double m_CHC_Esr;  // used as const after init
      inline G4double CHC_Esr() const { return m_CHC_Esr; };

#ifdef DEBUG_CHCL // non thread-safe debug of charge collection
      static const G4int    s_CHCMaxPrint=00; // exists only if debug activated
      static       G4int    s_CHCIprint;      // exists only if debug activated
      static       G4double s_CHCEbad;        // exists only if debug activated
      static       G4double s_CHCEtotal;      // exists only if debug activated
      static       G4double s_CHCStotal;      // exists only if debug activated
#endif

    private:
      //  inline void vector_to_msg(G4ThreeVector &v) const;

      /* to be developed...
         std::pair<double, double>DxToFans(Hep3Vector &p);
         double XDistanceToTheNeutralFibre(const Hep3Vector& P) const;
      */
      G4double GetGapSize(const G4ThreeVector &p) const;

      //  public:
      G4double distance_to_the_nearest_electrode(const G4ThreeVector &p) const;

      UnsignedIntegerProperty m_solidtypeProp;
      LArG4::LArWheelCalculator_t m_solidtype;
      int m_zside;
      LArG4BirksLaw *m_birksLaw;
      LArWheelCalculator *m_lwc;
      const LArWheelCalculator * lwc() const { return m_lwc; }

      void get_HV_map_from_DB(void);
      G4bool m_DB_HV;

      std::string m_suffix;

      // Aug 2007 AMS, lost Aug 2008, restored May 2009
      LArWheelCalculator *m_electrode_calculator;
      const LArWheelCalculator * elc() const { return m_electrode_calculator; }

      G4double GetCurrent1(const G4ThreeVector &, const G4ThreeVector &, G4double) const;

      G4double get_HV_value(const G4ThreeVector&, const std::pair<G4int, G4int> &) const;

      EnergyCalculator (const EnergyCalculator&);
      EnergyCalculator& operator= (const EnergyCalculator&);

      G4int _getIRlayer(G4double rforalpha) const;
      G4int _getIRlayerA(G4double rforalpha) const;

      G4double _interpolateCurrentSubStep(G4double rforalpha, G4int gapup, const G4double vmap[],
                                          G4double tol, const FoldArea & fa, G4int & gaperr ) const;


      G4double _interpolateCurrentSubStep1(G4double rforalpha, const G4double vmap[],
                                           const G4ThreeVector & Pe, int side_dte, int Pe_fan,
                                           const G4ThreeVector & Pa, int side_dta, int Pa_fan,
                                           const FoldArea & fa, G4int & gaperr ) const;

      G4double _AdjustedPhiOfPoint_Barrett(const G4ThreeVector& p, G4double PhiStartOfPhiDiv) const;

      static inline G4double _normalizeAngle2Pi(G4double a) {
        return ( a<0.) ?
          a + CLHEP::twopi
          :
          (a >= CLHEP::twopi ? a - CLHEP::twopi: a);
      }

      G4double getPhiStartOfPhiDiv(const G4Step* step) const;
    };

  } // namespace EC
} // namespace LArG4

#endif // LArG4_EC_EnergyCalculator_H
