// Whiteboard
// (C) Maciej Gajewski, 2016

#include "wb_instrumentation.h"

#include "wb_global_state.h"
#include "wb_alloctraces.h"

#include "pub_tool_machine.h"     // VG_(fnptr_to_fnentry)
#include "pub_tool_libcprint.h"
#include "pub_tool_libcbase.h"
#include "pub_tool_debuginfo.h"
#include "pub_tool_mallocfree.h"
#include "pub_tool_libcassert.h"


Bool wb_above_main = False;
Bool wb_inside_user_code = False;

// Mem access tracking ////////////////////

struct MemOp {
    Addr addr;
    SizeT size;
};

static struct MemOp last_write = {0, 0};

static void flush_mem_ops(void)
{
    if (last_write.size > 0 ) {
        VG_(fprintf)(wb_output, "{\"action\" : \"store\", \"addr\" : %p, \"size\" : %lu}\n", (void*)last_write.addr, last_write.size);
    }
    last_write.size = 0;
    last_write.addr = 0;
}

static void wb_on_store(Addr addr, SizeT size)
{
    if (wb_above_main) {
        
        // first write since flush
        if (last_write.addr == 0) {
            last_write.addr = addr;
            last_write.size = size;
        }
        // write imemdiately after the last one
        else if ((last_write.addr + last_write.size) == addr ) {
            last_write.size += size;
        }
        // write immediately preceeding the last one
        else if ((addr + size) == last_write.addr) {
            last_write.addr = addr;
            last_write.size += size;
        }
        // another region, need to report separately
        else {
            flush_mem_ops();
            last_write.addr = addr;
            last_write.size = size;
        }
    }
}


static int superblock_counter = 0;
static int imark_counter = 0;

// source and line tracking /////////////////////
static HChar* main_source_dir = NULL;

struct LastLine {
    const HChar* dirname;
    const HChar* filename;
    UInt linenum;    
};

static struct LastLine last_line = { NULL, NULL, 0 };

static void flush_last_line(void)
{
    if (last_line.linenum > 0) {
        flush_stack();
        flush_mem_ops();
        VG_(fprintf)(wb_output, "{\"action\" : \"line-step\", \"file\" : \"%s\", \"line\" : %u, \"dir\" : \"%s\"}\n", last_line.filename, last_line.linenum, last_line.dirname);
    }
}


static void wb_on_instruction(Addr addr)
{
    Vg_FnNameKind kind =  VG_(get_fnname_kind_from_IP)(addr);
    
    // detect first enter to main
    if (kind == Vg_FnNameMain && wb_above_main == False) {
        wb_above_main = True;
    }
    
    if (kind == Vg_FnNameMain || kind == Vg_FnNameNormal) {
        const HChar* dirname;
        const HChar* filename;
        UInt linenum;
  
        if (VG_(get_filename_linenum)(addr, &filename, &dirname, &linenum)) {
        
            if (main_source_dir == NULL && kind == Vg_FnNameMain) {
                main_source_dir = VG_(strdup) ("main_source_dir", dirname);
            }

            // Only emit lines for files inside main source
            if (main_source_dir && VG_(strcmp(main_source_dir, dirname)) == 0) {
                
                wb_inside_user_code = True;
                
                // line changed?
                if (linenum != last_line.linenum || filename != last_line.filename) {
                    flush_last_line();
                    last_line.linenum = linenum;
                    last_line.filename = filename;
                    last_line.dirname = dirname;
                }
            } else {
                wb_inside_user_code = False; 
            }
        }
    } else {
        wb_above_main = False;
        wb_inside_user_code = False;
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

    // copy all the other statements
    for (/* reused */; i < sbIn->stmts_used; i++) {
        IRStmt* st = sbIn->stmts[i];
        
        // IMark
        if (st->tag == Ist_IMark) {
            imark_counter ++;

            // instrument every IMark
            IRDirty* di = unsafeIRDirty_0_N(
                    0, "wb_on_instruction",  // TODO should this be '1'?
                    VG_(fnptr_to_fnentry)( &wb_on_instruction ), 
                    mkIRExprVec_1(mkIRExpr_HWord( st->Ist.IMark.addr )) );
            addStmtToIRSB( sbOut, IRStmt_Dirty(di) );
            
        }
        // Store
        else if (st->tag == Ist_Store) {
            
            IRExpr* data = st->Ist.Store.data;
            IRExpr* addr = st->Ist.Store.addr;
            IRType type = typeOfIRExpr(sbIn->tyenv, data);
            Int dsize = sizeofIRType(type);
            
//             addEvent_Dw( sbOut, st->Ist.Store.addr,
//                         sizeofIRType(type) );
            tl_assert(isIRAtom(addr));
            tl_assert(dsize >= 1 && dsize <= 512);
            
            IRExpr** argv =  mkIRExprVec_2(addr, mkIRExpr_HWord(dsize));
            IRDirty* di   = unsafeIRDirty_0_N(
                    2, // params 
                    "wb_on_store",
                    VG_(fnptr_to_fnentry)(&wb_on_store),
                    argv );

            addStmtToIRSB( sbOut, IRStmt_Dirty(di) );
        }
        
        addStmtToIRSB( sbOut, st );
    }
    
    return sbOut;
}

void wb_instrument_print_stats(void)
{
    VG_(printf)("the end, superblocks=%d, imarks=%d\n", superblock_counter, imark_counter);
}