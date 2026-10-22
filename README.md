# THE PROJECT : FAMINE

Subject version : 2.1
Team : tde-cama, knerini

## Guidelines

Languages : C, Assembly

Create a virus program that silently puts a signature inside binaries (ELF 64-bit).

For the bonus part, 32-bit ELF binaries are managed and sh scripts (with shebang `#!`) too (considered as non-binary files). The program is also launched as daemon and only when a specific process is running (Firefox for our work).
There are two options :
- one for make the infection recursive inside the project directories (/tmp/test and /tmp/test2)
- one to launch the program with infinite loop to infect all binaries added to the project directories on lifetime

## Resources used

- [Malicious Mechanics of ELF: Shellcode](https://anish833.github.io/posts/malicious-mechanics-of-elf/), Segments & Stealth Anish Bhowmick (22th June 2025)
- [Deep Dive: Writing an ELF Code Cave Infector in C](https://justthinkinghard.github.io/posts/elf-infektor/) JustThinkingHard (13th January 2026)

## Program

Compile the whole program with `make`.
- Run program `./Famine`
- Run program with recursive `./Famine -r`
- Run program with infinite loop `./Famine -w` 

### Behaviour

The program launches a daemon with the command `./Famine` and infects all binaries inside /tmp/test and /tmp/test2 directories. Virus writes the signature inside the program : "Famine version 1.0 (c)oded by <tde-cama>-<knerini>" and rewrite the entire binary. The infected binary is still able to executes itself as it is.

## Knowledge

The main code used for this program comes from the `woody-woodpacker` project, without the encryption/decryption and compression/decompression. 
