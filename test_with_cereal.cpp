#include "cereal/types/map.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/cereal.hpp"
#include <fstream>
#include <map>
#include <thread>

struct MyRecord
{
    int x, y;
    float z;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(x, y, z);
    }
};

struct SomeData
{
    int id;
    std::map<int, MyRecord> data;

    SomeData() 
    {
        static int32_t idGen = 0;
        id = idGen++;
    }

    template <class Archive>
    void save(Archive& ar) const
    {
        ar(data);
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar(data);
    }
};

int write()
{
    std::cout << "=== Writing Data Start ===" << std::endl;
    std::ofstream os("out.cereal", std::ios::binary); // Create an output archive
    cereal::BinaryOutputArchive archive(os);

    SomeData myData;
    for (int i = 0; i < 10; i++)
    {
        MyRecord myRecord;
        myRecord.x = rand();
        myRecord.y = rand();
        myRecord.z = rand();
        std::cout << " Adding those elements: " <<
            myRecord.x << ", " <<
            myRecord.y << ", " <<
            myRecord.z << std::endl;
        myData.data[i] = myRecord;
    }
    archive(myData); // Write the data to the archive
    std::cout << "=== Writing Data End ===" << std::endl;
    return 0;
}

int read()
{
    std::cout << "=== Reading Data Start ===" << std::endl;
    std::ifstream is("out.cereal", std::ios::binary);
    cereal::BinaryInputArchive iarchive(is); // Create an input archive

    SomeData myData;
    iarchive(myData); // Read the data from the archive

    for (auto elem : myData.data)
    {
        std::cout << elem.first << " : " <<
            elem.second.x << ", " <<
            elem.second.y << ", " <<
            elem.second.z << std::endl;
    }
    std::cout << "=== Reading Data End ===" << std::endl;
    return 0;
}

int main()
{
    for (int i = 0; i < 100; i++)
    {
        write();
        read();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    return 0;
}
