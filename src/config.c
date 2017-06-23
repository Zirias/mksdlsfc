#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define STATIC_POCAS_CORE
#endif
#include <pocas/core/cmdline.h>
#include <pocas/core/string.h>

#define OPTID_SURFACENAME 1
#define OPTID_SOURCEFILE 2
#define OPTID_HEADERFILE 3

#ifdef _WIN32
#define _O "/"
#else
#define _O "-"
#endif

struct Config
{
    char *imgFileName;
    char *srcFileName;
    char *hdrFileName;
    char *surfaceName;
};

static void usage(ConfigError errorHandler, const char *prgName)
{
    static const char *usageFmt =
            "USAGE: %s [" _O "n SURFACENAME] [" _O "o SOURCEFILE] "
            "[" _O "h HEADERFILE] IMAGEFILE\n";
    char *usageStr = malloc(strlen(usageFmt) + strlen(prgName) - 1);
    sprintf(usageStr, usageFmt, prgName);
    errorHandler(usageStr);
    free(usageStr);
}

static void extraArg(ConfigError errorHandler, const char *arg)
{
    static const char *extraArgFmt =
            "ERROR: extra argument `%s' found.\n";
    char *extraArgStr = malloc(strlen(extraArgFmt) + strlen(arg) - 1);
    sprintf(extraArgStr, extraArgFmt, arg);
    errorHandler(extraArgStr);
    free(extraArgStr);
}

static void extraOpt(ConfigError errorHandler, char opt, const char *arg)
{
    static const char *extraOptFmt =
            "ERROR: extra option " _O "%c found with argument `%s'.\n";
    char *extraOptStr = malloc(strlen(extraOptFmt) + strlen(arg) - 2);
    sprintf(extraOptStr, extraOptFmt, opt, arg);
    errorHandler(extraOptStr);
    free(extraOptStr);
}

Config *Config_create(int argc, char **argv, ConfigError errorHandler)
{
    PC_Cmdline *cmdline = PC_Cmdline_create(PC_CLS_Auto);
    if (!PC_Cmdline_addFromArgv(cmdline, argc, argv))
    {
        if (errorHandler) usage(errorHandler, argv[0]);
        PC_Cmdline_destroy(cmdline);
        return 0;
    }

    PC_CmdlineParser *parser = PC_CmdlineParser_create();
    PC_CmdlineParser_register(parser,
            OPTID_SURFACENAME, 'n', 0, PC_COM_ArgRequired);
    PC_CmdlineParser_register(parser,
            OPTID_SOURCEFILE, 'o', 0, PC_COM_ArgRequired);
    PC_CmdlineParser_register(parser,
            OPTID_HEADERFILE, 'h', 0, PC_COM_ArgRequired);

    const PC_CmdlineItem *item;
    char *imgFileName = 0;
    char *srcFileName = 0;
    char *hdrFileName = 0;
    char *surfaceName = 0;
    while ((item = PC_CmdlineParser_next(parser, cmdline)))
    {
        switch (PC_CmdlineItem_id(item))
        {
        case OPTID_POSITIONALARG:
            if (imgFileName)
            {
                if (errorHandler)
                {
                    extraArg(errorHandler, PC_CmdlineItem_arg(item));
                    usage(errorHandler, argv[0]);
                }
                goto error;
            }
            imgFileName = PC_String_copy(PC_CmdlineItem_arg(item));
            break;
        case OPTID_SURFACENAME:
            if (surfaceName)
            {
                if (errorHandler)
                {
                    extraOpt(errorHandler, 'n', PC_CmdlineItem_arg(item));
                    usage(errorHandler, argv[0]);
                }
                goto error;
            }
            surfaceName = PC_String_copy(PC_CmdlineItem_arg(item));
            break;
        case OPTID_SOURCEFILE:
            if (srcFileName)
            {
                if (errorHandler)
                {
                    extraOpt(errorHandler, 'o', PC_CmdlineItem_arg(item));
                    usage(errorHandler, argv[0]);
                }
                goto error;
            }
            srcFileName = PC_String_copy(PC_CmdlineItem_arg(item));
            break;
        case OPTID_HEADERFILE:
            if (hdrFileName)
            {
                if (errorHandler)
                {
                    extraOpt(errorHandler, 'h', PC_CmdlineItem_arg(item));
                    usage(errorHandler, argv[0]);
                }
                goto error;
            }
            hdrFileName = PC_String_copy(PC_CmdlineItem_arg(item));
            break;
        }
    }
    if (!imgFileName)
    {
        if (errorHandler)
        {
            errorHandler("ERROR: No image file specified.\n");
            usage(errorHandler, argv[0]);
        }
        goto error;
    }

    PC_CmdlineParser_destroy(parser);
    PC_Cmdline_destroy(cmdline);

    if (!surfaceName)
    {
        surfaceName = PC_String_copy(imgFileName);
        char *pos = strrchr(imgFileName, '.');
        if (pos) pos = 0;
    }

    if (!srcFileName)
    {
        srcFileName = malloc(strlen(surfaceName) + 3);
        strcpy(srcFileName, surfaceName);
        strcat(srcFileName, ".c");
    }

    if (!hdrFileName)
    {
        hdrFileName = PC_String_copy(srcFileName);
        char *pos = strrchr(hdrFileName, '.');
        if (pos && *(pos+1) == 'c' && !*(pos+2))
        {
            *++pos = 'h';
        }
        else
        {
            size_t len = strlen(hdrFileName);
            hdrFileName = realloc(hdrFileName, len+3);
            hdrFileName[len++] = '.';
            hdrFileName[len++] = 'h';
            hdrFileName[len] = 0;
        }
    }

    Config *self = malloc(sizeof(Config));
    self->imgFileName = imgFileName;
    self->srcFileName = srcFileName;
    self->hdrFileName = hdrFileName;
    self->surfaceName = surfaceName;
    return self;

error:
    free(imgFileName);
    free(srcFileName);
    free(hdrFileName);
    free(surfaceName);
    PC_CmdlineParser_destroy(parser);
    PC_Cmdline_destroy(cmdline);
    return 0;
}

const char *Config_imgFileName(const Config *self)
{
    return self->imgFileName;
}

const char *Config_srcFileName(const Config *self)
{
    return self->srcFileName;
}

const char *Config_hdrFileName(const Config *self)
{
    return self->hdrFileName;
}

const char *Config_surfaceName(const Config *self)
{
    return self->surfaceName;
}

void Config_destroy(Config *self)
{
    if (!self) return;
    free(self->imgFileName);
    free(self->srcFileName);
    free(self->hdrFileName);
    free(self->surfaceName);
    free(self);
}

