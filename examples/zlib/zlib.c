#include <emscripten.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>

typedef struct {
    char* data;
    int length;
} CompressResult;

EMSCRIPTEN_KEEPALIVE
CompressResult* compress_data(const char* data) {
    uLong source_length = strlen(data);
    uLong dest_length = compressBound(source_length);
    unsigned char* compressed = (unsigned char*)malloc(dest_length);

    if (compressed == NULL) {
        return NULL;
    }

    int result = compress(compressed, &dest_length, (const unsigned char*)data, source_length);
    if (result != Z_OK) {
        free(compressed);
        return NULL;
    }

    CompressResult* compress_result = (CompressResult*)malloc(sizeof(CompressResult));
    if (compress_result == NULL) {
        free(compressed);
        return NULL;
    }

    compress_result->data = (char*)compressed;
    compress_result->length = dest_length;

    return compress_result;
}

EMSCRIPTEN_KEEPALIVE
CompressResult* decompress_data(const char* compressed_data, int compressed_length) {
    uLong source_length = compressed_length;
    uLong dest_length = source_length * 2;  // 예상 크기, 필요에 따라 조정
    unsigned char* decompressed = (unsigned char*)malloc(dest_length);

    if (decompressed == NULL) {
        return NULL;
    }

    int result;
    do {
        result = uncompress(decompressed, &dest_length, (const unsigned char*)compressed_data, source_length);
        if (result == Z_BUF_ERROR) {
            dest_length *= 2;
            unsigned char* new_buffer = (unsigned char*)realloc(decompressed, dest_length);
            if (new_buffer == NULL) {
                free(decompressed);
                return NULL;
            }
            decompressed = new_buffer;
        } else if (result != Z_OK) {
            free(decompressed);
            return NULL;
        }
    } while (result == Z_BUF_ERROR);

    CompressResult* decompress_result = (CompressResult*)malloc(sizeof(CompressResult));
    if (decompress_result == NULL) {
        free(decompressed);
        return NULL;
    }

    decompress_result->data = (char*)decompressed;
    decompress_result->length = dest_length;

    return decompress_result;
}

EMSCRIPTEN_KEEPALIVE
void free_compress_result(CompressResult* result) {
    if (result) {
        free(result->data);
        free(result);
    }
}