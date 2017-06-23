#ifndef CONFIG_H
#define CONFIG_H

typedef struct Config Config;

typedef void (*ConfigError)(const char *);

Config *Config_create(int argc, char **argv, ConfigError errorHandler);
const char *Config_imgFileName(const Config *self);
const char *Config_srcFileName(const Config *self);
const char *Config_hdrFileName(const Config *self);
const char *Config_surfaceName(const Config *self);
void Config_destroy(Config *self);

#endif
