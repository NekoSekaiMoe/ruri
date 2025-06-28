// SPDX-License-Identifier: MIT
/*
 *
 * This file is part of libk2v, with ABSOLUTELY NO WARRANTY.
 *
 * MIT License
 *
 * Copyright (c) 2024 Moe-hacker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 */
#pragma once
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
// Bool!!!
#if __STDC_VERSION__ < 202000L
#ifndef bool
#define bool _Bool
#define true ((_Bool) + 1u)
#define false ((_Bool) + 0u)
#endif
#endif
#ifndef _Nullable
#define _Nullable
#endif
#ifndef _Nonnull
#define _Nonnull
#endif
// Version info.
#define LIBK2V_MAJOR 2
#define LIBK2V_MINOR 2
// Warning.
extern bool k2v_stop_at_warning;
extern bool k2v_show_warning;
// Functions.
char *key_get_char(const char *_Nonnull key, const char *_Nonnull buf);
int key_get_int(const char *_Nonnull key, const char *_Nonnull buf);
float key_get_float(const char *_Nonnull key, const char *_Nonnull buf);
bool key_get_bool(const char *_Nonnull key, const char *_Nonnull buf);
int key_get_int_array(const char *_Nonnull key, const char *_Nonnull buf, int *_Nonnull array, int limit);
int key_get_char_array(const char *_Nonnull key, const char *_Nonnull buf, char *_Nonnull array[], int limit);
int key_get_float_array(const char *_Nonnull key, const char *_Nonnull buf, float *_Nonnull array, int limit);
bool have_key(const char *_Nonnull key, const char *_Nonnull buf);
char *k2v_open_file(const char *_Nonnull path, size_t bufsize);
void k2v_to_shell(const char *_Nonnull buf);
char *char_to_k2v(const char *_Nonnull key, const char *val);
char *int_to_k2v(const char *_Nonnull key, int val);
char *bool_to_k2v(const char *_Nonnull key, bool val);
char *float_to_k2v(const char *_Nonnull key, float val);
char *char_array_to_k2v(const char *_Nonnull key, char *const *_Nonnull val, int len);
char *int_array_to_k2v(const char *_Nonnull key, int *_Nonnull val, int len);
char *float_array_to_k2v(const char *_Nonnull key, float *_Nonnull val, int len);
size_t k2v_get_filesize(const char *_Nonnull path);
#define k2v_get_key(type, ...) key_get_##type(__VA_ARGS__)
/* Helper function to handle the k2v configuration addition */
static inline char* k2v_add_config_impl(char* k2v_buf, char* k2v_tmp) {
    size_t k2v_size = 4;
    char* k2v_ret;

    if (k2v_buf != NULL) {
        k2v_size += strlen(k2v_buf);
    }
    k2v_size += strlen(k2v_tmp) + 4;
    
    k2v_ret = malloc(k2v_size);
    if (k2v_ret == NULL) {
        free(k2v_buf);
        free(k2v_tmp);
        return NULL;
    }

    if (k2v_buf != NULL) {
        sprintf(k2v_ret, "%s%s", k2v_buf, k2v_tmp);
    } else {
        sprintf(k2v_ret, "%s", k2v_tmp);
    }

    free(k2v_buf);
    free(k2v_tmp);
    return k2v_ret;
}

/* Macro that calls the helper function */
#define k2v_add_config(type, k2v_buf, ...) \
    k2v_add_config_impl(k2v_buf, type##_to_k2v(__VA_ARGS__))
char *k2v_add_comment(char *_Nullable buf, char *_Nonnull comment);
char *k2v_add_newline(char *_Nullable buf);
long long key_get_long(const char *_Nonnull key, const char *_Nonnull buf);
char *long_to_k2v(const char *_Nonnull key, long val);
