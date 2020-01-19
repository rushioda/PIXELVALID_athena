/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauJetRNN.h"

#include <algorithm>
#include <fstream>

#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/parse_json.hh"

#include "tauRecTools/TauJetRNNUtils.h"


TauJetRNN::TauJetRNN(const std::string &filename, const Config &config)
    : asg::AsgMessaging("TauJetRNN"), m_config(config), m_graph(nullptr),
    m_scalar_map(nullptr), m_track_map(nullptr), m_cluster_map(nullptr) {
    // Load the json file defining the network
    std::ifstream input_file(filename);
    lwt::GraphConfig lwtnn_config;
    try {
        lwtnn_config = lwt::parse_json_graph(input_file);
    } catch (const std::logic_error &e) {
        ATH_MSG_ERROR("Error parsing network config: " << e.what());
        throw;
    }

    // Search for input layer names specified in 'config'
    auto node_is_scalar = [&config](const lwt::InputNodeConfig &in_node) {
        return in_node.name == config.input_layer_scalar;
    };
    auto node_is_track = [&config](const lwt::InputNodeConfig &in_node) {
        return in_node.name == config.input_layer_tracks;
    };
    auto node_is_cluster = [&config](const lwt::InputNodeConfig &in_node) {
        return in_node.name == config.input_layer_clusters;
    };

    auto scalar_node = std::find_if(lwtnn_config.inputs.cbegin(),
                                    lwtnn_config.inputs.cend(),
                                    node_is_scalar);

    auto track_node = std::find_if(lwtnn_config.input_sequences.cbegin(),
                                   lwtnn_config.input_sequences.cend(),
                                   node_is_track);

    auto cluster_node = std::find_if(lwtnn_config.input_sequences.cbegin(),
                                     lwtnn_config.input_sequences.cend(),
                                     node_is_cluster);

    // Check which input layers were found
    auto has_scalar_node = scalar_node != lwtnn_config.inputs.cend();
    auto has_track_node = track_node != lwtnn_config.input_sequences.cend();
    auto has_cluster_node = cluster_node != lwtnn_config.input_sequences.cend();

    // Fill the variable names of each input layer into the corresponding vector
    if (has_scalar_node) {
        for (const auto &in : scalar_node->variables) {
            m_scalar_inputs.push_back(in.name);
        }
        // Insert key into input map and set the scalar vector pointer
        m_scalar_map = &m_input_map[config.input_layer_scalar];
    }

    if (has_track_node) {
        for (const auto &in : track_node->variables) {
            m_track_inputs.push_back(in.name);
        }
        // Insert key into input sequence map and set the track map pointer
        m_track_map = &m_input_sequence_map[config.input_layer_tracks];
    }

    if (has_cluster_node) {
        for (const auto &in : cluster_node->variables) {
            m_cluster_inputs.push_back(in.name);
        }
        // Insert key into input sequence map and set the cluster map pointer
        m_cluster_map = &m_input_sequence_map[config.input_layer_clusters];
    }

    // Configure the network
    try {
        m_graph = std::make_unique<lwt::LightweightGraph>(
            lwtnn_config, config.output_layer);
    } catch (const lwt::NNConfigurationException &e) {
        ATH_MSG_ERROR(e.what());
        throw;
    }

    // Load the default variable calculator
    m_var_calc = TauJetRNNUtils::get_default_calculator();
}

TauJetRNN::~TauJetRNN() {}

float TauJetRNN::compute(const xAOD::TauJet &tau,
                         const std::vector<const xAOD::TauTrack *> &tracks,
                         const std::vector<const xAOD::CaloCluster *> &clusters) {
    // Populate input (sequence) map with input variables
    for (const auto &varname : m_scalar_inputs) {
        if (!m_var_calc->compute(varname, tau, (*m_scalar_map)[varname])) {
            ATH_MSG_WARNING("Error computing '" << varname
                            << "' returning default");
            return -1111.0;
        }
    }

    for (const auto &varname : m_track_inputs) {
        if (!m_var_calc->compute(varname, tau, tracks,
                                 (*m_track_map)[varname])) {
            ATH_MSG_WARNING("Error computing '" << varname
                            << "' returning default");
            return -1111.0;
        }
    }

    for (const auto &varname : m_cluster_inputs) {
        if (!m_var_calc->compute(varname, tau, clusters,
                                 (*m_cluster_map)[varname])) {
            ATH_MSG_WARNING("Error computing '" << varname
                            << "' returning default");
            return -1111.0;
        }
    }

    // Compute the network outputs
    const auto outputs = m_graph->compute(m_input_map, m_input_sequence_map);
    // Return value of the output neuron
    return outputs.at(m_config.output_node);
}
