/**
 * @file query.hh
 * @author Jiannan Tian
 * @brief query machine information
 * @version 0.1.3
 * @date 2020-10-05
 *
 * @copyright (C) 2020 by Washington State University, Argonne National Laboratory
 * See LICENSE in top-level directory
 *
 */

#ifndef QUERY_HH
#define QUERY_HH

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <cuda_runtime.h>

#include "query_dev.hh"

using namespace std;
using std::cerr;
using std::cout;
using std::endl;
using std::string;

std::string ExecShellCommand(const char* cmd);

void GetMachineProperties();

#endif
