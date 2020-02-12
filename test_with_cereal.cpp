#include "cereal/types/map.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/cereal.hpp"
#include <fstream>
#include <map>
#include <thread>
#include <vector>

#define MAX_RECORD 10000
#define ITERATION 10

struct MyRecord
{
    std::vector<unsigned char> mVector;
    size_t mSize;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(mVector, mSize);
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
    std::cout << "   Writing Data Start" << std::endl;
    std::ofstream os("out.cereal", std::ios::binary); // Create an output archive
    cereal::BinaryOutputArchive archive(os);

    SomeData myData;
    for (int i = 0; i < MAX_RECORD; i++)
    {
        MyRecord myRecord;
        myRecord.mSize = rand() % MAX_RECORD;
        for (int i = 0; i < myRecord.mSize; i++)
        {
            myRecord.mVector.push_back((unsigned char)('A' + rand() % 24));
        }
        /* std::cout << " Adding those elements: " <<
             myRecord.x << ", " <<
             myRecord.y << ", " <<
             myRecord.z << std::endl;*/
        myData.data[i] = myRecord;
    }
    archive(myData); // Write the data to the archive
    std::cout << "   Writing Data End" << std::endl;
    return myData;
}

SomeData read()
{
    std::cout << "   Reading Data Start" << std::endl;
    std::ifstream is("out.cereal", std::ios::binary);
    cereal::BinaryInputArchive iarchive(is); // Create an input archive

    SomeData myData;
    iarchive(myData); // Read the data from the archive

   /* for (auto elem : myData.data)
    {
        std::cout << elem.first << " : " <<
            elem.second.x << ", " <<
            elem.second.y << ", " <<
            elem.second.z << std::endl;
    }*/
    std::cout << "   Reading Data End" << std::endl;
    return myData;
}

int main()
{
    srand((unsigned)time(0));
    bool hasToContinue = true;
    for (int i = 0; i < ITERATION; i++)
    {
        std::cout << "=== Test number " << i + 1 << " ===" << std::endl;

        SomeData dataWritten = write();
        SomeData dataRed = read();

        if (dataWritten.data.size() != dataRed.data.size())
            std::cout << "Error: data mismatch" << std::endl;

        std::cout << "   Verifying data..." << std::endl;
        std::vector<unsigned char>::iterator it = dataWritten.data.at(i).mVector.begin();
        std::vector<unsigned char>::iterator jt = dataRed.data.at(i).mVector.begin();

        while (true)
        {
            //
            //do stuff with iterators
            //
            if(*it != *jt)
            { 
                std::cout << "Error: data mismatch" << std::endl;
                hasToContinue = false;
                break;
            }

            if (it != dataWritten.data.at(i).mVector.end())
            {
                ++it;
            }
            if (jt != dataRed.data.at(i).mVector.end())
            {
                ++jt;
            }
            if (it == dataWritten.data.at(i).mVector.end() && jt == dataRed.data.at(i).mVector.end())
            {
                break;
            }
        }

        /*for (int i = 0; i < dataWritten.data.size(); i++)
        {
            for( unsigned i : std::zip(dataWritten.data.at(i).mVector))
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

        }*/

        if (!hasToContinue)
        {
            std::cout << "Exit with error!" << std::endl;
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "=== All Tests succeeded ===" << std::endl;
    return 0;
}
