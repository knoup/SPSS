#ifndef SPSS_FUNCTION_H_INCLUDED
#define SPSS_FUNCTION_H_INCLUDED

////////////////////////////////////////////////////////////
///
/// spss::Function will simply serve as an alias for
/// a std::function with a void return type and any
/// possible number of arguments
///
////////////////////////////////////////////////////////////
#include <functional>

namespace spss {

    template <typename ...Args>
    using Function = std::function<void(Args...)>;

} //namespace spss


#endif // SPSS_FUNCTION_H_INCLUDED
