#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <string>

////////////////////////////////////////////////////////////////////
/// spss::Message is a struct containing two strings, representing
/// the title and contents of a message
////////////////////////////////////////////////////////////////////

namespace spss {

	struct Message {
		Message(std::string _title = "", std::string _content = "")
		            : title{_title}, content{_content} {
		}
		std::string title;
		std::string content;
	};
} // namespace spss

#endif // MESSAGE_H_INCLUDED
