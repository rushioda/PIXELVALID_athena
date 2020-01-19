/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/*
 * MuonEfficiencyScaleFactors.h
 *
 *  Created on: Apr 9, 2014
 *      Author: goblirsc
 */

#ifndef MUONEFFICIENCYSCALEFACTORS_H_
#define MUONEFFICIENCYSCALEFACTORS_H_

#include "MuonEfficiencyCorrections/IMuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyType.h"
#include "MuonEfficiencyCorrections/EfficiencyScaleFactor.h"
#include "MuonEfficiencyCorrections/EffiCollection.h"

#include <AsgTools/ToolHandle.h>
#include <AsgTools/AsgTool.h>

#include <string>

#include <boost/unordered_map.hpp>

namespace CP {
    class MuonEfficiencyScaleFactors: public CP::IMuonEfficiencyScaleFactors, public asg::AsgTool {

        public:
            MuonEfficiencyScaleFactors(const std::string& name);

            virtual ~MuonEfficiencyScaleFactors();
            //Proper constructor for Athena
            ASG_TOOL_CLASS2( MuonEfficiencyScaleFactors, CP::IMuonEfficiencyScaleFactors, CP::ISystematicsTool )

            /// initialize the tool once all settings are in place!
            virtual StatusCode initialize();

            /// Retrieve the Scale factor and decorate the muon
            virtual CorrectionCode getEfficiencyScaleFactor(const xAOD::Muon& mu, float& sf, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyEfficiencyScaleFactor(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0) const;
            /// replica generation
            virtual CorrectionCode getEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0) const;

            /// returns: whether this tool is affected by the given systematis
            virtual bool isAffectedBySystematic(const SystematicVariation& systematic) const;

            /// returns: the list of all systematics this tool can be affected by
            virtual SystematicSet affectingSystematics() const;

            /// returns: the list of all systematics this tool recommends to use
            virtual SystematicSet recommendedSystematics() const;

            virtual SystematicCode applySystematicVariation(const SystematicSet& systConfig);

            /// Obtain the muon efficiency measured using the data
            virtual CorrectionCode getDataEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyDataEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0) const;

            virtual CorrectionCode getDataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyDataEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0) const;

            /// Obtain the muon efficiency measured using the MC
            virtual CorrectionCode getMCEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyMCEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0) const;

            virtual CorrectionCode getMCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyMCEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0) const;

            // copy constructor, to make reflex happy...
            MuonEfficiencyScaleFactors(const MuonEfficiencyScaleFactors& tocopy);

            // to make coverity happy
            MuonEfficiencyScaleFactors & operator =(const MuonEfficiencyScaleFactors & tocopy);

        private:
            unsigned int getRandomRunNumber(const xAOD::EventInfo* info) const;
            /// load the SF histos
            bool LoadEffiSet(MuonEfficiencySystType sysType);
            bool LoadInputs();

            /// construct the name of the input files from the configuration
            std::string filename_Central();
            std::string filename_Calo();
            std::string filename_HighEta();
            std::string filename_LowPt();
            std::string filename_LowPtCalo();

            // utility method to 'dress' a filename using the path resolver
            std::string resolve_file_location(const std::string &filename);

            //Some util functions
            void CopyInformation(const MuonEfficiencyScaleFactors & tocopy);
            //These are functions needed during initialization
            StatusCode CreateDecorator(SG::AuxElement::Decorator<float>* &Dec, std::string &DecName, const std::string& defaultName);
            StatusCode CreateVecDecorator(SG::AuxElement::Decorator<std::vector<float>>* &Dec, std::string &DecName, const std::string& defaultName);
            StatusCode IsDecoratorNameUnique(std::string &name);
            SystematicSet SetupSystematics(bool doUnfolded = false) const;
            void SetupCheckSystematicSets();
            /// the working point to operate on
            std::string m_wp;
            boost::unordered_map<MuonEfficiencySystType, EffiCollection*> m_sf_sets;

            EffiCollection *m_current_sf;

            std::string m_custom_dir;
            std::string m_custom_file_Combined;
            std::string m_custom_file_Calo;
            std::string m_custom_file_HighEta;
            std::string m_custom_file_LowPt;
            std::string m_custom_file_LowPtCalo;

            // info to apply to the muon when in audit mode
            std::string m_version_string;
            std::string m_sys_string;

            std::map<CP::SystematicSet, CP::SystematicSet> m_filtered_sys_sets;

            std::string m_efficiency_decoration_name_data;
            std::string m_efficiency_decoration_name_mc;
            std::string m_sf_decoration_name;
            std::string m_sf_replica_decoration_name;
            std::string m_eff_replica_decoration_name;
            std::string m_mc_eff_replica_decoration_name;

            // subfolder to load from the calibration db
            std::string m_calibration_version;

            // threshold below which low-pt SF (i.e. from JPsi) should be used
            double m_lowpt_threshold;
            // decorators to quickly apply the eff and SF
            SG::AuxElement::Decorator<float>* m_effDec;
            SG::AuxElement::Decorator<float>* m_MCeffDec;
            SG::AuxElement::Decorator<float>* m_sfDec;

            SG::AuxElement::Decorator<std::vector<float> >* m_sfrDec;
            SG::AuxElement::Decorator<std::vector<float> >* m_effrDec;
            SG::AuxElement::Decorator<std::vector<float> >* m_MCeffrDec;

            CP::SystematicSet m_affectingSys;

            // need CP::SystematicSets as members to retrieve MuonEfficiencySystType each event
            CP::SystematicSet* m_Sys1Down;
            CP::SystematicSet* m_Sys1Up;
            CP::SystematicSet* m_Stat1Down;
            CP::SystematicSet* m_Stat1Up;
            CP::SystematicSet* m_LowPtSys1Down;
            CP::SystematicSet* m_LowPtSys1Up;
            CP::SystematicSet* m_LowPtStat1Down;
            CP::SystematicSet* m_LowPtStat1Up;

            bool m_init;
            bool m_seperateSystBins;
            CP::MuonEfficiencyType m_Type;
    };

} /* namespace CP */

#endif /* MUONEFFICIENCYSCALEFACTORS_H_ */
