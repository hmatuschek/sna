#include "logger.hh"

LogMessage::LogMessage(LogLevel level)
  : std::stringstream(), _level(level)
{
  // pass...
}

LogMessage::LogMessage(const LogMessage &other)
  : std::stringstream(), _level(other._level)
{
  this->str(other.str());
}

LogLevel
LogMessage::level() const {
  return _level;
}


LogHandler::LogHandler() {
  // pass...
}

LogHandler::~LogHandler() {
  // pass...
}


StreamLogHandler::StreamLogHandler(LogLevel level, std::ostream &stream)
  : LogHandler(), _level(level), _stream(stream)
{
  // pass...
}

void
StreamLogHandler::handle(const LogMessage &msg) {
  if (msg.level() < _level) { return; }
  switch (msg.level()) {
  case LOG_DEBUG: _stream << "DEBUG: "; break;
  case LOG_INFO: _stream << "INFO: "; break;
  case LOG_WARNING: _stream << "WARN: "; break;
  case LOG_ERROR: _stream << "ERROR: "; break;
  }
  _stream << msg.str() << std::endl;
}


Logger *Logger::_instance = 0;

Logger::Logger()
  : _handler()
{
  // pass...
}

Logger::~Logger() {
  std::list<LogHandler *>::iterator handler = _handler.begin();
  for (; handler != _handler.end(); handler++) {
    delete *handler;
  }
}

Logger &
Logger::get() {
  if (0 == _instance) {
    _instance = new Logger();
  }
  return *_instance;
}

void
Logger::addHandler(LogHandler *handler) {
  _handler.push_back(handler);
}

void
Logger::log(const LogMessage &msg) {
  std::list<LogHandler *>::iterator handler = _handler.begin();
  for (; handler != _handler.end(); handler++) {
    (*handler)->handle(msg);
  }
}

