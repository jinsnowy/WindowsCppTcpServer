#pragma once
#ifndef _Singleton_H_
#define _Singleton_H_

template<typename T>
class ISingleton
{
protected:
	ISingleton() = default;

public:
	virtual ~ISingleton() = default;
	ISingleton(const ISingleton&) = delete;
	ISingleton& operator=(const ISingleton&) = delete;
	ISingleton(ISingleton&&) = delete;
	ISingleton& operator=(ISingleton&&) = delete;

	static T* getInstance()
	{
		static T instance;
		return &instance;
	}

	virtual bool initialize() { return true; }
	virtual void destroy() {};
};

#endif // ! _Singleton_H_
