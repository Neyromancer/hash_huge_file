#pragma once

#include <string>
#include <memory>

class InputReader {
public: 
  InputReader( int argc, const char *argv[] );
  ~InputReader();

  /// @brief Возращает имя файла источника.
  /// Если файл не удалось получить из argv, то кидает исключение.
  const std::string &get_src_file() const;

  /// @brief Возвращает имя конечного файла,
  /// Файл достается из argv или же возвращается выставленный по-умолчанию.
  const std::string &get_dst_file() const noexcept;

  /// @brief Возвращает размер блока в байтах.
  /// получает из argv или же выставленный по-умолчанию 1 МБт
  std::uint64_t get_block_size() const noexcept;

private:
  struct InputReaderImpl;
  std::unique_ptr<InputReader::InputReaderImpl> input_reader_impl;
};
