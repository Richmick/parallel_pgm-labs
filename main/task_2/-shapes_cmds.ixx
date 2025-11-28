export module main.task_2:shapes_cmds;

import <print>;
import <iostream>;
import <sstream>;
import <string_view>;
import <set>;
import square;
import :common;

namespace mains::task2
{
	export template< class Manager >
	square::composition collect(user_context< Manager >& ctx, std::string_view str)
	{
		square::composition result;
		return result;
	}
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
		if (ctx.shapes.empty())
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
		const auto& comp = ctx.compositions.at(name);
		square::rect_t res = square::get_frame::operator()(ctx.shapes.at(*comp.begin()));
		for (auto i = ++comp.begin(); i != comp.end(); ++i)
		{
			res = square::merge_frames(res, square::get_frame::operator()(ctx.shapes.at(*i)));
		}
		std::println(ctx.out, "{}", res);
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
		std::string name, line;
		ctx.in >> name;
		std::set< std::string > result;
		std::getline(ctx.in, line);
		std::istringstream in(line);
		while (in >> line)
		{
			if (!ctx.shapes.contains(line))
			{
				std::println(ctx.err, "shape \"{}\" doesn't exist", line);
				return;
			}
			result.insert(line);
		}
		if (result.empty())
		{
			std::println(ctx.err, "set is empty");
			return;
		}
		if (!ctx.compositions.try_emplace(std::move(name), std::move(result)).second)
		{
			std::println(ctx.err, "set with name \"{}\" already exists", name);
			return;
		}
	}
	export template< class Manager >
	void show_set(user_context< Manager >& ctx)
	{
		std::string name;
		ctx.in >> name;
		for (const std::string& i: ctx.compositions.at(name))
		{
			std::visit([&ctx, &i](auto f) { std::println(ctx.out, "{:{}s}", f, i); }, ctx.shapes.at(i));
		}
	}
	export template< class Manager >
	void show_sets(user_context< Manager >& ctx)
	{
		if (ctx.compositions.empty())
		{
			std::println(ctx.out, "no one set");
			return;
		}
		for (const auto&[name, set]: ctx.compositions)
		{
			std::println(ctx.out, "* {}", name);
		}
	}
}
