#include "file_manager.hpp"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "/storage/manager_storage.hpp"


namespace ifmap = boost::interprocess::file_mapping;

struct FileManager::FileManagerImpl {

};

void FileManager::read_from( const std::string &fname ) const noexcept
{
  
}

void FileManager::write_to( const std::string &fname ) const noexcept
{

}

void FileManager::transfer( const std::string &src, const std::string &dst,
                            const std::function<void()> &converter = std::function<void()>() )
{

}
