#pragma once

#include <inttypes.h>
#include "hfClock.h"
#include "radioDevice.h"
#include "system.h"  // Nvic
#include "types.h"


typedef enum {
	Receiving,
	Transmitting,
	Idle,
	PowerOff
} RadioState;


/*
 * High level driver for radio peripheral
 *
 * Understands collaboration of low-level devices:
 * - HfClock and RadioDevice
 * - DCDCPower and RadioDevice
 *
 * Not understand registers, i.e no dependence on nrf.h
 *
 * Understands wireless protocol:
 * - configures device for protocol
 * - abstracts protocol state from device state
 * - abstracts away the task/event architecture of device
 * - implements interrupt RX vs. spinning TX
 * - understands half-duplex (RX exclusive of TX)
 *
 * Protocol is defined by:
 * - constants for lengths, channels, bitrate
 * - behaviour
 * -- no acks xmitted as in ESB
 * -- all units use one address
 *
 * Singleton, all static class methods.
 */
class Radio {
public:
	// Define protocol lengths

	// FUTURE Variable payload up to 258 bytes?

	/*
	 * Fixed: all payloads same size.
	 * Device configured to not transmit S0, LENGTH, S1
	 * Buffer not include S0, LENGTH, S1 fields.
	 */
	static const uint8_t FixedPayloadCount = 10;

	static const uint8_t LongNetworkAddressLength = 4;	// 1 byte preamble, 3 bytes base
	static const uint8_t MediumNetworkAddressLength = 3;	// 1 byte preamble, 2 bytes base
	static const uint8_t ShortNetworkAddressLength = 2;	// 1 byte preamble, 1 bytes base

	static volatile uint8_t radioBuffer[FixedPayloadCount];
	static RadioState state;


private:
	static HfClock hfClock;
	static RadioDevice device;
	static bool wasTransmitting;  // false: wasReceiving
	static Nvic nvic;
	static PowerSupply powerSupply;

	static void (*aRcvMsgCallback)();


public:
	static void receivedEventHandler();

	static void init(void (*onRcvMsgCallback)());
	static void configureStatic();

	static void powerOnAndConfigure();
	static void powerOff();
	static bool isPowerOn();

	static bool isDisabledState();
	static bool isEnabledInterruptForPacketDoneEvent();

	// FUTURE DYNAMIC static void getBufferAddressAndLength(uint8_t** handle, uint8_t* lengthPtr);
	// Can't define in-line, is exported
	static BufferPointer getBufferAddress();
	static void setupFixedDMA();

	// Static: buffer owned by radio, of fixed length
	static void transmitStatic();
	static void transmitStaticSynchronously();
	static void receiveStatic();

#ifdef DYNAMIC
	static void transmit(BufferPointer data, uint8_t length);
	static void transmitSynchronously(BufferPointer data, uint8_t length);
	static void receive(BufferPointer data, uint8_t length);
#endif

	static void stopXmit();
	static void stopReceive();

	static void spinUntilXmitComplete();

private:
	static void powerOn();	// public uses powerOnAndConfigure, a radio on is useless without configuration
	static void spinUntilReady();
	static void dispatchPacketCallback();

	static void disable();
	static void spinUntilDisabled();
#ifdef DYNAMIC
	static void setupXmitOrRcv(BufferPointer data, uint8_t length);
#endif
	static void setupInterruptForMsgReceivedEvent();

	static void startXmit();
	static void startRcv();

	static void enableRXTask();
	static void enableTXTask();

	static bool isEventForMsgReceivedInterrupt();
	static void clearEventForMsgReceivedInterrupt();
	static void enableInterruptForMsgReceived();
	static void disableInterruptForMsgReceived();
	static void disableInterruptForEndTransmit();
public:
	static bool isPacketCRCValid();

	static bool isEnabledInterruptForMsgReceived();
	static bool isEnabledInterruptForEndTransmit();
};
