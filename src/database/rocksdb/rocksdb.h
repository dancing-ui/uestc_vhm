#ifndef IO_ROCKSDB_H
#define IO_ROCKSDB_H

#include <cstdint>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>

namespace ns_uestc_vhm {
namespace dbase {

struct RocksDB {
    rocksdb::DB *db_ptr = nullptr;
    rocksdb::Options options;

    void open(std::string const &save_path);

    void put(std::string const &key, bool value);
    void put(std::string const &key, int32_t value);
    void put(std::string const &key, int64_t value);
    void put(std::string const &key, std::string const &value);
    void put(std::string const &key, std::vector<float> const &value);
    // void put(std::string const& key, const char *value, size_t length);

    void get(std::string const &key, bool &value);
    void get(std::string const &key, int32_t &value);
    void get(std::string const &key, int64_t &value);
    void get(std::string const &key, std::string &value);
    void get(std::string const &key, std::vector<float> &value);

    rocksdb::Iterator *iter_key();
    void erase(std::string const &key);
    void close();
    void purge();
};

} // dbase
} // ns_uestc_vhm

#endif /* ifndef IO_ROCKSDB_H */
