/*
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <modm/architecture/interface/clock.hpp>
#include <modm/platform.hpp>
#include <modm/debug/logger.hpp>

#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_samv71_xplained_ultra
/// @{
using namespace modm::literals;
using namespace modm::platform;

struct SystemClock
{
	// 300MHz system clock generated by PLLA from internal Rc 12MHz clock
	static constexpr uint32_t PllAMult = 25;
	static constexpr uint32_t Frequency = 300_MHz;
	static constexpr uint32_t Mck = Frequency / 2;  // 150 MHz max.
	static constexpr uint32_t Usart1 = Mck;
//	static constexpr uint32_t Usb = 48_MHz;

	static bool inline
	enable()
	{
		ClockGen::setFlashLatency<Frequency / 2>();  // Flash runs off MCK

		ClockGen::enableMainInternal(MainInternalFreq::Rc12Mhz);
		ClockGen::selectMainClockSource(MainClockSource::Internal);
		ClockGen::enablePllA<PllAMult>();
		ClockGen::selectMasterClk<MasterClkSource::PLLA_CLK, MasterClkPrescaler::CLK_1, MasterClkDivider::Div2>();
		ClockGen::updateCoreFrequency<Frequency>();

		return true;
	}
};

using Led0 = GpioA23;
using Led1 = GpioC9;
using ButtonSW0 = GpioA9;

// No SoftwareGpioPort yet for SAM
struct Leds
{
	static constexpr std::size_t width{2};

	static void setOutput()
	{
		Led0::setOutput();
		Led1::setOutput();
	}

	static void write(uint32_t value)
	{
		Led0::set(value & 1);
		Led1::set(value & 2);
	}
};

struct Debug
{
	using Uart = Usart1;
	using UartTx = GpioB4;
	using UartRx = GpioA21;
};

using LoggerDevice = modm::IODeviceWrapper<Debug::Uart, modm::IOBuffer::BlockIfFull>;

inline void
initialize()
{
	// Turn off the watchdog
	WDT->WDT_MR = WDT_MR_WDDIS_Msk;

	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	// Disable JTAG TDI function on debug UART TX pin
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;

	Debug::Uart::initialize<SystemClock, 115200>();
	Debug::Uart::connect<Debug::UartTx::Tx, Debug::UartRx::Rx>();

	Leds::setOutput();
	ButtonSW0::setInput(InputType::PullUp);
}

/*
// TODO: usb
inline void initializeUsbFs()
{
	//SystemClock::enableUsb();
	//modm::platform::Usb::initialize<Board::SystemClock>();
}
*/
/// @}

} // namespace Board

