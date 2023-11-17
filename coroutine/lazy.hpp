#pragma once

// Reference implementation of std::lazy proposal D2506R0 https://wg21.link/p2506r0.
// https://godbolt.org/z/dxxavazPa

#include <cassert>
#include <coroutine>
#include <exception>
#include <memory>   // destroy_at
#include <type_traits>  // is_nothrow_move_constructible_v

namespace std{

struct alignas(__STDCPP_DEFAULT_NEW_ALIGNMENT__)_Aligned_block{
    char _Pad[__STDCPP_DEFAULT_NEW_ALIGNMENT__];
};

template<typename _Alloc>
using _Rebind = allocator_traits<_Alloc>::template rebind_alloc<_Aligned_block>;

template<typename _Allocator = void>
class _Promise_allocator{
    using _Alloc = _Rebind<_Allocator>;

    static void* _Allocate(_Alloc _Al, size_t const _Size){
        if constexpr (default_initializable<_Alloc> && allocator_traits<_Alloc>::is_always_equal::value){
            // do not store stateless allocator
            auto const _Count = (_Size + sizeof(_Aligned_block) - 1) / sizeof(_Aligned_block);
            return _Al.allocate(_Count);
        }else{
            // store stateful allocator
            static constexpr auto _Align = (max)(alignof(_Alloc), sizeof(_Aligned_block));
            auto const _Count = (_Size + _Align - 1) / sizeof(_Aligned_block);
            void* const _Ptr = _Al.allocate(_Count);
            auto const _Al_address = (reinterpret_cast<uintptr_t>(_Ptr) + _Size + alignof(_Alloc) - 1) & ~(alignof(_Alloc) - 1);
            ::new(reinterpret_cast<void*>(_Al_address)) _Alloc(move(_Al));
            return _Ptr;
        }
    }
public:
    static void* operator new(size_t const _Size)
        requires default_initializable<_Alloc>
    {
        return _Allocate(_Alloc{}, _Size);
    }

    template<typename _Alloc2, typename... _Args>
        requires convertible_to<_Alloc2, _Allocator>
    static void* operator new(size_t const _Size, allocator_arg_t, _Alloc2&& _Al, _Args&...){
        return _Allocate(static_cast<_Alloc>(static_cast<_Allocator>(forward<_Alloc2>(_Al))));
    }

    template<typename _This, typename _Alloc2, typename... _Args>
        requires convertible_to<_Alloc2, _Allocator>
    static void* operator new(size_t const _Size, _This&, allocator_arg_t, _Alloc2&& _Al, _Args&...){
        return _Allocate(static_cast<_Alloc>(static_cast<_Allocator>(forward<_Alloc2>(_Al))), _Size);
        return {};
    }

    static void operator delete(void* const _Ptr, size_t const _Size)noexcept{
        if constexpr (default_initializable<_Alloc> && allocator_traits<_Alloc>::is_always_equal::value){
            // make stateless allocator
            _Alloc _Al{};
            auto const _Count = (_Size + sizeof(_Aligned_block) - 1) / sizeof(_Aligned_block);
            _Al.deallocate(static_cast<_Aligned_block*>(_Ptr), _Count);
        }else{
            // retrieve stateful allocator
            auto const _Al_address = (reinterpret_cast<uintptr_t>(_Ptr) + _Size + alignof(_Alloc) - 1) & ~(alignof(_Alloc) - 1);
            auto& _Stored_al = *reinterpret_cast<_Alloc*>(_Al_address);
            _Alloc _Al{move(_Stored_al)};
            _Stored_al.~_Alloc();

            static constexpr auto _Align = (max)(alignof(_Alloc), sizeof(_Aligned_block));
            auto const _Count = (_Size + sizeof(_Alloc) + _Align -1) / sizeof(_Aligned_block);
            _Al.deallocate(static_cast<_Aligned_block*>(_Ptr), _Count);
        }
    }
};

template<>
class _Promise_allocator<void>{
    using _Dealloc_fn = void(*)(void*, size_t);

    template<typename _Alloc2>
    static void* _Allocate(_Alloc2 _Al2, size_t _Size){
        using _Alloc = _Rebind<_Alloc2>;
        auto _Al = static_cast<_Alloc>(_Al2);

        if constexpr (default_initializable<_Alloc> && allocator_traits<_Alloc>::is_always_equal::value){
            // don't store stateless allocator
            _Dealloc_fn const _Dealloc = [](void* const _Ptr, size_t const _Size){
                _Alloc _Al{};
                auto const _Count = (_Size + sizeof(_Dealloc_fn) + sizeof(_Aligned_block) - 1) / sizeof(_Aligned_block);
                _Al.deallocate(static_cast<_Aligned_block*>(_Ptr), _Count);
            };
            auto const _Count = (_Size + sizeof(_Dealloc_fn) + sizeof(_Aligned_block) - 1) / sizeof(_Aligned_block);
            void* const _Ptr = _Al.allocate(_Count);
            ::memcpy(static_cast<char*>(_Ptr) + _Size, &_Dealloc, sizeof(_Dealloc));    // 把函数地址复制到末尾
            return _Ptr;
        }else{
            // store stateful allocator
            static constexpr auto _Align = (max)(alignof(_Alloc), sizeof(_Aligned_block));

            _Dealloc_fn const _Dealloc = [](void* const _Ptr, size_t _Size){
                _Size += sizeof(_Dealloc_fn);
                auto const _Al_address = (reinterpret_cast<uintptr_t>(_Ptr) + _Size + alignof(_Alloc) - 1) & ~(alignof(_Alloc) - 1);
                auto& _Stored_al = *reinterpret_cast<_Alloc const*>(_Al_address);
                _Alloc _Al{move(_Stored_al)};
                _Stored_al.~_Alloc();

                auto const _Count = (_Size + sizeof(_Al) + _Align - 1) / sizeof(_Aligned_block);
                _Al.deallocate(static_cast<_Aligned_block*>(_Ptr), _Count);
            };

            auto const _Count = (_Size + sizeof(_Dealloc_fn) + sizeof(_Al) + _Align - 1) / sizeof(_Aligned_block);
            void* const _Ptr = _Al.allocate(_Count);
            ::memcpy(static_cast<char*>(_Ptr) + _Size, &_Dealloc, sizeof(_Dealloc));
            _Size += sizeof(_Dealloc_fn);
            auto const _Al_address = (reinterpret_cast<uintptr_t>(_Ptr) + _Size + alignof(_Alloc) - 1) & ~(alignof(_Alloc) - 1);
            ::new (reinterpret_cast<void*>(_Al_address))_Alloc{move(_Al)};
        }
    }
public:

    static void* operator new(size_t const _Size){
        // default: new/delete
        void* const _Ptr = ::operator new[](_Size + sizeof(_Dealloc_fn));
        _Dealloc_fn const _Dealloc = [](void* const _Ptr, size_t const _Size){
            ::operator delete[](_Ptr, _Size + sizeof(_Dealloc_fn));
        };
        ::memcpy(static_cast<char*>(_Ptr) + _Size, &_Dealloc, sizeof(_Dealloc));
        return _Ptr;
    }

    template<typename _Alloc, typename... _Args>
    static void* operator new(size_t const _Size, allocator_arg_t, _Alloc const& _Al, _Args&...){
        return _Allocate(_Al, _Size);
    }

    template<typename _This, typename _Alloc, typename... _Args>
    static void* operator new(size_t const _Size, _This&, allocator_arg_t, _Alloc const& _Al, _Args&...){
        return _Allocate(_Al, _Size);
    }

    static void operator delete(void* const _Ptr, size_t const _Size)noexcept{
        _Dealloc_fn _Dealloc;
        ::memcpy(&_Dealloc, static_cast<char const*>(_Ptr) + _Size, sizeof(_Dealloc_fn));
        _Dealloc(_Ptr, _Size);
    }
};

// concept
// 模板特化
template<typename, template<typename...> class>
inline constexpr bool __is_specialization_of = false;

template<typename... _Args, template<typename...> class _Templ>
inline constexpr bool __is_specialization_of<_Templ<_Args...>, _Templ> = true;

template<typename _Ty>
concept _Await_suspend_result = same_as<_Ty, void> || same_as<_Ty, bool> || __is_specialization_of<_Ty, coroutine_handle>;

template<typename _Ty, typename _Promise = void>
concept simple_awaitable = requires(_Ty& _Val, coroutine_handle<_Promise> const& _Coro){
    {
        _Val.await_ready()
    } -> convertible_to<bool>;
    {
        _Val.await_suspend(_Coro)
    } -> _Await_suspend_result;
    _Val.await_resume();
};

// 前向声明
template<typename _Ty = void, typename _Allocator = void>
class [[nodiscard]] lazy;

template<typename _Ty>
class _Lazy_promise_base{
public:
    _Lazy_promise_base()noexcept = default;

    _Lazy_promise_base(_Lazy_promise_base&& _That)noexcept(is_nothrow_move_constructible_v<_Ty>)
        : _Disc{_That._Disc}
    {
        switch (_Disc)
        {
        case _Discriminator::_Data:
            construct_at(addressof(_Data), move(_That._Data));
            destroy_at(addressof(_That._Data));
            break;
        case _Discriminator::_Exception:
            construct_at(addressof(_Except), move(_That._Except));
            destroy_at(addressof(_That._Except));
            break;
        }
        _That._Disc = _Discriminator::_Empty;
    }

    ~_Lazy_promise_base(){
        switch (_Disc)
        {
        case _Discriminator::_Data:
            destroy_at(addressof(_Data));
            break;
        case _Discriminator::_Exception:
            destroy_at(addressof(_Except));
            break;
        
        default:
            break;
        }
    }

    suspend_always initial_suspend()noexcept{return {};}
    auto final_suspend()noexcept{
        return _Final_awaiter{};
    }

    void return_value(_Ty _Val)noexcept
        requires is_reference_v<_Ty>
    {
        switch (_Disc)
        {
        case _Discriminator::_Data:
            _Data = addressof(_Val);
            break;
        }
    }

    template<typename _Uty>
        requires (!is_reference_v<_Ty> && convertible_to<_Uty, _Ty> && constructible_from<_Ty, _Uty>)
    void return_value(_Uty&& _Val){
        switch (_Disc)
        {
        case _Discriminator::_Data:
            destroy_at(addressof(_Data));
            _Disc = _Discriminator::_Empty;
            [[fallthrough]];
        case _Discriminator::_Empty:
            construct_at(addressof(_Data), forward<_Uty>(_Val));
            _Disc = _Discriminator::_Data;
            break;
        }
    }

    void unhandled_exception(){
        switch (_Disc)
        {
        case _Discriminator::_Data:
            destroy_at(addressof(_Data));
            _Disc = _Discriminator::_Empty;
            [[fallthrough]];
        case _Discriminator::_Empty:
            construct_at(addressof(_Except), current_exception());
            _Disc = _Discriminator::_Exception;
            break;
        }
    }

private:

    struct _Final_awaiter: suspend_always{

        template<typename _Promise>
        coroutine_handle<> await_suspend(coroutine_handle<_Promise> _Coro)noexcept{
            _Lazy_promise_base& _Current = _Coro.promise();
            return _Current._Cont ? _Current._Cont : noop_coroutine();
        }
    };

    struct _Awaiter
    {
        coroutine_handle<_Lazy_promise_base> _Coro;

        bool await_ready()const noexcept{
            return !_Coro;
        }

        coroutine_handle<_Lazy_promise_base> await_suspend(coroutine_handle<> _Cont){
            _Coro.promise()._Cont = _Cont;
            return _Coro;
        }

        _Ty await_resume(){
            auto& _Promise = _Coro.promise();
            switch (_Promise._Disc)
            {
            case _Discriminator::_Data:
                if constexpr (is_reference_v<_Ty>){
                    return static_cast<_Ty>(*_Promise._Data);
                }else{
                    return move(_Promise._Data);
                }

                break;
            case _Discriminator::_Exception:
                assert(_Promise._Except && "This can't happen ?");
                rethrow_exception(move(_Promise._Except));
            case _Discriminator::_Empty:
            default:
                assert(false && "This can't happen ?");
                terminate();
                break;
            }
        }
    };    

    enum class _Discriminator: unsigned char{
        _Empty,
        _Exception,
        _Data,
    };
    union{        
        exception_ptr _Except;
        conditional_t<is_reference_v<_Ty>, add_pointer_t<_Ty>, _Ty> _Data;
    };
    _Discriminator _Disc = _Discriminator::_Empty;
    coroutine_handle<> _Cont;
};

template<typename _Ty>
    requires is_void_v<_Ty>
class _Lazy_promise_base<_Ty>{
public:
    _Lazy_promise_base()noexcept = default;

    // noexcept = noexcept(true) 不会抛出异常, noexcept(false) 可能抛出异常
    _Lazy_promise_base(_Lazy_promise_base&& _That)noexcept(is_nothrow_move_constructible_v<_Ty>)
        : _Disc{_That._Disc}
    {
        if(_Discriminator::_Exception == _Disc){
            construct_at(addressof(_Except), move(_That._Except));
            destroy_at(addressof(_That._Except));
        }
        _That._Disc = _Discriminator::_Empty;
    }

    ~_Lazy_promise_base(){
        if (_Discriminator::_Exception == _Disc)
        {
            destroy_at(addressof(_Except));
        }        
    }

    suspend_always initial_suspend()noexcept{
        return {};
    }

    auto final_suspend()noexcept{
        return _Final_awaiter{};
    }

    void return_void()noexcept{}

    void unhandled_exception(){
        if (_Discriminator::_Empty == _Disc)
        {
            construct_at(addressof(_Except), current_exception());
            _Disc = _Discriminator::_Exception;
        }        
    }
private:

    struct _Final_awaiter: suspend_always
    {
        template<typename _Promise>
        coroutine_handle<> await_suspend(coroutine_handle<_Promise> _Coro)noexcept{
            _Lazy_promise_base& _Current = _Coro.promise();
            return _Current._Cont ? _Current._Cont : noop_coroutine();
        }
    };

    struct _Awaiter{
        coroutine_handle<_Lazy_promise_base> _Coro;

        bool await_ready()noexcept{
            return !_Coro;
        }
        
        auto await_suspend(coroutine_handle<> _Cont)noexcept{
            _Coro.promise()._Cont = _Cont;
            return _Coro;
        }

        void await_resume(){
            auto& _Promise = _Coro.promise();
            if (_Discriminator::_Exception == _Promise._Disc)
            {
                rethrow_exception(move(_Promise._Except));
            }
            
        }
    };
    
    enum class _Discriminator: unsigned char{
        _Empty,
        _Exception,
    };

    exception_ptr _Except;

    _Discriminator _Disc = _Discriminator::_Empty;
    coroutine_handle<> _Cont;
};

// _Ty 返回值
template<typename _Ty, typename _Allocator>
class [[nodiscard]] lazy{
public:
    static_assert(is_void_v<_Ty> || is_reference_v<_Ty> || (is_object_v<_Ty> && is_move_constructible_v<_Ty>),
        "lazy's first template must be void, a reference type, or a move-constructible object type");

    struct promise_type: _Promise_allocator<_Allocator>, _Lazy_promise_base<_Ty>
    {
        [[nodiscard]]
        lazy get_return_object()noexcept{
            return coroutine_handle<promise_type>::from_promise(*this);
        }
    };

    lazy(lazy&& _That)noexcept
        : _Coro(exchange(_That._Coro, {}))
    {

    }

    ~lazy(){
        if(_Coro)
            _Coro.destroy();
    }

    suspend_always operator co_await(){
        // Pre: handle_ refers to a coroutine suspened at its initial suspend point
        assert(_Coro && !_Coro.done() && "co_await requires the lazy object to be associated with a coroutine suspended at its initial suspend point");
        return {};
    }

    [[nodiscard]] _Ty sync_await(){
        // Pre: handle_ refers to a coroutine suspened at its initial suspend point
        assert(_Coro && !_Coro.done() && "sync_await requires the lazy object to be associated with a coroutine suspended at its initial suspend point");
        
        simple_awaitable<noop_coroutine_handle>
        auto _Simple = operator co_await();

        assert(!_Simple.await_ready() && "sync_await requires the lazy object to be associated with a coroutine suspended at its initial suspend point");
        // _Simple.await_suspend(noop_coroutine()).resume();
        return _Simple.await_resume();
    }
private:
    lazy(coroutine_handle<promise_type> _Coro_): _Coro{_Coro_}{}

    coroutine_handle<promise_type> _Coro;
};

}   // namespace std
