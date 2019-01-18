#pragma once
#include <stdint.h>



#include <cstdlib>
#include <string>
#include <regex>
#include <sstream>
#include <iostream>
#include <fstream>

#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include <functional>
#include <chrono>

#include <vector>
#include <list>
#include <map>
#include <deque>
#include <set>

#include <nlohmann/json.hpp>

using String = std::string;
using Json = nlohmann::json;
inline void echo(String msg)
{
	std::cout << msg << std::endl;
}


