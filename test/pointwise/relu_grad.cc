/*
 * Copyright 2019 Codeplay Software Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use these files except in compliance with the License.
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

// DO NOT MODIFY BY HAND
// This file was automatically generated by generate_pointwise_tests.py.
// Results calculated using Tensorflow v1.12.0.

#include <gtest/gtest.h>

#include "sycldnn/pointwise/direction.h"
#include "sycldnn/pointwise/launch.h"
#include "sycldnn/pointwise/operators.h"

#include "test/pointwise/pointwise_fixture.h"
#include "test/types/kernel_data_types.h"

#include <CL/sycl.hpp>

#include <vector>

using namespace sycldnn;  // NOLINT(google-build-using-namespace)
template <typename DataType>
using ReluGrad =
    PointwiseFixture<DataType, pointwise::Relu, pointwise::Gradient>;
TYPED_TEST_CASE(ReluGrad, types::GTestKernelDataTypes);
TYPED_TEST(ReluGrad, Shape_1x1) {
  using DataType = typename TestFixture::DataType;
  const std::vector<DataType> exp_out = {-0.};
  this->test_pointwise(exp_out);
}
TYPED_TEST(ReluGrad, Shape_8x1) {
  using DataType = typename TestFixture::DataType;
  const std::vector<DataType> exp_out = {-0., -0., -0., -0., 0., 1., 2., 3.};
  this->test_pointwise(exp_out);
}
TYPED_TEST(ReluGrad, Shape_9x1) {
  using DataType = typename TestFixture::DataType;
  const std::vector<DataType> exp_out = {-0., -0., -0., -0., -0.,
                                         0.,  1.,  2.,  3.};
  this->test_pointwise(exp_out);
}
TYPED_TEST(ReluGrad, Shape_10x1) {
  using DataType = typename TestFixture::DataType;
  const std::vector<DataType> exp_out = {-0., -0., -0., -0., -0.,
                                         0.,  1.,  2.,  3.,  4.};
  this->test_pointwise(exp_out);
}
