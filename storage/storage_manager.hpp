#pragma once

#include <boost/lockfree/queue.hpp>

#include "Util/Sys/memory_manager.hpp"

namespace lf = boost::lockfree;

template <class T>
class StorageManager {
public:
  StorageManager() : q{ MemoryManager::get_available_memory() }
  [[ nodiscard ]] bool push( const T &data );
  T pop() noexcept;
  std::size_t size() const noexcept{
    return size;
  }

private:
  lf::queue<T, lf::fixed_sized<true>> q;
  std::atomic<std::size_t> size{ 0 };
};

template<class T>
bool StorageManager::push( const T &data )
{
  
  if constexpr( q.bounded_push( data ) ) {
    ++size;
    return true;
  
  return false;
}

template<class T>
T pop()
{
  T res;
  if constexpr( !q.empty() && q.pop( res ) )
    --size;

  return res;
}
