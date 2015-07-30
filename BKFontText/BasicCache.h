#pragma once

#include <QtCore>
#include "Common.h"
using namespace std;

template<class _Key, class _Content>
class BKE_BasicCache
{
protected:
	//typedef CCObject * _Content;
	struct _Struct
	{
		_Content content;
		bkulong size;
	};
	typedef bkulong _Time;
    typedef QMap<_Time, _Key> _T2K_t;
    typedef QHash<_Key, _Time> _K2T_t;
    typedef QHash<_Key, _Struct> _K2S_t;
	typedef typename _K2S_t::iterator _Iterator;
	typedef typename _K2T_t::iterator _K2T_Iter;
	typedef typename _T2K_t::iterator _T2K_Iter;
	_T2K_t _T2K;
	_K2T_t _K2T;
	_K2S_t _K2S;
	_Time time;
	bkulong _limit;
	bkulong _nowsize;
	const wchar_t *_caller;

public:
	typedef _Iterator iterator;
	void add(const _Key &key, const _Content &content, bkulong size)
	{
		content->retain();
		_Struct s{ content, size };
		_Time now = ++time;
		_K2T_Iter i = _K2T.find(key);
		if (i != _K2T.end())
		{
            _Time t = i.value();
            _T2K.remove(t);
			_K2T.erase(i);
			_Iterator it = _K2S.find(key);
            it.value().content->release();
            _nowsize -= it.value().size;
            it.value() = s;
		}
		else
		{
			_K2S[key] = s;
		}
		_K2T[key] = now;
		_T2K[now] = key;
		_nowsize += size;
		if (_limit && _nowsize > _limit && !clean())
            ;
	}

	void remove(const _Key &key)
	{
		_Iterator i = _K2S.find(key);
		if (i != _K2S.end())
		{
            _Struct &s = i.value();
			_nowsize -= s.size;
			s.content->release();
			_K2T_Iter p = _K2T.find(key);
            _T2K.erase(p.value());
			_K2T.erase(p);
			_K2S.erase(i);
		}
	}

	bool get(const _Key &key, _Content &value)
	{
		_Iterator i = _K2S.find(key);
		if (i != _K2S.end())
		{
            _Struct & s = i.value();
			value = s.content;
			refresh(key);
			return true;
		}
		return false;
	}

	void refresh(const _Key &key)
	{
		_Time now = ++time;
		_K2T_Iter i = _K2T.find(key);
		if (i != _K2T.end())
		{
            _Time t = i.value();
            _T2K.remove(t);
			_K2T.erase(i);
			_K2T[key] = now;
			_T2K[now] = key;
		}
	}
	bool clean()
	{
		_T2K_Iter i = _T2K.begin();
		while (_nowsize > _limit)
		{
			_Iterator p;
			while (true)
			{
				if (i == _T2K.end())
					return false;
                p = _K2S.find(i.value());
                _Struct &s = p.value();
				if (s.content->isSingleReference())
				{
					break;
				}
				i++;
			}
            _K2T_Iter it = _K2T.find(p.key());
			i = _T2K.erase(i);
			_K2T.erase(it);
            p.value().content->release();
            _nowsize -= p.value().size;
			_K2S.erase(p);
		}
		return true;
	}

	bool cleanUntilHalfLimit()
	{
		bkulong l = _limit;
		_limit /= 2;
		bool b = clean();
		_limit = l;
		return b;
	}

	inline void setCaller(const wchar_t *caller)
	{
		_caller = caller;
	}

	inline void limit(bkulong s)
	{
		_limit = s;
	}

	BKE_BasicCache() :time(0), _limit(-1), _nowsize(0), _caller(NULL) {
#ifndef MINGW_SDK_INIT
        static_assert(std::is_convertible<_Content, QObject *>::value, "");
#endif
	}
	~BKE_BasicCache(){
		_Iterator i = _K2S.begin();
		while (i != _K2S.end())
		{
            i.value().content->release();
			i++;
		}
	}
};
