/*
 * Example MPG implementation using LUFA for ATmega32U4 boards.
 */

#define DEBOUNCE_MILLIS 5

#include <LUFA.h>
#include "LUFADriver.h"

// Define time function for gamepad debouncer
#include <GamepadDebouncer.h>
uint32_t getMillis() { return millis(); }

#include <MPGS.h>
MPGS mpg(DEBOUNCE_MILLIS); // The gamepad instance

char USB_STRING_MANUFACTURER[] = "FeralAI";
char USB_STRING_PRODUCT[] = "MPG Sample Gamepad";
char USB_STRING_VERSION[] = "1.0";

void setup()
{
	mpg.setup(); // Runs your custom setup logic
	mpg.load();  // Get saved options if enabled
	mpg.read();  // Perform an initial button read so we can set input mode

	// Use the inlined `pressed` convenience methods
	InputMode inputMode = mpg.options.inputMode;
	if (mpg.pressedR3())
		inputMode = INPUT_MODE_HID;
	else if (mpg.pressedS1())
		inputMode = INPUT_MODE_SWITCH;
	else if (mpg.pressedS2())
		inputMode = INPUT_MODE_XINPUT;

	if (inputMode != mpg.options.inputMode)
	{
		mpg.options.inputMode = inputMode;
		mpg.save();
	}

	// Initialize USB device driver
	setupHardware(mpg.options.inputMode);
}

void loop()
{
	static const uint8_t reportSize = mpg.getReportSize();  // Get report size from Gamepad instance
	static GamepadHotkey hotkey;                            // The last hotkey pressed

	mpg.read();                                             // Read raw inputs
	mpg.debounce();                                         // Run debouncing if enabled
	hotkey = mpg.hotkey();                                  // Check hotkey presses (D-pad mode, SOCD mode, etc.), hotkey enum returned
	mpg.process();                                          // Perform final input processing (SOCD cleaning, LS/RS emulation, etc.)
	sendReport(mpg.getReport(), reportSize);                // Convert and send it!
}
