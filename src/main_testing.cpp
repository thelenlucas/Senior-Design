#ifdef BUILDING_WOODWORKS_TEST

#include <cassert>
#include <optional>
#include <stdio.h>

#include "logs.hpp"
#include "types.hpp"
#include "cookies.hpp"

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

    // Take a foot off of kerf waste
    unsigned kerfWaste = 12;
    unsigned kerfWaste4ths = kerfWaste * 4;
    unsigned kerfWaste16ths = kerfWaste * 16;
    log.wasteKerf(kerfWaste16ths);
    cout << "Kerf waste applied. Remaining length: " << log.getAvailableLength() << endl;
    assert(log.getAvailableLength() == (log.getLenQuarters() - cutLength - kerfWaste4ths));

    // New log to test cookie cutting
    Log cookieLog(-1, "Maple", 12*12*4, 6*4, 100, 3, Drying::WET, "", "");
    cookieLog.insert();
    assert(cookieLog.get_id() != -1);
    cout << "Cookie Log ID: " << cookieLog.get_id() << endl;
    // 3 in cookie
    unsigned cookieThickness = 3*4;
    vector<Cookie> cookies = Cookie::make_from_log(cookieLog, cookieThickness);
    Cookie cookie = cookies[0];
    cookie.insert();
    assert(cookie.get_id() != -1);
    assert(cookie.getThicknessQuarters() == cookieThickness);
    assert(cookieLog.getAvailableLength() == (cookieLog.getLenQuarters() - cookieThickness));
    cout << "Cookie ID: " << cookie.get_id() << endl;
    cout << "Cookie tests passed" << endl;


    return 0;
}

#endif