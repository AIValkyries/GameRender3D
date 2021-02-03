#pragma once
#ifndef Singleton_H
#define Singleton_H

#include "Debug.h"

template<class T>
class Singleton
{
public:
	Singleton(T& object)
	{
		s_pInstance = &object;
	}
	~Singleton()
	{
		s_pInstance = 0;
	}

	static T* instance()
	{
		if (s_pInstance == 0)
			s_pInstance = new T();
		return (s_pInstance);
	}

private:
	static T* s_pInstance;

};

template<class T> T* Singleton<T>::s_pInstance = 0;

#endif

