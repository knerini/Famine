#include <ft_famine.h>

static bool find_entry(t_packer *p)
{
    Elf32_Ehdr *eh = (Elf32_Ehdr *)p->binary->buffer;
    p->entry = eh->e_entry;
    size_t   phnum = eh->e_phnum;
    Elf32_Phdr *ph = (Elf32_Phdr *)(p->binary->buffer + eh->e_phoff);
    /* ---------- locate entry's file offset ---------- */
    p->entry_off = 0;
    for (size_t i = 0; i < phnum; i++) {
        if (ph[i].p_type != PT_LOAD)
            continue;
        if (!(ph[i].p_flags & PF_X))
            continue;
        if (p->entry >= ph[i].p_vaddr && p->entry < ph[i].p_vaddr + ph[i].p_filesz)
        {
            p->entry_off = ph[i].p_offset + (p->entry - ph[i].p_vaddr);
            break;
        }
    }
    if (!p->entry_off)
    {
        fprintf(stderr, "entry not target exec segment\n");
        return false;
    }
    return true;
}

static bool find_text(t_packer *p)
{
    Elf32_Ehdr *eh = (Elf32_Ehdr *)p->binary->buffer;
    Elf32_Shdr *sh = (Elf32_Shdr *)(p->binary->buffer + eh->e_shoff);

    /* ---------- find .text section (to encrypt) ---------- */
    if (!eh->e_shoff || !eh->e_shnum || !eh->e_shstrndx) {
        fprintf(stderr, "no section headers (stripped?)\n");
        return false;
    }
    const char *shstr = (const char *)p->binary->buffer + sh[eh->e_shstrndx].sh_offset;

    p->text_off  = 0;
    p->text_size = 0;
    p->text_va   = 0;
    for (size_t i = 0; i < eh->e_shnum; i++) {
        if (ft_strcmp(shstr + sh[i].sh_name, ".text") == 0) {
            p->text_off  = sh[i].sh_offset;
            p->text_size = sh[i].sh_size;
            p->text_va   = sh[i].sh_addr;
            break;
        }
    }
    if (!p->text_size)
    {
        fprintf(stderr, "no .text section\n");
        return false;
    }
    return true;
}

static void save_entry(t_packer *p)
{
    p->original_entry = malloc(ENTRY_N);
    ft_memcpy(p->original_entry, p->binary->buffer + p->entry_off, ENTRY_N);
}

static bool find_ptnote(t_packer *p)
{
    Elf32_Ehdr *eh = (Elf32_Ehdr *)p->binary->buffer;
    Elf32_Phdr *ph = (Elf32_Phdr *)(p->binary->buffer + eh->e_phoff);
    size_t   phnum = eh->e_phnum;
    p->ptnote_pos = -1;
    for (size_t i = 0; i < phnum; i++)
        if (ph[i].p_type == PT_NOTE)
        {
            p->ptnote_pos = i;
            break;
        }
    if (p->ptnote_pos < 0)
    {
        fprintf(stderr, "no PT_NOTE\n");
        return false;
    }
    return true;
}

static void compute_payload_va(t_packer *p)
{
    Elf32_Ehdr *eh = (Elf32_Ehdr *)p->binary->buffer;
    Elf32_Phdr *ph = (Elf32_Phdr *)(p->binary->buffer + eh->e_phoff);
    size_t   phnum = eh->e_phnum;

    p->payload_offset = (p->binary->file_size + PAGE - 1) & ~(size_t)(PAGE - 1);
    uint64_t max_vend = 0;
    for (size_t i = 0; i < phnum; i++)
        if (ph[i].p_type == PT_LOAD) {
            uint64_t end = (uint64_t)ph[i].p_vaddr + ph[i].p_memsz;
            if (end > max_vend)
                max_vend = end;
        }
    p->payload_va = (max_vend + PAGE - 1) & ~(uint64_t)(PAGE - 1);
}

bool elf32_parser(t_packer *p)
{
    if (!find_entry(p))
        return false;
    if (!find_text(p))
        return false;
    save_entry(p);
    if (!find_ptnote(p))
        return false;
    compute_payload_va(p);
    return true;
}