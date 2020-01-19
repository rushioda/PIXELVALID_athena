
try:
    import google.protobuf
except:
    import sys
    sys.path.insert(1, '/cvmfs/atlas.cern.ch/repo/sw/external/protobuf/2.3.0/lib/python2.7/site-packages/')
    import google.protobuf
    
import EventIndexProducer.EventIndex_pb2 as EventIndex_pb2


class EIPBof():

    def __init__(self, version):
        self.version = version
        if version <= 0 or version > 1:
            raise RuntimeError("Invalid SPB version")

    def Header(self):
        if self.version == 1:
            return EventIndex_pb2.Header_p1()
        else:
            return None
        
    def Trailer(self):
        if self.version == 1:
            return EventIndex_pb2.Trailer_p1()
        else:
            return None
        
    def BeginGUID(self):
        if self.version == 1:
            return EventIndex_pb2.BeginGUID_p1()
        else:
            return None

    def EndGUID(self):
        if self.version == 1:
            return EventIndex_pb2.EndGUID_p1()
        else:
            return None
    
    def TriggerMenu(self):
        if self.version == 1:
            return EventIndex_pb2.TriggerMenu_p1()
        else:
            return None

    def EIEvent(self):
        if self.version == 1:
            return EventIndex_pb2.EIEvent_p1()
        else:
            return None
    
