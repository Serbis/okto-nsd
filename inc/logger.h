#ifndef NSD_LOGGER_H
#define NSD_LOGGER_H

int Logger_init(const char* fp);
void Logger_info(char *source, char *str, ...);
void Logger_fatal(char *source, char *str, ...);

#endif //NSD_LOGGER_H
