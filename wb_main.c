// Whiteboard
// (C) Maciej Gajewski, 2016

#include "wb_alloctraces.h"
#include "wb_instrumentation.h"
#include "wb_global_state.h"

#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"
#include "pub_tool_vki.h"
#include "pub_tool_libcprint.h"
#include "pub_tool_libcassert.h"



/// out ////
VgFile* wb_output = NULL;



///////////////////////// inits ///////////////////////////////
static void wb_post_clo_init(void)
{
    wb_output = VG_(fopen)("/dev/stdout", VKI_O_CREAT|VKI_O_TRUNC|VKI_O_WRONLY, VKI_S_IRUSR|VKI_S_IWUSR);
    if (wb_output == NULL)
    {
        VG_(tool_panic)("unable to open output file");
    }
}


static void wb_fini(Int exitcode)
{
    VG_(fclose) (wb_output);
    wb_instrument_print_stats();
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
