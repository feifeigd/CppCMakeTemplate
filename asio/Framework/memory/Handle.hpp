#pragma once

#include <cassert>
#include <cstdlib>
#include <cstdint>

// 用于管理动态对象，保存了handle，即使对象已经销毁，也可以通过判断handle是否有效来判断对象是否存在
// 避免了悬挂指针的问题
template<typename Tag>
class Handle{
public:
    
    enum{
        // handle_ 的 32 位中，高 8 位是 magic，低 24 位是 index
        MAGIC_BITS = 8,
        INDEX_BITS = 32 - MAGIC_BITS,

        MAX_INDEX = (1 << INDEX_BITS) - 1,
        MAX_MAGIC = (1 << MAGIC_BITS) - 1,
    };

public:
    Handle() = default;
    Handle(uint32_t handle): handle_(handle){}

    void index(uint32_t index){
        assert(! valid() && index <= MAX_INDEX);    // 不允许重赋值

        index_ = index;

        static uint32_t s_magic = rand();   // 重启后，可靠性

        if(++s_magic > MAX_MAGIC){
            s_magic = 1;
        }
        magic_ = s_magic;
    }

    // 更新之后 handle 就无效了
    void UpdateMagic(){
        if(++magic_ > MAX_MAGIC){
            magic_ = 1;
        }
    }

    void reset(){
        handle_ = 0;
    }

    operator bool()const{
        return valid();
    }

    bool valid()const{
        return handle_ != 0;
    }

    bool operator == (const Handle& rhs)const{
        return handle_ == rhs.handle_;
    }

    operator uint32_t ()const{
        return handle_;
    }
    uint32_t handle()const{
        return handle_;
    }

    uint32_t index()const{
        return index_;
    }

private:

    union 
    {
        struct{
            uint32_t index_ : INDEX_BITS;
            uint32_t magic_ : MAGIC_BITS;
        };

        uint32_t handle_{}; // 完整的数据
    };    
};
