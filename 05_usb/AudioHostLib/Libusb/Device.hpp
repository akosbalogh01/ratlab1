#pragma once

#include "Context.hpp"

#include <libusb-1.0/libusb.h>
#include <spdlog/spdlog.h>

#include <cstdint>

namespace Libusb {
	class Device {
		libusb_device_handle* device = nullptr;

	public:
		Device(const Context& context, std::uint16_t vendorId, std::uint16_t productId)
		{
			// TODO: 1. feladat
			device = libusb_open_device_with_vid_pid(context.get(), vendorId, productId);

			if (device == nullptr) {
				throw std::runtime_error("Open error");
			}

			spdlog::debug("Libusb device opened VID:{:#06x} and PID:{:#06x}.", vendorId, productId);
		}

		~Device()
		{
			// TODO: 1. feladat
			libusb_close(device);
			spdlog::debug("Libusb device closed.");
		}

		libusb_device_handle* get() const { return device; }
	};
}
