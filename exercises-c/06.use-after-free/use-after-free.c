#include <compartment.h>
#include <debug.h>
#include <unwind.h>
#include <stdlib.h>


#define DEBUG_CONTEXT "Use After Free Compartment"

/// Thread entry point.
__cheri_compartment("use-after-free") int vuln1()
{
    CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "Testing Use-After-Free (C)...");
    int* ptr = (int*)malloc(sizeof(int));
    if (ptr == NULL) {return 0;}
    *ptr = 123;
    CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "ptr points to memory with value: {}", *ptr);

    free(ptr);
    CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "Memory has been freed.");

    CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "Attempting to dereference dangling pointer... ");
    
    // START MODIFICATION
    CHERIOT_DURING {
        *ptr = 456;
        CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "Value is now: {}", *ptr);
    } CHERIOT_HANDLER {
        CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "Error detected: Use After Free!");
        return -1;
    } CHERIOT_END_HANDLER
    // END MODIFICATION

    return 0;
}