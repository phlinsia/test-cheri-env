#include <compartment.h>
#include <debug.hh>
#include <unwind.h>
// REMOVED: #include <fail-simulator-on-error.h>

/// Expose debugging features unconditionally for this compartment.
using Debug = ConditionalDebug<true, "Heap Buffer Over Read Compartment">;


/// Thread entry point.
int __cheri_compartment("heap-buffer-over-read") vuln1()
{
    Debug::log("Running Buffer Over-read (C++)...");
    int* arr = new int[3];
    if (arr == nullptr)
    {
        Debug::log("Allocation failed!");
        return -1;
    }
    Debug::log("Array created, assigning values...");
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    Debug::log("Accessing arr[10] (out-of-bounds)...");
    
    // START MODIFICATION
    CHERIOT_DURING {
        int value = arr[10]; // Should fault
        Debug::log("Value: {} (This should not be printed)", value);
    } CHERIOT_HANDLER {
        Debug::log("Error detected: Heap buffer over-read (C++)!");
        delete[] arr;
        return -1;
    } CHERIOT_END_HANDLER
    // END MODIFICATION

    delete[] arr;
    Debug::log("Completed without crashing (unexpected)");
    return 0;
}