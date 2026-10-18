#include <unistd.h>
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

std::string ftoa(float n, int afterpoint) {
    std::string res = "";
    int ipart = (int)n;
    float fpart = n - (float)ipart;
    if (ipart == 0 && n < 0) {
        res += "-";
    } else if (ipart < 0) {
        res += "-";
        ipart = -ipart;
        fpart = -fpart;
    }
    if (ipart == 0) {
        res += "0";
    } else {
        std::string temp = "";
        int t = ipart;
        while (t > 0) {
            temp = (char)((t % 10) + '0') + temp;
            t /= 10;
        }
        res += temp;
    }
    if (afterpoint > 0) {
        res += ".";
        for (int j = 0; j < afterpoint; j++) {
            fpart *= 10;
            int digit = (int)fpart;
            res += (char)(digit + '0');
            fpart -= digit;
        }
    }
    return res;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        write_error("Usage: child <filename>\n");
        return 1;
    }
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        write_error("Error opening file\n");
        return 1;
    }
    std::string line;
    while (read_line(STDIN_FILENO, line) > 0) {
        float sum = 0.0f;
        size_t i = 0;
        while (i < line.length()) {
            while (i < line.length() && (line[i] == ' ' || line[i] == '\t')) i++;
            if (i >= line.length() || line[i] == '\n') break;
            char *endptr;
            float val = strtof(line.c_str() + i, &endptr);
            if (line.c_str() + i == endptr) break;
            sum += val;
            i = endptr - line.c_str();
        }
        std::string res = ftoa(sum, 6) + "\n";
        write_str(fd, res);
    }
    close(fd);
    return 0;
}
