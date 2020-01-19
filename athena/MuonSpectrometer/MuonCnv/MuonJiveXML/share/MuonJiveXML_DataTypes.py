#Avoid multiple includes
include.block("MuonJiveXML/MuonJiveXML_DataTypes.py")

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

theEventData2XML.DataTypes += ["JiveXML::MdtPrepDataRetriever/MdtPrepDataRetriever"]
theEventData2XML.DataTypes += ["JiveXML::TgcPrepDataRetriever/TgcPrepDataRetriever"]
theEventData2XML.DataTypes += ["JiveXML::CscPrepDataRetriever/CscPrepDataRetriever"]
theEventData2XML.DataTypes += ["JiveXML::CSCClusterRetriever/CSCClusterRetriever"]

### RpcPrepData and TrigRpc share datatype 'RPC'. Must not be run together !
### RPCPrepData: default 'plain' retriever
### TrigRPC: new retrievers for L1Muon details
#if rec.doMuonJiveXMLTrigRPC():
#   theEventData2XML.DataTypes += ["JiveXML::TrigRpcDataRetriever/TrigRpcDataRetriever"]
#else:

theEventData2XML.DataTypes += ["JiveXML::RpcPrepDataRetriever/RpcPrepDataRetriever"]

#
#theEventData2XML.DataTypes += ["JiveXML::TrigMuonROIRetriever/TrigMuonROIRetriever"]

# Dataretrievers that are not enabled by default
#theEventData2XML.DataTypes += ["JiveXML::MuidTrackRetriever/MuidTrackRetriever];
 
# Configuration of the data retrievers can be done as follows:
from MuonJiveXML.MuonJiveXMLConf import JiveXML__MdtPrepDataRetriever
theMdtPrepDataRetriever = JiveXML__MdtPrepDataRetriever (name = "MdtPrepDataRetriever")
#theMdtPrepDataRetriever.StoreGateKey = "MDT_DriftCircles"
ToolSvc += theMdtPrepDataRetriever
