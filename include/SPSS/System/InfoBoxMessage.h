#ifndef INFOBOXMESSAGE_H_INCLUDED
#define INFOBOXMESSAGE_H_INCLUDED

#include <SPSS/Graphics/MulticolorText.h>
#include <SPSS/Util/Message.h>

#include <SFML/Graphics.hpp>

namespace spss {

	class InfoBoxMessage : public sf::Drawable {
	  public:
		InfoBoxMessage(const Message&  _msg,
					   const sf::Font& _font,
					   unsigned int    _charSize);

		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		void fitWidth(float _width);

		void setMessage(const Message& _msg, float _width = 0);
		void setTitle(const std::string& _title, float _width = 0);
		void setContent(const std::string& _content, float _width = 0);

		void setTitleColor(sf::Color _color);
		void setContentColor(sf::Color _color);

		void setTitleAffixes(const std::string& _prefix,
							 const std::string& _suffix,
							 float              _width = 0,
							 sf::Color _prefixColor = sf::Color::White,
							 sf::Color _suffixColor = sf::Color::White);

		void setPosition(sf::Vector2f _pos);
		void setPosition(float _x, float _y);

		const spss::MulticolorText& getText() const;

		unsigned int getNumberOfLines() const;

	  private:
	  	unsigned int getNumberOfLines(size_t _startPos,
									  size_t _endPos) const;
	  	const std::string getRawString();

	  	void setTextString(const std::string& _str);

		//Data members --------------------------------
		spss::MulticolorText m_text;
		Message              m_message;
		sf::Color            m_titleColor;
		sf::Color            m_contentColor;

		std::string          m_prefix;
		std::string          m_suffix;
		sf::Color            m_prefixColor;
		sf::Color            m_suffixColor;
		//---------------------------------------------
	};
} // namespace spss

#endif // INFOBOXMESSAGE_H_INCLUDED
