#include <cstdlib>

#include <iostream>

#include "input_reader.hpp"

int main( int argc, const char *argv[] )
{
  try{
    InputReader input_reader( argc, argv );
    std::cout << "source file: " << input_reader.get_src_file() << std::endl;
    std::cout << "destination file: " << input_reader.get_dst_file() << std::endl;
    std::cout << "block size: " << input_reader.get_block_size() << std::endl;
  } catch( const std::exception &exp ) {
    std::cerr << "Exception occured " << exp.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
