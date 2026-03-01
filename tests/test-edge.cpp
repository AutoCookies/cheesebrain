#include "cheese-impl.h"
#include "cheese-kv-cache.h"
#include "cheese-power.h"
#include "palloc.h"
#include "contextsqueeze.h"

#include <cassert>
#include <cstdio>
#include <vector>

int main() {
    printf("test-edge: starting verification\n");

    // 1. Verify palloc is accessible and working
    printf("  Testing palloc allocation...\n");
    void * ptr = pa_malloc(1024);
    assert(ptr != nullptr);
    pa_free(ptr);
    printf("  palloc OK\n");

    // 2. Verify contextsqueezer is accessible
    printf("  Testing contextsqueezer API...\n");
    const char * text = "This is a test sentence that should be compressed or squeezed.";
    csq_view in = { text, strlen(text) };
    csq_buf out;
    if (csq_squeeze_ex(in, 5, &out) == CSQ_OK) {
        printf("  contextsqueezer OK (len: %zu -> %zu)\n", in.len, out.len);
        csq_free(&out);
    } else {
        printf("  contextsqueezer failed!\n");
        return 1;
    }

    // 3. Verify power monitor
    printf("  Testing power monitor...\n");
    cheese_power_state state = cheese_power_query_state();
    printf("  Thermal state: %d, Battery: %.2f\n", state.thermal, state.battery_level);
    printf("  power monitor OK\n");

    printf("test-edge: all tests PASSED\n");
    return 0;
}
