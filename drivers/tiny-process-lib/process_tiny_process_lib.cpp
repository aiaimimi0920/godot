/*************************************************************************/
/*  process_tiny_process_lib.cpp                                         */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#if defined(UNIX_ENABLED) || defined(WINDOWS_ENABLED)
#include "core/templates/local_vector.h"
#include "process_tiny_process_lib.h"

#ifdef WINDOWS_ENABLED
#include <stringapiset.h>
#endif

void ProcessTinyProcessLibrary::make_default() {
	_create = create_tpl;
}

void ProcessTinyProcessLibrary::_on_stdout(const char *bytes, size_t size) {
#ifdef WINDOWS_ENABLED
	LocalVector<wchar_t> wchars;
	int total_wchars = MultiByteToWideChar(CP_ACP, 0, bytes, size, nullptr, 0);
	if (total_wchars > 0) {
		wchars.resize(total_wchars);
		if (MultiByteToWideChar(CP_ACP, 0, bytes, size, wchars.ptr(), total_wchars) == 0) {
			wchars.clear();
		}
	}

	mutex.lock();
	if (wchars.is_empty()) {
		// Let's hope it's compatible with UTF-8.
		stdout_lines.append(String::utf8(bytes, size));
	} else {
		stdout_lines.append(String(wchars.ptr(), total_wchars));
	}
	mutex.unlock();
#else
	mutex.lock();
	String pipe_out;
	if (pipe_out.parse_utf8(bytes,size) == OK) {
		stdout_lines.append(pipe_out);
	} else {
		stdout_lines.append(String(bytes,size)); // If not valid UTF-8 try decode as Latin-1
	}
	mutex.unlock();
#endif
}

void ProcessTinyProcessLibrary::_on_stderr(const char *bytes, size_t size) {
#ifdef WINDOWS_ENABLED
	LocalVector<wchar_t> wchars;
	int total_wchars = MultiByteToWideChar(CP_ACP, 0, bytes, size, nullptr, 0);
	if (total_wchars > 0) {
		wchars.resize(total_wchars);
		if (MultiByteToWideChar(CP_ACP, 0, bytes, size, wchars.ptr(), total_wchars) == 0) {
			wchars.clear();
		}
	}

	mutex.lock();
	if (wchars.is_empty()) {
		// Let's hope it's compatible with UTF-8.
		stderr_lines.append(String::utf8(bytes, size));
	} else {
		stderr_lines.append(String(wchars.ptr(), total_wchars));
	}
	mutex.unlock();
#else
	mutex.lock();
	String pipe_out;
	if (pipe_out.parse_utf8(bytes, size) == OK) {
		stderr_lines.append(pipe_out);
	} else {
		stderr_lines.append(String(bytes, size)); // If not valid UTF-8 try decode as Latin-1
	}
	mutex.unlock();
#endif
}

Ref<Process> ProcessTinyProcessLibrary::create_tpl(const String &p_path, const Vector<String> &p_arguments, const String &p_working_dir, bool p_open_stdin) {
	return memnew(ProcessTinyProcessLibrary(p_path, p_arguments, p_working_dir, p_open_stdin));
}

int ProcessTinyProcessLibrary::get_exit_status() const {
	int exit_status = -1;
	process->try_get_exit_status(exit_status);
	return exit_status;
}

void ProcessTinyProcessLibrary::kill(bool p_force) {
	process->kill(p_force);
}

int ProcessTinyProcessLibrary::get_id() const {
	return process->get_id();
}

bool ProcessTinyProcessLibrary::write(const String &p_input) {
	ERR_FAIL_COND_V_MSG(!has_open_stdin, false, "You must set open_stdin to true when creating a process for write to work.");
	return process->write(p_input.utf8().get_data());
}

void ProcessTinyProcessLibrary::close_stdin() {
	ERR_FAIL_COND(!has_open_stdin);
	process->close_stdin();
	has_open_stdin = false;
}

ProcessTinyProcessLibrary::ProcessTinyProcessLibrary(const String &m_path, const Vector<String> &p_arguments, const String &p_working_dir, bool p_open_stdin) {
	std::vector<std::string> args;

	args.reserve(p_arguments.size() + 1);

	args.emplace_back(m_path.utf8().get_data());

	for (int i = 0; i < p_arguments.size(); i++) {
		args.emplace_back(std::string(p_arguments[i].utf8().get_data()));
	}

	has_open_stdin = p_open_stdin;

	process = memnew(TinyProcessLib::Process(
			args, p_working_dir.utf8().get_data(),
			std::bind(&ProcessTinyProcessLibrary::_on_stdout, this, std::placeholders::_1, std::placeholders::_2),
			std::bind(&ProcessTinyProcessLibrary::_on_stderr, this, std::placeholders::_1, std::placeholders::_2),
			p_open_stdin));
}

ProcessTinyProcessLibrary::~ProcessTinyProcessLibrary() {
	memdelete(process);
}
#endif // UNIX_ENABLED || WINDOWS_ENABLED