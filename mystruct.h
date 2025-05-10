//Гринчак Роман, група К-15, варіант 1

#pragma once
#include <cstddef>
#include <initializer_list>

class MyStruct
{
protected:
    template <class T>
    class MyIterator{
        friend class MyStruct;
        public:
            using difference_type = void;
            using reference = T &;
            using pointer = T *;
            MyIterator() = default;
            MyIterator( MyStruct* container, T* ptr, int index ) : owner(container), cur(ptr), blockIndex(index) {}
            MyIterator &operator++();
            MyIterator operator++(int);
            MyIterator &operator--();
            MyIterator operator--(int);
            reference operator*() const;
            pointer operator->() const;
            bool operator== ( MyIterator other ) const noexcept { return cur == other.cur; }
        private:
            MyStruct* owner = nullptr;
            int blockIndex = 0; 
            pointer cur = nullptr;
    };

public:
    using value_type = long long;
    using reference = long long&;
    using const_reference = const long long&;
    using iterator = MyIterator <long long>;
    using const_iterator = MyIterator <const long long>;
    using size_type = std::size_t;

    MyStruct();
    MyStruct( const std::initializer_list< long long >& );
    MyStruct( const MyStruct& );
    MyStruct( MyStruct&& ) noexcept;
    MyStruct &operator= ( const MyStruct& );
    MyStruct &operator= ( MyStruct&& ) noexcept;
    ~MyStruct();

    size_type size() const noexcept { return size_; } 
    size_type capacity() const noexcept { return capacity_; } 
    bool empty() const noexcept { return (size() == 0); } 

    const_iterator cbegin() const noexcept { 
        if (capacity() == 0)
            return const_iterator( const_cast<MyStruct*>(this), nullptr, 0 );
        return const_iterator( const_cast<MyStruct*>(this), arr[0], 0 ); 
    }
    const_iterator cend() const noexcept { 
        if ( size() == 0 )
            return cbegin(); 
        std::size_t lastBlock = (size() - 1) / BLOCK_SIZE;
        std::size_t lastPosition = (size() - 1) % BLOCK_SIZE;
        if (lastPosition == BLOCK_SIZE - 1) 
        {
            if (lastBlock + 1 < blockNumber) {
                return const_iterator( const_cast<MyStruct*>(this), arr[lastBlock + 1], lastBlock + 1);
            }
        }
        return const_iterator( const_cast<MyStruct*>(this), arr[lastBlock] + lastPosition + 1, lastBlock ); 
    }

    const_iterator begin() const noexcept { return cbegin(); }
    const_iterator end() const noexcept { return cend(); }

    iterator begin() noexcept { 
        if (capacity() == 0)
            return iterator( this, nullptr, 0 );
        return iterator( this, arr[0], 0 ); 
    }
    iterator end() noexcept 
    { 
        if ( size() == 0 )
            return begin();
        size_t lastBlock = (size() - 1) / BLOCK_SIZE;
        size_t lastPosition = (size() - 1) % BLOCK_SIZE;
        if (lastPosition == BLOCK_SIZE - 1) 
        {
            if (lastBlock + 1 < blockNumber) {
                return iterator( this, arr[lastBlock + 1], lastBlock + 1);
            }
        }
        return iterator( this, arr[lastBlock] + lastPosition + 1, lastBlock ); 
    }

    iterator insert( iterator pos, long long value );
    void push_back( long long value );
    void push_front( long long value );
    void pop_back();
    void pop_front();

    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    iterator erase( const_iterator pos );
    void clear() noexcept;
    void shrink_to_fit();

protected:
    struct Fictive {};
    explicit MyStruct( Fictive );
    const static size_type BLOCK_SIZE = 3; // Потім заміню, зараз таку поставив для зручного тестування
    size_type blockNumber = 1;
    long long** arr = nullptr;
    size_type size_ = 0; 
    size_type capacity_ = 0; 
    void create( std::size_t n );
    bool full() const noexcept { return (size() == capacity()); }
    void addBlock();
    void slideRight ( iterator pos );
    void slideLeft ( iterator pos );
    bool tooBig() const noexcept { return ( capacity() >= 3 * size() ) && ( blockNumber > 1 ); }
    void makeSmaller();
};

// БЛОК ІТЕРАТОРІВ
template <class T>
MyStruct::MyIterator<T> &MyStruct::MyIterator<T>::operator++()
{
    if ( owner->blockNumber == 0 )
    {
        cur = nullptr;
        blockIndex = 0;
        return *this;
    }
    
    if (blockIndex >= owner->blockNumber)
    {
        blockIndex = owner->blockNumber - 1;
    }
    if ( blockIndex < 0 )
    {
        blockIndex = 0;
    }

    if ( cur == owner->arr[blockIndex] + BLOCK_SIZE - 1 )
    {
        ++blockIndex;
        if ( blockIndex == owner->blockNumber )
        {
            ++cur;
            --blockIndex;
        }
        else 
            cur = owner->arr[blockIndex];
    }
    else
        ++cur;
    return *this;
}

template <class T>
MyStruct::MyIterator<T> MyStruct::MyIterator<T>::operator++( int )
{
    MyIterator tmp = *this;
    ++(*this);
    return tmp;
}

template <class T>
MyStruct::MyIterator<T> &MyStruct::MyIterator<T>::operator--()
{
    if ( owner->blockNumber == 0 )
    {
        cur = nullptr;
        blockIndex = 0;
        return *this;
    }
    
    if (blockIndex >= owner->blockNumber)
    {
        blockIndex = owner->blockNumber - 1;
    }
    if ( blockIndex < 0 )
    {
        blockIndex = 0;
    }

    if ( cur == owner->arr[blockIndex] )
    {
        --blockIndex;
        if ( blockIndex < 0 )
        {
            blockIndex = 0;
            --cur;
        }
        else 
            cur = owner->arr[blockIndex] + BLOCK_SIZE - 1;
    }
    else
        --cur;
    return *this;
}

template <class T>
MyStruct::MyIterator<T> MyStruct::MyIterator<T>::operator--( int )
{
    MyIterator tmp = *this;
    --(*this);
    return tmp;
}

template <class T>
MyStruct::MyIterator<T>::reference MyStruct::MyIterator<T>::operator*() const
{
    return *cur;
}

template <class T>
MyStruct::MyIterator<T>::pointer MyStruct::MyIterator<T>::operator->() const
{
    return &(this->operator*());
}
// КІНЕЦЬ БЛОКУ ІТЕРАТОРІВ