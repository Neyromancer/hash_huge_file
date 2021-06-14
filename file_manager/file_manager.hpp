#pragma once

#include <functional>
#include <memory>
#include <string>

class FileManager {
public:
  void read_from( const std::string &fname ) const noexcept;
  void write_to( const std::string &fname ) const noexcept;
  void transfer( const std::string &src, const std::string &dst, 
                 const std::function<void()> &converter );

private:
  struct FileMnagerImpl;
  std::unique<FileManager::FileManagerImpl> file_manager_impl;
};
