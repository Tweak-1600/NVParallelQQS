#pragma once
class Exception
{
private:
	char const* _msg;
public:
	Exception()
	{
		_msg = "Unknown Exception";
	}
	Exception(const char* errorMsg)
	{
		_msg = errorMsg;
	}
	char const* Message() const
	{
		return _msg;
	}
	void Throw() const
	{
		throw* this;
	}
};

class InvalidIteratorException : public Exception
{
public:
	InvalidIteratorException() : Exception() {  }
	InvalidIteratorException(const char* errorMsg) : Exception(errorMsg) {  }
};



class IndexOutOfRangeException : public Exception
{
public:
	IndexOutOfRangeException(const char* errorMsg = "Index out of range.") : Exception(errorMsg) {  }
};


/*class InvalidOperationException : public Exception
{
public:
	InvalidOperationException() : Exception() {  }
	InvalidOperationException(char const* errorMsg) : Exception(errorMsg) {  }
};*/
