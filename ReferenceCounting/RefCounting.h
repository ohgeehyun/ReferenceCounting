#pragma once
#include<atomic>


using namespace std;
using int32 = __int32;
/*---------------------
	  RefCountable
-----------------------*/

/*
	카운팅을 하여 객체가 사라지지 않게 해야함.
	1. 최상위 객체를 하나를 만들어서 거기다 Refcounting기능을 넣어 상위 클래스를 상속받아 만드는 방법.
*/
class RefCountable
{
public:
	RefCountable() : _refCount(1) {};
	//최상위 클래스 특히 상속해야할 최상위클래스에서는 소멸자는 vritual이 선언되어야한다
	//이 부분은 전에 개인적으로 찾아봤는데.
	//java에서는 일단 객체가 생성될떄 기본 바인딩이 동적바인딩이다.but c++ 정적바인딩이다.
	//c++는 결과적으로 객체의 생성이 자식아 아닌 부모쪽으로 포인터가 맞춰지게된다.
	//이상황에서 부모A와 자식B가있다면 부모A에서 소멸자로 삭제시 부모의 소멸자만 호출이된다.
	//그리고 자식B의 소멸자는 호출되지않고 메모리를 잡아먹게된다.
	virtual ~RefCountable() { };

	int32 GetRefCount() { return _refCount; };

	int32 AddRef() { return ++_refCount; };

	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
		{
			delete this;
		}
		return refCount;
	}
protected:
	atomic<int32> _refCount;
};
/*-------------------
	   SharedPtr
---------------------*/
template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() {};
	TSharedPtr(T* ptr) { Set(ptr); };

	// 복사
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); };
	//이동
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; };
	//상속 관계 복사
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); };

	~TSharedPtr() { Release(); };

public:// 복사 연산자
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	};
	//이동 연산자
	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	};
	//같은 TSharedPtr 끼리 비교 
	bool	 operator == (const TSharedPtr& rhs) const { return _ptr == rhs._ptr; };
	//다른 포인터와의 비교 
	bool	 operator == (T* ptr) const { return _ptr == ptr; };

	bool	 operator != (const TSharedPtr& rhs) const { return _ptr != rhs._ptr; };
	bool	 operator != (T* ptr) const { return _ptr != ptr; };
	// 대소 관계 비교
	bool	 operator < (const TSharedPtr& rhs) const { return _ptr < rhs._ptr; };
	//해당 객체의 포인터가 필요할경우 오버로딩
	T* operator*() { return _ptr; };
	const T* operator*() const { return _ptr; };
	operator T* () const { return _ptr; };
	T* operator->() { return _ptr; };
	const T* operator->() const { return _ptr; };

	//널 체크
	bool IsNull() { return _ptr == nullptr; };


private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();

	}
	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}
private:
	T* _ptr = nullptr;
};

