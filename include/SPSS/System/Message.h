#pragma once

#include <string>

//A struct of two strings representing the
//title and content of a message

namespace spss {

	struct Message {
		Message(std::string _title = "", std::string _content = "")
					: title{_title}, content{_content} {
		}
		std::string title;
		std::string content;
	};
}
