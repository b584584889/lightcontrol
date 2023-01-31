#include "ParserSax.h"

void htmlcxx::HTML::ParserSax::parse(const std::string &html, int special_char)
{
//	std::cerr << "Parsing string" << std::endl;
	parse(html.c_str(), html.c_str() + html.length(), special_char);
}
