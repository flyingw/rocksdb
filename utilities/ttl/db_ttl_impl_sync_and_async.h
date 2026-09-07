//  Copyright (c) Meta Platforms, Inc. and affiliates. 
//  This source code is licensed under both the GPLv2 (found in the 
//  COPYING file in the root directory) and Apache 2.0 License 
//  (found in the LICENSE.Apache file in the root directory).

#include "util/coro_utils.h"

#if defined(WITHOUT_COROUTINES) || (USE_COROUTINES && defined(WITH_COROUTINES))

namespace ROCKSDB_NAMESPACE {

DEFINE_SYNC_AND_ASYNC(Status, DBWithTTLImpl::Get)
(const ReadOptions& options, ColumnFamilyHandle* column_family,
 const Slice& key, PinnableSlice* value, std::string* timestamp) {
  if (timestamp) {
    CO_RETURN Status::NotSupported(
        "Get() that returns timestamp is not supported");
  }

  Status status = CO_AWAIT(DBWithTTLImplBase::Get, options, column_family, key,
                           value, /*timestamp=*/nullptr);
  if (!status.ok()) {
    CO_RETURN status;
  }
  CO_RETURN CheckAndStripTimestamp(value);
}

DEFINE_SYNC_AND_ASYNC(void, DBWithTTLImpl::MultiGet)
(const ReadOptions& options, const size_t num_keys,
 ColumnFamilyHandle** column_families, const Slice* keys, PinnableSlice* values,
 std::string* timestamps, Status* statuses, const bool sorted_input) {
  if (timestamps) {
    for (size_t i = 0; i < num_keys; ++i) {
      statuses[i] = Status::NotSupported(
          "MultiGet() returning timestamps not implemented.");
    }
    CO_RETURN;
  }

  CO_AWAIT(DBWithTTLImplBase::MultiGet, options, num_keys, column_families,
           keys, values, /*timestamps=*/nullptr, statuses, sorted_input);
  ProcessMultiGetResults(num_keys, values, statuses);
}

}  // namespace ROCKSDB_NAMESPACE

#endif  // WITHOUT_COROUTINES || (USE_COROUTINES && WITH_COROUTINES)
