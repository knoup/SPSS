#pragma once

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
		auto f{fabs(_a - _b)};
		auto z{_a * _magnitude};
		if (_a == 0 || _b == 0) {
			z = _magnitude;
		}
		return (f < z);
	}

} // namespace spss::Util::Math
