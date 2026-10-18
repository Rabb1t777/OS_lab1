#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <cstdlib>

void write_error(const std::string &msg) {
    write(STDERR_FILENO, msg.c_str(), msg.length());
}

ssize_t read_line(int fd, std::string &line) {
    line.clear();
    char c;
    while (true) {
        ssize_t n = read(fd, &c, 1);
        if (n <= 0) break;
        line += c;
        if (c == '\n') break;
    }
    return line.length();
}

void write_str(int fd, const std::string &str) {
    write(fd, str.c_str(), str.length());
}

int main() {
    std::string filename;
    if (read_line(STDIN_FILENO, filename) <= 0) {
        write_error("Error reading filename\n");
        return 1;
    }
    if (!filename.empty() && filename.back() == '\n') {
        filename.pop_back();
    }
    int pipe1[2];
    if (pipe(pipe1) == -1) {
        write_error("Error creating pipe\n");
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        write_error("Error forking\n");
        return 1;
    }
    if (pid == 0) {
        close(pipe1[1]);
        if (dup2(pipe1[0], STDIN_FILENO) == -1) {
            write_error("Error dup2\n");
            exit(1);
        }
        close(pipe1[0]);
        execl("./child", "child", filename.c_str(), NULL);
        write_error("Error executing child\n");
        exit(1);
    } else {
        close(pipe1[0]);
        std::string line;
        while (read_line(STDIN_FILENO, line) > 0) {
            write_str(pipe1[1], line);
        }
        close(pipe1[1]);
        waitpid(pid, NULL, 0);
    }
    return 0;
}
