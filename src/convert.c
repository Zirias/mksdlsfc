#include "convert.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static char *upperStr(const char *str)
{
    char *upper = malloc(strlen(str)+1);
    if (!upper) return 0;
    const char *r = str;
    char *w = upper;
    while (*r)
    {
        *w++ = toupper(*r++);
    }
    *w = 0;
    return upper;
}

int convert(const char *imgFileName, const char *surfaceName,
        FILE *header, FILE *implementation)
{
    IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG|IMG_INIT_TIF);
    SDL_Surface *image = IMG_Load(imgFileName);
    IMG_Quit();
    if (!image) return 0;

    char *upper = upperStr(surfaceName);
    fprintf(header, "#ifndef MKSDLSFC_%s_SURFACE\n"
            "#define MKSDLSFC_%s_SURFACE\n\n"
            "#include <SDL2/SDL.h>\n\n", upper, upper);
    free(upper);
    fprintf(header, "SDL_Surface *%s_surface();\n\n#endif\n", surfaceName);

    fputs("#include <SDL2/SDL.h>\n\n", implementation);
    fprintf(implementation, "static unsigned char %s_pixels[] = {",
            surfaceName);

    unsigned char *data = image->pixels;
    for (size_t i = 0; i < (size_t)image->h * (size_t)image->pitch; ++i)
    {
        if (i) fputc(',', implementation);
        if (i%12) fputc(' ', implementation);
        else fputs("\n    ", implementation);
        fprintf(implementation, "0x%02x", (unsigned int)data[i]);
    }
    fputs("\n};\n\n", implementation);

    fprintf(implementation, "SDL_Surface *%s_surface()\n{\n"
            "    return SDL_CreateRGBSurfaceWithFormatFrom(\n"
            "            %s_pixels, %d, %d,\n"
            "            %d, %d, %s);\n}\n", surfaceName, surfaceName,
            image->w, image->h, (int)image->format->BitsPerPixel,
            image->pitch, SDL_GetPixelFormatName(image->format->format));
    SDL_FreeSurface(image);

    return 1;
} 
