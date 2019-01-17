#pragma once

#include "defines.h"
#include "defineTypes.h"

namespace utils
{
	template<typename Out>
	inline void split(const std::string &s, char delim, Out result) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			*(result++) = item;
		}
	}

	inline std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}

	inline std::vector<std::string> split_by_cuts(const std::string &s, char delim) {
		std::vector<std::string> elems;
		std::vector<int> cuts;
		int i = 0;
		for (auto c : s)
		{
			if (c == delim)
			{
				cuts.push_back(i);
			}
			i++;
		}
		cuts.push_back((int)s.size());
		int start = 0;
		int end = 0;
		for (int i=0;i<cuts.size();i++)
		{
			end = cuts[i];
			if (start != end)
			{
				elems.push_back(s.substr(start, end - start));
			}
			else
			{
				elems.push_back("");
			}
			start = end + 1;
		}
		return elems;
	}

	inline std::vector<std::string> split_by_cnt(const std::string &s, char delim, int fixcnt) {
		auto& elems = split_by_cuts(s, delim);
		while (elems.size() < fixcnt)
			elems.push_back("");
		return elems;
	}

	uint32_t tsv_parse_wasstr(std::string);

	class tsv
	{
	public:
		tsv(String path);

		~tsv();

		std::vector<std::map<String,String>> Rows;

		std::map<std::string, std::map<String, String>> MapRows;

		std::vector<String> Keys;

		std::string jsonstr;
	};
};

void luaopen_tsv(lua_State* L);