#include "InDetTrigPrepRawDataFormat/SCT_TrgClusterization.h"
#include "InDetTrigPrepRawDataFormat/Pixel_TrgClusterization.h"
#include "InDetTrigPrepRawDataFormat/TRT_TrgRIO_Maker.h"
#include "InDetTrigPrepRawDataFormat/FTK_TrackMaker.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, SCT_TrgClusterization )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, Pixel_TrgClusterization )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_TrgRIO_Maker )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, FTK_TrackMaker )

DECLARE_FACTORY_ENTRIES( InDetTrigPrepRawDataFormat )
{
  DECLARE_NAMESPACE_ALGORITHM( InDet, SCT_TrgClusterization )
  DECLARE_NAMESPACE_ALGORITHM( InDet, Pixel_TrgClusterization )
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_TrgRIO_Maker )
  DECLARE_NAMESPACE_ALGORITHM( InDet, FTK_TrackMaker )
}


