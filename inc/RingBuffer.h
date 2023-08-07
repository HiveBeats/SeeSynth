#pragma once
#include <cstddef>
template <typename T>
class RingBuffer
{
private:
    T* m_items; /* data */
    std::size_t m_head;
    std::size_t m_tail;
    bool m_is_full;
    bool m_is_empty;
    std::size_t m_size;
    void advance_pointer();
    void retreat_pointer();
public:
    RingBuffer(std::size_t size);
    ~RingBuffer();
    bool IsFull() { return m_is_full; }
    bool IsEmpty() { return m_is_empty; }
    std::size_t GetSize();
    std::size_t GetCapacity() { return m_size; }
    void Reset();
    void Write(T* data, size_t count);
    bool Read(T* output, size_t count);
    void Print();
};