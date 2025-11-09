#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

bool is_same_file(const char* lhs_path, const char* rhs_path) {
    struct stat stat1, stat2;
    if(stat(lhs_path, &stat1) == -1 || stat(rhs_path, &stat2) == -1) {
        return false;
    }
    else if(stat1.st_ino == stat2.st_ino) {
        return true;
    }

    return false;
}

int main(int argc, const char* argv[]) {
    if(argc != 3) {
        return 1;
    }

    const char * path1 = argv[1];
    const char * path2 = argv[2];
    char * ans = is_same_file(path1, path2) ? "yes" : "no";

    printf("%s\n", ans);

    return 0;
}
