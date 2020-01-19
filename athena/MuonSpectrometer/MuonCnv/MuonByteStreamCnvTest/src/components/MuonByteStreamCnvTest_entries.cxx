#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonByteStreamCnvTest/MdtDigitToMdtRDO.h"
#include "MuonByteStreamCnvTest/RpcDigitToRpcRDO.h"
#include "MuonByteStreamCnvTest/TgcDigitToTgcRDO.h"
#include "MuonByteStreamCnvTest/CscDigitToCscRDO.h"

#include "MuonByteStreamCnvTest/ReadMdtRDO.h"
#include "MuonByteStreamCnvTest/ReadRpcRDO.h"
#include "MuonByteStreamCnvTest/ReadTgcRDO.h"

#include "MuonByteStreamCnvTest/ReadRpcDigit.h"
#include "MuonByteStreamCnvTest/ReadMdtDigit.h"
#include "MuonByteStreamCnvTest/ReadTgcDigit.h"
#include "MuonByteStreamCnvTest/ReadCscDigit.h"

#include "MuonByteStreamCnvTest/ReadMdtPRD.h"

#include "MuonByteStreamCnvTest/MuonRdoToMuonDigit.h"
#include "MuonByteStreamCnvTest/MuonRdoToMuonDigitTool.h"

DECLARE_ALGORITHM_FACTORY( RpcDigitToRpcRDO )
DECLARE_ALGORITHM_FACTORY( MdtDigitToMdtRDO )
DECLARE_ALGORITHM_FACTORY( TgcDigitToTgcRDO )
DECLARE_ALGORITHM_FACTORY( CscDigitToCscRDO )

DECLARE_ALGORITHM_FACTORY( ReadRpcRDO )
DECLARE_ALGORITHM_FACTORY( ReadMdtRDO )
DECLARE_ALGORITHM_FACTORY( ReadTgcRDO )

DECLARE_ALGORITHM_FACTORY( ReadRpcDigit )
DECLARE_ALGORITHM_FACTORY( ReadMdtDigit )
DECLARE_ALGORITHM_FACTORY( ReadTgcDigit )
DECLARE_ALGORITHM_FACTORY( ReadCscDigit )
DECLARE_ALGORITHM_FACTORY( ReadMdtPRD )
DECLARE_ALGORITHM_FACTORY( MuonRdoToMuonDigit )

DECLARE_TOOL_FACTORY( MuonRdoToMuonDigitTool )

DECLARE_FACTORY_ENTRIES(MuonByteStreamCnvTest) {

    DECLARE_ALGORITHM ( RpcDigitToRpcRDO )
    DECLARE_ALGORITHM ( MdtDigitToMdtRDO )
    DECLARE_ALGORITHM ( TgcDigitToTgcRDO )
    DECLARE_ALGORITHM ( CscDigitToCscRDO )

    DECLARE_ALGORITHM ( ReadRpcRDO )
    DECLARE_ALGORITHM ( ReadMdtRDO )
    DECLARE_ALGORITHM ( ReadTgcRDO )

    DECLARE_ALGORITHM ( ReadRpcDigit )
    DECLARE_ALGORITHM ( ReadMdtDigit )
    DECLARE_ALGORITHM ( ReadTgcDigit )
    DECLARE_ALGORITHM ( ReadCscDigit )

    DECLARE_ALGORITHM ( ReadMdtPRD )

    DECLARE_ALGORITHM ( MuonRdoToMuonDigit )
 
    DECLARE_TOOL( CscDigitToCscRDOTool )
    DECLARE_TOOL( MuonRdoToMuonDigitTool )
}

