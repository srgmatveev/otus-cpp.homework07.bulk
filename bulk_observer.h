#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <memory>
#include <cstddef>
#include "bulk.h"

class Observable;
class Observer : public std::enable_shared_from_this<Observer>
{
private:
  std::vector<std::weak_ptr<Observable>> _observables;

public:
  virtual void update(BulkStorage &source, std::size_t) = 0;
  void subscribe_on_observable(const std::weak_ptr<Observable> &);
  void unsubscribe_on_observable(const std::weak_ptr<Observable> &);

protected:
  virtual void printOstream(std::ostream &out, BulkStorage &source, std::size_t id)
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
  void update(BulkStorage &, std::size_t) override;
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
  void update(BulkStorage &, std::size_t) override;

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