module;
#include <plf_hive.h>

export module nx.core.container;

import nx.core.memory;

export namespace nx { 

	template<typename T>
	using PmrHive = plf::hive<T, std::pmr::polymorphic_allocator<T>>;

}

