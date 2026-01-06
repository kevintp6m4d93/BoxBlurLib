#pragma once
#include <stdexcept>

enum class BlurErrorCode {
    BufferMismatch = 1001,
    NullArgument = 1002,
    BadParams = 1003,

    AllocationFailed = 2001,
    ExternalPackageError = 2002,
    IOError = 2003,

    Unexpected = 3001,
};

class BlurException : public std::runtime_error {
public:
	BlurException(BlurErrorCode code, const std::string& message, const char* filename, int line)
		: std::runtime_error(message), errorCode(code), filename(filename), line(line) {}
	BlurErrorCode GetErrorCode() const { return errorCode; }
	const std::string& GetFilename() const { return filename; }
	std::string GetFormattedMessage() const;
	int GetLine() const { return line; }

private:
	BlurErrorCode errorCode;
	std::string filename;
	int line;
};

#define THROW_BLUR_EXCEPTION(code, msg) throw BlurException((code), (msg), __FILE__, __LINE__)
