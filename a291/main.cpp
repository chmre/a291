#include <unitManager.h>
#include <stdio.h>
#include <algorithm>
#include <signal.h>

using namespace a291unit;

int main(int argc, char **argv)
{
	a291unit::UnitManager& unitManager = a291unit::UnitManager::instance();
	unitManager.setConfigurationFile("a291.config");
	unitManager.run();
	return 0;
}
