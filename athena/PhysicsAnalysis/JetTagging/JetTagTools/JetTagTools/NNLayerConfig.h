/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// WARNING: this code was copied automatically from
// https://github.com/lwtnn/lwtnn.git (rev v2.0)
// Please don't edit it! To get the latest version, run
// > ./update-lwtnn.sh
// from JetTagTools/share

#ifndef NN_LAYER_CONFIG_HH
#define NN_LAYER_CONFIG_HH

// Layer Configiruation for Lightweight Tagger
//
// The structures below are used to initalize
// `LightweightNeuralNetwork` and the simpler `Stack`.
//
// Author: Dan Guest <dguest@cern.ch>

#include <vector>
#include <string>
#include <map>

namespace lwt {
  enum class Activation {NONE, LINEAR, SIGMOID, RECTIFIED, SOFTMAX, TANH,
      HARD_SIGMOID};
  enum class Architecture {NONE, DENSE, NORMALIZATION, MAXOUT, HIGHWAY, 
      LSTM, GRU, EMBEDDING};
  // components (for LSTM, etc)
  enum class Component {
    I, O, C, F,                 // LSTM
      Z, R, H,                  // GRU
      T, CARRY};                // Highway

  // structure for embedding layers
  struct EmbeddingConfig
  {
    std::vector<double> weights;
    int index;
    int n_out;
  };

  // main layer configuration
  struct LayerConfig
  {
    // dense layer info
    std::vector<double> weights;
    std::vector<double> bias;
    std::vector<double> U;      // TODO: what is this thing called in LSTMs?
    Activation activation;
    Activation inner_activation; // for LSTMs and GRUs

    // additional info for sublayers
    std::vector<LayerConfig> sublayers;
    std::map<Component, LayerConfig> components;
    std::vector<EmbeddingConfig> embedding;

    // arch flag
    Architecture architecture;
  };

  struct Input
  {
    std::string name;
    double offset;
    double scale;
  };
}

#endif
