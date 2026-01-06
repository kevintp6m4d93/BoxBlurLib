#include <sstream>
#include "Error.h"

std::string BlurException::GetFormattedMessage() const {
    std::ostringstream oss;
    oss << "code=" << static_cast<int>(errorCode)
        << ", message=\"" << what() << "\""
        << ", at " << filename << ":" << line;
    return oss.str();
}
