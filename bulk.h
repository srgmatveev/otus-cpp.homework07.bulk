#pragma once
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <memory>
#include <cstddef>
#include <map>
#include "utils.h"
#include "bulk_storage.h"

struct Observable;

class Observer : public std::enable_shared_from_this<Observer>
{
private:
  std::vector<std::weak_ptr<Observable>> _observables;

public:
  virtual void update(BulkStorage &source, const std::size_t &) = 0;
  void subscribe_on_observable(const std::weak_ptr<Observable> &);
  void unsubscribe_on_observable(const std::weak_ptr<Observable> &);

protected:
  virtual void printOstream(std::ostream &out, BulkStorage &source, const std::size_t &id)
  {
    if (source.get_commands(id).size())
    {
      out << "bulk: ";
      std::copy(std::cbegin(source.get_commands(id)), std::cend(source.get_commands(id)) - 1,
                std::ostream_iterator<std::string>(out, ", "));
      std::copy(std::cend(source.get_commands(id)) - 1, std::cend(source.get_commands(id)),
                std::ostream_iterator<std::string>(out, ""));
      out << std::endl;
    }
  }
};

class ToConsolePrint : public Observer
{
public:
  ToConsolePrint(std::ostream &out) : Observer(), _out{out} {}
  void update(BulkStorage &, const std::size_t &) override;
  static std::shared_ptr<ToConsolePrint> create(std::ostream &out, const std::weak_ptr<Observable> &_obs)
  {
    std::shared_ptr _tmpToConsolePrint = std::make_shared<ToConsolePrint>(out);
    auto tmpObservable = _obs.lock();
    if (tmpObservable)
    {
      _tmpToConsolePrint->subscribe_on_observable(tmpObservable);
      tmpObservable.reset();
    }
    return _tmpToConsolePrint;
  }
  static std::shared_ptr<ToConsolePrint> create(std::ostream &out)
  {
    return std::make_shared<ToConsolePrint>(out);
  }

private:
  std::ostream &_out;
};

class ToFilePrint : public Observer
{
public:
  ToFilePrint() : Observer() {}
  void update(BulkStorage &, const std::size_t &id) override;

  static std::shared_ptr<ToFilePrint> create()
  {
    return std::make_shared<ToFilePrint>();
  }

  static std::shared_ptr<ToFilePrint> create(const std::weak_ptr<Observable> &_observable)
  {
    std::shared_ptr _tmpToFilePrint = std::make_shared<ToFilePrint>();
    auto tmpObservable = _observable.lock();
    if (tmpObservable)
    {
      _tmpToFilePrint->subscribe_on_observable(tmpObservable);
      tmpObservable.reset();
    }
    return _tmpToFilePrint;
  }
};

struct Observable
{

  void subscribe(const std::weak_ptr<Observer> &observer_ptr)
  {
    auto observer = observer_ptr.lock();

    if (observer)
    {
      auto it = std::find_if(observers.cbegin(), observers.cend(), [&](std::weak_ptr<Observer> e) { return e.lock() == observer; });
      if (it == observers.cend())
        observers.emplace_back(observer);
      observer.reset();
    }
  }
  void unsubscribe(const std::weak_ptr<Observer> &observer_ptr)
  {
    auto observer = observer_ptr.lock();
    if (observer)
    {
      observers.erase(
          std::remove_if(observers.begin(), observers.end(),
                         [observer_ptr](const auto &p) { return !(observer_ptr.owner_before(p) || p.owner_before(observer_ptr)); }),
          observers.end());

      observer.reset();
    }
  }

protected:
  void notify(BulkStorage &source, const std::size_t &id)
  {
    for (const auto &obs : observers)
    {
      if (auto ptr = obs.lock())
      {
        ptr->update(source, id);
        ptr.reset();
      }
    }
  }

private:
  std::vector<std::weak_ptr<Observer>> observers;
};

class BulkReadCmd : public Observable
{
public:
  BulkReadCmd() : Observable(),
                  _chunkSize{1},
                  _bulkStorage{std::make_shared<BulkStorage>()} {}

  BulkReadCmd(const std::size_t &chunkSize) : Observable(),
                                              _chunkSize{chunkSize},
                                              _bulkStorage{std::make_shared<BulkStorage>()} {}

  void process(std::istream &);

  void append(const std::string &);
  void push();

  static std::shared_ptr<BulkReadCmd> create(const std::size_t &size)
  {
    return std::make_shared<BulkReadCmd>(size);
  }

private:
  std::shared_ptr<BulkStorage> _bulkStorage{nullptr};
  std::size_t _chunkSize{0};
  std::size_t _numb_of_current_chunk{0};
  std::size_t _current_numb_of_cell{0};
  std::size_t open_braces_count{0};
};