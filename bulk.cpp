#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <fstream>
#include "bulk.h"
#include "utils.h"

void BulkReadCmd::process(std::istream &in)
{
    std::string tmp_str;
    while (std::getline(in, tmp_str))
    {
        this->append(tmp_str);
    }
    if (!open_braces_count && _current_numb_of_cell)
        push();
}

void BulkReadCmd::push()
{
    std::size_t tmp_time = std::chrono::duration_cast<std::chrono::seconds>(
                               std::chrono::system_clock::now().time_since_epoch())
                               .count();
    _bulkStorage->set_timestamp(_numb_of_current_chunk, tmp_time);
    notify(*_bulkStorage, _numb_of_current_chunk);
    _current_numb_of_cell = 0;
    _bulkStorage->deleteStorageCell(_numb_of_current_chunk);
}

void BulkReadCmd::append(const std::string &tmp)
{
    if (tmp == "{")
    {
        ++open_braces_count;
        if (open_braces_count == 1)
        {
            if (_current_numb_of_cell > 0)
                push();

            _numb_of_current_chunk = _bulkStorage->create_bulk();
        }
    }
    else if (tmp == "}")
    {

        if (!open_braces_count)
        {
            if (_current_numb_of_cell > 0)
                push();
        }
        else if (!--open_braces_count)
        {
            push();
        }
    }
    else
    {
        if (!open_braces_count)
        {
            if (!_current_numb_of_cell)
                _numb_of_current_chunk = _bulkStorage->create_bulk();

            _bulkStorage->appendToCmdStorage(_numb_of_current_chunk, tmp);

            if (++_current_numb_of_cell == _chunkSize)
                push();
        }
        else
        {
            if (open_braces_count)
            {
                _bulkStorage->appendToCmdStorage(_numb_of_current_chunk, tmp);
            }
        }
    }
}

std::size_t BulkStorage::create_bulk()
{
    using cellType = BulkStorageCell;
    using ptr_cellType = std::shared_ptr<cellType>;
    auto bulkCell =
        std::make_shared<BulkStorageCell>();
    std::size_t chunk_number = TypeID<std::size_t>::value();
    _cmdStorage.insert(std::pair<std::size_t, ptr_cellType>(chunk_number, bulkCell));
    return chunk_number;
}

void BulkStorage::appendToCmdStorage(const std::size_t &i, const std::string &str)
{
    auto it = _cmdStorage.find(i);
    if (it != _cmdStorage.cend())
        it->second->add(str);
}

std::size_t BulkStorage::get_timestamp(const std::size_t &id)
{
    auto it = _cmdStorage.find(id);
    if (it != _cmdStorage.cend())
        return it->second->timestamp;
    else
        return 0;
}
void BulkStorage::set_timestamp(const std::size_t &id, const std::size_t &stamp)
{
    auto it = _cmdStorage.find(id);
    if (it != _cmdStorage.cend())
        it->second->timestamp = stamp;
}
void ToConsolePrint::update(BulkStorage &source, const std::size_t &id)
{
    printOstream(_out, source, id);
}

void ToFilePrint::update(BulkStorage &source, const std::size_t &id)
{
    std::ostringstream oss;
    oss << "bulk";
    oss << source.get_timestamp(id);
    oss << ".log";
    std::string fName = oss.str();

    std::ofstream ofs;
    ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try
    {
        ofs.open(fName, std::ofstream::out | std::ofstream::trunc);
        printOstream(ofs, source, id);
        ofs.close();
    }
    catch (std::ofstream::failure e)
    {
        std::cerr << "Exception opening/reading/closing file: " << fName << std::endl;
    }
}

std::vector<std::string> &BulkStorage::get_commands(const std::size_t &id)
{
    auto it = _cmdStorage.find(id);
    if (it != _cmdStorage.cend())
        return it->second->get_cells();
    else
        return false_cell;
}

void BulkStorage::deleteStorageCell(const std::size_t &id)
{
    auto it = _cmdStorage.find(id);

    if (it != _cmdStorage.cend())
        _cmdStorage.erase(it);
}

void Observer::subscribe_on_observable(const std::weak_ptr<Observable> &observable)
{
    auto item = observable.lock();
    if (item)
    {
        auto it = std::find_if(_observables.cbegin(), _observables.cend(), [&](std::weak_ptr<Observable> e) { return e.lock() == item; });
        if (it == _observables.cend())
        {
            _observables.emplace_back(item);
            item->subscribe(shared_from_this());
        }
        item.reset();
    }
}

void Observer::unsubscribe_on_observable(const std::weak_ptr<Observable> &observable_ptr)
{
    auto observable = observable_ptr.lock();
    if (observable)
    {
        _observables.erase(
            std::remove_if(_observables.begin(), _observables.end(),
                           [observable_ptr](const auto &p) { return !(observable_ptr.owner_before(p) || p.owner_before(observable_ptr)); }),
            _observables.end());

        observable.reset();
    }
}
 Observer::Observer(const std::weak_ptr<Observable> &observable_ptr){
     this->subscribe_on_observable(observable_ptr);
 }