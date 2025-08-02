MIK32_HAL_DIR = hardware/mik32-hal
MIK32_SHARED_DIR = hardware/mik32v2-shared

BUILD_DIR = build

.PHONY: clean build_app flash tests

build_app: update_submodules $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

# not .PHONY because we dont want to launch it every build
update_submodules: $(MIK32_HAL_DIR)/README.md $(MIK32_SHARED_DIR)/README.md

$(MIK32_HAL_DIR)/README.md:
	git submodule update --init hardware/mik32-hal

$(MIK32_SHARED_DIR)/README.md:
	git submodule update --init hardware/mik32v2-shared

$(BUILD_DIR):
	cmake -G Ninja --toolchain cmake_toolchain/toolchain_config.cmake -B $(BUILD_DIR)

tests: update_submodules
	ceedling test:all


flash:
	python3 $(MIK32_UPLOADER_DIR)/mik32_upload.py build/src/SmartCooling.hex --run-openocd \
	--openocd-exec /usr/bin/openocd \
	--openocd-target $(MIK32_UPLOADER_DIR)/openocd-scripts/target/mik32.cfg \
	--openocd-interface $(MIK32_UPLOADER_DIR)/openocd-scripts/interface/ftdi/mikron-link.cfg \
	--adapter-speed 500 --mcu-type MIK32V2


clean:
	rm -rf $(BUILD_DIR)
