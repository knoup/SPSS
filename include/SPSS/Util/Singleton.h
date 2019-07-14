#ifndef SPSS_SINGLETON_H_INCLUDED
#define SPSS_SINGLETON_H_INCLUDED

////////////////////////////////////////////////////////////
/// A simple and straightforward abstract Singleton wrapper
/// that ensures only one instance of any derived class
/// exists.
////////////////////////////////////////////////////////////

#include <type_traits>

namespace spss {

	template<typename T>
	class Singleton {
	  protected:
		Singleton() noexcept        = default;
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;

		////////////////////////////////////////////////////////////
		/// To silence "Base class Singleton<T> has a
		/// non-virtual destructor" [-Weffc++]"
		////////////////////////////////////////////////////////////
		virtual ~Singleton() = default;

	  public:
		////////////////////////////////////////////////////////////
		/// Guaranteed to be destroyed.
		///	Instantiated on first use.
		/// Thread safe in C++11
		////////////////////////////////////////////////////////////
		static T& get_instance() noexcept(std::is_nothrow_constructible<T>::value) {
			static T instance;
			return instance;
		}
	};

} //namespace spss

#endif // SPSS_SINGLETON_H_INCLUDED
