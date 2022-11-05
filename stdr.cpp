#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <memory>
#include <vector>
#include <unistd.h>

constexpr std::size_t buf_size = 8*1024*1024; // 1MB
constexpr long INF = -1; // 適当な負の値

int main(int argc, char *argv[]) {

    long loop_count = INF;
    bool wait = false;
    int c;

    while ((c=getopt(argc, argv, "w::")) != -1) {
        switch (c) {
            case 'w':
                wait = true;
                break;
            default:
                return 1;
        }
    }

    while (optind < argc) {
        char * ep;
        loop_count = strtol(argv[optind], &ep, 10);
        if (*ep != '\0' || loop_count < 0) {
            constexpr char err_msg[] = "invalid argument.\n";
            write(2, err_msg, strlen(err_msg));
            write(2, argv[optind], strlen(argv[optind]));
            return 1;
        }
        ++optind;
    }

    std::vector<std::shared_ptr<char>> buffer{0};
    buffer.emplace_back(new char[buf_size]);

    int offset = 0;
    int read_size = 0;
    int last_size;

    // stdin
    while (true) {

        read_size = read(0, buffer.back().get()+offset, buf_size - offset);

        if (read_size > 0) {
            if (!wait && loop_count != 0) {
                write(1, buffer.back().get()+offset, read_size);
            }
            offset += read_size;
            if (offset == buf_size) {
                buffer.emplace_back(new char[buf_size]);
                offset = 0;
            }
        } else if (read_size == 0) {
            last_size = offset;
            break;
        } else {
            constexpr char err_msg[] = "read error.\n";
            write(2, err_msg, strlen(err_msg));
            write(2, strerror(errno), strlen(strerror(errno)));
            write(2, "\n", 1);
            return 1;
        }
    }

    if (!wait && (loop_count > 0)) {
        --loop_count;
    }

    // stdout
    for (long i = 0; loop_count == INF || i < loop_count; ++i) {
        for (int j = 0; static_cast<unsigned long>(j+1) < buffer.size(); ++j) {
            write(1, buffer[j].get(), buf_size);
        }
        write(1, buffer.back().get(), last_size);
    }
}

