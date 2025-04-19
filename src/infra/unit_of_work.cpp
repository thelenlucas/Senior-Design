#include "infra/unit_of_work.hpp"
#include <stdexcept>
#include <QSqlError>

using namespace woodworks::infra;

UnitOfWork::UnitOfWork(QSqlDatabase& db) : db_(db)
{
    if (!db_.transaction()) {
        throw std::runtime_error("Failed to start transaction: " + db_.lastError().text().toStdString());
    }
}

UnitOfWork::~UnitOfWork()
{
    if (!committed_) {
        db_.rollback();
    }
}

void UnitOfWork::commit()
{
    if (!db_.commit()) {
        throw std::runtime_error("Failed to commit transaction: " + db_.lastError().text().toStdString());
    }
    committed_ = true;
}