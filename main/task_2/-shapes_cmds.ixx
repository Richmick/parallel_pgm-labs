export module main.task_2:shapes_cmds;

import <print>;
import <iostream>;
import :common;

namespace mains::task2
{
	export template< class Manager >
	void get_frame(user_context< Manager >& ctx)
	{
		std::string name;
		ctx.in >> name;
		std::println(ctx.out, "{}", square::get_frame::operator()(ctx.shapes.at(name)));
	}
	export template< class Manager >
	void get_set_frame(user_context< Manager >& ctx)
	{
		std::string name;
		ctx.in >> name;
		const square::composition& comp = ctx.compositions.at(name);
		square::rect_t res = square::get_frame::operator()(comp.shapes.front());
		for (std::size_t i = 1; i < comp.shapes.size(); i++)
		{
			res = square::merge_frames(res, square::get_frame::operator()(comp.shapes[i]));
		}
		std::println(ctx.out, "{}", square::get_frame::operator()(ctx.shapes.at(name)));
	}
}
