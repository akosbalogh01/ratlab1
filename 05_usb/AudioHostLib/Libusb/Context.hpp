#pragma once

#include <libusb-1.0/libusb.h>
#include <spdlog/spdlog.h>

#include <chrono>

namespace Libusb {
	class Context {
		libusb_context* context = nullptr;

	public:
		Context()
		{
			// TODO: 1. feladat
			int err=libusb_init(&context);
			if (err != 0)
				throw std::runtime_error(libusb_error_name(err));
			spdlog::debug("Libusb context created.");
		}

		~Context()
		{
			// TODO: 1. feladat
			libusb_exit(context);
			spdlog::debug("Libusb context destroyed.");
		}

		libusb_context* get() const { return context; }

		void handleEvents(const std::chrono::milliseconds& timeout)

		{
			auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timeout).count();
			auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(timeout).count();

			// TODO: 4. feladat
			timeval t;
			
			t.tv_sec = seconds;
			t.tv_usec = microseconds;

			int err=libusb_handle_events_timeout(context, &t);
			if (err != 0) {
				throw std::runtime_error(libusb_error_name(err));
			}
		}
	};
}