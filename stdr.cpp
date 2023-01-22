#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <memory>
#include <vector>
#include <unistd.h>

constexpr std::size_t buf_size = 8*1024*1024; // 1MB

int main(int argc, char *argv[]) {

    long loop_count = 0;
    bool wait = false;
    bool loop_inf = true;
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
        loop_inf = false;
        ++optind;
    }

    std::vector<std::unique_ptr<char[]>> buffer{};
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

    if (!wait) {
        --loop_count;
    }

    // stdout
    if (loop_inf) {
        while (true) {
            for (std::size_t j = 0; j+1 < buffer.size(); ++j) {
                write(1, buffer[j].get(), buf_size);
            }
            write(1, buffer.back().get(), last_size);
        }
    } else {
        for (long i = 0; i < loop_count; ++i) {
            for (std::size_t j = 0; j+1 < buffer.size(); ++j) {
                write(1, buffer[j].get(), buf_size);
            }
            write(1, buffer.back().get(), last_size);
        }
    }
}

