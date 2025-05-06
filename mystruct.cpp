#include "mystruct.h"
#include <utility>
#include <stdexcept>

// БЛОК КОНСТРУКТОРІВ І ДЕСТРУКТОРІВ
MyStruct::MyStruct( Fictive f) {}

void MyStruct::create( std::size_t n )
{
    if ( n == 0 )
        return;
    int blocksRequired = ( n - 1 ) / BLOCK_SIZE + 1;
    arr = new long long*[blocksRequired] {};
    for ( std::size_t i = 0; i < blocksRequired; ++i )
    {
        arr[i] = new long long[BLOCK_SIZE] {};
    }
    size_ = 0;
    blockNumber = blocksRequired;
    capacity_ = blocksRequired * BLOCK_SIZE;
}

MyStruct::MyStruct() : MyStruct(Fictive{})
{
    create( BLOCK_SIZE );
}

MyStruct::MyStruct( const std::initializer_list< long long >& list ) : MyStruct(Fictive{})
{
    if ( list.size() == 0 )
    {
        create( BLOCK_SIZE );
        return;
    }
    else
        create( list.size() );
    int index = 0;
    for ( auto it = list.begin(); it != list.end(); ++it )
    {
        arr[index / BLOCK_SIZE][index % BLOCK_SIZE] = *it;
        ++index;
    }
    size_ = list.size();
}

MyStruct::MyStruct( const MyStruct& other ) : MyStruct(Fictive{})
{
    *this = other;
}

MyStruct::MyStruct( MyStruct&& other ) noexcept 
{
    *this = std::move( other );
}

MyStruct& MyStruct::operator=( const MyStruct& other )
{
    if ( this == &other )
        return *this;
    if ( other.blockNumber > blockNumber ){
        clear();
        create( other.size_ );
    }
    for ( std::size_t i = 0; i < other.size(); ++i )
        arr[i / BLOCK_SIZE][i % BLOCK_SIZE] = other.arr[i / BLOCK_SIZE][i % BLOCK_SIZE];
    size_ = other.size_;
    blockNumber = other.blockNumber;
    return *this;
}

MyStruct& MyStruct::operator=( MyStruct&& other ) noexcept 
{
    if ( this == &other )
        return *this;
    clear();
    arr = other.arr;
    size_ = other.size_;
    blockNumber = other.blockNumber;
    capacity_ = other.capacity_;
    other.arr = nullptr;
    other.size_ = 0;
    other.blockNumber = 0;
    other.capacity_ = 0;
    return *this;
}

MyStruct::~MyStruct()
{
    clear();
}

void MyStruct::clear() noexcept
{
    if ( arr == nullptr )
        return;
    for ( std::size_t i = 0; i < blockNumber; ++i )
        delete[] arr[i];
    delete[] arr;
    arr = nullptr;
    size_ = 0;
    blockNumber = 0;
    capacity_ = 0;
}
// КІНЕЦЬ БЛОКУ КОНСТРУКТОРІВ І ДЕСТРУКТОРІВ

// БЛОК front back
MyStruct::reference MyStruct::front()
{
    if ( size() == 0 )
        throw std::out_of_range( "MyStruct::front: empty container" );
    return arr[0][0];
}

MyStruct::const_reference MyStruct::front() const
{
    if ( size() == 0 )
        throw std::out_of_range( "MyStruct::front: empty container" );
    return arr[0][0];
}

MyStruct::reference MyStruct::back()
{
    if ( size() == 0 )
        throw std::out_of_range( "MyStruct::back: empty container" );
    return arr[(size() - 1) / BLOCK_SIZE][(size() - 1) % BLOCK_SIZE];
}

MyStruct::const_reference MyStruct::back() const
{
    if ( size() == 0 )
        throw std::out_of_range( "MyStruct::back: empty container" );
    return arr[(size() - 1) / BLOCK_SIZE][(size() - 1) % BLOCK_SIZE];
}
// Кінець БЛОКУ front back

// БЛОК ЗМІН КОНТЕЙНЕРА
void MyStruct::addBlock()
{
    long long* newBlock = new long long[BLOCK_SIZE] {};
    try{
        long long** tmp = new long long*[blockNumber + 1] {};
        for ( std::size_t i = 0; i < blockNumber; ++i )
            tmp[i] = arr[i];
        tmp[blockNumber] = newBlock;
        delete[] arr;
        arr = std::move(tmp);
        ++blockNumber;
        capacity_ += BLOCK_SIZE;
    }
    catch (...)
    {
        delete[] newBlock;
        throw;
    }
}

void MyStruct::makeSmaller()
{
    int newCapacity = size() * 1.2;
    int newBlocksRequired = ( newCapacity - 1 ) / BLOCK_SIZE + 1;
    for ( std::size_t i = newBlocksRequired; i < blockNumber; ++i ){
        delete[] arr[i];
        arr[i] = nullptr;
    }
    long long** tmp = new long long*[newBlocksRequired] {};
    for ( std::size_t i = 0; i < newBlocksRequired; ++i )
        tmp[i] = arr[i];
    delete[] arr;
    arr = tmp;
    blockNumber = newBlocksRequired;
    capacity_ = newCapacity;
}

void MyStruct::slideRight( iterator pos )
{
    if ( size() == 0 || pos == end() )
         return;

    if ( full() )
        addBlock();
    
    iterator tmp1 = --end();
    iterator tmp2 = tmp1;
    ++tmp1;
    while ( tmp1 != pos )
    {
        *tmp1 = *tmp2;
        --tmp1;
        --tmp2;
    }
}

void MyStruct::slideLeft( iterator pos )
{
    if ( size() == 0 || pos == end() )
         return;

    iterator next = pos;
    ++next;
    iterator end = this->end();
    while ( next != end )
    {
        *pos = *next;
        ++pos;
        ++next;
    }
}

void MyStruct::push_back( long long value ) // Через особливості ітератора end метод не був реалізований через insert(end(), value)
{
    if ( full() )
        addBlock();
    arr[size() / BLOCK_SIZE][size() % BLOCK_SIZE] = value;
    ++size_;
}

void MyStruct::push_front( long long value )
{
    insert( begin(), value );
}

MyStruct::iterator MyStruct::insert( iterator pos, long long value )
{
    if ( full() )
        addBlock();
    slideRight( pos );
    *pos = value;
    ++size_;
    return iterator( this, pos.cur, pos.blockIndex );
}

void MyStruct::pop_back()
{
    erase( --end() );
}

void MyStruct::pop_front()
{
    erase( begin() );
}

void MyStruct::erase( iterator pos )
{
    if ( size() == 0 )
        throw std::out_of_range( "MyStruct::erase: empty container" );
    if ( pos == end() )
        return;
    slideLeft( pos );
    arr[(size() - 1) / BLOCK_SIZE][(size() - 1) % BLOCK_SIZE] = 0;
    --size_;
    if ( tooBig() )
        makeSmaller();
}

void MyStruct::shrink_to_fit()
{
    if ( size() == 0 )
    {
        clear();
        return;
    }

    int blocksRequired = ( size() - 1 ) / BLOCK_SIZE + 1;
    if ( blocksRequired == blockNumber )
        return;

    for ( std::size_t i = blocksRequired; i < blockNumber; ++i )
    {
        delete[] arr[i];
        arr[i] = nullptr;
    }
    long long** tmp = new long long*[blocksRequired] {};
    for ( std::size_t i = 0; i < blocksRequired; ++i )
        tmp[i] = arr[i];
    delete[] arr;
    arr = tmp;
    blockNumber = blocksRequired;
    capacity_ = blocksRequired * BLOCK_SIZE;
}
// КІНЕЦЬ БЛОКУ ЗМІН КОНТЕЙНЕРА