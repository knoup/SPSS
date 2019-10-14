#ifndef RESOURCEMANAGER_H_INCLUDED
#define RESOURCEMANAGER_H_INCLUDED

////////////////////////////////////////////////////////////////////
/// spss::ResourceManager is a very simple class intended to manage
/// lifetime ownership of resources.
///
/// Only derived classes can add resources.
////////////////////////////////////////////////////////////////////

#include <map>
#include <memory>

namespace spss {

	template<class KeyType, class ResourceType>
	class ResourceManager {
	  public:

	  	////////////////////////////////////////////////////////////
		/// \brief Default constructor
		///
		////////////////////////////////////////////////////////////
		ResourceManager(){};

		////////////////////////////////////////////////////////////
		/// \brief Get a resource
		///
		/// Returns nullptr if a resource with the given Id doesn't exist.
		///
		/// \param _Id The resource's id
		///
		////////////////////////////////////////////////////////////
		const ResourceType* get(const KeyType& _Id) {
			if(m_resources.find(_Id) == m_resources.end()) {
				return nullptr;
			}

			return m_resources[_Id].get();
		};

	  protected:
		////////////////////////////////////////////////////////////
		/// \brief Adds a resource to the map
		///
		/// \param _Id       The resource's id
		/// \param _resource The resource
		///
		////////////////////////////////////////////////////////////
		void add(const KeyType& _Id, std::unique_ptr<ResourceType> _resource) {
			m_resources.insert({_Id, std::move(_resource)});
		};

	  private:
		std::map<const KeyType, std::unique_ptr<ResourceType>> m_resources; ///< Our resource map
	};

} //namespace spss

#endif // RESOURCEMANAGER_H_INCLUDED
