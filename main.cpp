#include <iostream>
#include <cstddef>
#include "bulk.h"
#include "utils.h"
#include <memory>

int main(int argc, char const *argv[])
{
    std::size_t chunk_size{0};
    if (argc != 2)
    {
        std::cerr << "Параметр N передается как единственный "
                  << "параметр командной строки в виде целого числа" << std::endl;
        std::cerr << "Пример: bulk 15" << std::endl;
    }

    if (!is_numeric<char const *, elem_traits<std::size_t>::value_type>(argv[1]))
    {
        return 0;
    }

    chunk_size = std::atoi(argv[1]);

    auto ptrBulkRead = BulkReadCmd::create(chunk_size);
    auto ptrToConsolePrint = ToConsolePrint::create(std::cout);
    auto ptrToFilePrint = ToFilePrint::create();

    ptrBulkRead->subscribe(ptrToConsolePrint.get());
    ptrBulkRead->subscribe(ptrToFilePrint.get());
    ptrBulkRead->process(std::cin);
    ptrBulkRead->unsubscribe(ptrToFilePrint.get());
    ptrBulkRead->unsubscribe(ptrToConsolePrint.get());

    return 0;
}
