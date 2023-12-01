#pragma once

#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

/// @brief 基于句柄的对象池
/// @tparam DataType 
/// @tparam CHUNK_COUNT 
template<typename DataType, int CHUNK_COUNT = 256>
class HandlePool;

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

    Handle& init(uint32_t index){
        assert(! valid() && index <= MAX_INDEX);    // 不允许重赋值

        index_ = index;

        static uint32_t s_magic = rand();   // 重启后，可靠性

        if(++s_magic > MAX_MAGIC){
            s_magic = 1;
        }
        magic_ = s_magic;
        return *this;
    }

    // 更新之后 handle 就无效了
    Handle& UpdateMagic(){
        if(++magic_ > MAX_MAGIC){
            magic_ = 1;
        }
        return *this;
    }

    void reset(){
        magic_ = 0; // 设置为无效
    }

    operator bool()const{
        return valid();
    }

    operator uint32_t ()const{
        return handle_;
    }

    bool valid()const{
        return magic_ != 0; // index 可以为0， 因为需要预分配
    }

    bool operator == (const Handle& rhs)const{
        return handle_ == rhs.handle_;
    }
    auto operator <=> (const Handle& rhs)const{
        return index_ <=> rhs.index_;
    }

    uint32_t handle()const{
        return handle_;
    }

    uint32_t index()const{
        return index_;
    }

private:
    template<typename DataType, int CHUNK_COUNT>
    friend class HandlePool;

    void index(uint32_t idx){
        index_ = idx;
    }

    union 
    {
        struct{
            uint32_t index_ : INDEX_BITS;
            uint32_t magic_ : MAGIC_BITS;
        };

        uint32_t handle_{}; // 完整的数据
    };    
};

/**
 * @brief 基于句柄的对象池
 * @tparam DataType
 * @tparam CHUNK_COUNT
@code
    HandlePool<int> mgr("test");

    Handle<int> handle0;
    *mgr.acquire(handle0) = rand();

    Handle<int> handle1;
    *mgr.acquire(handle1) = rand();

    Handle<int> handle2;
    *mgr.acquire(handle2) = rand();

    for(auto const& i: mgr){
        Writer{} << i;
    }
@endcode
*/
template<typename DataType, int CHUNK_COUNT>
class HandlePool{
public:
    using HandleType = Handle<DataType>;
    // friend class HandleType;

    HandlePool(std::string&& desc)
        : name_(std::move(desc))
    {

    }

    ~HandlePool(){
        if(firstUsedNode_){
            std::cerr << "还有节点没释放" << std::endl;
        }
        for(int i = 0; i < CHUNK_COUNT; ++i){
            delete chunks_[i];
        }
        chunks_.clear();
    }

    // 申请一个对象
    DataType* acquire(HandleType& handle);
    // 析构对象
    void release(HandleType const& handle);

    // 修改一下 magic，防止被误用
    bool reNew(HandleType& handle){
        auto internalData = getInternalData(handle);
        if (!internalData)
        {
            return false;
        }
        return internalData->handle_ = handle.UpdateMagic();
    }

    DataType* get(HandleType const& handle){
        auto internalData = getInternalData(handle);
        if(!internalData)
            return {};
            
        return &internalData->data_;
    }

    size_t totalCount()const{
        return CHUNK_COUNT * chunks_.size();    // 已经分配的内存
    }
    size_t usedCount()const{
        return usedCount_;
    }

    std::string const& name()const{
        return name_;
    }
private:

    uint32_t getChunkIdAndPos(HandleType const& handle, uint32_t& pos){
        auto index = handle.index();
        pos = index % CHUNK_COUNT;
        return index / CHUNK_COUNT;
    }

    static constexpr uint32_t INVALID_INDEX = ~0;

    std::string name_;
    size_t usedCount_{};

    struct Chunk{
        struct InternalData{
            DataType data_;
            HandleType handle_;
            InternalData* next_;
        };

        Chunk(int chunkId){
            auto index = chunkId * CHUNK_COUNT;
            for(int i = 0; i < CHUNK_COUNT; ++i){
                internalData_[i].next_ = &internalData_[i] + 1; // 指向下一个
                internalData_[i].handle_.index(index + i);
            }
            internalData_[CHUNK_COUNT - 1].next_ = nullptr; // 最后一个要特殊处理
        }

        InternalData* findPreUsedNode(uint32_t prePos){
            do
            {
                if(prePos >= CHUNK_COUNT){
                    break;
                }

                do{
                    auto node = &internalData_[prePos];
                    if(node->handle_.valid()){
                        return node;
                    }
                }while(prePos-- > 0);
                
            } while (false);
            return {};
        }

        InternalData internalData_[CHUNK_COUNT];
    };
    
    
    Chunk::InternalData* getInternalData(HandleType const& handle){
        if(!handle)
            return {};
        
        uint32_t pos = 0;
        auto chunkId = getChunkIdAndPos(handle, pos);
        if(chunkId >= chunks_.size()){
            return {};
        }
        if(chunks_[chunkId]->internalData_[pos].handle_ != handle){
            return {};
        }
        return &chunks_[chunkId]->internalData_[pos];
    }

    Chunk::InternalData* findPreUsedNode(HandleType const& handle){
        if(! handle){
            return {};
        }

        uint32_t pos = 0;
        auto chunkId = getChunkIdAndPos(handle, pos);
        if(chunkId >= chunks_.size()){
            return {};
        }

        auto curChunk = chunks_[chunkId];

        auto node = curChunk->findPreUsedNode(pos - 1);
        do
        {
            if(node){
                break;
            }
            if(chunkId-- > 0){
                curChunk = chunks_[chunkId];
                node = curChunk->findPreUsedNode(CHUNK_COUNT - 1);
            }            
        } while (false);
        return node;        
    }

    std::vector<Chunk*> chunks_;

    // 无论是有效还是无效的节点，都说按照 handle.index 升序 < 排序的 
    Chunk::InternalData* firstUsedNode_{};  // 第一个使用的节点

    Chunk::InternalData* freeList_{};

public:

    class iterator{
        Chunk::InternalData* node_{};   // 当前节点
    public:
        iterator(Chunk::InternalData* node = {})
            : node_(node)
        {

        }

        iterator& operator ++ (){
            node_ = node_->next_;
            return *this;
        }

        bool operator != (iterator const& rhs)const{
            return node_ != rhs.node_;
        }

        DataType& operator * ()const{
            return node_->data_;
        }
    };

    iterator begin(){
        return iterator(firstUsedNode_);
    }
    iterator end(){
        return {};
    }
};

//////////////////////////////////////////////////////////////////////////
template<typename DataType, int CHUNK_COUNT>
DataType* HandlePool<DataType, CHUNK_COUNT>::acquire(HandleType& handle){
    do
    {
        if( handle){
            break;
        }

        if (freeList_ == nullptr)
        {
            Chunk* chunk = new (std::nothrow)Chunk{ static_cast<int>(chunks_.size()) };
            if (chunk == nullptr)
            {
                break;
            }
            
            chunks_.push_back(chunk);
            freeList_ = chunk->internalData_;
        }

        decltype(freeList_) node = freeList_;
        freeList_ = freeList_->next_;
        node->next_ = nullptr;
        handle = node->handle_.init(node->handle_.index()); // 初始化 handle
        new(node)DataType;
        ++usedCount_;
        
        // 链到前面一个
        auto prevNode = findPreUsedNode(handle);
        if(prevNode){
            node->next_ = prevNode->next_;
            prevNode->next_ = node;            
        }else{
            if(firstUsedNode_){
                node->next_ = firstUsedNode_->next_;
            }
            firstUsedNode_ = node;
        }     
        
        return &node->data_;
    } while (false);

    return {};    
}

template<typename DataType, int CHUNK_COUNT>
void HandlePool<DataType, CHUNK_COUNT>::release(HandleType const& handle){
    auto node = getInternalData(handle);
    if (!node)
    {
        return;
    }
    
    auto& data = node->data_;
    data.~DataType();
    node->handle_.reset();
    auto *const next = node->next_;
    if(--usedCount_ > 0){
        auto prevNode = findPreUsedNode(handle);
        if (prevNode)
        {
            prevNode->next_ = next;
        }else{  // 删除第一个节点
            firstUsedNode_ = next;
        }
    }
    else    // 删除 最后一个节点
    {
        assert(firstUsedNode_ == node && node->next_ == nullptr);
        firstUsedNode_ = next;
    }  
    
    if(freeList_){
        if (node->handle_ < freeList_->handle_)
        {
            node->next_ = freeList_;
            freeList_ = node;
        }else{
            auto cur = freeList_;
            auto next = freeList_->next_;
            while(next && next->handle_ < handle){
                cur = next;
                next = next->next_;
            }
            node->next_ = next;
            cur->next_ = node;
        }
    }else{
        node->next_ = nullptr;
        freeList_ = node;
    }
}
