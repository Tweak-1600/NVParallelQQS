#pragma once
#include "Exception.h"
#pragma warning(disable:4996)
template <class T> class List
{
private:
	int _Version = 0;
	T* _Data;
	T* _DataEnd;
	T* _Last;
	void MoveObjectsToBuffer(T* _Buff)
	{
		if constexpr (std::is_standard_layout<T>::value)
		{
			memcpy(_Buff, _Data, (_Last - _Data) * sizeof(T));
		}
		else
		{
			for (T* _Dest = _Buff, *_End = _Buff + (_Last - _Data), *_Src = _Data; _Dest != _End; ++_Dest, ++_Src)
			{
				new(_Dest++) T(static_cast<T&&>(*_Src));
				(*_Src).~T();
			}
		}
	}
	void CopyObjectsFromBuffer(T* _Buff, const size_t& _Count)
	{
		if constexpr (std::is_standard_layout<T>::value)
		{
			memcpy(_Data, _Buff, _Count);
		}
		else
		{
			for (T* _Dest = _Data, *_Src = _Buff, *_End = _Data + _Count; _Dest != _End; ++_Dest, ++_Src)
			{
				new(_Dest++) T(*_Src);
			}
		}
	}

	void EnsureCapacity()
	{
		if (_Last == _DataEnd)
		{
			size_t _new_capacity = (_DataEnd - _Data) * 2;
			T* _Buff = (T*)malloc(sizeof(T) * _new_capacity);
			MoveObjectsToBuffer(_Buff);

			_Last = _Buff + (_Last - _Data);
			free(_Data);
			_Data = _Buff;
			_DataEnd = _Data + _new_capacity;
		}
	}
public:
	/// <summary>
	/// 表示一个 List 初始化的容量.
	/// </summary>
	constexpr static int DefaultCapacity = 32;
	class Iterator
	{
	private:
		T* _First;
		T* _Current;
		T* _Last;
		int _VerifyVersion;
		List<T>* _TargetList;
	public:
		explicit Iterator(List<T>& _collection)
		{
	
			_First = _collection._Data;
			_Current = _collection._Data;
			_Last = _collection._Last;
			_VerifyVersion = _collection._Version;
			_TargetList = &_collection;
		}
		bool MoveNext()
		{
			if (_VerifyVersion != _TargetList->_Version)
			{
				throw InvalidIteratorException();
			}
			if (_Current + 1 == _Last)
			{
				return false;
			}
			++_Current;
			return true;
		}
		bool MoveBack()
		{
			if (_VerifyVersion != _TargetList->_Version)
			{
				throw InvalidIteratorException();
			}
			if (_Current == _First) return false;
			--_Current;
			return true;
		}
		T& Current()
		{
			return *_Current;
		}
		void Reset()
		{
			_VerifyVersion = _TargetList->_Version;
			_First = _TargetList->_Data;
			_Current = _First;
			_Last = _TargetList->_Last;
		}
	};
	List()
	{
		_Data = (T*)malloc(sizeof(T) * DefaultCapacity);
		_DataEnd = _Data + DefaultCapacity;
		_Last = _Data;
	}
	explicit List(size_t capacity)
	{
		if (!capacity) capacity = DefaultCapacity;
		_Data = (T*)malloc(sizeof(T) * capacity);
		_DataEnd = _Data + capacity;
		_Last = _Data;
	}
	List(const List<T>& other)
	{
		if (!other._Data)
		{
			_Data = (T*)malloc(sizeof(T) * DefaultCapacity);
			_DataEnd = _Data + DefaultCapacity;
			_Last = _Data;
		}
		else
		{
			const size_t _Capacity = other._DataEnd - other._Data;
			_Data = (T*)malloc(sizeof(T) * _Capacity);
			_DataEnd = _Data + _Capacity;
			_Last = _Data + (other._Last - other._Data);
			CopyObjectsFromBuffer(other._Data, other.Size());
		}
	}
	List(List<T>&& other) noexcept
	{
		_Data = other._Data;
		_DataEnd = other._DataEnd;
		_Last = other._Last;

		other._Data = nullptr;
		other._DataEnd = nullptr;
		other._DataLast = nullptr;
	}
	List<T>& operator=(List<T>&& other) noexcept
	{
		if (_Data)
		{
			for (T* _Curr = _Data; _Curr != _Last; ++_Curr)
			{
				(*_Curr).~T();
			}
			free(_Data);
		}
		_Data = other._Data;
		_DataEnd = other._DataEnd;
		_Last = other._Last;

		other._Data = nullptr;
		other._DataEnd = nullptr;
		other._Last = nullptr;

		return *this;
	}
	bool Any() const
	{
		return _Data;
	}
	void Add(const T& val)
	{
		++_Version;
		EnsureCapacity();
		new(_Last++) T(val);
	}
	void Add(T&& val)
	{
		++_Version;
		EnsureCapacity();
		new(_Last++) T(static_cast<T&&>(val));
	}
	void AddRange(const List<T>& other)
	{
		if constexpr (std::is_pod_v<T>)
		{
			const size_t _SizeOfOther = other.Size();
			Reserve(Size() + _SizeOfOther);
			memcpy(_Last, other._Data, _SizeOfOther * sizeof(T));
			_Last += _SizeOfOther;
		}
		else
		{
			for (T* p = other._Data; p != other._Last; ++p)
			{
				Add(*p);
			}
		}
	}
	void Insert(const size_t& index, const T& item)
	{
		T* insertpos = _Data + index;
		if (insertpos > _Last) throw IndexOutOfRangeException();
		else if (insertpos == _Last)
		{
			Add(item);
			return;
		}
		memmove(insertpos + 1, insertpos, sizeof(T) * (_Last - insertpos));
		new(insertpos) T(item);
		++_Last;
		++_Version;
	}
	inline void RemoveLast()
	{
		if (_Data != _Last)
		{
			--_Last;
			(*_Last).~T();
		}
	}
	void RemoveAt(const size_t& index)
	{
		T* removepos = _Data + index;
		if (removepos >= _Last) throw IndexOutOfRangeException();
		(*removepos).~T();
		memmove(removepos, removepos + 1, sizeof(T) * (_Last - removepos));
		++_Version;
		--_Last;
	}
	void Remove(const T& item)
	{
		for (T* _Ptr = _Data; _Ptr != _Last; ++_Ptr)
		{
			if (*_Ptr == item)
			{
				RemoveAt(_Ptr - _Data);
				return;
			}
		}

	}
	void TrimExcess()
	{
		size_t threshold = (_DataEnd - _Data) * 9 / 10;
		if ((_Last - _Data) < threshold)
		{
			if (_DataEnd - _Data)
			{
				T* buffer = (T*)malloc(sizeof(T) * (_Last - _Data));
				memcpy(buffer, _Data, sizeof(T) * (_Last - _Data));
				_Last = buffer + (_Last - _Data);
				free(_Data);
				_Data = buffer;
				_DataEnd = _Last;
			}
			else
			{
				T* buffer = (T*)malloc(sizeof(T) * DefaultCapacity);
				_Last = buffer;
				free(_Data);
				_Data = buffer;
				_DataEnd = _Data + DefaultCapacity;
			}
		}
	}

	/// <summary>
	/// 保留指定大小的容量.
	/// </summary>
	/// <param name="Size">当前 List 实例的最小容量.</param>
	void Reserve(const size_t& Size)
	{
		if (Size <= (_DataEnd - _Data)) return;

		const size_t _newCapacity = Size;
		T* buff = (T*)malloc(sizeof(T) * _newCapacity);
		memcpy(buff, _Data, sizeof(T) * (_Last - _Data));
		_Last = buff + (_Last - _Data);
		free(_Data);
		_Data = buff;
		_DataEnd = _Data + _newCapacity;
	}
	T& At(const size_t& index)
	{
		if (index >= (_Last - _Data))
		{
			throw IndexOutOfRangeException();
		}
		return *(_Data + index);
	}
	const T& At(const size_t& index) const
	{
		if (index >= (_Last - _Data))
		{
			throw IndexOutOfRangeException();
		}
		return *(_Data + index);
	}
	size_t Size() const noexcept
	{
		return _Last - _Data;
	}
	size_t Capacity() const noexcept
	{
		return _DataEnd - _Data;
	}
	Iterator GetIterator()
	{
		return Iterator(*this);
	}
	void Clear()
	{
		for (T* ptr = _Data; ptr != _DataEnd; ++ptr)
		{
			(*ptr).~T();
		}
		free(_Data);

		_Data = (T*)malloc(sizeof(T) * DefaultCapacity);
		_Last = _Data;
		_DataEnd = _Data + DefaultCapacity;

		++_Version;
	}
	~List()
	{
		++_Version;
		if (_Data)
		{
			for (T* _Curr = _Data; _Curr != _Last; ++_Curr)
			{
				(*_Curr).~T();
			}
			free(_Data);
		}
		_Data = _DataEnd = _Last = nullptr;
	}
	T* begin()
	{
		return _Data;
	}
	T* end()
	{
		return _Last;
	}
};

