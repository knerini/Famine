#include "ft_famine.h"

// Build the injected stub in its own buffer and patch it for this binary.
// Class-independent: the caller passes the assembled stub bytes (the 64-bit or
// the 32-bit stub) via `stub`/`stub_len`. The payload is just the stub.
// The only value unknown at assembly time is JMP_MKR, replaced here with the
// rel32 displacement of the return jump to the original entry point.
bool build_stub(t_famine *p, const unsigned char *stub, size_t stub_len)
{
    p->payload_size = stub_len;
    p->payload      = ft_calloc(1, p->payload_size);
    if(!p->payload)
    {
        perror("Payload allocation failed.");
        return false;
    }

    // Copy the assembled stub.
    ft_memcpy(p->payload, stub, stub_len);

    // Locate the JMP_MKR placeholder (the jmp's rel32 displacement).
    size_t  disp_offset = 0;
    bool    found       = false;
    for(size_t k = 0; k + 4 <= stub_len; k++)
    {
        if(*(uint32_t *)(p->payload + k) == JMP_MKR)
        {
            disp_offset = k;
            found       = true;
            break;
        }
    }
    if(!found)
    {
        fprintf(stderr, "Jump marker not found in stub.\n");
        return false;
    }

    size_t  next_offset = disp_offset + 4;
    int64_t jmp_delta   = (int64_t)p->entry - (int64_t)(p->payload_va + next_offset);
    if(jmp_delta < INT32_MIN || jmp_delta > INT32_MAX)
    {
        fprintf(stderr, "Jump out of range.\n");
        return false;
    }
    *(int32_t *)(p->payload + disp_offset) = (int32_t)jmp_delta;

    return true;
}
