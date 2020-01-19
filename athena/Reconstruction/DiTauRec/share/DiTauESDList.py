################################################################################
##
#@file DiTauESDList.py
#
#@brief List ESD output containers. 
################################################################################

#------------------------------------------------------------------------------
# ESD output list
#------------------------------------------------------------------------------
DiTauESDList = []

#------------------------------------------------------------------------------
# TauRec main xAOD containers
#------------------------------------------------------------------------------
DiTauESDList += [ "xAOD::DiTauJetContainer#DiTauJets" ]
DiTauESDList += [ "xAOD::DiTauJetAuxContainer#DiTauJetsAux." ]