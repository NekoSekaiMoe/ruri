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

// ANSI color codes
#define RED "\033[31m"
#define RESET "\033[0m"

// Global variables
static char log_path[256] = "./program_crash.log";

// Function to log errors
void logError(const char* func, const char* file, int line) {
    fprintf(stderr, "%sIn %s() in %s line %d:%s\n", RED, func, file, line, RESET);
}

#define LOG_ERROR() logError(__func__, __FILE__, __LINE__)

// Function to set log path
void set_log_path(const char* path) {
    strncpy(log_path, path, sizeof(log_path) - 1);
    log_path[sizeof(log_path) - 1] = '\0';
}

// Function to get log path
const char* get_log_path(void) {
    return log_path;
}

// Function to write log
void write_log(const char* msg) {
    FILE* logfile = fopen(get_log_path(), "a");
    if (logfile != NULL) {
        time_t t = time(NULL);
        char timestamp[100];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&t));
        fprintf(logfile, "[%s] %s\n", timestamp, msg);
        fclose(logfile);
    } else {
        fprintf(stderr, "[ERROR] Failed to open log file.\n");
        exit(123);
    }
}

// Signal handler function
static void panic(int sig)
{
    char buf[1024];
    char error_msg[256];
    
    signal(sig, SIG_DFL);  // Restore default signal handling
    
    int clifd = open("/proc/self/cmdline", O_RDONLY | O_CLOEXEC);
    ssize_t bufsize __attribute__((unused)) = read(clifd, buf, sizeof(buf));
    close(clifd);

    // Record crash log
    snprintf(error_msg, sizeof(error_msg), "[ERROR] Fatal error (%d), the program has been stopped.", sig);
    printf("%s\n", error_msg);
    LOG_ERROR();
    write_log(error_msg);
    
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("[INFO] Log file path: %s/program_crash.log\n", cwd);
    }

    // Get stack trace
    #ifdef __GLIBC__
    #define BACKTRACE_DEPTH 50
    void* array[BACKTRACE_DEPTH];
    int size = backtrace(array, BACKTRACE_DEPTH);
    char **stackTrace = backtrace_symbols(array, size);

    if (stackTrace) {
        printf("[INFO] Stack trace:\n");
        write_log("[INFO] Stack trace:");
        for (int i = 0; i < size; i++) {
            printf("%s\n", stackTrace[i]);
            write_log(stackTrace[i]);
        }
        free(stackTrace);
    }
    #else
    write_log("[ERROR] Stack trace not available.");
    #endif
    
    exit(127);
}
// Catch coredump signal.
void ruri_register_signal(void)
{
	/*
	 * Only SIGSEGV means segmentation fault,
	 * but we catch all signals that might cause coredump.
	 */
	signal(SIGABRT, panic);
	signal(SIGBUS, panic);
	signal(SIGFPE, panic);
	signal(SIGILL, panic);
	signal(SIGQUIT, panic);
	signal(SIGSEGV, panic);
	signal(SIGSYS, panic);
	signal(SIGTRAP, panic);
	signal(SIGXCPU, panic);
	signal(SIGXFSZ, panic);
}
