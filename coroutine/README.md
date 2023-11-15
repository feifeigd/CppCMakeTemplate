
# 新加关键字
+ co_await
+ co_yield
+ co_return

# coroutine 分为 asymmetric coroutine 和 symmetric coroutine, C++20的是非对称协程
+ asymmetric coroutine 不对称协程：工作流返回到 caller
+ symmetric coroutine 对称协程：可以把工作流切换到另外一个 coroutine

# 在普通函数中使用关键字使其变成coroutine，有四种方法：
+ co_return expression, 遇到这个关键字，会调用 promise.return_void() 或者 promise.return_value(expression)
+ co_await
+ co_yield
+ co_await awaitable expression, in a range-based for loop

# coroutine 的两个方面
+ Coroutine Factory 协程函数
+ Coroutine Object 协程函数的返回值

Coroutine Factory 不能有return语句，main函数不能成为 Coroutine Factory，也就是不能有 co_* 那几个关键字

# Framework
实现 Coroutine，由超过20个函数组成，其中有些函数你必须实现，有些函数你可以重载
## Coroutine 与三部分关联：
+ promise object
+ coroutine handle，用于与 promise object 交互
+ coroutine frame，用于保存 coroutine handle 的状态

## promise object 必须支持的接口
|Member Function|Description|
|-|-|
|默认构造函数| promise 必须能默认构造|
| awaitable initial_suspend()| determines if the coroutine suspends before it runs.
|awaitable final_suspend noexcept()| determines if the coroutine suspends before it ends.
|unhandled_exception()|called when an exception happens.
|resumable get_return_object()|returns the coroutine object(resumable object). resumable type 必须有个内部类 promise_type
|return_value(val)|is invoked by co_return val.
|return_void()|is invoked by co_return.
|awaitable yield_value(val)|is invoked by co_yield val.

## coroutine handle
用于控制 coroutine frame 恢复或者销毁

## coroutine frame
调用协程函数，会复制参数到 coroutine frame

# coroutine framework 至关重要的抽象：
+ awaitable
+ awaiter

## awaitable
awaitable 是可以对其进行 co_await 操作的对象, 用于决定 croutine 是否暂停
awaitable 必须实现三个函数
|Function|Description|
|-|-|
|bool await_ready|如果return false，await_suspend 将会被调用|
|void|bool|handle await_suspend|用于调度协程是重新开始还是销毁|
|void await_resume|给 co_await expression 提供返回结果|

## awaiter
有两种方法获得 awaiter
+ 定义了 co_await 操作符
+ awaitable 转变成 awaiter, awaiter = awaitable

# workflow
编译器会将协程函数转换为两个工作流
+ 外部 promise workflow
+ 内部 awaiter workflow

## promise workflow
# 协程函数的框架
```cpp
{
    Promise prom;   // 使用了 resumable内嵌的 promise_type 类型
    // resumable = prom.get_return_object(); return resumable;    // 创建协程函数的返回值
    co_await prom.initial_suspend();
    try{
        <function body having co_return, co_yield, or co_wait >
    }catch(...){
        prom.unhandled_exception();
    }
FinalSuspend:
    co_await prom.final_suspend();
}
```

## awaiter workflow
当 promise workflow 显式或隐式调用下面的函数时，切换到 awaiter workflow
+ co_await prom.initial_suspend()
+ co_await prom.final_suspend()
+ co_await prom.yield_value(value)

awaiter workflow 的框架

```cpp
{
    // 根据 co_await expr 获得 awaiter
    auto&& value = <expr>;
    auto&& awaitable = get_awaitable(promise, static_cast<decltype(value)>(value));
    auto&& awaiter = get_awaiter(static_cast<decltype(awaitable)>(awaitable));

awaiter.await_ready() returns false:
    <suspend coroutine>
    // 根据返回类型和返回值的不同，流程也不同
    // 无论哪种情况，都会先调用 awaiter.await_suspend(coroutineHandle);
    awaiter.await_suspend(coroutineHandle) returns:
        void:   // keeps suspended and return to caller, 即总是挂起
            try{
                awaiter.await_suspend(coroutineHandle);
                coroutine keeps suspended
                return to caller
            }catch(...){
                exception = std::current_exception();
                goto resume_point;
            }
        bool:
            bool result;
            try{
                result = awaiter.await_suspend(coroutineHandle);
            }catch(...){
                exception = std::current_exception();
                goto resume_point;
            }

            if result:  // keeps suspended and return to caller
                coroutine keep suspended
                return to caller
            else:   // false
                goto resumptionPoint
        // 返回另一个协程的句柄
        another coroutine handle:   // the other coroutine is resumed and returns to the caller
            decltype(awaiter.await_suspend(std::declval<coro_handle_t>())) anotherCoroutineHandle;
            try{
                anotherCoroutineHandle = awaiter.await_suspend(coroutineHandle);
            }catch(...){
                exception = std::current_exception();
                goto resume_point;
            }
            anotherCoroutineHandle.resume();
            return to caller

resumptionPoint:
if(exception)
    std::rethrow_exception(exception);
// resume and does not return to the caller
return awaiter.await_resume(); // 给 co_await expression 提供返回结果
}
```

最后 co_yield <expr> 其实是 co_await 的语法糖，生成如下代码：
```cpp
co_await promise.yield_value(expression);
```
而co_return则会调用Promise对象的return_void/return_value方法。

当 awaiter workflow 发送异常时，按照异常发送在 awaitable 的3个函数里，而不同处理：
|异常函数||
|-|-|
|await_ready|coroutine 不挂起, 也不执行 await_suspend 或 await_resume|
|await_suspend|croutine 恢复，捕获异常重新抛出，不会调用await_resume函数|
|await_resume|await_ready 和 await_suspend 都已执行并返回值，await_resume 不会 return 一个 result|

# 参考文章
+ https://zhuanlan.zhihu.com/p/569480618
