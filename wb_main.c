// Whiteboard
// (C) Maciej Gajewski, 2016

#include "wb_alloctraces.h"

#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"
#include "pub_tool_replacemalloc.h"
#include "pub_tool_libcprint.h"
#include "pub_tool_vki.h"
#include "pub_tool_libcassert.h"
#include "pub_tool_machine.h"     // VG_(fnptr_to_fnentry)

/// out ////
VgFile* output = NULL;



/////////////////////////////// stack ///////////////////////////
// this guys are called a lot!
static void wb_new_mem_stack(Addr a, SizeT len)
{
    //VG_(fprintf)(output, "{\"action\" : \"new-stack\", \"size\" : %lu, \"addr\" : %p }\n", len, (void*)a);
}

static void wb_die_mem_stack(Addr a, SizeT len)
{
    //VG_(fprintf)(output, "{\"action\" : \"die-stack\", \"size\" : %lu, \"addr\" : %p }\n", len, (void*)a);
}

///////////////////////// inits ///////////////////////////////
static void wb_post_clo_init(void)
{
    output = VG_(fopen)("/dev/stdout", VKI_O_CREAT|VKI_O_TRUNC|VKI_O_WRONLY, VKI_S_IRUSR|VKI_S_IWUSR);
    if (output == NULL)
    {
        VG_(tool_panic)("unable to open output file");
    }
}

static int superblock_counter = 0;
static int imark_counter = 0;
static int sb_enter_counter = 0;

static void wb_sb_entered(void)
{
    sb_enter_counter ++;
}

static
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
        }
        
        addStmtToIRSB( sbOut, st );
    }
    
    return sbOut;
}

static void wb_fini(Int exitcode)
{
    VG_(fclose) (output);
    VG_(printf)("the end, superblocks=%d, imarks=%d, sb entered=%d\n", superblock_counter, imark_counter, sb_enter_counter);
}

static void wb_pre_clo_init(void)
{
    VG_(details_name)            ("Whiteboard");
    VG_(details_version)         (NULL);
    VG_(details_description)     ("TODO");
    VG_(details_copyright_author)(
        "Copyright (C) 2016 Maciej Gajewski");
    VG_(details_bug_reports_to)  (VG_BUGS_TO);

    VG_(details_avg_translation_sizeB) ( 275 );

    VG_(basic_tool_funcs)        (wb_post_clo_init,
                                    wb_instrument,
                                    wb_fini);

    // needs
    VG_(needs_var_info)();
    VG_(needs_libc_freeres)();    // do i
    VG_(needs_cxx_freeres)();    // really need this?
    VG_(needs_malloc_replacement)  (wb_malloc,
        wb___builtin_new,
        wb___builtin_vec_new,
        wb_memalign,
        wb_calloc,
        wb_free,
        wb___builtin_delete,
        wb___builtin_vec_delete,
        wb_realloc,
        wb_malloc_usable_size,
        0 );
    VG_(track_new_mem_stack)(wb_new_mem_stack);
    VG_(track_die_mem_stack)(wb_die_mem_stack);

}

VG_DETERMINE_INTERFACE_VERSION(wb_pre_clo_init)

/*--------------------------------------------------------------------*/
/*--- end                                                          ---*/
/*--------------------------------------------------------------------*/
