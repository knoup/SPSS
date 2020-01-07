#pragma once

////////////////////////////////////////////////////////////
///
/// spss::Function will simply serve as an alias for
/// a std::function with a void return type and any
/// possible number of arguments
///
////////////////////////////////////////////////////////////
#include <functional>

namespace spss {

	template<typename... Args>
	using Function = std::function<void(Args...)>;

} //namespace spss
