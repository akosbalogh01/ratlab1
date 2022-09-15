#pragma once

#include "Device.hpp"

#include <libusb-1.0/libusb.h>
#include <spdlog/spdlog.h>

namespace Libusb {
	class Interface : public Device {
		int interfaceNumber;

	public:
		Interface(const Context& context, std::uint16_t vendorId, std::uint16_t productId, int interfaceNumber)
			: Device(context, vendorId, productId)
			, interfaceNumber(interfaceNumber)
		{
			// TODO: 1. feladat
			int err = libusb_claim_interface(this->get(), interfaceNumber);

			if (err != 0) {
				throw std::runtime_error(libusb_error_name(err));
			}
			
			spdlog::debug("Libusb interface {} claimed.", interfaceNumber);
		}

		~Interface()
		{
			// TODO: 1. feladat
			int err = libusb_release_interface(this->get(), interfaceNumber);

			if (err != 0) {
				std::printf("%s", libusb_error_name(err));
			}
			
			spdlog::debug("Libusb interface {} released.", interfaceNumber);
		}
	};
}