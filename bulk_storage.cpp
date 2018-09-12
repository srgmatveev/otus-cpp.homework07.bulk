#include <iostream>
#include <string>
#include "bulk_storage.h"

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
        return it->second->get_timestamp();
    else
        return 0;
}
void BulkStorage::set_timestamp(const std::size_t &id, const std::size_t &stamp)
{
    auto it = _cmdStorage.find(id);
    if (it != _cmdStorage.cend())
        it->second->set_timestamp(stamp);
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


