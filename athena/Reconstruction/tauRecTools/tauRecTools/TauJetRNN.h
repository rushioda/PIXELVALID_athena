/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUJETRNN_H
#define TAUREC_TAUJETRNN_H

#include <memory>

#include "AsgTools/AsgMessaging.h"

#include "xAODTau/TauJet.h"


// Forward declaration
namespace lwt {
    class LightweightGraph;
}

namespace TauJetRNNUtils {
    class VarCalc;
}

/**
 * @brief Wrapper around lwtnn to compute the output score of a neural network
 *
 *   Configures the network and computes the network outputs given the input
 *   objects. Retrieval of input variables is handled internally.
 *
 * @author C. Deutsch
 * @author W. Davey
 *
 */
class TauJetRNN : public asg::AsgMessaging {
public:
    // Configuration of the weight file structure
    struct Config {
        std::string input_layer_scalar;
        std::string input_layer_tracks;
        std::string input_layer_clusters;
        std::string output_layer;
        std::string output_node;
    };

public:
    // Construct a network from the .json specification created by the lwtnn
    // converters (kerasfunc2json.py).
    TauJetRNN(const std::string &filename, const Config &config);
    ~TauJetRNN();

    // Compute the signal probability in [0, 1] or a default value
    float compute(const xAOD::TauJet &tau,
                  const std::vector<const xAOD::TauTrack *> &tracks,
                  const std::vector<const xAOD::CaloCluster *> &clusters);

    // retrieve scalar RNN inputs for tau trigger monitoring
    const std::map<std::string, std::map<std::string, double> >* getScalarInputs() {
      return &m_input_map;
    }
    // retrieve vector RNN inputs for tau trigger monitoring
    const std::map<std::string, std::map<std::string, std::vector<double>> >* getVectorInputs() {
      return &m_input_sequence_map;
    }

    // Getter for the variable calculator
    TauJetRNNUtils::VarCalc *variable_calculator() {
        return m_var_calc.get();
    }

    explicit operator bool() const {
        return static_cast<bool>(m_graph);
    }

private:
    // Abbreviations for lwtnn
    using VariableMap = std::map<std::string, double>;
    using VectorMap = std::map<std::string, std::vector<double>>;

    using InputMap = std::map<std::string, VariableMap>;
    using InputSequenceMap = std::map<std::string, VectorMap>;

private:
    const Config m_config;
    std::unique_ptr<const lwt::LightweightGraph> m_graph;

    // Names of the input variables
    std::vector<std::string> m_scalar_inputs;
    std::vector<std::string> m_track_inputs;
    std::vector<std::string> m_cluster_inputs;

    // Maps to hold input values for lwtnn
    InputMap m_input_map;
    InputSequenceMap m_input_sequence_map;

    // Pointers to contents of the input (sequence) map
    VariableMap *m_scalar_map;
    VectorMap *m_track_map;
    VectorMap *m_cluster_map;

    // Variable calculator to calculate input variables on the fly
    std::unique_ptr<TauJetRNNUtils::VarCalc> m_var_calc;
};

#endif // TAUREC_TAUJETRNN_H
