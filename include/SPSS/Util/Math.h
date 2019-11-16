#ifndef UTILMATH_H_INCLUDED
#define UTILMATH_H_INCLUDED

#include <cmath>

namespace spss::Util::Math {

	////////////////////////////////////////////////////////////
	/// \brief A safer way to compare floats
	///
	/// \param _a         First parameter to compare
	/// \param _a         Second parameter to compare
	/// \param _magnitude The accuracy cutoff value
	///
	////////////////////////////////////////////////////////////
	inline bool almostEqual(float _a, float _b, float _magnitude = 0.01) {
		return (fabs(_a - _b) < _a * _magnitude);
	}

} // namespace spss::Util::Math

#endif // UTILMATH_H_INCLUDED
