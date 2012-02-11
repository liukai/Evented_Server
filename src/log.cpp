
#include "log.h"
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/helpers/exception.h"
using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr innerLogger(Logger::getLogger("MyApp"));

void init_debugger() {
    BasicConfigurator::configure();
}
void trace(const char* message) { LOG4CXX_TRACE(innerLogger, message); }
void debug(const char* message) { LOG4CXX_DEBUG(innerLogger, message); }
void info(const char* message) { LOG4CXX_INFO(innerLogger, message); }
void warn(const char* message) { LOG4CXX_WARN(innerLogger, message); }
void error(const char* message) { LOG4CXX_WARN(innerLogger, message); };
void fatal(const char* message) { LOG4CXX_FATAL(innerLogger, message); }

std::ostream& log = std::cout;
