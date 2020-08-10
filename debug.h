#ifndef CARD_DEBUG_H
#define CARD_DEBUG_H
int LOG_INDENT = 0;
#ifdef DEBUG
#define LOG_GROUP_BEGIN()                                      LOG_INDENT++;
#define LOG_GROUP_BEGIN_MSG(x)        { LOGF("/ %s", x);           LOG_INDENT++;  }
#define LOG_GROUP_BEGIN_MSGF(x, ...)   { LOGF("/ " x, __VA_ARGS__); LOG_INDENT++;  }
#define LOG_GROUP_END()           LOG_INDENT--;                             
#define LOG_GROUP_END_MSG(x)        { LOG_INDENT--; LOGF("\\ %s", x);            }
#define LOG_GROUP_END_MSGF(x, ...)   { LOG_INDENT--; LOGF("\\ " x, __VA_ARGS__);  }
#define LOG(x)        { for(int i=0;i<LOG_INDENT;++i)printf("|   "); puts(x); }
#define LOGNONL(x)          printf(x);
#define LOGC(c)          putc(c, stdout);
#define LOGF(x, ...)   { for(int i=0;i<LOG_INDENT;++i)printf("|   "); printf(x "\n", __VA_ARGS__); }
#else
#define LOG_GROUP_BEGIN()       ;{};
#define LOG_GROUP_BEGIN_MSG(x)      ;{};
#define LOG_GROUP_BEGIN_MSGF(x, ...) ;{};
#define LOG_GROUP_END()       ;{};
#define LOG_GROUP_END_MSG(x)      ;{};
#define LOG_GROUP_END_MSGF(x, ...) ;{};
#define LOG(x)      ;{};
#define LOGNONL(x)      ;{};
#define LOGC(c)      ;{};
#define LOGF(x, ...) ;{};
#endif
#define ERROR(msg)      { printf("[%s]: %s\n", __FUNCTION__, msg); exit(1); }
#endif//CARD_DEBUG_H
