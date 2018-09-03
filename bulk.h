#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <memory>
#include <cstddef>
#include <map>
#include "utils.h"

class BulkReadCmd;

class BulkStorageCell
{
private:
  std::vector<std::string> _commands;

public:
  Property<std::size_t> timestamp{0};
  void add(const std::string &val)
  {
    _commands.emplace_back(val);
  }

  std::vector<std::string> &get_cells() { return _commands; }
};

class BulkStorage
{
private:
  std::map<std::size_t, std::shared_ptr<BulkStorageCell>> _cmdStorage;

public:
  BulkStorage() : _cmdStorage{} {};
  std::size_t create_bulk();
  void appendToCmdStorage(const std::size_t &, const std::string &);
  std::vector<std::string> &get_commands(const std::size_t &);
  std::size_t get_timestamp(const std::size_t &);
  void set_timestamp(const std::size_t &, const std::size_t &);
  void deleteStorageCell(const std::size_t &);
};

template <typename T>
struct Observer
{
  virtual void printOut(T &source, const std::size_t &) = 0;
  protected:
  virtual void printOstream(std::ostream& out, T &source , const std::size_t & id){
    if (source.get_commands(id).size())
        {
            out << "bulk: ";
            std::copy(std::cbegin(source.get_commands(id)), std::cend(source.get_commands(id)),
                      infix_ostream_iterator<std::string>(out, ", "));
            out << std::endl;
        }
  }
};

class ToConsolePrint : public Observer<BulkReadCmd>
{
public:
  ToConsolePrint(std::ostream &out) : Observer<BulkReadCmd>(), _out{out} {}
  void printOut(BulkReadCmd &, const std::size_t &);
  static std::shared_ptr<ToConsolePrint> create(std::ostream &out)
  {
    return std::make_shared<ToConsolePrint>(out);
  }

private:
  std::ostream &_out;
};

class ToFilePrint : public Observer<BulkReadCmd>
{
public:
  void printOut(BulkReadCmd &source, const std::size_t &id);

  static std::shared_ptr<ToFilePrint> create()
  {
    return std::make_shared<ToFilePrint>();
  }
};

template <typename T>
struct Observable
{
  
  void subscribe(Observer<T> *observer)
  {
    std::scoped_lock<std::mutex> lock{mtx};
    if (!observer)
      return;
    auto it = std::find_if(observers.cbegin(), observers.cend(), [&](Observer<T> *e) { return e == observer; });
    if (it == observers.cend())
      observers.emplace_back(observer);
  }
  void unsubscribe(Observer<T> *observer)
  {
    std::scoped_lock<std::mutex> lock{mtx};
    observers.erase(
        remove(observers.begin(), observers.end(), observer),
        observers.end());
  }
protected:
void notify(T &source, const std::size_t &id)
  {
    std::scoped_lock<std::mutex> lock{mtx};
    for (auto obs : observers)
      obs->printOut(source, id);
  }
private:
  std::vector<Observer<T> *> observers;
  std::mutex mtx;
};

class BulkReadCmd : public Observable<BulkReadCmd>
{
public:
  BulkReadCmd() : Observable<BulkReadCmd>(),
                  _chunkSize{1},
                  _bulkStorage{new BulkStorage()} {}

  BulkReadCmd(const std::size_t &chunkSize) : Observable<BulkReadCmd>(),
                                              _chunkSize{chunkSize},
                                              _bulkStorage{new BulkStorage()} {}

  void process(std::istream &);

  void append(const std::string &);
  void push();

  static std::shared_ptr<BulkReadCmd> create(const std::size_t &size)
  {
    return std::make_shared<BulkReadCmd>(size);
  }
  std::vector<std::string> &get_commands(const std::size_t &id) { return _bulkStorage->get_commands(id); }
  std::size_t get_timestamp(const std::size_t &id) { return _bulkStorage->get_timestamp(id); }
  virtual ~BulkReadCmd()
  {
    if (_bulkStorage)
      delete _bulkStorage;
  }

private:
  BulkStorage *_bulkStorage{nullptr};
  std::size_t _chunkSize{0};
  std::size_t _numb_of_current_chunk{0};
  std::size_t _current_numb_of_cell{0};
  int open_braces_count{0};
};