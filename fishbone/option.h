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

#ifndef OPTION_H
#define OPTION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CONFIG_ENV_SIZE     8 * 1024

#ifdef CONFIG_SYS_REDUNDAND_ENVIRONMENT
# define ENV_HEADER_SIZE	(sizeof(uint32_t) + 1)
#else
# define ENV_HEADER_SIZE	(sizeof(uint32_t))
#endif

#define ENV_SIZE (CONFIG_ENV_SIZE - ENV_HEADER_SIZE)

typedef unsigned char       uchar;

typedef	struct environment_s {
    uint32_t	crc;		/* CRC32 over data bytes	*/
#ifdef CONFIG_SYS_REDUNDAND_ENVIRONMENT
    unsigned char	flags;		/* active/obsolete flags	*/
#endif
    unsigned char	data[ENV_SIZE]; /* Environment data		*/
} env_t;

#define option              Option::instance()

class Option
{
public:
    static Option *instance();

    void setfile(const char *name, int offset = 0)
    {
        filename = name;
        optionOffset = offset;
    }

    int printenv(char *name = NULL, int state = 1);
    int setenv(char *varname, char *varvalue = NULL);
    int delenv(char *varname)
    {
        setenv(varname, NULL);
    }

    char *getenv(char *name);
    int saveenv();

    void destoryenv()
    {
        env_crc_destroy();
    }

private:
    Option();
    Option(const Option &);
    Option &operator=(const Option &);

    int env_init();
    void use_default();
    void env_crc_update();
    void env_crc_destroy();
    uchar env_get_char_spec(int index);
    uchar env_get_char_init(int index);
    uchar env_get_char_memory(int index);
    uchar env_get_char(int index);
    uchar *env_get_addr(int index);
    void set_default_env();
    int get_env_id (void);
    int _do_setenv(int flag, int argc, char *argv[]);
    int envmatch(uchar *s1, int i2);

private:
    static Option *self;
    const char *filename;
    int optionOffset;
    int env_id;
    unsigned long env_addr;	/* Address  of Environment struct */
    unsigned long env_valid;	/* Checksum of Environment valid? */
    env_t *env_ptr;
    env_t env_memory;
};

#endif // OPTION_H
