// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2017 Preferred Networks and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <map>
#include <vector>
#include <utility>
#include <string>
#include <gtest/gtest.h>

#include "jubatus/core/fv_converter/exception.hpp"

#include "python_bridge.hpp"
#include "pb_num_feature.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {
namespace python {

using std::map;
using std::vector;
using std::pair;
using std::string;

TEST(pb_num_feature, trivial) {
  // Test fixture setup.
  initialize();
  add_path(".");

  map<string, string> params;
  params["module"] = "test_input.test_module";
  params["class"] = "NumFeature";
  pb_num_feature pb(setup(params));

  vector<pair<string, float> > ret_fv;
  pb.add_feature("key", 1.0, ret_fv);
  ASSERT_EQ(1, ret_fv.size());
  EXPECT_EQ("result", ret_fv[0].first);
  EXPECT_EQ(1.0, ret_fv[0].second);
}

}  // namespace python
}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus
