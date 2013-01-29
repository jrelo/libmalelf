/* 
 * The malelf library was written in pure C, with the objective to 
 * provide a quick and easy way a set functions for programmers to 
 * manipulate ELF files. With libmalelf can dissect and infect ELF 
 * files. Evil using this library is the responsibility of the programmer.
 *
 * Author: Tiago Natel de Moura <tiago4orion@gmail.com>
 *
 * Contributor: Daniel Ricardo dos Santos <danielricardo.santos@gmail.com>
 *              Paulo Leonardo Benatto <benatto@gmail.com>
 *
 * Copyright 2012, 2013 by Tiago Natel de Moura. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <elf.h>

#include <malelf/defines.h>
#include <malelf/types.h>
#include <malelf/error.h>
#include <malelf/ehdr.h>

static MalelfEhdrType _me_type[] = {
        {ET_NONE,        0, "No filetype"},
        {ET_REL,         1, "Relocatable file"},
        {ET_EXEC,        2, "Executable file"},
        {ET_DYN,         3, "Shared object file"},
        {ET_CORE,        4, "Core file"},
        {ET_LOPROC, 0xff00, "Processor-specific"},
        {ET_HIPROC, 0xffff, "Processor-specific"}
};

static MalelfEhdrVersion _me_version[] = {
        {EV_NONE,    0, "Invalid version"},
        {EV_CURRENT, 1, "Current Version"}
};

static MalelfEhdrMachine _me_machine[] = {
        {EM_NONE,  0, "No machine"},
        {EM_M32,   1, "AT&T WE 32100"},
        {EM_SPARC, 2, "SPARC"},
        {EM_386,   3, "Intel 80386"},
        {EM_68K,   4, "Motorola 68000"},
        {EM_88K,   5, "Motorola 88000"},
        {EM_860,   7, "Intel 80860"},
        {EM_MIPS,  8, "MIPS RS3000"}
};

/* static methods */

static _i32 _malelf_ehdr_get_machine(MalelfEhdr *ehdr, _u8 class, _u8 *machine)
{
        int error = MALELF_SUCCESS;
        
        assert(NULL != ehdr);

        *machine = MALELF_HDR(ehdr, class, e_machine, error);

        return error;
}



static _i32 _malelf_ehdr_get_type(MalelfEhdr *ehdr, _u8 class, _u16 *type)
{
        int error = MALELF_SUCCESS;
        assert(NULL != ehdr);

        *type = MALELF_HDR(ehdr, class, e_type, error);
         
        return error;
}

static _i32 _malelf_ehdr_get_version(MalelfEhdr *ehdr, _u8 class, _u8 *version)
{
        int error = MALELF_SUCCESS;
        assert(NULL != ehdr);

        *version = MALELF_HDR(ehdr, class, e_version, error);

        return error;
}

_i32 malelf_ehdr_get_version(MalelfEhdr *ehdr, 
                             _u8 class, 
                             MalelfEhdrVersion *me_version)
{
        int error = MALELF_SUCCESS;
        _u8 version;

        error = _malelf_ehdr_get_version(ehdr, class, &version);

        if (MALELF_SUCCESS != error) {
                return error;
        }

        switch(version) {
        case EV_NONE: 
                *me_version = _me_version[0];
                break; 
        case EV_CURRENT: 
                *me_version = _me_version[1];
                break;
        default:
                error = MALELF_ERROR;
        }
        
        return error;
}

_i32 malelf_ehdr_get_type (MalelfEhdr *ehdr, 
                           _u8 class, 
                           MalelfEhdrType *me_type)
{
        int error = MALELF_SUCCESS;
        _u16 type;

        error = _malelf_ehdr_get_type(ehdr, class, &type);

        if (MALELF_SUCCESS != error) {
                me_type = NULL;
                return error;
        }
        
        switch(type) {
        case ET_NONE: 
                *me_type = _me_type[0];
                break;
        case ET_REL: 
                *me_type = _me_type[1];
                break;
        case ET_EXEC: 
                *me_type = _me_type[2];
                break;
        case ET_DYN: 
                *me_type = _me_type[3];
                break;
        case ET_CORE: 
                *me_type = _me_type[4];
                break;
        case ET_LOPROC: 
                *me_type = _me_type[5];
                break;
        case ET_HIPROC: 
                *me_type = _me_type[6];
                break;
        default:
                me_type = NULL;
                error = MALELF_ERROR;
        }
        
        return error;
}



_i32 malelf_ehdr_get_machine(MalelfEhdr *ehdr, 
                             _u8 class, 
                             MalelfEhdrMachine *me_machine)
{
        int error = MALELF_SUCCESS;
        _u8 machine;

        error = _malelf_ehdr_get_machine(ehdr, class, &machine);

        if (MALELF_SUCCESS != error) {
                return error;
        }

        switch(machine) {
        case EM_NONE:
                *me_machine = _me_machine[0];
                break;
        case EM_M32:
                *me_machine = _me_machine[1];
                break;
        case EM_SPARC:
                *me_machine = _me_machine[2];
                break;
        case EM_386:
                *me_machine = _me_machine[3];
                break;
        case EM_68K:
                *me_machine = _me_machine[4];
                break;
        case EM_88K:
                *me_machine = _me_machine[5];
                break;
        case EM_860:
                *me_machine = _me_machine[6];
                break;
        case EM_MIPS:
                *me_machine = _me_machine[7];
                break;
        default:
                me_machine = NULL;
                error = MALELF_ERROR;
        }
        return error;
}


_i32 malelf_ehdr_get_entry(MalelfEhdr *ehdr, _u8 class, _u32 *entry)
{
        int error = MALELF_SUCCESS;

        *entry = MALELF_HDR(ehdr, class, e_entry, error);

        return error;
}

_i32 malelf_ehdr_get_phoff(MalelfEhdr *ehdr, _u8 class, _u32 *phoff)
{
        int error = MALELF_SUCCESS;

        *phoff = MALELF_HDR(ehdr, class, e_phoff, error);

        return error;
}

_i32 malelf_ehdr_get_shoff(MalelfEhdr *ehdr, _u8 class, _u32 *shoff)
{
        int error = MALELF_SUCCESS;

        *shoff = MALELF_HDR(ehdr, class, e_shoff, error);

        return error;
}

_i32 malelf_ehdr_get_ehsize(MalelfEhdr *ehdr, _u8 class, _u32 *size)
{
        int error = MALELF_SUCCESS;

        *size = MALELF_HDR(ehdr, class, e_ehsize, error);

        return error;
}


_i32 malelf_ehdr_get_phentsize(MalelfEhdr *ehdr, _u8 class, _u32 *phentsize)
{
        int error = MALELF_SUCCESS;

        *phentsize = MALELF_HDR(ehdr, class, e_phentsize, error);

        return error;
}


_i32 malelf_ehdr_get_phnum(MalelfEhdr *ehdr, _u8 class, _u32 *phnum)
{
        int error = MALELF_SUCCESS;

        assert(ehdr != NULL);

        *phnum = MALELF_HDR(ehdr, class, e_phnum, error);

        return MALELF_SUCCESS;
}


_i32 malelf_ehdr_get_shentsize(MalelfEhdr *ehdr, _u8 class, _u32 *shentsize)
{
        int error = MALELF_SUCCESS;
        assert(NULL != ehdr);

        *shentsize = MALELF_HDR(ehdr, class, e_shentsize, error);

        return error;
}


_i32 malelf_ehdr_get_shnum(MalelfEhdr *ehdr, _u8 class, _u32 *shnum)
{
        int error = MALELF_SUCCESS;
        assert(NULL != ehdr);

        *shnum = MALELF_HDR(ehdr, class, e_shnum, error);

        return error;
}


_i32 malelf_ehdr_get_shstrndx(MalelfEhdr *ehdr, _u8 class, _u32 *shstrndx)
{
        int error = MALELF_SUCCESS;
        assert(NULL != ehdr);

        *shstrndx = MALELF_HDR(ehdr, class, e_shstrndx, error);

        return error;
}

_i32 malelf_ehdr_set(MalelfEhdr* ehdr, _u8 class, _u8 *mem, _u32 size) 
{
        assert(NULL != ehdr);
        assert(NULL != mem);

        switch (class) {
        case MALELF_ELF32:
                if (size > sizeof(Elf32_Ehdr)) {
                        return MALELF_EEHDR_OVERFLOW;
                }

                memcpy(ehdr->h32, mem, size);
                break;
        case MALELF_ELF64:
                if (size > sizeof(Elf64_Ehdr)) {
                        return MALELF_EEHDR_OVERFLOW;
                }

                memcpy(ehdr->h64, mem, size);
                break;
        default:
                return MALELF_EINVALID_CLASS;
        }

        return MALELF_SUCCESS;
}

_i32 malelf_ehdr_get_flags(MalelfEhdr *ehdr, _u8 class, _u32 *flags)
{
        int error = MALELF_SUCCESS;
        assert(NULL != ehdr);

        *flags = MALELF_HDR(ehdr, class, e_flags, error);

        return error;
}

