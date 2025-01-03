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

static std::string get_log_path() {
    const char* logPath = getenv("LOG_PATH");
    return logPath ? logPath : "./program_crash.log";
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
        std::cerr << "Failed to open log file.\n";
    }
}

static void sighandle(int sig){
    signal(sig, SIG_DFL);  // 恢复默认信号处理
    int clifd = open("/proc/self/cmdline", O_RDONLY | O_CLOEXEC);
    char buf[1024];
    ssize_t bufsize __attribute__((unused)) = read(clifd, buf, sizeof(buf));
    close(clifd);

    // 记录崩溃日志
    std::string errorMsg = "Fatal error (" + std::to_string(sig) + "), the program has been stopped.";
    std::cout << errorMsg << std::endl;
    LOG_ERROR();
    write_log(errorMsg.c_str());
    std::cout << "Log file path: " << std::filesystem::current_path() << "/program_crash.log" << std::endl;

    // 获取调用栈
    #ifdef __GLIBC__
    #define BACKTRACE_DEPTH 50
    void* array[BACKTRACE_DEPTH];
    int size = backtrace(array, BACKTRACE_DEPTH);
    char **stackTrace = backtrace_symbols(array, size);

    if (stackTrace) {
    std::cout << "Stack trace:" << std::endl;
        write_log("Stack trace:");
        for (int i = 0; i < size; i++) {  // 使用 int 类型
            std::cout << stackTrace[i] << std::endl;
            write_log(stackTrace[i]);
        }
        free(stackTrace);
    }
    #else
    write_log("Stack trace not available.");
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
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);
        execl("/bin/bash", "bash", scriptPath.c_str(), NULL);
        _exit(1);
    } else if (pid > 0) {
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        std::stringstream stdoutStream, stderrStream;
        char buffer[128];
        ssize_t n;

        while ((n = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0) {
            stdoutStream.write(buffer, n);
        }
        while ((n = read(stderr_pipe[0], buffer, sizeof(buffer))) > 0) {
            stderrStream.write(buffer, n);
        }

        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        std::cout << "[STDOUT] " << stdoutStream.str();
        std::cerr << "[STDERR] " << stderrStream.str();
        monitor_child(pid);
    } else {
        std::cerr << "Failed to fork process.\n";
        exit(1);
    }
}

void monitor_child(pid_t pid) {
    int status;
    for (int i = 0; i < 400; i++) {
        sleep(1);
        if (waitpid(pid, &status, WNOHANG) == pid) {
            if (WIFEXITED(status)) {
                std::cout << "Script exited with status " << WEXITSTATUS(status) << std::endl;
            } else if (WIFSIGNALED(status)) {
                std::cout << "Script terminated by signal " << WTERMSIG(status) << std::endl;
            }
            return;
        }
    }
    if (kill(pid, SIGKILL) == 0) {
        waitpid(pid, &status, 0);
        std::cerr << "Child process killed due to timeout.\n";
    } else {
        std::cerr << "Failed to kill child process.\n";
        exit(4);
    }
}

int main() {
    register_signal();
    std::array<char, 128> buffer;
    std::string result;
    execute_script("./test-root.sh");
    return 0;
}
