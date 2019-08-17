#pragma once

#include <cstdio>
#include <filesystem>
#include <stdexcept>

#include <fmt/format.h>
#include <toolbox/Log.h>

namespace fs = std::filesystem;

class CleanupFixture {
public:
	CleanupFixture(bool cleanup = true)
		: _path(fs::temp_directory_path() / std::tmpnam(nullptr))
		, _cleanup{cleanup}
	{
		if(!fs::create_directory(_path)) {
			throw std::runtime_error(
					fmt::format("Could not create tmp dir: {}", _path.string()));
		}
	}

	~CleanupFixture() {
		if(_cleanup && exists()) {
			std::error_code ec;
			fs::remove_all(_path, ec);
			if(ec) {
				LOG_CERR("Could not cleanup directory: {}. Err={}"
					, _path.string(), ec.message());
			}
		}
	}

	const fs::path& path() const noexcept
	{
		return _path;
	}

	bool exists() const
	{
		return fs::exists(_path);
	}

private:
	const fs::path _path;
	const bool _cleanup = true;
};

