#include "StateTransmitter.hpp"

#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>

StateTransmitter::StateTransmitter(Controller& controller, const Libusb::Device& device)
    : controller(controller)
    , device(device)
{
    // TODO: 5. feladat
    rxTransfer = libusb_alloc_transfer(0);

    if (rxTransfer != nullptr) {
        libusb_fill_interrupt_transfer(rxTransfer, device.get(), this->IN_ENDPOINT, rxBuffer.data(), rxBuffer.size(), transferCompletedStatic, this, 0);

        int err = libusb_submit_transfer(rxTransfer);
        if (err != 0) {
            throw std::runtime_error(libusb_error_name(err));
		}
    }
    else {
        throw std::bad_alloc();
	}
}

StateTransmitter::~StateTransmitter()
{
    // TODO: 5. feladat
    libusb_cancel_transfer(rxTransfer);
}

void StateTransmitter::sendCommand(AudioState state)
{
    // TODO: 2. feladat
    int txlen = 0;
    unsigned char buf = (unsigned char)state;

    int err = libusb_interrupt_transfer(device.get(), this->OUT_ENDPOINT, &buf, sizeof(buf), &txlen, 0);
    if (err != 0) {
        throw std::runtime_error(libusb_error_name(err));
	}

    spdlog::debug("State {} transmitted synchronously.", state);
}

void StateTransmitter::sendCommandAsync(AudioState state)
{
    libusb_transfer* trans = libusb_alloc_transfer(0);

    txBuffer[0] = (unsigned char)state;

    libusb_fill_interrupt_transfer(trans, device.get(), this->OUT_ENDPOINT, txBuffer.data(), txBuffer.size(), transferCompletedStatic, this, 0);

    trans->flags = LIBUSB_TRANSFER_FREE_TRANSFER;

    int err = libusb_submit_transfer(trans);
    if (err != 0) {
        throw std::runtime_error(libusb_error_name(err));
	}
}

void LIBUSB_CALL StateTransmitter::transferCompletedStatic(libusb_transfer* xfr)
{
    if (xfr->user_data != nullptr) {
        auto controller = static_cast<StateTransmitter*>(xfr->user_data);
        controller->transferCompleted(xfr);
        return;
    }
    spdlog::error("No controller set to transfer's user data!");
    throw std::runtime_error("Callback calling error!");
}

void StateTransmitter::transferCompleted(libusb_transfer* xfr)
{
    // TODO: 4. feladat
    if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
        throw std::runtime_error("Error!");
    }
	
    unsigned char newstate = xfr->buffer[0];
    this->controller.updateState((AudioState)newstate);
    if ((AudioState)newstate == AudioState::Stop) {
        return;
    }

    // TODO: 5. feladat
    int err=libusb_submit_transfer(xfr);
    if (err != 0) {
        throw std::runtime_error(libusb_error_name(err));
	}
}