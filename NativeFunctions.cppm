
export module NativeFunctions;

import <chrono>;

import Object;


export namespace NativeFunction {
	Object clock(CallParams) {
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		);
		return Object(static_cast<double>(ms.count()));
	}
}