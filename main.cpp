#include <iostream>
#include <cstddef>
#include <memory>
#include <exception>
#include "bulk.h"
#include "utils.h"

int main(int argc, char const *argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cout << "Параметр N передается как единственный "
                      << "параметр командной строки в виде целого числа" << std::endl;
            std::cout << "Пример: bulk 15" << std::endl;
            return 0;
        }

        if (!is_numeric<char const *, elem_traits<std::size_t>::value_type>(argv[1]))
        {
            return 0;
        }

        std::size_t chunk_size = std::atoi(argv[1]);

        auto ptrBulkRead = BulkReadCmd::create(chunk_size);
        auto ptrToConsolePrint = ToConsolePrint::create(std::cout);
        auto ptrToFilePrint = ToFilePrint::create();
        ptrBulkRead->process(std::cin);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
