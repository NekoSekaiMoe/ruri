#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <memory>
#include <array>
#include <cstdio>
#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
#include <filesystem>
#include <cstdlib>
#ifdef __GLIBC__
#include <execinfo.h> // For backtrace
#else
#define backtrace(array, size) 0 // for musl
#define backtrace_symbols(array, size) nullptr
#endif
#include <ctime> // For timestamp

using namespace std;

void register_signal();
void execute_script(const std::string& scriptPath);
void monitor_child(pid_t pid);
void handle_signal(int sig);

void logError(const std::string& func, const std::string& file, int line) {
    const std::string RED = "\033[31m";
    const std::string RESET = "\033[0m";
    std::cerr << RED << "In " << func << "() in " << file << " line " << line << ":" << RESET << std::endl;
}

#define LOG_ERROR() logError(__func__, __FILE__, __LINE__)

static std::string log_path = "./program_crash.log";

static void set_log_path(const std::string& path) {
    log_path = path;
}

static std::string get_log_path() {
    return log_path;
}

static void write_log(const char* msg) {
    std::ofstream logfile(get_log_path(), std::ios::app);
    if (logfile.is_open()) {
        std::time_t t = std::time(nullptr);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        logfile << "[" << timestamp << "] " << msg << std::endl;
        logfile.close();
    } else {
        std::cerr << "[ERROR] Failed to open log file.\n";
        exit(123);
    }
}

static void sighandle(int sig){
    signal(sig, SIG_DFL);  // 恢复默认信号处理
    int clifd = open("/proc/self/cmdline", O_RDONLY | O_CLOEXEC);
    char buf[1024];
    ssize_t bufsize __attribute__((unused)) = read(clifd, buf, sizeof(buf));
    close(clifd);

    // 记录崩溃日志
    std::string errorMsg = "[ERROR] Fatal error (" + std::to_string(sig) + "), the program has been stopped.";
    std::cout << errorMsg << std::endl;
    LOG_ERROR();
    write_log(errorMsg.c_str());
    std::cout << "[INFO] Log file path: " << std::filesystem::current_path() << "/program_crash.log" << std::endl;

    // 获取调用栈
    #ifdef __GLIBC__
    #define BACKTRACE_DEPTH 50
    void* array[BACKTRACE_DEPTH];
    int size = backtrace(array, BACKTRACE_DEPTH);
    char **stackTrace = backtrace_symbols(array, size);

    if (stackTrace) {
    std::cout << "[INFO] Stack trace:" << std::endl;
        write_log("[INFO] Stack trace:");
        for (int i = 0; i < size; i++) {  // 使用 int 类型
            std::cout << stackTrace[i] << std::endl;
            write_log(stackTrace[i]);
        }
        free(stackTrace);
    }
    #else
    write_log("[ERROR] Stack trace not available.");
    #endif
    exit(127);
}

void register_signal(){
    signal(SIGABRT, sighandle);
    signal(SIGBUS, sighandle);
    signal(SIGFPE, sighandle);
    signal(SIGILL, sighandle);
    signal(SIGQUIT, sighandle);
    signal(SIGSEGV, sighandle);  // 段错误处理
    signal(SIGSYS, sighandle);
    signal(SIGTRAP, sighandle);
    signal(SIGXCPU, sighandle);
    signal(SIGXFSZ, sighandle);
}

void execute_script(const std::string& scriptPath) {
    int stdout_pipe[2], stderr_pipe[2];
    pipe(stdout_pipe);
    pipe(stderr_pipe);

    pid_t pid = fork();
    if (pid == 0) {
        // 子进程执行脚本
        close(stdout_pipe[0]);  // 关闭不需要的管道读端
        close(stderr_pipe[0]);  // 关闭不需要的管道读端
        dup2(stdout_pipe[1], STDOUT_FILENO);  // 重定向标准输出
        dup2(stderr_pipe[1], STDERR_FILENO);  // 重定向标准错误
        execl("/bin/bash", "bash", scriptPath.c_str(), NULL);  // 执行脚本
        _exit(1);  // 如果 execl 失败，退出子进程
    } else if (pid > 0) {
        // 父进程捕获子进程输出
        close(stdout_pipe[1]);  // 关闭不需要的管道写端
        close(stderr_pipe[1]);  // 关闭不需要的管道写端

        std::array<char, 128> buffer;
        ssize_t n;

	// Ensure the buffer is correctly null-terminated before outputting
	while ((n = read(stdout_pipe[0], buffer.data(), buffer.size() - 1)) > 0) {
	    buffer[n] = '\0'; // Null-terminate the buffer
	    std::cout << "[SHINFO] " << buffer.data(); // Print script output
	}
	while ((n = read(stderr_pipe[0], buffer.data(), buffer.size() - 1)) > 0) {
	    buffer[n] = '\0'; // Null-terminate the buffer
	    std::cerr << "[SHERR] " << buffer.data(); // Print script errors
	}

        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        // 等待子进程结束
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "[INFO] Script " << scriptPath << " exited with status " << WEXITSTATUS(status) << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cout << "[ERROR] Script " << scriptPath << " terminated by signal " << WTERMSIG(status) << std::endl;
        }
    } else {
        std::cerr << "[ERROR] Failed to fork process.\n";
        exit(1);
    }
}

void monitor_child(pid_t pid) {
    int status;
    const int timeout = 400; // 超时时间为 400 秒
    for (int i = 0; i < timeout; ++i) {
        if (waitpid(pid, &status, WNOHANG) == pid) {
            if (WIFEXITED(status)) {
                std::cout << "[INFO] Script exited with status " << WEXITSTATUS(status) << std::endl;
            } else if (WIFSIGNALED(status)) {
                std::cout << "[ERROR] Script terminated by signal " << WTERMSIG(status) << std::endl;
            }
            return;
        }
        sleep(1);
    }
    // 超时后强制终止子进程
    if (kill(pid, SIGKILL) == 0) {
        waitpid(pid, &status, 0); // 确保回收资源
        std::cerr << "[INFO] Child process killed due to timeout.\n";
    } else {
        std::cerr << "[ERROR] Failed to kill child process.\n";
        exit(4);
    }
}

int main() {
    register_signal();
    std::array<char, 128> buffer;
    std::string result;
    execute_script("./test-root.sh");
    write_log(result.c_str());
    return 0; // Ensure the correct exit status is returned
}
