#include <iostream>
#include <atomic>
#include "RefCounting.h"



class Wraight : public RefCountable
{
public:
    int _hp = 150;
    int _posX = 0;
    int _PosY = 0;

private:

};

using WraightRef = TSharedPtr<Wraight>;

class Missile : public RefCountable

{
public:
    void SetTarget(WraightRef target)
    {
        _target = target;
        //중간에 다른 스레드가 개입을한다면?
        //레이스의 참조카운트를 +1 하기전에 미사일스레드가 레이스의 체력을 0으로만든다면?
        //target->AddRef();
    }
    bool Update()
    {
        if (_target == nullptr)
            return true;

        int posX = _target->_posX;
        int posY = _target->_PosY;
        //쫒아간다.
        if (_target->_hp == 0)
        {
            // _target->ReleaseRef();
            _target = nullptr;
            return true;
        }
        return false;
    }
    WraightRef _target = nullptr;

private:

};
using MissileRef = TSharedPtr<Missile>;


int main()
{
    //명시적으로 delete를 삭제하는  것이 아닌 항상 refcount를 통해 알아서 객체가 사라지게 되어야 함 
    WraightRef wraight(new Wraight());
    wraight->ReleaseRef(); // 지금 만들어진 클래스의 정책상 담고 삭제가 필요 바로 set()이 호출되어 +1이 추가로 되기때문
    MissileRef missile(new Missile());
    missile->ReleaseRef();

    missile->SetTarget(wraight);

    //레이스 피격 당함
    wraight->_hp = 0;
    //wraight->ReleaseRef();
    wraight = nullptr;

    while (true)
    {
        if (missile)
        {
            if (missile->Update())
            {
                // missile->ReleaseRef();
                missile = nullptr;
            }
        }
    }
    // missile->ReleaseRef();
    missile = nullptr;

}
