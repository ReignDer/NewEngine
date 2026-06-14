#pragma once
#include <cstdint>
namespace Sign {
	using MouseCode = uint16_t;

	namespace Mouse {
		enum : MouseCode {
			LeftButton   =	0x01, 	
			RightButton  =	0x02,
			MiddleButton =  0x04, 	
			XButton1 	 =  0x05, 	
			XButton2 	 =  0x06 	
		};
	}
}