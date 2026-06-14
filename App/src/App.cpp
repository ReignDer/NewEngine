#include "Application.h"
#include "Core.h"

int main() {
	Sign::PrintWorld();


	Sign::Application application{};
	application.Run();
}