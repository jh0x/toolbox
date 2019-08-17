#include <toolbox/FileMon.h>

#include <toolbox/Assert.h>
#include <toolbox/Log.h>

#include <cerrno>
#include <map>
#include <system_error>

#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/inotify.h>

TOOLBOX_NAMESPACE_BEGIN

struct FileMon::impl
{
private:
	struct Details
	{
		Details(const std::string& d, Handler&& h)
			: dir{d}, h{std::move(h)} {};
		std::string dir;
		Handler h;
	};

public:
	impl(int fd) noexcept;

	~impl();

	outcome::result<Handle> addWatch(const std::string& dir, Handler&& h);
	outcome::result<void> removeWatch(int wd);
	outcome::result<void> poll();

private:
	outcome::result<void> _handle_events();
	void _handle(int wd, const char* name, uint32_t mask);
	outcome::result<void> _unwatch(int fd, const std::string& dir);

private:
	int _fd{-1};
	std::map<Handle, Details> _watches;

	::pollfd _pfd = {0};
	constexpr static auto kBufSize = 8192;
	alignas(alignof(::inotify_event)) std::byte _buf[kBufSize] = {};
};

FileMon::impl::impl(int fd) noexcept
	: _fd{fd}
{
	_pfd.fd = _fd;
	_pfd.events = POLLIN;
}

FileMon::impl::~impl()
{
	for(auto& w: _watches) {
		const auto fd = w.first;
		const auto& dir = w.second.dir;
		[[maybe_unused]] auto&& discard = _unwatch(fd, dir);
	}
	if(_fd) {
		if(::close(_fd) < 0) {
			LOG_CERR("FileMon::~FileMon: close error: {}", std::strerror(errno));
		}
	}
}

outcome::result<FileMon::Handle> FileMon::impl::addWatch(
		const std::string& dir
		, FileMon::Handler&& h)
{
	constexpr static auto kFlags =
		IN_CLOSE_WRITE
		| IN_MOVED_TO
		| IN_MOVED_FROM
		| IN_CREATE
		| IN_MODIFY
		| IN_DELETE;
	if(auto wd = ::inotify_add_watch(_fd, dir.c_str(), kFlags); wd >= 0) {
		const auto res = _watches.emplace(
				std::piecewise_construct
				, std::forward_as_tuple(wd)
				, std::forward_as_tuple(dir, std::move(h))
				);
		XYZ_ASSERT(res.second, "Duplicate watch descriptor?");
		return wd;
	}
	return {errno, std::system_category()};
}

outcome::result<void> FileMon::impl::removeWatch(int wd)
{
	auto it = _watches.find(wd);
	if(it == _watches.end()) return outcome::success();
	const auto fd = it->first;
	const auto& dir = it->second.dir;
	auto&& res = _unwatch(fd, dir);
	_watches.erase(it);
	return res;
}

outcome::result<void> FileMon::impl::poll()
{
	const auto res = ::poll(&_pfd, 1, 0);
	if(res == -1 && errno != EINTR) {
		return {errno, std::system_category()};
	}
	if(res <= 0) {
		return outcome::success();
	}
	if(_pfd.revents & POLLIN) {
		return _handle_events();
	}
	return outcome::success();
}

outcome::result<void> FileMon::impl::_handle_events()
{
	ssize_t len{0};
	// Loop while events can be read from inotify file descriptor.
	while(true) {
		len = ::read(_fd, _buf, kBufSize );
		if(len == -1 && errno != EAGAIN) {
			return {errno, std::system_category()};
		}
		// If the nonblocking read() found no events to read, 
		// it returns -1 with errno set to EAGAIN. In that case,
		// we exit the loop.
		if(len <= 0) {
			return outcome::success();
		}
		// Loop over all events in the buffer
		for(auto pos = 0; pos < len;) {
			const auto* evt = reinterpret_cast<::inotify_event*>(_buf + pos);
			_handle(evt->wd, evt->name, evt->mask);
			pos += sizeof(::inotify_event) + evt->len;
		}
	}
}

void FileMon::impl::_handle(int wd, const char* name, uint32_t mask)
{
	auto it = _watches.find(wd);
	if(it == _watches.end()) return;

	const auto& det = it->second;

	// Translate someway into our Flags
	std::underlying_type_t<Flag> f{0};
	if(mask & IN_ISDIR) f |= Flag::Dir;
	if((mask & IN_CREATE) || (mask & IN_MOVED_TO)) f |= Flag::Add;
	if((mask & IN_CLOSE_WRITE) || (mask & IN_MODIFY)) f |= Flag::Modify;
	if((mask & IN_DELETE) || (mask & IN_MOVED_FROM)) f |= Flag::Delete;

	LOG_COUT_VRB("wd={}, dir={}, name={}, mask=0x{:08x}, flag={}"
			, wd, det.dir, name, mask, f);
	if(!f) return;
	det.h(wd, det.dir, name, static_cast<Flag>(f));
}

outcome::result<void> FileMon::impl::_unwatch(int fd, const std::string& dir)
{
	if(::inotify_rm_watch(_fd, fd) < 0) {
		return {errno, std::system_category()};
	}
	return outcome::success();
}



void FileMon::del::operator()(impl *p)
{
  delete p;
}



outcome::result<FileMon> FileMon::create()
{
	if(auto fd = ::inotify_init1(IN_NONBLOCK); fd >= 0) {
		return FileMon(std::unique_ptr<impl, del>(new impl{fd}));
	}
	return {errno, std::system_category()};

}

FileMon::FileMon(std::unique_ptr<impl, del>&& p)
	: _p{std::move(p)}
{}

FileMon::~FileMon() = default;

outcome::result<FileMon::Handle> FileMon::addWatch(
		const std::string& s
		, Handler&& h)
{
	return _p->addWatch(s, std::move(h));
}

outcome::result<void> FileMon::removeWatch(Handle id)
{
	return _p->removeWatch(id);
}

outcome::result<void> FileMon::poll()
{
	return _p->poll();
}

TOOLBOX_NAMESPACE_END

