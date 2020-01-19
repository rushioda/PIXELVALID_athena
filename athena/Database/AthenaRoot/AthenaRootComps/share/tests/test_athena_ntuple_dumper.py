from glob import glob
if 1:
    _cbntfiles = [
        'root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/ntuple.0.root',
        'root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/ntuple.1.root',
        ]
    ## _cbntfiles = [
    ##     '/tmp/binet/data/ntuple.0.root',
    ##     '/tmp/binet/data/ntuple.1.root',
    ##     ]
    
if 0:
    _cbntfiles = [
        'root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/small.ntuple.0.root',
        'root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/small.ntuple.1.root',
        ]
    _cbntfiles = [
        '/tmp/binet/data/small.ntuple.0.root',
        '/tmp/binet/data/small.ntuple.1.root',
        ]
if not 'FNAMES' in dir():
    FNAMES = _cbntfiles[:]

if not isinstance(FNAMES, (list,tuple)):
    FNAMES = [FNAMES]
    pass

import AthenaRootComps.ReadAthenaRoot
svcMgr.EventSelector.InputCollections = FNAMES
svcMgr.EventSelector.TupleName = "egamma"

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

import AthenaRootComps.AthenaRootCompsConf as arcc
if not 'USEVARHANDLE' in dir():
    USEVARHANDLE=1
if USEVARHANDLE: job += arcc.Athena__RootAsciiDumperAlgHandle("rootdumper")
else:            job += arcc.Athena__RootAsciiDumperAlg      ("rootdumper")

if not 'EVTMAX' in dir():
    EVTMAX=-1
theApp.EvtMax = EVTMAX

if not 'DOWRITE' in dir():
    DOWRITE=1
if DOWRITE:
    svcMgr += CfgMgr.DecisionSvc()
    import AthenaRootComps.WriteAthenaRoot as arcw
    out = arcw.createNtupleOutputStream("StreamD3PD", "d3pd.root", "egamma")
    out.ItemList += [
        "el_n",
        "el_eta",
        "el_jetcone_dr",
        ]
    out.ForceRead = True
    if DOWRITE>1:
        # another output stream... with the same content.
        out = arcw.createNtupleOutputStream("StreamD3PD_2",
                                            "d3pd_2.root",
                                            "egamma")
        out.ItemList += [
            "el_n",
            "el_eta",
            "el_jetcone_dr",
            ]
        out.ForceRead = True
        
    pass

