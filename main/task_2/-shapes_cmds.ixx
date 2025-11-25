export module main.task_2:shapes_cmds;

import <print>;
import <iostream>;
import :common;

export template< class Manager >
void mains::task2::user_commands_parser< Manager >::get_frame(std::istream& in)
{
	std::string name;
	in >> name;
	std::println(out, "{}", square::get_frame::operator()(shapes.at(name)));
}
export template< class Manager >
void mains::task2::user_commands_parser< Manager >::get_set_frame(std::istream& in)
{
	std::string name;
	in >> name;
	const square::composition& comp = compositions.at(name);
	square::rect_t res = square::get_frame::operator()(comp.shapes.front());
	for (std::size_t i = 1; i < comp.shapes.size(); i++)
	{
		res = square::merge_frames(res, square::get_frame::operator()(comp.shapes[i]));
	}
	std::println(out, "{}", square::get_frame::operator()(shapes.at(name)));
}
