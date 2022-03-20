#pragma once
// ��ע��: ��header����������ʷ����

/// <summary>
/// ��ʾ�˿�ȫ���쳣���͵Ļ���.
/// </summary>
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

/// <summary>
/// ��ʾ��ǰ�������汾���ɵ��쳣.
/// </summary>
class InvalidIteratorException : public Exception
{
public:
	InvalidIteratorException() : Exception() {  }
	InvalidIteratorException(const char* errorMsg) : Exception(errorMsg) {  }
};


/// <summary>
/// ��ʾ����������������Χ.
/// </summary>
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
