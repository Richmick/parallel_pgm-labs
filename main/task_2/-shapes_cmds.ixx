export module main.task_2:shapes_cmds;

import <print>;
import <iostream>;
import square;
import :common;

namespace mains::task2
{
	export template< class Manager >
	void show_figure(user_context< Manager >& ctx)
	{
		std::string name;
		ctx.in >> name;
		const square::composition::shape& shape = ctx.shapes.at(name);
		std::visit([&ctx](auto f) { std::println(ctx.out, "{}", f); }, shape);
	}
	export template< class Manager >
	void show_shapes(user_context< Manager >& ctx)
	{
		if (ctx.shapes.size() == 0)
		{
			std::println(ctx.out, "no one shape");
			return;
		}
		for (const auto& i: ctx.shapes)
		{
			std::visit([&ctx,&name=i.first](auto f) { std::println(ctx.out, "{:{}s}", f, name); }, i.second);
		}
	}
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
	export template< class Manager >
	void create_circle(user_context< Manager >& ctx)
	{
		std::string name;
		float radius;
		square::point_t center;
		ctx.in >> name >> radius >> center.x >> center.y;
		if (!ctx.shapes.try_emplace(name, square::circle_t{radius, center}).second)
		{
			std::println(ctx.err, "circle with name \"{}\" already exists", name);
		}
	}
	export template< class Manager >
	void create_set(user_context< Manager >& ctx)
	{
		std::string name;
		ctx.in >> name;
		std::pair ins = ctx.shapes.try_emplace(name);
		if (!ins.second)
		{
			std::println(ctx.err, "set with name \"{}\" already exists", name);
		}
	}
}
