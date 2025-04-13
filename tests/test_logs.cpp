#include <gtest/gtest.h>
#include "logs.hpp"           // data‑model under test
#include <SQLiteCpp/SQLiteCpp.h>

namespace {

class LogFixture : public ::testing::Test {
protected:
    void SetUp() override
    {
        // Use an **in‑memory** database so tests stay hermetic.
        db = std::make_unique<SQLite::Database>(":memory:", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
        db->exec("CREATE TABLE taken_len_all(from_log INT, len_quarters INT);");
    }

    std::unique_ptr<SQLite::Database> db;
};

// ------------------------------------------------------------------
// Construction & simple getters
// ------------------------------------------------------------------
TEST_F(LogFixture, BasicProperties)
{
    Log oak{1, "Oak", 120, 36, 25000, 4, Drying::AIR_DRIED};
    EXPECT_EQ(oak.get_id(), 1);
    EXPECT_EQ(oak.getSpecies(), "Oak");
    EXPECT_EQ(oak.getLenQuarters(), 120u);
}

// ------------------------------------------------------------------
// getAvailableLength() when nothing has been cut
// ------------------------------------------------------------------
TEST_F(LogFixture, AvailableLengthInitiallyEqualsTotal)
{
    Log l{2, "Maple", 80, 28, 20000, 3, Drying::WET};
    EXPECT_EQ(l.getAvailableLength(), 80u);
}

// ------------------------------------------------------------------
// getAvailableLength() after a partial cut
// ------------------------------------------------------------------
TEST_F(LogFixture, AvailableLengthSubtractsTakenLength)
{
    Log l{3, "Walnut", 100, 30, 30000, 5, Drying::KILN_DRIED};
    // Simulate 40 quarters already taken:
    db->exec("INSERT INTO taken_len_all(from_log,len_quarters) VALUES(3,40);");
    EXPECT_EQ(l.getAvailableLength(), 60u);
}

} // namespace