// SPDX-License-Identifier: MIT
/*
 *
 * This file is part of ruri, with ABSOLUTELY NO WARRANTY.
 *
 * MIT License
 *
 * Copyright (c) 2022-2024 Moe-hacker
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
#include "include/ruri.h"
/*
 * This file is used to catch segfault,
 * So that we can show some extra info when segfault.
 * I hope my program will never panic() QwQ.
 */

#include <sys/types.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// ANSI color codes
#define RED "\033[31m"
#define RESET "\033[0m"

// Global variables
static char log_path[256] = "./ruri_crash.log";

#ifdef __TINYC__
    #define CURRENT_FUNCTION "<unknown>"
#else
    #define CURRENT_FUNCTION __func__
#endif

// Function to log errors
void logError(const char* func, const char* file, int line) {
    fprintf(stderr, "%sIn %s() in %s line %d:%s\n", RED, func, file, line, RESET);
}

#define LOG_ERROR() logError(CURRENT_FUNCTION, __FILE__, __LINE__)

// Function to set log path
void set_log_path(const char* path) {
    if (path) {
        strncpy(log_path, path, sizeof(log_path) - 1);
        log_path[sizeof(log_path) - 1] = '\0';
    }
}

// Function to get log path
const char* get_log_path(void) {
    return log_path;
}

// Function to write log
void write_log(const char* msg) {
    if (!msg) return;
    
    FILE* logfile = fopen(get_log_path(), "a");
    if (logfile != NULL) {
        time_t t = time(NULL);
        char timestamp[100];
        struct tm* tm_info = localtime(&t);
        if (tm_info) {
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
            fprintf(logfile, "[%s] %s\n", timestamp, msg);
        }
        fclose(logfile);
    } else {
        fprintf(stderr, "[ERROR] Failed to open log file.\n");
        exit(123);
    }
}

#ifdef _WIN32
    #define PROC_SELF_CMDLINE "NUL"
#else
    #define PROC_SELF_CMDLINE "/proc/self/cmdline"
#endif

// Signal handler function
static void sighandle(int sig) {
    char buf[1024] = {0};
    char error_msg[256];
    char cwd[PATH_MAX] = {0};
    
    // Restore default signal handling
    signal(sig, SIG_DFL);
    
    // Try to read command line
    int clifd = open(PROC_SELF_CMDLINE, O_RDONLY);
    if (clifd >= 0) {
        ssize_t bytes_read = read(clifd, buf, sizeof(buf) - 1);
        if (bytes_read > 0) {
            buf[bytes_read] = '\0';
        }
        close(clifd);
    }

    // Record crash log
    snprintf(error_msg, sizeof(error_msg), 
             "[ERROR] Fatal error (signal %d), the program has been stopped.", sig);
    
    printf("%s\n", error_msg);
    LOG_ERROR();
    write_log(error_msg);
    
    // Get and log current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("[INFO] Log file path: %s/%s\n", cwd, log_path);
    }

    write_log("[INFO] Program terminated by signal");
    
    exit(127);
}

void ruri_register_signal(void) {
    // Register signal handlers
    struct {
        int signum;
        const char* name;
    } signals[] = {
        {SIGABRT, "SIGABRT"},
        {SIGBUS,  "SIGBUS"},
        {SIGFPE,  "SIGFPE"},
        {SIGILL,  "SIGILL"},
        {SIGQUIT, "SIGQUIT"},
        {SIGSEGV, "SIGSEGV"},
        {SIGSYS,  "SIGSYS"},
        {SIGTRAP, "SIGTRAP"},
        {SIGXCPU, "SIGXCPU"},
        {SIGXFSZ, "SIGXFSZ"},
        {0, NULL}
    };

    for (int i = 0; signals[i].name != NULL; i++) {
        signal(signals[i].signum, sighandle);
    }
}
