#pragma once
#include "List.h"
#include "Macros.h"
#include <thread>
#include <mutex>
class Parallel
{
private:
	INLINE_VAR static int _MaxThread = std::thread::hardware_concurrency();
	using ThreadList = List<std::thread>; // ʹ���Զ�������Ա�: ����ҪԤ�ȹ������.
	template <class _RangeT> struct _TaskRange
	{
		_RangeT _Start;
		_RangeT _Stop;
	};
	Parallel() = delete;
public:
	INLINE_CONST static int DefaultMaximumThreads = std::thread::hardware_concurrency();
	static bool SetMaximumThreads(const unsigned int _Max)
	{
		if (_Max >= 65535) return false;
		if (_Max <= 0) return false;
		return _MaxThread = _Max;
	}
	static int GetMaximumThreads()
	{
		return _MaxThread;
	}
	/// <summary>
	/// ִ�в��е� for ѭ��.
	/// </summary>
	/// <param name="_Start">for ѭ�������. ���ֵӦ��С�� stop ����.</param>
	/// <param name="_Stop">for ѭ�����յ�. ���ֵ������Ϊ����������.</param>
	/// <param name="_Action">��Ҫִ�еķ���.</param>
	template <class _Fn, class _RT> static void For(_RT _Start, _RT _Stop, const _Fn& _Action)
	{
		static_assert(std::is_integral<_RT>::value, "The parameter that represents the for loop should be an integer type.");
		const _RT _Range = _Stop - _Start;
		if (_Range <= 0)
		{
			return;
		}
		const _RT _MaxConcurrencyThread = _Range < _MaxThread ? _Range : _MaxThread;
		_TaskRange<_RT>* _TaskRanges = new _TaskRange<_RT>[_MaxConcurrencyThread];
		const _RT _AverageRangeSize = _Range / _MaxConcurrencyThread;
		for (_RT i = 0; i != _MaxConcurrencyThread; ++i)
		{
			_TaskRanges[i]._Start = i * _AverageRangeSize + _Start;
			_TaskRanges[i]._Stop = (i + 1) * _AverageRangeSize + _Start;
		}

		const auto _RangeStart = _TaskRanges[_MaxConcurrencyThread - 1]._Stop;
		const auto _RangeStop = _Stop;

		std::mutex _MuxLock;
		ThreadList _TList(_MaxConcurrencyThread);

		for (int i = 0; i != _MaxConcurrencyThread; ++i)
		{
			_TList.Add(std::thread([_TaskRanges, i, _Action, &_MuxLock]()
				{
					_RT _RangeStart, _RangeStop;

					_MuxLock.lock();
					_RangeStart = _TaskRanges[i]._Start;
					_RangeStop = _TaskRanges[i]._Stop;
					_MuxLock.unlock();

					for (_RT k = _RangeStart; k != _RangeStop; ++k)
					{
						_Action(k);
					}
				}));
		}
		for (_RT k = _RangeStart; k != _RangeStop; ++k)
		{
			_Action(k);
		}
		for (size_t k = 0; k != _MaxConcurrencyThread; ++k)
		{
			_TList.At(k).join();
		}
	}
};

