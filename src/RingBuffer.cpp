#include "RingBuffer.h"

template <typename T> RingBuffer<T>::RingBuffer(std::size_t size) 
{
    m_items = new T[size];
    m_head = 0;
    m_tail = 0;
    m_is_full = 0;
    m_is_empty = 1;
    m_size = size;
}

template <typename T> RingBuffer<T>::~RingBuffer() 
{
    delete m_items;
}

template <typename T> void RingBuffer<T>::Reset() 
{
    m_head = 0;
    m_tail = 0;
    m_is_full = 0;
}

template <typename T> void RingBuffer<T>::advance_pointer() 
{
    if(m_is_full) {
        m_tail++;
		if (m_tail == m_size) { 
			m_tail = 0;
		}
	}
    m_head++;
	if (m_head == m_size) { 
		m_head = 0;
	}
    std::size_t p_is_full = m_head == m_tail ? 1 : 0;
	m_is_full = p_is_full;
}

template <typename T> void RingBuffer<T>::retreat_pointer() 
{
	m_is_full = 0;
    m_tail++;
	if (m_tail == m_size) { 
		m_tail = 0;
	}
}

template <typename T> void RingBuffer<T>::Write(T* data, std::size_t count) 
{
    if (m_is_full || m_head + count > m_size) {
        //todo: implement
        // write_log("[WARN] Trying to overfill the ring buffer: \n\tIsFull:%d\n\tHead:%zu\n\tCount:%zu\n\t", 
        //     m_is_full,
        //     m_head, 
        //     count);
        return;
    }
    m_is_empty = 0;

    for (std::size_t i = 0; i < count; i++) {
        m_items[m_head] = data[i];
        advance_pointer(buffer);
    }
    //m_is_empty = m_is_full && (m_head == m_tail);
}

template <typename T> bool RingBuffer<T>::Read(T* output, std::size_t count) 
{
    if (m_is_empty) {
        //todo: implement
        //write_log("[WARN] Trying to read empty buffer");
        return 0;
    }

    for (std::size_t i = 0; i < count; i++) {
        output[i] = m_items[m_tail];
        retreat_pointer(buffer);
    }
    m_is_empty = !m_is_full && (m_head == m_tail);
    return 1;
}

template <typename T> std::size_t RingBuffer<T>::GetSize() 
{
	size_t p_size = m_size;
	if(!m_is_full) {
		if(m_head >= m_tail) {
			p_size = (m_head - m_tail);
		}
		else {
			p_size = (m_size + m_head - m_tail);
		}
	}

	return p_size;
}

template <typename T> void RingBuffer<T>::Print() {
    //todo: implement
    // write_log("[INFO] The ring buffer: \n\tIsFull:%d\n\tIsEmpty:%d\n\tHead:%zu\n\tTail:%zu\n\t", 
    //             me->m_is_full, 
    //             me->m_is_empty, 
    //             me->m_head, 
    //             me->m_tail);
}