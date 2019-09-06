#include <toolbox/Backtrace.h>
#include <toolbox/Demangler.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>

// References:
// http://eli.thegreenplace.net/2015/programmatic-access-to-the-call-stack-in-c/
// https://pastebin.com/0JW1Kh9R

TOOLBOX_NAMESPACE_BEGIN

void generateStackTrace(fmt::memory_buffer& out) {
	thread_local char buf[4096];

	unw_context_t uc;
	if(unw_getcontext(&uc) < 0) {
		fmt::format_to(out, "unw_getcontext() failed");
		return;
	}

	unw_cursor_t cursor;
	if(auto rc = unw_init_local(&cursor, &uc); rc < 0) {
		fmt::format_to(out, "unw_init_local() failed ({}) - {}"
				, rc, unw_strerror(rc));
		return;
	}

	int frameId = 0;
	int rc_;
	while((rc_ = unw_step(&cursor)) > 0) {
		unw_word_t ip, sp, rel_ip;
		if(auto rc = unw_get_reg(&cursor, UNW_REG_IP, &ip); rc < 0) {
			fmt::format_to(out, "unw_get_reg(UNW_REG_IP) failed ({}) - {}"
					, rc, unw_strerror(rc));
			break;
		}
		if(!ip) break;
		if(auto rc = unw_get_reg(&cursor, UNW_REG_SP, &sp); rc < 0) {
			fmt::format_to(out, "unw_get_reg(UNW_REG_SP) failed ({}) - {}"
					, rc, unw_strerror(rc));
			break;
		}
		if(auto rc = unw_get_proc_name(&cursor, buf, sizeof(buf), &rel_ip); rc < 0) {
			fmt::format_to(out, "unw_get_proc_name() failed ({}) - {}"
					, rc, unw_strerror(rc));
			break;
		}
		fmt::format_to(out, "{:>3} : {} offset={} ip={} sp=0x{}\n"
				, frameId++, demangle(buf), rel_ip, ip, sp);
	}
	if(rc_ < 0) {
		fmt::format_to(out, "unw_step() failed ({}) - {}"
				, rc_, unw_strerror(rc_));
	}
}

TOOLBOX_NAMESPACE_END

