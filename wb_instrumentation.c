// Whiteboard
// (C) Maciej Gajewski, 2016

#include "wb_instrumentation.h"

#include "pub_tool_machine.h"     // VG_(fnptr_to_fnentry)
#include "pub_tool_libcprint.h"
#include "pub_tool_debuginfo.h"

extern VgFile* output;

static int superblock_counter = 0;
static int imark_counter = 0;
static int sb_enter_counter = 0;

static void wb_sb_entered(void)
{
    sb_enter_counter ++;
}

static void wb_on_function_call(Addr addr)
{
    const HChar *fnname;
    if (VG_(get_fnname_if_entry)(addr, &fnname)) {
        VG_(fprintf)(output, "{\"action\" : \"function-call\", \"addr\" : %p, \"name\" : \"%s\"}\n", (void*)addr, fnname);
    } else {
        VG_(fprintf)(output, "{\"action\" : \"function-call\", \"addr\" : %p}\n", (void*)addr);
    }
}

IRSB* wb_instrument ( VgCallbackClosure* closure,
        IRSB* sbIn, // superblock (single entry, multiple exit code sequence)
        const VexGuestLayout* layout, 
        const VexGuestExtents* vge, // used once by lackey 
        const VexArchInfo* archinfo_host,
        IRType gWordTy, IRType hWordTy )
{
    superblock_counter ++;
    
    IRSB* sbOut = deepCopyIRSBExceptStmts(sbIn);

    Int i = 0;
    // Copy verbatim any IR preamble preceding the first IMark
    while (i < sbIn->stmts_used && sbIn->stmts[i]->tag != Ist_IMark) {
        addStmtToIRSB( sbOut, sbIn->stmts[i] );
        i++;
    }

    // add block-enter counter just before the first IMark
    {
        IRDirty* di = unsafeIRDirty_0_N( 0, "wb_sb_entered", 
                VG_(fnptr_to_fnentry)( &wb_sb_entered ),
                mkIRExprVec_0() );
        addStmtToIRSB( sbOut, IRStmt_Dirty(di) );
    }
    
    // copy all the other statements
    for (/* reused */; i < sbIn->stmts_used; i++) {
        IRStmt* st = sbIn->stmts[i];
        if (st->tag == Ist_IMark) {
            imark_counter ++;

            // detect function calls
            const HChar *fnname;
            if (VG_(get_fnname_if_entry)(st->Ist.IMark.addr, &fnname)) {
                // TODO pass the param or smthn
                IRDirty* di = unsafeIRDirty_0_N(
                        0, "wb_on_function_call", 
                        VG_(fnptr_to_fnentry)( &wb_on_function_call ), 
                        mkIRExprVec_1(mkIRExpr_HWord( st->Ist.IMark.addr )) );
                addStmtToIRSB( sbOut, IRStmt_Dirty(di) );
            }
            
        }
        
        addStmtToIRSB( sbOut, st );
    }
    
    return sbOut;
}

void wb_instrument_print_stats(void)
{
    VG_(printf)("the end, superblocks=%d, imarks=%d, sb entered=%d\n", superblock_counter, imark_counter, sb_enter_counter);
}