#include "AudioTransmitter.hpp"

#include <spdlog/spdlog.h>

AudioTransmitter::AudioTransmitter(Controller& controller, const Libusb::Device& device)
    : controller(controller)
    , device(device)
{
    // TODO: 6. feladat
    this->transfer1 = libusb_alloc_transfer(this->PACKET_PER_TRANSFER);

    if (transfer1 != nullptr) {
        libusb_fill_iso_transfer(transfer1, device.get(), this->ENDPOINT, buffer1.data(), buffer1.size(), this->PACKET_SIZE, transferCompletedStatic, this, 0);
    } else {
        throw std::bad_alloc();
    }
    
    // TODO: 7. feladat
    this->transfer2 = libusb_alloc_transfer(this->PACKET_PER_TRANSFER);

    if (transfer2 != nullptr) {
        libusb_fill_iso_transfer(transfer2, device.get(), this->ENDPOINT, buffer2.data(), buffer2.size(), this->PACKET_SIZE, transferCompletedStatic, this, 0);
    } else {
        libusb_free_transfer(transfer1);
        throw std::bad_alloc();
    }

    spdlog::debug("Audio transmitter created.");
}

AudioTransmitter::~AudioTransmitter()
{
    stopTransmission();
    spdlog::debug("Audio transmitter destroyed.");
}

void AudioTransmitter::startTransmission(std::unique_ptr<FileReader> file)
{
    fileReader = std::move(file);

    // Load first buffer
    size_t size = fileReader->readChunk(buffer1.data(), BUFFER_SIZE);
    if (size != BUFFER_SIZE) {
        spdlog::error("Error reading first buffer!");
        throw std::runtime_error("File read error!");
    }

    // Load second buffer, if used
    if (transfer2 != nullptr) {
        size_t size = fileReader->readChunk(buffer2.data(), BUFFER_SIZE);
        if (size != BUFFER_SIZE) {
            spdlog::error("Error reading second buffer!");
            throw std::runtime_error("File read error!");
        }
    }
}

void AudioTransmitter::resumeTransmission()

{
    if (fileReader == nullptr) {
        spdlog::warn("No FileReader is set!");
        return;
    }

    if (fileReader->isEndOfFile()) {
        spdlog::warn("File is reached the end, reinitialize before resume!");
        return;
    }

    if (transmitterState) {
        spdlog::warn("Transmission already started!");
        return;
    }

    // TODO: 6. feladat
    int err = libusb_submit_transfer(transfer1);

    if (err != 0) {
        throw std::runtime_error(libusb_error_name(err));
	}    
    
    // TODO: 7. feladat
    if (transfer2 != nullptr) {
        int err = libusb_submit_transfer(transfer2);
        if (err != 0) {
            libusb_cancel_transfer(transfer1);
            throw std::runtime_error(libusb_error_name(err));
        }
    }

    transmitterState = true;
}

void AudioTransmitter::stopTransmission()
{
    if (!transmitterState) {
        return;
    }

    transmitterState = false;

    // TODO: 6. feladat
    int err=libusb_cancel_transfer(transfer1);
    if (err!=0) {
        throw std::runtime_error(libusb_error_name(err));
	}
	
    libusb_free_transfer(transfer1);
    

    // TODO: 7. feladat    
    if (transfer2 != nullptr) {
        err = libusb_cancel_transfer(transfer2);
        if (err != 0) {
            throw std::runtime_error(libusb_error_name(err));
        }
		
        libusb_free_transfer(transfer2);
    }
}

void AudioTransmitter::transferCompleted(libusb_transfer* xfr)
{
    // TODO: 6. feladat
    if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
        controller.transmissionEnded();
        return;
    }
    
    if (transmitterState && !fileReader->isEndOfFile()) {
        size_t size = fileReader->readChunk(xfr->buffer, BUFFER_SIZE);

        if (size < BUFFER_SIZE) {
            for (int i = size; i < BUFFER_SIZE; i++)
                xfr->buffer[i] = 0;
        }

        // TODO: 6. feladat
        int err=libusb_submit_transfer(xfr);
        if (err != 0) {
            controller.transmissionEnded();
            return;
        }
       
    }

    if (transmitterState && fileReader->isEndOfFile()) {
        transmitterState = false;
        controller.transmissionEnded();
    }
}

void LIBUSB_CALL AudioTransmitter::transferCompletedStatic(libusb_transfer* xfr)
{
    if (xfr->user_data != nullptr) {
        auto transmitter = static_cast<AudioTransmitter*>(xfr->user_data);
        transmitter->transferCompleted(xfr);
        return;
    }
    spdlog::error("No transmitter set to transfer's user data!");
    throw std::runtime_error("Callback calling error!");
}