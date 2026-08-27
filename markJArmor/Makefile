# Firmware test suite — see test/*.cpp for the contracts under test.
#
#   make test    host unit tests (mode transitions, machine end-to-end)
#   make verify  test + real AVR compile of the sketch (the raw-cpp/Arduino
#                boundary gate: proves src/ builds inside arduino-cli)
#
# The suite uses the REAL firmware sources untouched. Mocks are confined to
# test/mocks/ and are the only two seams in the whole suite:
#   Arduino.h          F(), millis() (injected clock), Serial
#   Adafruit_NeoPixel.h recording ring write log
# Pure-logic modules (state/Mode, subsystems/InputToggles, SonicCannon)
# include no Arduino headers and are tested with zero mocks.

CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror -Wno-unused-parameter
BUILD     = build
TESTDIR   = test
MOCKS     = -I$(TESTDIR)/mocks
FQBN      = arduino:avr:mega

REAL_SRCS = src/state/Machine.cpp src/state/Mode.cpp \
            src/subsystems/ChestBuster.cpp src/subsystems/InputToggles.cpp \
            src/subsystems/SonicCannon.cpp \
            src/subsystems/ChestBuster/Animations/AnimationOff.cpp \
            src/subsystems/ChestBuster/Animations/LoaderBlueToGreen.cpp

.PHONY: test verify clean

test: $(BUILD)/mode_test $(BUILD)/animation_test $(BUILD)/machine_test
	$(BUILD)/mode_test
	$(BUILD)/animation_test
	$(BUILD)/machine_test
	@echo "SUITE PASS"

verify: test
	arduino-cli compile --fqbn $(FQBN) --warnings all markJArmor.ino

# mode_test links the real Mode + InputToggles sources; InputToggles.cpp uses
# Serial (Arduino.h) for its progress line, so it needs the tiny Arduino seam.
$(BUILD)/mode_test: $(TESTDIR)/mode_test.cpp src/state/Mode.cpp src/subsystems/InputToggles.cpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(MOCKS) -I. $^ -o $@

# animation_test links only the pure-logic animation classes (no mocks):
# per-step full-frame rendering, packing, wrap semantics.
$(BUILD)/animation_test: $(TESTDIR)/animation_test.cpp \
    src/subsystems/ChestBuster/Animations/AnimationOff.cpp \
    src/subsystems/ChestBuster/Animations/LoaderBlueToGreen.cpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(MOCKS) -I. $^ -o $@

$(BUILD)/machine_test: $(TESTDIR)/machine_test.cpp $(REAL_SRCS)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(MOCKS) -I. $^ -o $@

clean:
	rm -rf $(BUILD)