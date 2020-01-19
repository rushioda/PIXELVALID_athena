/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/*
 * MuonEfficiencyScaleFactors.cxx
 *
 *  Created on: Apr 9, 2014
 *      Author: goblirsc
 */

#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyType.h"

#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PathResolver/PathResolver.h"

namespace CP {

//    static std::string CURRENT_MEC_VERSION = "MuonEfficiencyCorrections-03-03-17";

    MuonEfficiencyScaleFactors::MuonEfficiencyScaleFactors(const std::string& name) :
                    asg::AsgTool(name),
                    m_wp("Medium"),
                    m_sf_sets(),
                    m_current_sf(0),
                    m_custom_dir(),
                    m_custom_file_Combined(),
                    m_custom_file_Calo(),
                    m_custom_file_HighEta(),
                    m_custom_file_LowPt(),
                    m_custom_file_LowPtCalo(),
                    m_version_string(),
                    m_sys_string(),
                    m_filtered_sys_sets(),
                    m_efficiency_decoration_name_data(),
                    m_efficiency_decoration_name_mc(),
                    m_sf_decoration_name(),
                    m_sf_replica_decoration_name(),
                    m_eff_replica_decoration_name(),
                    m_mc_eff_replica_decoration_name(),
                    m_calibration_version("170410_Moriond"),
                    m_lowpt_threshold(15.e3),
                    m_effDec(0),
                    m_MCeffDec(0),
                    m_sfDec(0),
                    m_sfrDec(0),
                    m_effrDec(0),
                    m_MCeffrDec(0),
                    m_affectingSys(),
                    m_Sys1Down(NULL),
                    m_Sys1Up(NULL),
                    m_Stat1Down(NULL),
                    m_Stat1Up(NULL),
                    m_LowPtSys1Down(NULL),
                    m_LowPtSys1Up(NULL),
                    m_LowPtStat1Down(NULL),
                    m_LowPtStat1Up(NULL),
                    m_init(false),
                    m_seperateSystBins(false),
                    m_Type(CP::MuonEfficiencyType::Undefined) {

        declareProperty("WorkingPoint", m_wp);

        // these are for debugging / testing, *not* for general use!
        declareProperty("CustomInputFolder", m_custom_dir);
        declareProperty("CustomFileCaloTag", m_custom_file_Calo);
        declareProperty("CustomFileCombined", m_custom_file_Combined);
        declareProperty("CustomFileHighEta", m_custom_file_HighEta);
        declareProperty("CustomFileLowPt", m_custom_file_LowPt);
        declareProperty("CustomFileLowPtCalo", m_custom_file_LowPtCalo);

        declareProperty("DataEfficiencyDecorationName", m_efficiency_decoration_name_data);
        declareProperty("MCEfficiencyDecorationName", m_efficiency_decoration_name_mc);
        declareProperty("ScaleFactorDecorationName", m_sf_decoration_name);
        declareProperty("ScaleFactorReplicaDecorationName", m_sf_replica_decoration_name);
        declareProperty("DataEfficiencyReplicaDecorationName", m_eff_replica_decoration_name);
        declareProperty("MCEfficiencyReplicaDecorationName", m_mc_eff_replica_decoration_name);

        declareProperty("CalibrationRelease", m_calibration_version);
        declareProperty("LowPtThreshold", m_lowpt_threshold);
        declareProperty("UncorrelateSystematics", m_seperateSystBins);
    }

    MuonEfficiencyScaleFactors::~MuonEfficiencyScaleFactors() {
        // remember to clean up afterwards...
        for (auto& it : m_sf_sets) {
            if (it.second) delete it.second;
        }
        if (m_effDec) delete m_effDec;
        if (m_sfDec) delete m_sfDec;
        if (m_MCeffDec) delete m_MCeffDec;
        if (m_sfrDec) delete m_sfrDec;
        if (m_effrDec) delete m_effrDec;
        if (m_MCeffrDec) delete m_MCeffrDec;

        if (m_Sys1Down) delete m_Sys1Down;
        if (m_Sys1Up) delete m_Sys1Up;
        if (m_Stat1Down) delete m_Stat1Down;
        if (m_Stat1Up) delete m_Stat1Up;
        if (m_LowPtSys1Down) delete m_LowPtSys1Down;
        if (m_LowPtSys1Up) delete m_LowPtSys1Up;
        if (m_LowPtStat1Down) delete m_LowPtStat1Down;
        if (m_LowPtStat1Up) delete m_LowPtStat1Up;
    }

    StatusCode MuonEfficiencyScaleFactors::initialize() {
        if (m_init) {
            ATH_MSG_INFO("The tool using working point" << m_wp << " is already initialized");
            return StatusCode::SUCCESS;
        }
        if (m_wp.find("Iso") != std::string::npos) {
            m_Type = CP::MuonEfficiencyType::Iso;

        } else if (m_wp.find("BadMuon") != std::string::npos) {
            m_Type = CP::MuonEfficiencyType::BadMuonVeto;

        } else if (m_wp.find("TTVA") != std::string::npos) {
            m_Type = CP::MuonEfficiencyType::TTVA;
        } else {
            m_Type = CP::MuonEfficiencyType::Reco;
        }
        ATH_MSG_INFO("Efficiency type is = " << EfficiencyTypeName(m_Type));

        /// for isolation efficiencies, we don't use a low pt component for now - set the low pt threshold to -1
        /// same holds for TTVA SF, and for the HighPt WP
        if (m_Type == CP::MuonEfficiencyType::Iso || m_Type == CP::MuonEfficiencyType::TTVA || m_Type == MuonEfficiencyType::BadMuonVeto || (m_Type == CP::MuonEfficiencyType::Reco && m_wp.find("HighPt") != std::string::npos)) {
            ATH_MSG_DEBUG("We are running Isolation or TTVA or High Pt reco SF, so we use Zmumu based SF for the whole pt range!");
            m_lowpt_threshold = -1;
        } else if (m_lowpt_threshold < 0) {
            ATH_MSG_INFO("Low pt SF turned off as crossover threshold is negative! Using Zmumu based SF for all pt values.");
        } else {
            ATH_MSG_INFO("JPsi based low pt SF will start to rock below " << m_lowpt_threshold / 1000. << " GeV!");
        }
        ATH_CHECK(CreateDecorator(m_effDec, m_efficiency_decoration_name_data, "Efficiency"));
        ATH_CHECK(CreateDecorator(m_MCeffDec, m_efficiency_decoration_name_mc, "mcEfficiency"));
        ATH_CHECK(CreateDecorator(m_sfDec, m_sf_decoration_name, "EfficiencyScaleFactor"));

        ATH_CHECK(CreateVecDecorator(m_sfrDec, m_sf_replica_decoration_name, "EfficiencyScaleFactorReplicas"));
        ATH_CHECK(CreateVecDecorator(m_effrDec, m_eff_replica_decoration_name, "EfficiencyReplicas"));
        ATH_CHECK(CreateVecDecorator(m_MCeffrDec, m_mc_eff_replica_decoration_name, "mcEfficiencyReplicas"));

        if (!m_custom_dir.empty()) ATH_MSG_WARNING("Note: setting up with user specified input file location " << m_custom_dir << " - this is not encouraged!");
        if (!m_custom_file_Calo.empty()) ATH_MSG_WARNING("Note: setting up with user specified CaloTag input file " << m_custom_file_Calo << " - this is not encouraged!");
        if (!m_custom_file_Combined.empty()) ATH_MSG_WARNING("Note: setting up with user specified Central muon input file " << m_custom_file_Combined << " - this is not encouraged! ");
        if (!m_custom_file_HighEta.empty()) ATH_MSG_WARNING("Note: setting up with user specified High Eta input file " << m_custom_file_HighEta << " - this is not encouraged! ");
        if (!m_custom_file_LowPt.empty()) ATH_MSG_WARNING("Note: setting up with user specified Low Pt input file " << m_custom_file_LowPt << " - this is not encouraged! ");
        if (!m_custom_file_LowPtCalo.empty()) ATH_MSG_WARNING("Note: setting up with user specified Low Pt CaloTag input file " << m_custom_file_LowPtCalo << " - this is not encouraged! ");
        if (m_custom_dir.empty()) ATH_MSG_INFO("Trying to initialize, with working point " << m_wp << ", using calibration release " << m_calibration_version);

        if (!LoadInputs()) {
            return StatusCode::FAILURE;
        }
        SetupCheckSystematicSets();

        // m_init has to be true for affectingSystematics()
        m_init = true;
        m_affectingSys = affectingSystematics();

        // set up for default running without systematics
        if (!applySystematicVariation(SystematicSet())) {
            ATH_MSG_ERROR("loading the central value systematic set failed");
            return StatusCode::FAILURE;
        }

        // Add the affecting systematics to the global registry
        SystematicRegistry& registry = SystematicRegistry::getInstance();
        if (!registry.registerSystematics(*this)) {
            ATH_MSG_ERROR("unable to register the systematics");
            return StatusCode::FAILURE;
        }
        ATH_MSG_INFO("Successfully initialized! ");

        return StatusCode::SUCCESS;
    }
    void MuonEfficiencyScaleFactors::SetupCheckSystematicSets() {
        if (!m_Sys1Down) {
            m_Sys1Down = new SystematicSet(std::vector<SystematicVariation> { SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS", -1) });
            m_Sys1Up = new SystematicSet(std::vector<SystematicVariation> { SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS", 1) });
            m_Stat1Down = new SystematicSet(std::vector<SystematicVariation> { SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT", -1) });
            m_Stat1Up = new SystematicSet(std::vector<SystematicVariation> { SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT", 1) });
        }
        if (!m_LowPtSys1Down && (m_Type == MuonEfficiencyType::Reco && m_lowpt_threshold > 0)) {
            m_LowPtSys1Down = new SystematicSet(std::vector<SystematicVariation> { SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS_LOWPT", -1) });
            m_LowPtSys1Up = new SystematicSet(std::vector<SystematicVariation> { SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS_LOWPT", 1) });
            m_LowPtStat1Down = new SystematicSet(std::vector<SystematicVariation> { SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT_LOWPT", -1) });
            m_LowPtStat1Up = new SystematicSet(std::vector<SystematicVariation> { SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT_LOWPT", 1) });
        }
    }
    StatusCode MuonEfficiencyScaleFactors::CreateDecorator(SG::AuxElement::Decorator<float>* &Dec, std::string &DecName, const std::string& defaultName) {
        if (Dec) delete Dec;
        if (DecName.empty()) DecName = (m_Type == CP::MuonEfficiencyType::Reco) ? defaultName : EfficiencyTypeName(m_Type) + defaultName;
        ATH_MSG_INFO(defaultName << " decoration_name is " << DecName);
        ATH_CHECK(IsDecoratorNameUnique(DecName));
        Dec = new SG::AuxElement::Decorator<float>(DecName);
        return StatusCode::SUCCESS;
    }
    StatusCode MuonEfficiencyScaleFactors::CreateVecDecorator(SG::AuxElement::Decorator<std::vector<float>>* &Dec, std::string &DecName, const std::string& defaultName) {
        if (Dec) {
            delete Dec;
            Dec = 0;
        }
        if (DecName.empty()) DecName = (m_Type == CP::MuonEfficiencyType::Reco) ? defaultName : EfficiencyTypeName(m_Type) + defaultName;
        ATH_MSG_INFO(defaultName << " decoration_name is " << DecName);
        ATH_CHECK(IsDecoratorNameUnique(DecName));
        Dec = new SG::AuxElement::Decorator<std::vector<float>>(DecName);
        return StatusCode::SUCCESS;
    }

    StatusCode MuonEfficiencyScaleFactors::IsDecoratorNameUnique(std::string &name) {
        if (name.empty()) return StatusCode::SUCCESS;
        if (&name != &m_efficiency_decoration_name_data && name == m_efficiency_decoration_name_data) ATH_MSG_ERROR("Decorator " << name << " corresponds to the name of the data efficiencies");
        else if (&name != &m_efficiency_decoration_name_mc && name == m_efficiency_decoration_name_mc) ATH_MSG_ERROR("Decorator " << name << " corresponds to the name of the MC efficiencies");
        else if (&name != &m_sf_decoration_name && name == m_sf_decoration_name) ATH_MSG_ERROR("Decorator " << name << " corresponds to the name of the SF themselves");
        else if (&name != &m_sf_replica_decoration_name && name == m_sf_replica_decoration_name) ATH_MSG_ERROR("Decorator " << name << " corresponds to name of the SF replicas");
        else if (&name != &m_eff_replica_decoration_name && name == m_eff_replica_decoration_name) ATH_MSG_ERROR("Decorator " << name << " corresponds to name of the data efficiency replicas");
        else if (&name != &m_mc_eff_replica_decoration_name && name == m_mc_eff_replica_decoration_name) ATH_MSG_ERROR("Decorator " << name << " corresponds to name of the MC efficiency replicas");
        else return StatusCode::SUCCESS;
        return StatusCode::FAILURE;
    }
    unsigned int MuonEfficiencyScaleFactors::getRandomRunNumber(const xAOD::EventInfo* info) const {
        if (!info && !evtStore()->retrieve(info, "EventInfo")) {
            ATH_MSG_ERROR("Could not retrieve the xAOD::EventInfo. Return 999999");
            return 999999;
        }
        if (!info->eventType(xAOD::EventInfo::IS_SIMULATION)) {
            ATH_MSG_DEBUG("The current event is a data event. Return runNumber instead.");
            return info->runNumber();
        }
        static SG::AuxElement::ConstAccessor<unsigned int> dec_rnd("RandomRunNumber");
        if (!dec_rnd.isAvailable(*info)) {
            ATH_MSG_WARNING("Failed to find the RandomRunNumber decoration. Please call the apply() method from the PileupReweightingTool before hand in order to get period dependent SFs. You'll receive SFs from the most recent period.");
            return 999999;
        } else if (dec_rnd(*info) == 0) {
            ATH_MSG_DEBUG("Pile up tool has given runNumber 0. Return SF from latest period.");
            return 999999;
        }
        return dec_rnd(*info);
    }
    CorrectionCode MuonEfficiencyScaleFactors::getEfficiencyScaleFactor(const xAOD::Muon& mu, float& sf, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet.");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ScaleFactor(mu, sf);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyEfficiencyScaleFactor(const xAOD::Muon& mu, const xAOD::EventInfo* info) const {
        float sf = 0;
        CorrectionCode result = getEfficiencyScaleFactor(mu, sf, info);
// Decorate the muon
        if (m_sfDec) (*m_sfDec)(mu) = sf;
        return result;
    }

    CorrectionCode MuonEfficiencyScaleFactors::getEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & sfs, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ScaleFactorReplicas(mu, sfs);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, int nreplicas, const xAOD::EventInfo* info) const {
        std::vector<float> replicas(nreplicas);
        CorrectionCode result = getEfficiencyScaleFactorReplicas(mu, replicas, info);
// Decorate the muon
        if (m_sfrDec) (*m_sfrDec)(mu) = replicas;
        return result;
    }
    CorrectionCode MuonEfficiencyScaleFactors::getDataEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->DataEfficiency(mu, eff);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyDataEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info) const {
        float eff = 0;
        CorrectionCode result = getDataEfficiency(mu, eff, info);
// Decorate the muon
        if (m_effDec) (*m_effDec)(mu) = eff;
        return result;
    }
    CorrectionCode MuonEfficiencyScaleFactors::getDataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->DataEfficiencyReplicas(mu, sf_err);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyDataEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas, const xAOD::EventInfo* info) const {
        std::vector<float> replicas(nreplicas);
        CorrectionCode result = getDataEfficiencyReplicas(mu, replicas, info);
// Decorate the muon
        if (m_effrDec) (*m_effrDec)(mu) = replicas;
        return result;
    }
    CorrectionCode MuonEfficiencyScaleFactors::getMCEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->MCEfficiency(mu, eff);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyMCEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info) const {
        float eff = 0;
        CorrectionCode result = getMCEfficiency(mu, eff, info);
// Decorate the muon
        if (m_MCeffDec) (*m_MCeffDec)(mu) = eff;
        return result;
    }
    CorrectionCode MuonEfficiencyScaleFactors::getMCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->MCEfficiencyReplicas(mu, sf_err);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyMCEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas, const xAOD::EventInfo* info) const {
        std::vector<float> replicas(nreplicas);
        CorrectionCode result = getMCEfficiencyReplicas(mu, replicas, info);
// Decorate the muon
        if (m_MCeffrDec) (*m_MCeffrDec)(mu) = replicas;
        return result;
    }

    std::string MuonEfficiencyScaleFactors::resolve_file_location(const std::string &filename) {

// can be overridden by user defined directory - mainly for development, not for everyday use!
        if (!m_custom_dir.empty()) {
            return m_custom_dir + "/" + filename;
        }
        std::string fullPathToFile = PathResolverFindCalibFile(Form("MuonEfficiencyCorrections/%s/%s", m_calibration_version.c_str(), filename.c_str()));
        if (fullPathToFile.empty()) {
            ATH_MSG_ERROR("Unable to resolve the input file " << m_calibration_version << "/" << filename << " via the PathResolver!");
        }
        return fullPathToFile;
    }

    std::string MuonEfficiencyScaleFactors::filename_Central() {

        if (!m_custom_file_Combined.empty()) return (resolve_file_location(m_custom_file_Combined));
        else if (m_Type == CP::MuonEfficiencyType::Iso) {
            return resolve_file_location(Form("Iso_%s_Z.root", m_wp.c_str()));
        } else if (m_Type == CP::MuonEfficiencyType::TTVA) {
            return resolve_file_location("TTVA_Z.root");
        } else if (m_Type == CP::MuonEfficiencyType::BadMuonVeto) {
            return resolve_file_location("BadMuonVeto_HighPt_Z.root");
        } else if (m_Type == CP::MuonEfficiencyType::Reco) {
            return resolve_file_location(Form("Reco_%s_Z.root", m_wp.c_str()));
        }
        ATH_MSG_ERROR("What?");
        return "";
    }
    std::string MuonEfficiencyScaleFactors::filename_Calo() {

        if (!m_custom_file_Calo.empty()) return resolve_file_location(m_custom_file_Calo);
        else if (m_Type != CP::MuonEfficiencyType::Reco) {
            return filename_Central();
        } else return resolve_file_location("Reco_CaloTag_Z.root");
    }

    std::string MuonEfficiencyScaleFactors::filename_HighEta() {

        if (!m_custom_file_HighEta.empty()) return resolve_file_location(m_custom_file_HighEta);
        else if (m_Type != CP::MuonEfficiencyType::Reco) {
            return filename_Central();
        } else return resolve_file_location("Reco_HighEta_Z.root");
    }

    std::string MuonEfficiencyScaleFactors::filename_LowPt() {

        if (!m_custom_file_LowPt.empty()) return resolve_file_location(m_custom_file_LowPt);
// for the no reco WPs, we currently use the existing Z SF also for the low pt regime
        else if (m_Type != CP::MuonEfficiencyType::Reco || m_lowpt_threshold < 0) {
            return filename_Central();
        } else return resolve_file_location(Form("Reco_%s_JPsi.root", m_wp.c_str()));
    }

    std::string MuonEfficiencyScaleFactors::filename_LowPtCalo() {

        if (!m_custom_file_LowPtCalo.empty()) return resolve_file_location(m_custom_file_LowPtCalo);
// for the no reco WPs, we currently use the existing Z SF also for the low pt regime
        else if (m_Type != CP::MuonEfficiencyType::Reco || m_lowpt_threshold < 0) {
            return filename_Central();
        } else return resolve_file_location("Reco_CaloTag_JPsi.root");
    }

// load the SF histos
    bool MuonEfficiencyScaleFactors::LoadInputs() {
        // load all MuonEfficiencySystTypes
        for (int i = MuonEfficiencySystType::Nominal; i <= MuonEfficiencySystType::LowPtStat1Up; ++i) {
            // skip the low-pt systematics when they are not activated
            if (m_Type != MuonEfficiencyType::Reco || m_lowpt_threshold <= 0) {
                if (i >= MuonEfficiencySystType::LowPtSys1Down) {
                    break;
                }
            }
            if (!LoadEffiSet((MuonEfficiencySystType) i)) {
                return false;
            }
        }
        return true;
    }
    bool MuonEfficiencyScaleFactors::LoadEffiSet(MuonEfficiencySystType sysType) {

        // delete already existing SF sets first
        if (m_sf_sets.find(sysType) != m_sf_sets.end()) {
            ATH_MSG_WARNING("Deleting SF map for MuonEfficiencySystType=" << (int) (sysType));
            delete m_sf_sets[sysType];
        }
        EffiCollection* ec = NULL;
        if (!m_seperateSystBins || IsSystVariation(sysType) || sysType == MuonEfficiencySystType::Nominal) {
            ec = new EffiCollection(filename_Central(), filename_Calo(), filename_HighEta(), filename_LowPt(), filename_LowPtCalo(), sysType, m_Type, m_lowpt_threshold);
        } else {
            //Parse nominal EffiCollection as fallback for all bins except for the current syst bin
            ec = new EffiCollection(m_sf_sets.at(MuonEfficiencySystType::Nominal), filename_Central(), filename_Calo(), filename_HighEta(), filename_LowPt(), filename_LowPtCalo(), sysType, m_Type, m_lowpt_threshold);
        }
        m_sf_sets.insert(std::make_pair(sysType, ec));

        return ec->CheckConsistency();

    }
    bool MuonEfficiencyScaleFactors::isAffectedBySystematic(const SystematicVariation& systematic) const {
        SystematicSet sys = affectingSystematics();
        return sys.find(systematic) != sys.end();
    }

    SystematicSet MuonEfficiencyScaleFactors::SetupSystematics(bool doUnfolded) const {
        SystematicSet result;
        if (!m_init) {
            ATH_MSG_ERROR("The tool is not initialized yet");
        } else if (!doUnfolded) {
            result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT", 1));
            result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT", -1));
            result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS", 1));
            result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS", -1));
            if (m_Type == MuonEfficiencyType::Reco && m_lowpt_threshold > 0) {
                result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT_LOWPT", 1));
                result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT_LOWPT", -1));
                result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS_LOWPT", 1));
                result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS_LOWPT", -1));
            }
        }
        //Let the world implode... Yeaaha
        else {
            //From technical point of view we could in principle  decorrelate the Systematics too, however
            //SYS variations are correlated. So skip those for the starting. If we come up with a better model
            //then be happy about twice times more variations
            result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS", 1));
            result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS", -1));
            if (m_Type == MuonEfficiencyType::Reco && m_lowpt_threshold > 0) {
                result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS_LOWPT", 1));
                result.insert(SystematicVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_SYS_LOWPT", -1));
            }
            int nBins = m_sf_sets.at(MuonEfficiencySystType::Nominal)->nBins();
            ATH_MSG_INFO("Going to setup for each bin of the SF map a separate statistical variation. This will make the world to implode.... Yeeaha. ");
            for (int bin = 1; bin < nBins; ++bin) {
                // toyScale has to be toyScale > 0
                if (!m_sf_sets.at(MuonEfficiencySystType::Nominal)->IsLowPtBin(bin)) {
                    CP::SystematicVariation StatUpBin = CP::SystematicVariation::makeToyVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT", bin, MuonEfficiencySystType::Stat1Up);
                    result.insert(StatUpBin);
                    CP::SystematicVariation StatUpDown = CP::SystematicVariation::makeToyVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT", bin, MuonEfficiencySystType::Stat1Down);
                    result.insert(StatUpDown);
                }
                if ((m_sf_sets.at(MuonEfficiencySystType::Nominal)->IsLowPtBin(bin) || m_sf_sets.at(MuonEfficiencySystType::Nominal)->IsForwardBin(bin)) && (m_Type == MuonEfficiencyType::Reco && m_lowpt_threshold > 0)) {
                    CP::SystematicVariation StatUpBinLowPt = CP::SystematicVariation::makeToyVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT_LOWPT", bin, MuonEfficiencySystType::LowPtStat1Up);
                    result.insert(StatUpBinLowPt);
                    CP::SystematicVariation StatUpDownLowPt = CP::SystematicVariation::makeToyVariation("MUON_EFF_" + EfficiencyTypeName(m_Type) + "_STAT_LOWPT", bin, MuonEfficiencySystType::LowPtStat1Down);
                    result.insert(StatUpDownLowPt);
                }
            }
            ATH_MSG_INFO("The world has been imploded with " << result.size() << " systematic variations...");

        }
        return result;
    }

/// returns: the list of all systematics this tool can be affected by
    SystematicSet MuonEfficiencyScaleFactors::affectingSystematics() const {
        return SetupSystematics(m_seperateSystBins);
    }

/// returns: the list of all systematics this tool recommends to use
    SystematicSet MuonEfficiencyScaleFactors::recommendedSystematics() const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CP::SystematicSet();
        }
        return affectingSystematics();
    }
    SystematicCode MuonEfficiencyScaleFactors::applySystematicVariation(const SystematicSet& systConfig) {
        SystematicSet mySysConf;
        if (m_filtered_sys_sets.find(systConfig) == m_filtered_sys_sets.end()) {
            if (!SystematicSet::filterForAffectingSystematics(systConfig, m_affectingSys, mySysConf)) {
                ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool! ");
                return SystematicCode::Unsupported;
            }
            m_filtered_sys_sets[systConfig] = mySysConf;
        }
        mySysConf = m_filtered_sys_sets[systConfig];
        ATH_MSG_DEBUG(mySysConf.name() << " made it into applySystematicVariation()");

        unsigned int currentBinNumber = 0;
        MuonEfficiencySystType currentEfficiencySystType = MuonEfficiencySystType::Nominal;
        if (!m_init) {
            ATH_MSG_ERROR("Initialize first the tool!");
            return SystematicCode::Unsupported;
        }
        if ((*m_Sys1Down) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::Sys1Down;
        else if ((*m_Sys1Up) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::Sys1Up;
        else if (m_LowPtSys1Up && (*m_LowPtSys1Up) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::LowPtSys1Down;
        else if (m_LowPtSys1Down && (*m_LowPtSys1Down) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::LowPtSys1Up;
        else if (!m_seperateSystBins) {
            if ((*m_Stat1Down) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::Stat1Down;
            else if ((*m_Stat1Up) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::Stat1Up;
            else if (m_LowPtStat1Up && (*m_LowPtStat1Up) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::LowPtStat1Down;
            else if (m_LowPtStat1Down && (*m_LowPtStat1Down) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::LowPtStat1Up;
        } else if (!mySysConf.name().empty()) {
            for (std::set<SystematicVariation>::iterator t = mySysConf.begin(); t != mySysConf.end(); ++t) {
                if ((*t).isToyVariation()) {
                    std::pair<unsigned, float> m_pair = (*t).getToyVariation();
                    currentBinNumber = m_pair.first;
                    currentEfficiencySystType = MuonEfficiencySystType(m_pair.second);
                }
            }
            ATH_MSG_DEBUG("need to access currentBinNumber=" << currentBinNumber);
        }
        boost::unordered_map<MuonEfficiencySystType, EffiCollection*>::iterator SFiter = m_sf_sets.find(currentEfficiencySystType);
        if (SFiter != m_sf_sets.end()) {
            m_current_sf = SFiter->second;
            if (m_seperateSystBins && currentBinNumber != 0) {
                if (!m_current_sf->SetSystematicBin(currentBinNumber)) {
                    ATH_MSG_WARNING("Could not apply systematic " << EfficiencySystName(currentEfficiencySystType) << " for bin " << currentBinNumber);
                    return SystematicCode::Unsupported;
                }
            }
            return SystematicCode::Ok;
        } else {
            ATH_MSG_ERROR("Illegal combination of systematics passed to the tool! Did you maybe request multiple variations at the same time? You passed " << mySysConf.name());
            ATH_MSG_DEBUG(" List of relevant systematics included in your combination:");
            for (std::set<SystematicVariation>::iterator t = mySysConf.begin(); t != mySysConf.end(); ++t) {
                ATH_MSG_DEBUG("\t" << (*t).name());
            }
            return SystematicCode::Unsupported;
        }
    }
    MuonEfficiencyScaleFactors::MuonEfficiencyScaleFactors(const MuonEfficiencyScaleFactors& toCopy) :
                    MuonEfficiencyScaleFactors(toCopy.name() + "_copy") {
        CopyInformation(toCopy);

    }

    void MuonEfficiencyScaleFactors::CopyInformation(const MuonEfficiencyScaleFactors & toCopy) {
        m_wp = toCopy.m_wp;
        m_sf_sets = toCopy.m_sf_sets;
        m_current_sf = NULL;
        m_custom_dir = toCopy.m_custom_dir;
        m_custom_file_Combined = toCopy.m_custom_file_Combined;
        m_custom_file_Calo = toCopy.m_custom_file_Calo;
        m_custom_file_HighEta = toCopy.m_custom_file_HighEta;
        m_custom_file_LowPt = toCopy.m_custom_file_LowPt;
        m_custom_file_LowPtCalo = toCopy.m_custom_file_LowPtCalo;
        m_version_string = toCopy.m_version_string;
        m_sys_string = toCopy.m_sys_string;
        m_filtered_sys_sets = toCopy.m_filtered_sys_sets;
        m_efficiency_decoration_name_data = toCopy.m_efficiency_decoration_name_data;
        m_efficiency_decoration_name_mc = toCopy.m_efficiency_decoration_name_mc;
        m_sf_decoration_name = toCopy.m_sf_decoration_name;
        m_sf_replica_decoration_name = toCopy.m_sf_replica_decoration_name;
        m_eff_replica_decoration_name = toCopy.m_eff_replica_decoration_name;
        m_mc_eff_replica_decoration_name = toCopy.m_mc_eff_replica_decoration_name;
        m_calibration_version = toCopy.m_calibration_version;
        m_lowpt_threshold = toCopy.m_lowpt_threshold;
        m_Sys1Down = NULL;
        m_Sys1Up = NULL;
        m_Stat1Down = NULL;
        m_Stat1Up = NULL;
        m_LowPtSys1Down = NULL;
        m_LowPtSys1Up = NULL;
        m_LowPtStat1Up = NULL;
        m_current_sf = NULL;
        m_init = false;
        m_seperateSystBins = false;
        m_Type = toCopy.m_Type;
    }

    MuonEfficiencyScaleFactors & MuonEfficiencyScaleFactors::operator =(const MuonEfficiencyScaleFactors & tocopy) {

        if (this == &tocopy) {
            return *this;
        }
        CopyInformation(tocopy);
        return *this;

    }
} /* namespace CP */
