from RecBackgroundAlgs.RecBackgroundAlgsConf import BeamBackgroundFiller
BeamBackgroundFiller=BeamBackgroundFiller()
topSequence+=BeamBackgroundFiller

from RecBackgroundAlgs.RecBackgroundAlgsConf import BackgroundWordFiller
BackgroundWordFiller=BackgroundWordFiller()
BackgroundWordFiller.MBTSBeamVeto_MultiplicityCut=5
topSequence+=BackgroundWordFiller