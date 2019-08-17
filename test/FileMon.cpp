#include <toolbox/FileMon.h>

#include "CleanupFixture.h"

#include <doctest/doctest.h>

#include <fmt/format.h>
#include <fstream>
#include <vector>

#include <thread>
#include <chrono>

#define XYZ_CHECK_OUTCOME(x) \
	if(!x) { \
		FAIL("Unexpected outcome: " << x.error().message()); \
	}

namespace t = TOOLBOX_NAMESPACE;

auto getFileMon() {
	auto result = t::FileMon::create();
	XYZ_CHECK_OUTCOME(result);
	return std::move(result.value());
}


TEST_CASE("Example")
{
	struct Event
	{
		t::FileMon::Handle id;
		std::string dir;
		std::string file;
		uint16_t flag;
		auto operator==(const Event& o) const {
			return std::tie(id, dir, file, flag)
				== std::tie(o.id, o.dir, o.file, o.flag);
		}
	};
	std::vector<Event> actual;
	auto recorder = [&](
			const auto id
			, const auto& dir
			, const auto& file
			, const auto flag) {
		LOG_COUT_DBG("id={}, dir={}, file={}, flag={}", id, dir, file, flag);
		actual.push_back({id, dir, file, flag});
	};


	CleanupFixture cf;
	INFO("Path: " << cf.path());

	const auto& p = cf.path();
	const auto dir = p.string();
	auto d_existing = p / "d_existing";
	REQUIRE(fs::create_directory(d_existing));
	auto f_existing = p / "f_existing";
	auto f_in_d_existing = d_existing / "f_in_d_existing";

	auto create = [](const auto& f) {
		INFO("Creating " << f.string());
		std::ofstream of{f.c_str()};
		REQUIRE(of.is_open());
	};

	auto append = [](const auto& f) {
		INFO("Writing to " << f.string());
		std::ofstream of{f.c_str(), std::fstream::out | std::fstream::app};
		REQUIRE(of.is_open());
		of << "def";
	};

	auto rename = [](const auto& a, const auto& b) {
		INFO("Renaming " << a.string() << " to " << b.string());
		fs::rename(a, b);
	};

	auto remove = [](const auto& f) {
		INFO("Deleting " << f.string());
		REQUIRE(fs::remove(f));
	};

	create(f_existing);
	create(f_in_d_existing);

	auto filemon = getFileMon();
	const auto res = filemon.addWatch(p.string(), recorder);
	XYZ_CHECK_OUTCOME(res);
	const auto wid = res.value();

	auto f_new = p / "f_new";
	create(f_new);
	append(f_new);
	auto f_renamed = p / "f_renamed";
	rename(f_new, f_renamed);
	remove(f_renamed);

	// Moving from unmonitored dir to monitored
	rename(f_in_d_existing, p / "f_moved_into");

	for(size_t i = 0; i != 10; ++i) {
		REQUIRE(filemon.poll().has_value());
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	using Expected = Event;
	// We might get more, but these should appear:
	std::vector<Expected> expected = {
		{wid, dir, "f_new", 1},
		{wid, dir, "f_new", 4},
		{wid, dir, "f_new", 2},
		{wid, dir, "f_renamed", 1},
		{wid, dir, "f_renamed", 2},
		{wid, dir, "f_moved_into", 1},
	};
	REQUIRE(actual.size() >= expected.size());

	size_t e_idx = 0;
	for(const auto& a : actual) {
		if(a == expected[e_idx]) {
			if(++e_idx == expected.size()) {
				break;
			}
		}
	}
	REQUIRE(e_idx == expected.size());

	XYZ_CHECK_OUTCOME(filemon.removeWatch(wid));
	actual.clear();
	create(f_new);
	REQUIRE(actual.empty());
}

