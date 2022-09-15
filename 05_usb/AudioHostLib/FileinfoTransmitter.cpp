#include "FileinfoTransmitter.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>

FileinfoTransmitter::FileinfoTransmitter(const Libusb::Device& device)
    : device(device)
{
}

void FileinfoTransmitter::sendFileinfo(const Fileinfo& info)
{
    union intToCharArray {
        unsigned int number;
        unsigned char data[sizeof(unsigned int)];
    };
    intToCharArray u;

    std::vector<unsigned char> titleData(info.title.length() + 5, 0);
    u.number = 0;
    std::copy(std::begin(u.data), std::end(u.data), std::begin(titleData));
    std::copy(std::cbegin(info.title), std::cend(info.title), std::begin(titleData) + 4);
    transmitBulkData(titleData);

    std::vector<unsigned char> artistData(info.artist.length() + 5, 0);
    u.number = 1;
    std::copy(std::begin(u.data), std::end(u.data), std::begin(artistData));
    std::copy(std::cbegin(info.artist), std::cend(info.artist), std::begin(artistData) + 4);
    transmitBulkData(artistData);

    std::vector<unsigned char> lengthData(8, 0);
    u.number = 2;
    std::copy(std::begin(u.data), std::end(u.data), std::begin(lengthData));
    u.number = info.sizeInBytes;
    std::copy(std::begin(u.data), std::end(u.data), std::begin(lengthData) + 4);
    transmitBulkData(lengthData);
}

void FileinfoTransmitter::transmitBulkData(const std::vector<unsigned char>& data)
{
    // TODO: 3. feladat
    int trlen = 0;

    int err = libusb_bulk_transfer(device.get(), this->ENDPOINT, const_cast<unsigned char*>(data.data()), data.size(), &trlen, 0);
    if (err != 0) {
        throw std::runtime_error(libusb_error_name(err));
	}
}