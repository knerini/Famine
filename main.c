#include <ft_famine.h>


int main(int argc, char **argv)
{
    if (argc > 2) 
    { 
        usage();
        return 1;
    }

    t_binary *binary = ft_calloc(1, sizeof(t_binary));
    if(!binary)
        return 1;
    open_binary_in_memory(argv[1], binary);
    if(!binary->buffer)
        return 1;
    if(!check_file_arch(binary))
        return 1;

    int ret = 0;
    if(binary->arch == ARCH64)
        ret = infects_64(binary);
    else
        ret = infects_32(binary);
    free_binary(binary);
    return ret;

    
}