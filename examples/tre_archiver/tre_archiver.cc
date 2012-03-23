
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include "swganh/tre/tre_archive.h"
#include "swganh/tre/tre_reader.h"

using namespace std;
using namespace swganh::tre;

const string resource_name = "appearance/mesh/ply_corl_house_lg_s01_fp1_r0_mesh_r0_l2.msh";

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <path to swg directory>" << endl;
        exit(0);
    }
    
    auto start_time = chrono::high_resolution_clock::now();

    string swg_directory(argv[1]);

    {
        vector<shared_ptr<TreReader>> readers;

        readers.push_back(make_shared<TreReader>(swg_directory + "/default_patch.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_sku1_14_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_14_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_sku1_13_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_13_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_sku1_12_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_12_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_11_03.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sku1_07.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_11_02.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sku1_06.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_11_01.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_11_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sku1_05.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sku1_04.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sku1_03.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sku1_02.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sku1_01.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sku1_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_10.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_09.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_08.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_07.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_06.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_05.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_04.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_03.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_02.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_01.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/patch_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_other_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_static_mesh_01.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_static_mesh_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_texture_07.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_texture_06.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_texture_05.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_texture_04.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_texture_03.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_texture_02.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_texture_01.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_texture_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_skeletal_mesh_01.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_skeletal_mesh_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_animation_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sample_04.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sample_03.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sample_02.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sample_01.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_sample_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/data_music_00.tre"));
        readers.push_back(make_shared<TreReader>(swg_directory + "/bottom.tre"));

        TreArchive<TreReader> archive(readers);

        auto data = archive.GetResource(resource_name);

        cout << "\nLoaded resource from archive:\n\n"
             << "   Name: " << resource_name << "\n"
             << "   Filesize: " << data.size() << "\n"
             << "   MD5: " << archive.GetMd5Hash(resource_name) << "\n\n"
             << endl;
    }

    auto stop_time = chrono::high_resolution_clock::now();

    cout << "Duration: " << 
        chrono::duration_cast<chrono::milliseconds>
            (stop_time - start_time).count() << "ms" << endl;

    return 0;
}
