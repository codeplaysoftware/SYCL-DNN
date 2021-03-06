/*
 * Copyright 2018 Codeplay Software Ltd.
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
#ifndef SYCLDNN_TEST_CONV2D_CONVOLUTION_FIXTURE_H_
#define SYCLDNN_TEST_CONV2D_CONVOLUTION_FIXTURE_H_

#include <gtest/gtest.h>
#include <vector>

#include "sycldnn/conv2d/launch.h"
#include "sycldnn/conv2d/params.h"
#include "sycldnn/conv2d/sizes.h"

#include "sycldnn/helpers/scope_exit.h"

#include "test/backend/backend_test_fixture.h"
#include "test/gen/iota_initialised_data.h"
#include "test/helpers/float_comparison.h"

template <typename Triple>
struct ConvolutionFixture
    : public BackendTestFixture<typename Triple::ThirdType> {
  using SelectorType = typename Triple::FirstType;
  using DataType = typename Triple::SecondType;
  using Backend = typename Triple::ThirdType;

 protected:
  /** Test a convolution with both input and filter set to `1, 2, 3,...` */
  template <typename ConvType>
  void test_conv(std::vector<DataType> const& exp,
                 sycldnn::conv2d::Conv2DParams const& params,
                 DataType max_val = static_cast<DataType>(0),
                 size_t input_offset = 0u, size_t filter_offset = 0u,
                 size_t output_offset = 0u) {
    auto conv_sizes = sycldnn::conv2d::get_sizes<ConvType>(params);
    ASSERT_EQ(conv_sizes.output_size, exp.size());

    std::vector<DataType> input =
        iota_initialised_data(conv_sizes.input_size, max_val);
    input.insert(input.begin(), input_offset, 0);
    std::vector<DataType> filter =
        iota_initialised_data(conv_sizes.filter_size, max_val);
    filter.insert(filter.begin(), filter_offset, 0);
    std::vector<DataType> output(conv_sizes.output_size + output_offset,
                                 static_cast<DataType>(0));

    auto& provider = this->provider_;
    auto& backend = provider.get_backend();

    auto inp_gpu = provider.get_initialised_device_memory(input.size(), input);
    auto fil_gpu =
        provider.get_initialised_device_memory(filter.size(), filter);
    auto out_gpu =
        provider.get_initialised_device_memory(output.size(), output);
    SNN_ON_SCOPE_EXIT {
      provider.deallocate_ptr(inp_gpu);
      provider.deallocate_ptr(fil_gpu);
      provider.deallocate_ptr(out_gpu);
    };

    SelectorType selector{};
    if (selector.template select<ConvType>(params) ==
        sycldnn::conv2d::Algorithm::NotSupported) {
      // Do not run the test if the implementation is not supported.
      return;
    }
    try {
      auto status = sycldnn::conv2d::launch<DataType, ConvType>(
          inp_gpu + input_offset, fil_gpu + filter_offset,
          out_gpu + output_offset, params, selector, backend);

      if (status.status == sycldnn::StatusCode::InvalidAlgorithm) {
        // Do not check results if the implementation is not supported.
        return;
      }
      ASSERT_EQ(sycldnn::StatusCode::OK, status.status);
      status.event.wait_and_throw();
    } catch (cl::sycl::exception const& e) {
      std::cerr << "Caught SYCL exception:\n" << e.what() << std::endl;
      throw;
    }

    provider.copy_device_data_to_host(output.size(), out_gpu, output);

    for (size_t i = 0; i < output_offset; ++i) {
      SCOPED_TRACE("Element: " + std::to_string(i));
      EXPECT_EQ(DataType{0}, output[i]);
    }
    for (size_t i = 0; i < exp.size(); ++i) {
      SCOPED_TRACE("Element: " + std::to_string(i));
      SNN_ALMOST_EQUAL(exp[i], output[i + output_offset], 10u);
    }
  }
};

#endif  // SYCLDNN_TEST_CONV2D_CONVOLUTION_FIXTURE_H_
