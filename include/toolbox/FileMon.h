#pragma once

#include <toolbox/Config.h>
#include <toolbox/Outcome.h>

#include <cstdint>
#include <experimental/propagate_const>
#include <functional>
#include <memory>
#include <string>


TOOLBOX_NAMESPACE_BEGIN

class FileMon
{
public:
	using Handle = int;
	enum Flag : uint16_t
	{
		Add = 1,
		Delete = 2,
		Modify = 4,
		Dir = 8
	};
	using Handler = std::function<void(
			Handle
			, const std::string& /*dir*/
			, const std::string& /*file*/
			, Flag)>;

	static outcome::result<FileMon> create();

	FileMon(const FileMon&) = delete;
	FileMon& operator=(const FileMon&) = delete;
	FileMon(FileMon&&) = default;
	FileMon& operator=(FileMon&&) = default;
	~FileMon();

	outcome::result<Handle> addWatch(const std::string& dir, Handler&& h);
	outcome::result<void> removeWatch(Handle id);

	outcome::result<void> poll();

private:
	struct impl;
	struct del
	{
		void operator()(impl *p);
	};
	std::experimental::propagate_const<
		std::unique_ptr<impl, del>> _p;
	FileMon(std::unique_ptr<impl, del>&& p);
};

TOOLBOX_NAMESPACE_END

