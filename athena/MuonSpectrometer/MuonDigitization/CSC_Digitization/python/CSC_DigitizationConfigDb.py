# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("CSC_Digitization.CSC_DigitizationConfig.CscDigitizationTool" , "CscDigitizationTool") 
addTool("CSC_Digitization.CSC_DigitizationConfig.getCscRange"         , "CscRange")
