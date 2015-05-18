#include <stdlib.h>
#include <stdio.h>      /* fgets, sscanf */
#include <string.h>     /* strcmp */
#include "lz4frame.h"

#define CHECK(value) { if (((int)(value)) < 0) {printf("ERROR ON LINE %d\n", __LINE__); exit(1);} }

int main(int argc, char** argv)
{
    char srcBuffer[1024 * 1024];
    // Fill source array by pseudo random data.
    unsigned long rnd = 42;
    for (size_t i = 0; i < sizeof(srcBuffer); ++i) {
        srcBuffer[i] = (char)rnd;
        rnd = 1664525L * rnd + 1013904223L;
    }

    LZ4F_compressionContext_t ctx;
    LZ4F_preferences_t prefs;
    memset(&prefs, 0, sizeof(prefs));
    prefs.autoFlush = 0;
    prefs.compressionLevel = 1;
    prefs.frameInfo.blockMode = blockLinked;
    prefs.frameInfo.blockSizeID = max64KB;
    prefs.frameInfo.contentChecksumFlag = contentChecksumEnabled;

for (int x = 0; x < 5000; ++x) {
    CHECK(LZ4F_createCompressionContext(&ctx, LZ4F_VERSION));
    // Try compress data.
    size_t srcSize = 32 * 1024;
    size_t dstSize = LZ4F_compressBound(srcSize, &prefs);
    char *dstBuffer = malloc(dstSize);
    printf("test: %d, %d\n", x, dstSize);

    CHECK(LZ4F_compressBegin(ctx, dstBuffer, dstSize, &prefs));
    size_t offset = 0;
    for (int i = 0; i < 3; ++i)
    {
        CHECK(LZ4F_compressUpdate(ctx, dstBuffer, dstSize, srcBuffer + offset, srcSize, NULL));
        offset += srcSize;
    }
    CHECK(LZ4F_compressUpdate(ctx, dstBuffer, dstSize, srcBuffer + offset, srcSize - x, NULL));
    CHECK(LZ4F_flush(ctx, dstBuffer, dstSize, NULL));
    CHECK(LZ4F_compressEnd(ctx, dstBuffer, dstSize, NULL));
    CHECK(LZ4F_freeCompressionContext(ctx));
}
    return 0;
}
