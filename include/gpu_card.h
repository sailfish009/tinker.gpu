#pragma once
#include "rc_man.h"
#include <string>
#include <vector>


#if TINKER_CUDART
namespace tinker {
std::string get_cuda_runtime_version_string();
std::string get_cuda_driver_version_string();
std::string get_thrust_version_string();


struct DeviceAttribute
{
   int device;
   std::string name;
   std::string pci_string;


   int cc_major, cc_minor;
   int cc;
   int single_double_ratio;


   std::string compute_mode_string;
   std::string ecc_string;


   size_t free_mem_bytes, total_mem_bytes;


   int max_threads_per_block;
   int max_shared_bytes_per_block;


   int multiprocessor_count;
   int max_threads_per_multiprocessor;
   int max_shared_bytes_per_multiprocessor;
   int max_blocks_per_multiprocessor;
   int cores_per_multiprocessor;


   int clock_rate_kHz; // not memory clock rate
};


std::vector<DeviceAttribute>& get_device_attributes();
}
#endif


namespace tinker {
/// \ingroup nvidia
/// \brief Number of threads in a warp.
constexpr unsigned WARP_SIZE = 32;


/// \ingroup nvidia
/// \brief Mask for all of the lanes in a warp.
constexpr unsigned ALL_LANES = 0xFFFFFFFF;


/// \ingroup nvidia
/// \brief Default dimension of thread blocks.
// constexpr unsigned BLOCK_DIM = 64;
constexpr unsigned BLOCK_DIM = 128;
// constexpr unsigned BLOCK_DIM = 256;


constexpr int PME_BLOCKDIM = 64;


TINKER_EXTERN int ndevice, idevice;


void gpu_card_data(rc_op op);
int get_grid_size(int nthreads_per_block);
}
