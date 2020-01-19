//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "../EventSelectorByteStream.h"
#include "../ByteStreamMergeOutputSvc.h"
#include "../EventInfoByteStreamCnv.h"
#include "../ByteStreamRDP_OutputSvc.h"
#include "../ByteStreamEventStorageInputSvc.h"
#include "../ByteStreamEventStorageOutputSvc.h"
#include "../ByteStreamSingleEventInputSvc.h"
#include "../ByteStreamNavigationProviderSvc.h"
#include "../ByteStreamOutputStreamCopyTool.h"
#include "../ByteStreamMetadataTool.h"
#include "../ByteStreamAttListMetadataSvc.h"

DECLARE_CONVERTER_FACTORY( EventInfoByteStreamCnv )
DECLARE_SERVICE_FACTORY( ByteStreamCnvSvc )
DECLARE_SERVICE_FACTORY( EventSelectorByteStream )
DECLARE_SERVICE_FACTORY( ByteStreamMergeOutputSvc )
DECLARE_SERVICE_FACTORY( ByteStreamRDP_OutputSvc )
DECLARE_SERVICE_FACTORY( ByteStreamEventStorageInputSvc )
DECLARE_SERVICE_FACTORY( ByteStreamEventStorageOutputSvc )
DECLARE_SERVICE_FACTORY( ByteStreamSingleEventInputSvc )
DECLARE_SERVICE_FACTORY( ByteStreamNavigationProviderSvc )
DECLARE_SERVICE_FACTORY( ByteStreamAttListMetadataSvc )

DECLARE_TOOL_FACTORY( ByteStreamOutputStreamCopyTool )
DECLARE_TOOL_FACTORY( ByteStreamMetadataTool )

DECLARE_FACTORY_ENTRIES( ByteStreamCnvSvc ) {
    DECLARE_SERVICE    ( ByteStreamCnvSvc )
    DECLARE_SERVICE    ( EventSelectorByteStream )
    DECLARE_SERVICE    ( ByteStreamMergeOutputSvc )
    DECLARE_SERVICE    ( ByteStreamRDP_OutputSvc )
    DECLARE_CONVERTER  ( EventInfoByteStreamCnv )
    DECLARE_SERVICE    ( ByteStreamEventStorageInputSvc )
    DECLARE_SERVICE    ( ByteStreamEventStorageOutputSvc )
    DECLARE_SERVICE    ( ByteStreamSingleEventInputSvc )
    DECLARE_SERVICE    ( ByteStreamNavigationProviderSvc )
    DECLARE_SERVICE    ( ByteStreamAttListMetadataSvc )

    DECLARE_TOOL    ( ByteStreamOutputStreamCopyTool )
    DECLARE_TOOL    ( ByteStreamMetadataTool )
}
