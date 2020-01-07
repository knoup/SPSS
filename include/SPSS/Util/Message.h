#pragma once

#include <string>

////////////////////////////////////////////////////////////////////
/// spss::Message is a struct containing two strings, representing
/// the title and contents of a message
////////////////////////////////////////////////////////////////////

namespace spss {

	struct Message {
		Message(const std::string& _title = "", const std::string& _content = "")
		            : title{_title}, content{_content} {
		}
		std::string title;
		std::string content;
	};
} // namespace spss
