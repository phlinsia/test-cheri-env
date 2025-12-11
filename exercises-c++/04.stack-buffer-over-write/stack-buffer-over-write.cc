#include <compartment.h>
#include <debug.hh>
#include <assert.h>
#include <unwind.h>
// REMOVED: #include <fail-simulator-on-error.h>

using Debug = ConditionalDebug<true, "Stack Buffer Over Write Compartment">;

void write_buf(char* buf, size_t ix)
{
    buf[ix] = 'b';
}

int __cheri_compartment("stack-buffer-over-write") vuln1()
{
    Debug::log("Testing Stack Buffer Over Write (C++)...");
    char upper[0x10];
    char lower[0x10];
    Debug::log(
        "upper = {}, lower = {}, diff = {}",
        static_cast<void*>(upper),
        static_cast<void*>(lower),
        static_cast<long>(reinterpret_cast<uintptr_t>(upper) - reinterpret_cast<uintptr_t>(lower))
    );
    assert((uintptr_t)upper == (uintptr_t)&lower[sizeof(lower)]);
    upper[0] = 'a';
    Debug::log("upper[0] = {}", upper[0]);
    
    // START MODIFICATION
    CHERIOT_DURING {
        write_buf(lower, sizeof(lower));
    } CHERIOT_HANDLER {
        Debug::log("Error detected: Stack buffer over-write (C++)!");
        return -1;
    } CHERIOT_END_HANDLER
    // END MODIFICATION

    Debug::log("upper[0] = {}", upper[0]);
    return 0;
}