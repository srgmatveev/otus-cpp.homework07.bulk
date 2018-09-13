#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <memory>
#include <cstddef>
#include <map>
#include "utils.h"

class BulkStorageCell
{
private:
  std::vector<std::string> _commands;
  std::size_t timestamp{0};

public:
  void add(const std::string &val)
  {
    _commands.emplace_back(val);
  }

  void set_timestamp(std::size_t ts) { timestamp = ts; }
  std::size_t get_timestamp() const { return timestamp; }

  std::vector<std::string> &get_cells() { return _commands; }
};

class BulkStorage
{
private:
  std::map<std::size_t, std::shared_ptr<BulkStorageCell>> _cmdStorage;
  std::vector<std::string> false_cell{};

public:
  BulkStorage() : _cmdStorage{} {};
  std::size_t create_bulk();
  void appendToCmdStorage(std::size_t, const std::string &);
  std::vector<std::string> &get_commands(std::size_t);
  std::size_t get_timestamp(std::size_t);
  void set_timestamp(std::size_t, std::size_t);
  void deleteStorageCell(std::size_t);
};