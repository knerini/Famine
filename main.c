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
    int         ret = 0;

    daemonize();
    
    if(stat(path_test, &stats) == 0 && S_ISDIR(stats.st_mode))
    {
        ret = file_handler(path_test);
    }
    if(stat(path_test2, &stats) == 0 && S_ISDIR(stats.st_mode))
    {
        ret = file_handler(path_test2);
    }
    return ret;

    
}