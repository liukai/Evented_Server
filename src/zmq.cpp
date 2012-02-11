// #include "com/foo/bar.h"
// using namespace com::foo;

// include log4cxx header files.
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/helpers/exception.h"

using namespace log4cxx;
using namespace log4cxx::helpers;

LoggerPtr logger(Logger::getLogger("MyApp"));
int main(int argc, char **argv)
{

        int result = EXIT_SUCCESS;
        try
        {
                // Set up a simple configuration that logs on the console.
                BasicConfigurator::configure();
                char* x = "a";

                LOG4CXX_INFO(logger, x);
                LOG4CXX_INFO(logger, "Entering application.");
                LOG4CXX_INFO(logger, "Exiting application.");
        }
        catch(Exception&)
        {
                result = EXIT_FAILURE;
        }

        return result;
}
