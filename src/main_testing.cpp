#ifdef BUILDING_WOODWORKS_TEST

#include <cassert>
#include <optional>
#include <stdio.h>

#include "logs.hpp"
#include "types.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    Log log(-1, "Cherry", 12*12*4, 6*4, 100, 3, Drying::WET, "", "");
    assert(log.get_id() == -1);
    log.insert();
    assert(log.get_id() != -1);
    cout << "Log ID: " << log.get_id() << endl;

    optional<Log> fetchLog = Log::get_by_id(log.get_id());
    assert(fetchLog.has_value());
    assert(fetchLog->getSpecies() == "Cherry");
    assert(fetchLog->getLenQuarters() == 12*12*4);
    assert(fetchLog->getDiameterQuarters() == 6*4);
    cout << "Log insertion and retreival successful." << endl;

    // Test cutting a length from a log of 12 inches
    unsigned cutLength = 12*4;
    log.multiCut(cutLength, "TestCut");
    assert(log.getAvailableLength() == (log.getLenQuarters() - cutLength));
    cout << "Log cut successful. Remaining length: " << log.getAvailableLength() << endl;

    return 0;
}

#endif