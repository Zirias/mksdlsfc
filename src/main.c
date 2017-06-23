#include "config.h"
#include "convert.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void printError(const char *error)
{
    fputs(error, stderr);
}

int main(int argc, char **argv)
{
    Config *cfg = Config_create(argc, argv, printError);
    if (!cfg) return EXIT_FAILURE;

    FILE *hdr = fopen(Config_hdrFileName(cfg), "w");
    if (!hdr)
    {
        fprintf(stderr, "Error opening `%s' for writing: %s\n",
                Config_hdrFileName(cfg), strerror(errno));
        Config_destroy(cfg);
        return EXIT_FAILURE;
    }

    FILE *imp = fopen(Config_srcFileName(cfg), "w");
    if (!imp)
    {
        fprintf(stderr, "Error opening `%s' for writing: %s\n",
                Config_srcFileName(cfg), strerror(errno));
        fclose(hdr);
        Config_destroy(cfg);
        return EXIT_FAILURE;
    }

    int result = convert(Config_imgFileName(cfg), Config_surfaceName(cfg),
            hdr, imp);
    fclose(hdr);
    fclose(imp);

    if (!result)
    {
        fprintf(stderr, "Error converting image `%s'\n",
                Config_imgFileName(cfg));
        remove("surface.h");
        remove("surface.c");
        Config_destroy(cfg);
        return EXIT_FAILURE;
    }

    Config_destroy(cfg);
    return EXIT_SUCCESS;
}
