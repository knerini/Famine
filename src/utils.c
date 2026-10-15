#include <ft_famine.h>

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	uint8_t		*tcdest;
	const uint8_t	*tcsrc;

	tcdest = (uint8_t*)dest;
	tcsrc = (const uint8_t*)src;
	if (!dest && !src)
		return (0);
	while (n--)
		*(tcdest++) = *(tcsrc++);
	return (dest);
}

int	ft_memcmp(const void *str1, const void *str2, size_t n)
{
	unsigned char *tcstr1;
	unsigned char *tcstr2;

	tcstr1 = (unsigned char*)str1;
	tcstr2 = (unsigned char*)str2;
	while (n--)
	{
		if (*tcstr1 != *tcstr2)
			return (*tcstr1 - *tcstr2);
		tcstr1++;
		tcstr2++;
	}
	return (0);
}

void	*ft_memset(void *str, int c, size_t n)
{
	uint8_t *tcstr;

	tcstr = (uint8_t*)str;
	while (n--)
		*(tcstr++) = c;
	return (str);
}

uint8_t *read_file(const char *path, size_t *size_out)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return NULL;

    off_t size = lseek(fd, 0, SEEK_END);
    if (size < 0)
    {
        close(fd);
        return NULL;
    }
    lseek(fd, 0, SEEK_SET);

    uint8_t *buffer = malloc(size);
    if (!buffer)
    { 
        close(fd); 
        return NULL; 
    }

    if (read(fd, buffer, size) != size) {
        close(fd);
        free(buffer);
        return NULL;
    }
    close(fd);

    *size_out = (size_t)size;
    return buffer;
}

int	ft_strcmp(const char *s1, const char *s2)
{
	uint8_t *tcstr1;
	uint8_t *tcstr2;

	tcstr1 = (uint8_t*)s1;
	tcstr2 = (uint8_t*)s2;
	while (tcstr1 != 0x0 && tcstr2 != 0x0)
	{
		if (!*tcstr1 || !*tcstr2 || *tcstr1 != *tcstr2)
			return (*tcstr1 - *tcstr2);
		tcstr1++;
		tcstr2++;
	}
	return (0);
}

void *ft_calloc(size_t nb, size_t mem)
{
    void *ptr = malloc(nb * mem);
    if (!ptr)
        return(NULL);
    ft_memset(ptr, 0, (nb * mem));
    return(ptr);
}

void open_binary_in_memory(const char *binary_file, t_binary *binary)
{
    int fd = open(binary_file, O_RDONLY);
    if(fd == -1)
    {
        perror("Failed to open binary.");
        return;
    }

    binary->file_size = lseek(fd, 0, SEEK_END);
    if(binary->file_size < (off_t)sizeof(Elf64_Ehdr) || binary->file_size < (off_t)sizeof(Elf32_Ehdr))
    {
        fprintf(stderr, "File to small to be an ELF binary.\n");
        close(fd);
        return;
    }

    binary->buffer = mmap(NULL, binary->file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(binary->buffer == MAP_FAILED)
    {
        binary->buffer = NULL;
        perror("Failed to map binary in memory.");
        close(fd);
        return;
    }
    close(fd);
}

bool check_file_arch(t_binary *binary)
{
    if(binary->file_size < EI_NIDENT
    || (!binary->buffer[EI_MAG0] || binary->buffer[EI_MAG0] != ELFMAG0)
    || (!binary->buffer[EI_MAG1] || binary->buffer[EI_MAG1] != ELFMAG1)
    || (!binary->buffer[EI_MAG2] || binary->buffer[EI_MAG2] != ELFMAG2)
    || (!binary->buffer[EI_MAG3] || binary->buffer[EI_MAG3] != ELFMAG3)
    || (!binary->buffer[EI_CLASS] || (binary->buffer[EI_CLASS] != ELFCLASS32 && binary->buffer[EI_CLASS] != ELFCLASS64))
    || (!binary->buffer[EI_VERSION] || binary->buffer[EI_VERSION] != EV_CURRENT))
    {
        fprintf(stderr, "Binary is not a regular ELF file.\n");
        free_binary(binary);
        return false;
    }
    if(binary->buffer[EI_CLASS] == ELFCLASS32)
        binary->arch = ARCH32;
    else
        binary->arch = ARCH64;
    return true;
}

void usage(void)
{
    const char *msg = "Usage : woody_woodpacker <binary_file>.\n";
    printf("%s", msg);
}

int free_packer(t_packer *p, int ret)
{
    if(p->payload)
        free(p->payload);
    if(p->target_file)
        free(p->target_file);
    if(p->blob)
        free(p->blob);
    if(p->original_entry)
        free(p->original_entry);
    return ret;
}

void free_binary(t_binary *binary)
{
    if(binary)
    {
        if(binary->buffer)
            munmap(binary->buffer, binary->file_size);
        free(binary);
    }
}