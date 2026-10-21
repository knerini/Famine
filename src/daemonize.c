#include <ft_famine.h>

void daemonize(void) {
    pid_t pid;

    /* First fork: parent exits, child continues.
       This ensures the child is not a process group leader,
       which is required for setsid() to succeed. */
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);   /* parent */
    }

    /* Become session leader and detach from controlling terminal. */
    if (setsid() < 0) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    /* Clear file mode creation mask so we control permissions fully. */
    umask(0);

    /* Second fork: the session leader exits, so the daemon can never
       reacquire a controlling terminal (it's no longer a session leader). */
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);   /* first child */
    }

    /* Flush stdio buffers before we replace the descriptors. */
    fflush(stdout);
    fflush(stderr);

    /* Redirect stdin, stdout, stderr to /dev/null. */
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0) {
        perror("open /dev/null");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, STDIN_FILENO) < 0 ||
        dup2(fd, STDOUT_FILENO) < 0 ||
        dup2(fd, STDERR_FILENO) < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    /* If the original fd isn't one of the standard ones, close it. */
    if (fd > STDERR_FILENO) {
        close(fd);
    }
}