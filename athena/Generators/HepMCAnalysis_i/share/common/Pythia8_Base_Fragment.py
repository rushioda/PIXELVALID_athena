## Base config for Pythia8
from Pythia8_i.Pythia8_iConf import Pythia8_i
topAlg += Pythia8_i("Pythia8")
evgenConfig.generators += ["Pythia8"]

topAlg.Pythia8.Commands += [
    "Main:timesAllowErrors = 500",
    "6:m0 = 172.5",
    "23:m0 = 91.1876",
    "23:mWidth = 2.4952",
    "24:m0 = 80.399",
    "24:mWidth = 2.085",
    "StandardModel:sin2thetaW = 0.23113",
    "StandardModel:sin2thetaWbar = 0.23146",
    "ParticleDecays:limitTau0 = on",
    "ParticleDecays:tau0Max = 10.0"]
