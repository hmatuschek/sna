#ifndef __DAWN_LOGGER_HH__
#define __DAWN_LOGGER_HH__

#include <list>
#include <sstream>

typedef enum {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR
} LogLevel;


class LogMessage: public std::stringstream
{
public:
  LogMessage(LogLevel level);
  LogMessage(const LogMessage &other);

  LogLevel level() const;

protected:
  LogLevel _level;
};


class LogHandler
{
public:
  LogHandler();
  virtual ~LogHandler();

  virtual void handle(const LogMessage &msg) = 0;
};


class StreamLogHandler: public LogHandler
{
public:
  StreamLogHandler(LogLevel level, std::ostream &stream);

  virtual void handle(const LogMessage &msg);

protected:
  LogLevel _level;
  std::ostream &_stream;
};


class Logger
{
protected:
  Logger();

public:
  static Logger &get();
  ~Logger();

  void addHandler(LogHandler *handler);
  void log(const LogMessage &msg);

protected:
  static Logger *_instance;
  std::list<LogHandler *> _handler;
};

#endif // __DAWN_LOGGER_HH__
