#include "cereal/types/map.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/cereal.hpp"
#include <fstream>
#include <map>
#include <thread>

#define MAX_RECORD 1000000
#define ITERATION 10

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

SomeData write()
{
    std::cout << "=== Writing Data Start ===" << std::endl;
    std::ofstream os("out.cereal", std::ios::binary); // Create an output archive
    cereal::BinaryOutputArchive archive(os);

    SomeData myData;
    for (int i = 0; i < MAX_RECORD; i++)
    {
        MyRecord myRecord;
        myRecord.x = rand();
        myRecord.y = rand();
        myRecord.z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10000));;
        std::cout << " Adding those elements: " <<
            myRecord.x << ", " <<
            myRecord.y << ", " <<
            myRecord.z << std::endl;
        myData.data[i] = myRecord;
    }
    archive(myData); // Write the data to the archive
    std::cout << "=== Writing Data End ===" << std::endl;
    return myData;
}

SomeData read()
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
    return myData;
}

int main()
{
    bool hasToContinue = true;
    for (int i = 0; i < ITERATION; i++)
    {
        SomeData dataWritten = write();
        SomeData dataRed = read();

        std::cout << "=== Test number " << i + 1 << " ===" <<  std::endl;

        if (dataWritten.data.size() != dataRed.data.size())
            std::cout << "Error: data mismatch" << std::endl;

        for (int i = 0; i < dataWritten.data.size(); i++)
        {
            if (dataWritten.data.at(i).x != dataRed.data.at(i).x)
            {
                std::cout << "Error: data mismatch" << std::endl;
                hasToContinue = false;
                break;
            }

            if (dataWritten.data.at(i).y != dataRed.data.at(i).y)
            {
                std::cout << "Error: data mismatch" << std::endl;
                hasToContinue = false;
                break;
            }

            if (dataWritten.data.at(i).z != dataRed.data.at(i).z)
            {
                std::cout << "Error: data mismatch" << std::endl;
                hasToContinue = false;
                break;
            }
                
        }

        if (!hasToContinue)
        {
            std::cout << "Exit with error!" << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
