#include "dbms.hpp"
#include "interpreter.hpp"
#include "api.hpp"
#include "record_manager.hpp"
#include "index_manager.hpp"
#include "catalog_manager.hpp"
#include "buffer_manager.hpp"

DBMS::DBMS() {
    DBMS_INFORMATION = _DBMS_INFORMATION;
}

DBMS::~DBMS() {

}

void DBMS::Init() {
    buffer_manager_.Init();
    catalog_manager_.Init(&buffer_manager_);
    index_manager_.Init(&buffer_manager_);
    record_manager_.Init(&buffer_manager_);
    api_.Init(&record_manager_, &index_manager_, &catalog_manager_);
    interpreter_.Init(&api_, DBMS_INFORMATION);
}

void DBMS::Run() {
    interpreter_.Run();
}

void DBMS::Terminate() {
    buffer_manager_.Terminate();
    catalog_manager_.Terminate();
    index_manager_.Terminate();
    record_manager_.Terminate();
    api_.Terminate();
    interpreter_.Terminate();
}
