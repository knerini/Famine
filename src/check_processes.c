
#include <ft_famine.h>

bool check_process_by_name(const char *name) {
    DIR *dir = opendir("/proc");
    if (!dir) 
        return 0;

    struct dirent *entry;
    bool found = false;

    while ((entry = readdir(dir)) != NULL)
    {
        // Only look at numeric directories (these are PIDs)
        if (entry->d_type != DT_DIR)
            continue;

        char *end;
        long pid = strtol(entry->d_name, &end, 10);
        if (*end != '\0' || pid <= 0) 
            continue;

        // /proc/<pid>/comm path
        char path[64];
        snprintf(path, sizeof(path), "/proc/%ld/comm", pid);

        int fd = open(path, O_RDONLY);
        if (fd < 0)
            continue;

        char comm[256];
        ssize_t n = read(fd, comm, sizeof(comm) - 1);
        close(fd);

        if (n <= 0)
            continue;
        comm[n] = '\0';

        // comm is "name\n" — strip the trailing newline (if present)
        size_t len = (size_t)n;
        if (len > 0 && comm[len - 1] == '\n') {
            comm[len - 1] = '\0';
        } else {
            comm[len] = '\0';
        }

        if (ft_strcmp(comm, name) == 0) {
            found = true;
            break;
        }
    }

    closedir(dir);
    return found;
}