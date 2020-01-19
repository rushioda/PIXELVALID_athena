#*******************************************************************
#
# JobOption to analyse LAr testbeam data :  LArDigit to  LArRawChannel
#
#
#*******************************************************************

#if not doSim:

theApp.Dlls += [ "LArByteStream" ]
# read LArDigit
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
ToolSvc = Service( "ToolSvc" )

# read TDC
#abc ByteStreamAddressProviderSvc.TypeNames += ["TBTDC/LArTBTDC"] 
#abc ToolSvc.TBByteStreamCnvTool.SubDetID = 129

theApp.Dlls += ["LArRawUtils", "LArROD", "LArTools" , "LArEventTest","LArCalibUtils" ]

#abc LArDigit->LArRawChannel part 1 : Digit pre-processor 

#abc theApp.TopAlg += [
#abc     "LArDigitPreProcessor<LArDigitContainer>/LArDigitPreProcessor"]
#abc LArDigitPreprocessor = Algorithm( "LArDigitPreprocessor" )
#abc LArDigitPreprocessor.NumberOfSamples = 5
#abc LArDigitPreprocessor.FirstSample = 1
#abc LArDigitPreprocessor.InputContainers += [ "FREE" ]
#abc LArDigitPreprocessor.OutputContainer = "LArDigit"

#abc  LArDigit->LArRawChannel part 2 : raw channel builder
#----- Check LAr LVL1 and BCID consistency
theApp.TopAlg += [ "CheckLArFebHeader" ]
include("LArConditionsCommon/LArConditionsCommon_H8_jobOptions.py") 
#abc  IOVDbSvc.GlobalTag = "TB04-4"

theApp.TopAlg += [ "LArTimeChecker" ]
LArTimeChecker = Algorithm( "LArTimeChecker" )
LArTimeChecker.DataLocation = "FREE"


theApp.TopAlg += [ "LArRawChannelBuilder" ]
LArRawChannelBuilder = Algorithm("LArRawChannelBuilder");
LArRawChannelBuilder.DataLocation = "FREE" 
LArRawChannelBuilder.UseTDC=True


ToolSvc.LArADC2MeV.BeginRunPriority = 100
ToolSvc.LArRodDecoder.FirstSample=2
# Turn off printing for LArRoI_Map
ToolSvc.LArRoI_Map.Print=FALSE; 

# write out a list of all Storegate collection with their keys and
# lock/unlock state. Very useful for debugging purpose
#
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE
#DetectorStore = Service( "DetectorStore" )
#DetectorStore.Dump = TRUE
#ConditionsStore = Service( "ConditionsStore" )
#ConditionsStore.Dump = TRUE


