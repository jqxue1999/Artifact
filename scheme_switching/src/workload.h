#pragma once

#define PROFILE
#include "openfhe.h"
#include "binfhecontext.h"
#include "utils.h"
#include <chrono>
#include <unistd.h>
#include <random>


using namespace lbcrypto;
using namespace std;

double Workload_3(uint32_t integerBits);
double Workload_2(uint32_t integerBits);
double Workload_1(uint32_t integerBits);