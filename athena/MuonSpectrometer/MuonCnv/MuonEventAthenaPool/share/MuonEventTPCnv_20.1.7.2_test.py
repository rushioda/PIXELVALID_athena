infile = 'esd/ESD-20.1.7.2.pool.root'
keys = [
    #Muon::CscPrepDataContainer_tlp1
    'CSC_Clusters',

    #Muon::CscStripPrepDataContainer_tlp1
    'CSC_Measurements',

    #Muon::CscSimDataCollection_p1
    'CSC_SDO',

    #Muon::MuonPRD_Container_p2<Muon::RpcCoinData_p1>
    'RPC_triggerHits',

    #Muon::MuonPRD_Container_p2<Muon::TgcPrepData_p1>
    'TGC_MeasurementsAllBCs',

    #Muon::MuonPRD_Container_p2<Muon::RpcPrepData_p3>
    'RPC_Measurements',

    #Muon::MuonPRD_Container_p2<Muon::MdtPrepData_p2>
    'MDT_DriftCircles',

    #Muon::TgcCoinDataContainer_tlp3
    'TrigT1CoinDataCollectionNextBC',
    'TrigT1CoinDataCollectionPriorBC',
    'TrigT1CoinDataCollection',

    #Muon::MuonSimDataCollection_p1
    'TGC_SDO',
    'RPC_SDO',
    'MDT_SDO',
         ]

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOn()
include ('AthenaPoolUtilities/TPCnvTest.py')
