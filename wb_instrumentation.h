// Whiteboard
// (C) Maciej Gajewski, 2016

#pragma once

#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"



IRSB* wb_instrument ( VgCallbackClosure* closure,
        IRSB* sbIn, // superblock (single entry, multiple exit code sequence)
        const VexGuestLayout* layout, 
        const VexGuestExtents* vge, // used once by lackey 
        const VexArchInfo* archinfo_host,
        IRType gWordTy, IRType hWordTy );

void wb_instrument_print_stats(void);
