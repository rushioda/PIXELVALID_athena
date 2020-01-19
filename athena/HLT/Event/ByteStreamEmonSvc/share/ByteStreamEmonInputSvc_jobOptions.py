###############################################################
#
# Job options file for reading bytestream events from emon
#==============================================================

# get a handle on the ServiceManager
svcMgr = theApp.serviceMgr()

# Services
from ByteStreamEmonSvc.ByteStreamEmonSvcConf import ByteStreamEmonInputSvc
svcMgr += ByteStreamEmonInputSvc("ByteStreamInputSvc")
from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
svcMgr += ROBDataProviderSvc()
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import EventSelectorByteStream
svcMgr += EventSelectorByteStream("EventSelector")
theApp.EvtSel = "EventSelector"

# for EventType
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()

# Properties 
EventSelector = svcMgr.EventSelector
EventSelector.ByteStreamInputSvc     = "ByteStreamInputSvc"; 
EventSelector.FileBased = False
EventPersistencySvc = svcMgr.EventPersistencySvc
EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

# ByteStreamAddressProviderSvc
from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
svcMgr += ByteStreamAddressProviderSvc()
ByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc

# proxy provider
from SGComps.SGCompsConf import ProxyProviderSvc
svcMgr += ProxyProviderSvc()
ProxyProviderSvc = svcMgr.ProxyProviderSvc
ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]
