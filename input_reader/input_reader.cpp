#include "input_reader.hpp"

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(linux) || defined(__linux__)
# define PREDEF_PLATFORM_UNIX 1
#endif

#if defined( __APPLE__ ) || defined( __MACH__ )
# define PREDEF_PLATFORM_MAC 1
#endif

#ifdef PREDEF_PLATFORM_UNIX
#include <sys/sysinfo.h>
#elif PREDEF_PLATFORM_MAC
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#endif

#include <cerrno>

#include <stdexcept>
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

struct InputReader::InputReaderImpl {

  InputReaderImpl();
  ~InputReaderImpl() = default;

  void parse_input( int argc, const char *argv[] );
  void set_dst( const std::string &dst );
  void set_src( const std::string &src );
  void set_block_size( std::int64_t );
  void print_help();

  po::variables_map m_vm;
  static constexpr std::uint32_t m_bytes_in_megabyte{ 1048576 };
  std::uint32_t m_block_top_limit{ 1 };
  std::uint64_t m_block_size{ m_bytes_in_megabyte };
  std::string m_src_file;
  std::string m_dst_file{ "hashed_file" };
};

InputReader::InputReader( int argc, const char *argv[] )
  : input_reader_impl{ std::make_unique<InputReader::InputReaderImpl>() }
{
  input_reader_impl->parse_input( argc, argv );
}

InputReader::~InputReader() {}

const std::string &InputReader::get_src_file() const
{
  if( input_reader_impl->m_src_file.empty() )
    throw std::invalid_argument( "No source file passed" );

  return input_reader_impl->m_src_file;
}

const std::string &InputReader::get_dst_file() const noexcept
{
  return input_reader_impl->m_dst_file;
}

std::uint64_t InputReader::get_block_size() const noexcept
{
  return input_reader_impl->m_block_size;
}

// ============================================================================== //


InputReader::InputReaderImpl::InputReaderImpl()
{
#ifdef PREDEF_PLATFORM_UNIX
  struct sysinfo info;
  if( sysinfo( &info ) < 0 )
       throw std::system_error( "Fail to calculate block top bound" );

    m_block_top_limit = info.freeram;
#elif PREDEF_PLATFORM_MAC
    int mib[] = { CTL_HW, HW_PAGESIZE };
    u_int page_size{ 0 };
    struct vmtotal vmt;
    size_t vmt_size, uint_size;

    vmt_size = sizeof( vmt );
    uint_size = sizeof( page_size );

    int rc = sysctl( mib, 2, &vmt, &vmt_size, NULL, 0 );
    if ( rc < 0 )
       throw std::system_error( errno, std::generic_category().default_error_condition( errno ).category(),
                                "Fail to calculate block top bound" );

    rc = sysctl( mib, 2, &page_size, &uint_size, NULL, 0 );
    if ( rc < 0 )
       throw std::system_error( errno, std::generic_category().default_error_condition( errno ).category(),
                                "Fail to calculate block top bound" );

    m_block_top_limit = ( vmt.t_free * static_cast<u_int64_t>( page_size ) ) / m_bytes_in_megabyte;
#endif
}

void InputReader::InputReaderImpl::parse_input( int argc, const char *argv[] )
{
  if( argc <= 1 )
    throw std::invalid_argument( "No arguments provided" );

  try {
    po::options_description desc{ "Options" };
    desc.add_options()
      ( "help,h", "Available options" )
      ( "src", po::value<std::string>(), "source file" )
      ( "dst", po::value<std::string>(), "destination file" )
      ( "block", po::value<std::int64_t>()->default_value(1), "block size" );

    po::store( po::parse_command_line( argc, argv, desc ), m_vm );
    po::notify( m_vm );

    if( m_vm.count( "help" ) ) {
      print_help();
    } else {
      if( m_vm.count( "src" ) )
	set_src( m_vm[ "src" ].as<std::string>() );

      if( m_vm.count( "dst" ) )
	set_dst( m_vm[ "dst" ].as<std::string>() );

      if( m_vm.count( "block" ) )
	set_block_size( m_vm[ "block" ].as<std::int64_t>() );
    }

  } catch( const std::exception &exp ) {
    std::cerr << "On parsing user input error occured. Err: " << exp.what()  << std::endl;
  }
}

void InputReader::InputReaderImpl::set_dst( const std::string &dst )
{
  if( dst.empty() )
    return;

  m_dst_file = dst;
}

void InputReader::InputReaderImpl::set_src( const std::string &src )
{
  m_src_file = src;
}

void InputReader::InputReaderImpl::set_block_size( std::int64_t megabytes )
{
  if( megabytes <= 0 )
    throw std::invalid_argument( "Block size cannot be less then 1 MB" );
  
  if( megabytes >= std::numeric_limits<std::uint64_t>::max() / m_bytes_in_megabyte )
    throw std::invalid_argument( "Block size is to huge" );

  m_block_size = megabytes * m_bytes_in_megabyte;
}

void InputReader::InputReaderImpl::print_help()
{
  std::cout << "Available options: " << std::endl;
  std::cout << "--help/-h --> output this help " << std::endl;
  std::cout << "--src --> [REQUIRED] source file. File to hash" << std::endl;
  std::cout << "--dst --> [OPTIONAL] destination file. File where hash will be written\n"
            << "                     If not provided then default \"hashed_file\" in the /tmp \n"
            << "                     directory will be created and used for\n"
            << "                     writing hashed information from source file." << std::endl;
  std::cout << "--block --> [OPTIONAL] size of the hashed block. By default it is set to 1 MB.\n"
            << "                       You have to provide block size in MB. Input must not exceed "
            << m_block_top_limit << " MB"
            << "                       and should be more than 0" << std::endl;

}
