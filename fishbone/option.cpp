/*
 * option utility
 *
 * Copyright (c) 2015, longfeng.xiao <xlongfeng@126.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "option.h"

/************************************************************************
 * Default settings to be used when no valid environment is found
 */
#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

uchar filename_data[] = {
    "\0"
};

/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */
static const uint32_t crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
uint32_t crc32 (uint32_t crc, const unsigned char *buf, int len)
{
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}

Option *Option::self = 0;

Option::Option() :
    env_id(1)
{
#ifdef __arm__
    setfile("/dev/mmcblk0", (768 + 8) * 1024);
#else
    setfile("option.txt");
#endif
    env_init();
}

Option *Option::instance()
{
    if (!self) {
        self = new Option();
    }
    return self;
}

/************************************************************************
 * Command interface: print one or all environment variables
 */

/*
 * state 0: finish printing this string and return (matched!)
 * state 1: no matching to be done; print everything
 * state 2: continue searching for matched name
 */
int Option::printenv(char *name, int state)
{
    int i, j;
    char c, buf[17];

    i = 0;
    buf[16] = '\0';

    while (state && env_get_char(i) != '\0') {
        if (state == 2 && envmatch((uchar *)name, i) >= 0)
            state = 0;

        j = 0;
        do {
            buf[j++] = c = env_get_char(i++);
            if (j == sizeof(buf) - 1) {
                if (state <= 1)
                    printf(buf);
                j = 0;
            }
        } while (c != '\0');

        if (state <= 1) {
            if (j)
                printf(buf);
            printf("\n");
        }
    }

    if (state == 0)
        i = 0;
    return i;
}

int Option::setenv(char *varname, char *varvalue)
{
    char *argv[4] = { "setenv", varname, varvalue, NULL };
    if (varvalue == NULL)
        return _do_setenv (0, 2, argv);
    else
        return _do_setenv (0, 3, argv);
}

char *Option::getenv(char *name)
{
    int i, nxt;

    for (i=0; env_get_char(i) != '\0'; i=nxt+1) {
        int val;

        for (nxt=i; env_get_char(nxt) != '\0'; ++nxt) {
            if (nxt >= CONFIG_ENV_SIZE) {
                return (NULL);
            }
        }
        if ((val=envmatch((uchar *)name, i)) < 0)
            continue;
        return ((char *)env_get_addr(val));
    }

    return (NULL);
}

int Option::saveenv()
{
    if (get_env_id() > 1) {
#ifdef __arm__
        int fd = open(filename, O_WRONLY);
#else
        int fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
#endif
        if (fd > 0) {
            lseek(fd, optionOffset, SEEK_SET);
            int len = write(fd, env_ptr, CONFIG_ENV_SIZE);
            fsync(fd);
            close(fd);
        } else {
            return -1;
        }
    }
    return 0;
}

int Option::env_init()
{
    env_ptr = &env_memory;
    int fd = open(filename, O_RDONLY);
    if (fd > 0) {
        lseek(fd, optionOffset, SEEK_SET);
        int len = read(fd, env_ptr, CONFIG_ENV_SIZE);
        close(fd);
    }
    if (crc32(0, env_ptr->data, ENV_SIZE) != env_ptr->crc)
        use_default();

    env_addr = (ulong)&(env_ptr->data);
    env_valid = 1;

    return 0;
}

void Option::use_default()
{
    printf("*** Warning - bad CRC or EEPROM, using default data\n");
    set_default_env();
}

void Option::env_crc_update()
{
    env_ptr->crc = crc32(0, env_ptr->data, ENV_SIZE);
}

void Option::env_crc_destroy()
{
    env_ptr->crc += 1;
}

uchar Option::env_get_char_spec(int index)
{
    return *((uchar *)(env_addr + index));
}

uchar Option::env_get_char_init (int index)
{
    uchar c;

    /* if crc was bad, use the default environment */
    if (env_valid)
    {
        c = env_get_char_spec(index);
    } else {
        c = filename_data[index];
    }

    return (c);
}

uchar Option::env_get_char_memory(int index)
{
    if (env_valid) {
        return ( *((uchar *)(env_addr + index)) );
    } else {
        return ( filename_data[index] );
    }
}

uchar Option::env_get_char(int index)
{
    uchar c;

    /* if relocated to RAM */
    if (0)
        c = env_get_char_memory(index);
    else
        c = env_get_char_init(index);

    return (c);
}


uchar *Option::env_get_addr(int index)
{
    if (env_valid) {
        return ( ((uchar *)(env_addr + index)) );
    } else {
        return (&filename_data[index]);
    }
}

void Option::set_default_env()
{
    memset(env_ptr, 0, ENV_SIZE);
    memcpy(env_ptr->data, filename_data, sizeof(filename_data));
#ifdef CONFIG_SYS_REDUNDAND_ENVIRONMENT
    env_ptr->flags = 0xFF;
#endif
    env_crc_update();
}

int Option::get_env_id (void)
{
    return env_id;
}

/************************************************************************
 * Set a new environment variable,
 * or replace or delete an existing one.
 *
 * This function will ONLY work with a in-RAM copy of the environment
 */

int Option::_do_setenv(int flag, int argc, char *argv[])
{
    int   i, len, oldval;
    uchar *env, *nxt = NULL;
    char *name;

    uchar *env_data = env_get_addr(0);

    if (!env_data)	/* need copy in RAM */
        return 1;

    name = argv[1];

    if (strchr(name, '=')) {
        printf ("## Error: illegal character '=' in variable name \"%s\"\n", name);
        return 1;
    }

    env_id++;
    /*
     * search if variable with this name already exists
     */
    oldval = -1;
    for (env=env_data; *env; env=nxt+1) {
        for (nxt=env; *nxt; ++nxt)
            ;
        if ((oldval = envmatch((uchar *)name, env-env_data)) >= 0)
            break;
    }

    /*
     * Delete any existing definition
     */
    if (oldval >= 0) {
        if (*++nxt == '\0') {
            if (env > env_data) {
                env--;
            } else {
                *env = '\0';
            }
        } else {
            for (;;) {
                *env = *nxt++;
                if ((*env == '\0') && (*nxt == '\0'))
                    break;
                ++env;
            }
        }
        *++env = '\0';
    }

    /* Delete only ? */
    if ((argc < 3) || argv[2] == NULL) {
        env_crc_update ();
        return 0;
    }

    /*
     * Append new definition at the end
     */
    for (env=env_data; *env || *(env+1); ++env)
        ;
    if (env > env_data)
        ++env;
    /*
     * Overflow when:
     * "name" + "=" + "val" +"\0\0"  > ENV_SIZE - (env-env_data)
     */
    len = strlen(name) + 2;
    /* add '=' for first arg, ' ' for all others */
    for (i=2; i<argc; ++i) {
        len += strlen(argv[i]) + 1;
    }
    if (len > (&env_data[ENV_SIZE]-env)) {
        printf ("## Error: environment overflow, \"%s\" deleted\n", name);
        return 1;
    }
    while ((*env = *name++) != '\0')
        env++;
    for (i=2; i<argc; ++i) {
        char *val = argv[i];

        *env = (i==2) ? '=' : ' ';
        while ((*++env = *val++) != '\0')
            ;
    }

    /* end is marked with double '\0' */
    *++env = '\0';

    /* Update CRC */
    env_crc_update ();

    return 0;
}

int Option::envmatch(uchar *s1, int i2)
{

    while (*s1 == env_get_char(i2++))
        if (*s1++ == '=')
            return(i2);
    if (*s1 == '\0' && env_get_char(i2-1) == '=')
        return(i2);
    return(-1);
}
