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

const char* TYPES[] = {
      "INVALID",
      "I1",
      "I8",
      "I16",
      "I32",
      "I64",
      "I128",
      "F16",
      "F32",
      "F64",
      "D32",
      "D64",
      "D128",
      "F128",
      "V128",
      "V256"
};

static void wb_on_load(Addr addr, SizeT size, SizeT type)
{
    
    if (wb_above_main) {
        VG_(fprintf)(wb_output, "{\"action\" : \"mem-load\", \"addr\" : %p, \"size\" : %lu, \"type\" : \"%s\"}\n", (void*)addr, size, TYPES[type]);
    }
}

static void wb_on_store(Addr addr, SizeT size, SizeT type)
{
    if (wb_above_main) {
        
        VG_(fprintf)(wb_output, "{\"action\" : \"mem-store\", \"addr\" : %p, \"size\" : %lu, \"type\" : \"%s\"}\n", (void*)addr, size, TYPES[type]);
        
    }
}

static void wb_on_store_with_data(Addr addr, SizeT size, SizeT type, HWord data)
{
    if (wb_above_main) {
        
        VG_(fprintf)(wb_output, "{\"action\" : \"mem-store\", \"addr\" : %p, \"size\" : %lu, \"type\" : \"%s\", \"data\" : %lu}\n", (void*)addr, size, TYPES[type], data);
        
    }
}

static void wb_instrument_store(IRExpr* dataExpr, IRExpr* addrExpr, IRSB* sbOut, IRExpr* const guard)
{
    IRType type = typeOfIRExpr(sbOut->tyenv, dataExpr);
    Int dataSize = sizeofIRType(type);
    
    IRDirty* di;
    // can emit data?
    if (type == Ity_I64) {
        di = unsafeIRDirty_0_N(0,
                         "wb_on_store_with_data",
                         VG_(fnptr_to_fnentry)(wb_on_store_with_data),
                         mkIRExprVec_4(addrExpr, mkIRExpr_HWord(dataSize), mkIRExpr_HWord(type-Ity_INVALID), dataExpr));
    }
    else {
        di = unsafeIRDirty_0_N(0,
                         "wb_on_store",
                         VG_(fnptr_to_fnentry)(wb_on_store),
                         mkIRExprVec_3(addrExpr, mkIRExpr_HWord(dataSize), mkIRExpr_HWord(type-Ity_INVALID)));
    }
    if (guard)
        di->guard = guard;
   
   addStmtToIRSB(sbOut, IRStmt_Dirty(di) );
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
            wb_instrument_store(data, addr, sbOut, NULL);
        }
        
        // Store with guard
        else if (st->tag == Ist_StoreG) {
            IRStoreG* sg   = st->Ist.StoreG.details;
            IRExpr*   data = sg->data;
            IRExpr*   addr = sg->addr;
            wb_instrument_store(data, addr, sbOut, sg->guard);
        }
        
        // Load with guard
        else if (st->tag == Ist_LoadG) {
            IRLoadG* lg        = st->Ist.LoadG.details;
            IRType   type      = Ity_INVALID; /* loaded type */
            IRType   typeWide  = Ity_INVALID; /* after implicit widening */
            IRExpr*  addr_expr = lg->addr;
            typeOfIRLoadGOp(lg->cvt, &typeWide, &type);
            tl_assert(type != Ity_INVALID);

            IRDirty* di = unsafeIRDirty_0_N(
                    0, "wb_on_load",
                    VG_(fnptr_to_fnentry)( &wb_on_load ), 
                    mkIRExprVec_3(addr_expr, mkIRExpr_HWord(sizeofIRType(type)), mkIRExpr_HWord(type-Ity_INVALID)) );
            di->guard = lg->guard;
            addStmtToIRSB( sbOut, IRStmt_Dirty(di) );
        }
        
        // Wrtie temp
        else if (st->tag == Ist_WrTmp) {
            const IRExpr* const data = st->Ist.WrTmp.data;
            IRExpr* addr_expr = data->Iex.Load.addr;
            if (data->tag == Iex_Load) {
                IRType type = data->Iex.Load.ty;
                IRDirty* di = unsafeIRDirty_0_N(
                        0, "wb_on_load",
                        VG_(fnptr_to_fnentry)( &wb_on_load ), 
                        mkIRExprVec_3(addr_expr, mkIRExpr_HWord(sizeofIRType(type)), mkIRExpr_HWord(type-Ity_INVALID)) );
                addStmtToIRSB( sbOut, IRStmt_Dirty(di) );
            }
        }
        
        addStmtToIRSB( sbOut, st );
    }
    
    return sbOut;
}

void wb_instrument_print_stats(void)
{
    VG_(printf)("the end, superblocks=%d, imarks=%d\n", superblock_counter, imark_counter);
}