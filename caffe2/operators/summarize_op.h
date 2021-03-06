/**
 * Copyright (c) 2016-present, Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CAFFE2_OPERATORS_SUMMARIZE_OP_H_
#define CAFFE2_OPERATORS_SUMMARIZE_OP_H_

#include <fstream>

#include "caffe2/core/context.h"
#include "caffe2/core/operator.h"
#include "caffe2/utils/math.h"

namespace caffe2 {

constexpr char kSummaryzeOpExtension[] = ".summary";

template <typename T, class Context>
class SummarizeOp final : public Operator<Context> {
 public:
  SummarizeOp(const OperatorDef& def, Workspace* ws)
      : Operator<Context>(def, ws),
        to_file_(OperatorBase::GetSingleArgument<int>("to_file", 0)) {
    if (to_file_) {
      // We will output to file instead of printing on screen.
      const string& target_folder = ws->RootFolder();
      // We will write each individual tensor to its individual file.
      // Also, since the namescope is currently represented by "/", we will
      // need to replace it with a symbol that does not conflict with the
      // folder separator in Linux.
      string proper_name = def.input(0);
      std::replace(proper_name.begin(), proper_name.end(), '/', '#');
      log_file_.reset(new std::ofstream(
          target_folder + "/" + proper_name + kSummaryzeOpExtension,
          std::ofstream::out | std::ofstream::trunc));
      CAFFE_ENFORCE(
          log_file_->good(),
          "Failed to open summarize file for tensor ",
          def.input(0),
          ". rdstate() = ",
          log_file_->rdstate());
    }
  }
  ~SummarizeOp() {
    if (to_file_)
      log_file_->close();
  }
  USE_OPERATOR_CONTEXT_FUNCTIONS;
  bool RunOnDevice() override;

  static constexpr int MIN_IDX = 0;
  static constexpr int MAX_IDX = 1;
  static constexpr int MEAN_IDX = 2;
  static constexpr int STD_IDX = 3;

  static constexpr int NUM_STATS = 4;

 protected:
  bool to_file_;
  std::unique_ptr<std::ofstream> log_file_;
};

} // namespace caffe2

#endif // CAFFE2_OPERATORS_SUMMARIZE_OP_H_
