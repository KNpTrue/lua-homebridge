// Copyright (c) 2021 KNpTrue and homekit-bridge contributors
//
// Licensed under the MIT License.
// You may not use this file except in compliance with the License.
// See [CONTRIBUTORS.md] for the list of homekit-bridge project authors.

#include <mbedtls/cipher.h>
#include <pal/cipher.h>
#include <pal/memory.h>

struct pal_cipher_ctx {
    mbedtls_cipher_context_t ctx;
};

static const mbedtls_cipher_type_t pal_cipher_mbedtls_types[] = {
    [PAL_CIPHER_TYPE_AES_128_CBC] = MBEDTLS_CIPHER_AES_128_CBC,
};

static const mbedtls_cipher_padding_t pal_cipher_mbedtls_paddings[] = {
    [PAL_CIPHER_PADDING_NONE] = MBEDTLS_PADDING_NONE,
    [PAL_CIPHER_PADDING_PKCS7] = MBEDTLS_PADDING_PKCS7,
    [PAL_CIPHER_PADDING_ISO7816_4] = MBEDTLS_PADDING_ONE_AND_ZEROS,
    [PAL_CIPHER_PADDING_ANSI923] = MBEDTLS_PADDING_ZEROS_AND_LEN,
    [PAL_CIPHER_PADDING_ZERO] = MBEDTLS_PADDING_ZEROS,
};

static const mbedtls_operation_t pal_cipher_mbedtls_ops[] = {
    [PAL_CIPHER_OP_ENCRYPT] = MBEDTLS_ENCRYPT,
    [PAL_CIPHER_OP_DECRYPT] = MBEDTLS_DECRYPT,
};

pal_cipher_ctx *pal_cipher_new(pal_cipher_type type) {
    HAPPrecondition(type > PAL_CIPHER_TYPE_NONE && type < PAL_CIPHER_TYPE_MAX);
    pal_cipher_ctx *ctx = pal_mem_alloc(sizeof(*ctx));
    if (!ctx) {
        return NULL;
    }

    mbedtls_cipher_init(&ctx->ctx);
    mbedtls_cipher_setup(&ctx->ctx,
        mbedtls_cipher_info_from_type(pal_cipher_mbedtls_types[type]));
    return ctx;
}

void pal_cipher_free(pal_cipher_ctx *ctx) {
    if (!ctx) {
        return;
    }
    mbedtls_cipher_free(&ctx->ctx);
}

bool pal_cipher_reset(pal_cipher_ctx *ctx) {
    HAPPrecondition(ctx);

    return mbedtls_cipher_reset(&ctx->ctx) == 0;
}

size_t pal_cipher_get_block_size(pal_cipher_ctx *ctx) {
    HAPPrecondition(ctx);

    return mbedtls_cipher_get_block_size(&ctx->ctx);
}

size_t pal_cipher_get_key_len(pal_cipher_ctx *ctx) {
    HAPPrecondition(ctx);

    return mbedtls_cipher_get_key_bitlen(&ctx->ctx) / 8;
}

size_t pal_cipher_get_iv_len(pal_cipher_ctx *ctx) {
    HAPPrecondition(ctx);

    return mbedtls_cipher_get_key_bitlen(&ctx->ctx);
}

bool pal_cipher_set_padding(pal_cipher_ctx *ctx, pal_cipher_padding padding) {
    HAPPrecondition(ctx);
    HAPPrecondition(padding >= PAL_CIPHER_PADDING_NONE &&
        padding < PAL_CIPHER_PADDING_MAX);

    return mbedtls_cipher_set_padding_mode(&ctx->ctx, pal_cipher_mbedtls_paddings[padding]);
}

bool pal_cipher_begin(pal_cipher_ctx *ctx, pal_cipher_operation op, const uint8_t *key, const uint8_t *iv) {
    HAPPrecondition(ctx);
    HAPPrecondition(mbedtls_cipher_get_operation(&ctx->ctx) == MBEDTLS_OPERATION_NONE);
    HAPPrecondition(op > PAL_CIPHER_OP_NONE && op < PAL_CIPHER_OP_MAX);

    if (mbedtls_cipher_set_iv(&ctx->ctx, iv, mbedtls_cipher_get_iv_size(&ctx->ctx))) {
        return false;
    }
    return mbedtls_cipher_setkey(&ctx->ctx, key,
        mbedtls_cipher_get_key_bitlen(&ctx->ctx), pal_cipher_mbedtls_ops[op]) == 0;
}

bool pal_cipher_update(pal_cipher_ctx *ctx, const void *in, size_t ilen, void *out, size_t *olen) {
    HAPPrecondition(ctx);
    HAPPrecondition(mbedtls_cipher_get_operation(&ctx->ctx) != MBEDTLS_OPERATION_NONE);
    HAPPrecondition(in);
    HAPPrecondition(out);

    return mbedtls_cipher_update(&ctx->ctx, in, ilen, out, olen) == 0;
}

bool pal_cipher_finsh(pal_cipher_ctx *ctx, void *out, size_t *olen) {
    HAPPrecondition(ctx);
    HAPPrecondition(mbedtls_cipher_get_operation(&ctx->ctx) != MBEDTLS_OPERATION_NONE);
    HAPPrecondition(out);

    return mbedtls_cipher_finish(&ctx->ctx, out, olen) == 0;
}
