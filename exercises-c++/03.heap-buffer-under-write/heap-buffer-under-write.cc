#include <compartment.h>
#include <debug.hh>
#include <unwind.h>
// REMOVED: #include <fail-simulator-on-error.h>

using Debug = ConditionalDebug<true, "Heap Buffer Under Write Compartment">;

int __cheri_compartment("heap-buffer-under-write") vuln1()
{
    Debug::log("Testing Heap Buffer Under-write (C++)...");
    int* arr = new int[3];
    if (!arr)
    {
        Debug::log("Allocation failed!");
        return 0;
    }
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    Debug::log("Attempting under-write arr[-1] = 999 ...");
    
    // START MODIFICATION
    CHERIOT_DURING {
        arr[-1] = 999;
        Debug::log("Under-write completed (this should not be printed).");
        Debug::log("Inserted element: {}.", arr[-1]);
    } CHERIOT_HANDLER {
        Debug::log("Error detected: Heap buffer under-write (C++)!");
        delete[] arr;
        return -1;
    } CHERIOT_END_HANDLER
    // END MODIFICATION

    delete[] arr;
    return 0;
}