#include "order.hpp"

void order_DO_serialize_argument_into_stream_EXPECT_order_getting_unaltered_argument() {
  using namespace k2o;

  uint8_t value;
  order assign_to_value{[&](uint8_t x) { value = x; }};

  auto error = assign_to_value([]() -> byte_t { return 0xaf; });

  TEST_ASSERT_EQUAL_UINT(status::OK, error);
  TEST_ASSERT_EQUAL_INT(0xaf, value);
}

void order_DO_give_then_return_argument_from_order_EXPECT_unaltered_value() {
  using namespace k2o;

  constexpr int argument = 0xabc;
  auto serialized_argument = upd::make_tuple(argument);
  auto serialized_return_value = upd::make_tuple(int{0});
  order return_argument{[](int value) { return value; }};

  size_t i = 0, j = 0;
  auto error = return_argument([&]() { return serialized_argument[i++]; },
                               [&](byte_t byte) { serialized_return_value[j++] = byte; });

  TEST_ASSERT_EQUAL_UINT(status::OK, error);
  TEST_ASSERT_EQUAL_INT(argument, serialized_return_value.get<0>());
}

void order_DO_instantiate_order_with_functor_taking_arguments_EXPECT_input_and_output_sizes_correct() {
  using namespace k2o;

  order f([](int, int(&)[16], char) -> int { return 0; });

  TEST_ASSERT_EQUAL_INT(sizeof(int) + 16 * sizeof(int) + sizeof(char), f.input_size());
  TEST_ASSERT_EQUAL_INT(sizeof(int), f.output_size());
}

void order_DO_instantiate_order_with_functor_taking_no_arguments_EXPECT_input_and_output_sizes_correct() {
  using namespace k2o;

  order f([]() -> int { return 0; });

  TEST_ASSERT_EQUAL_INT(0, f.input_size());
  TEST_ASSERT_EQUAL_INT(sizeof(int), f.output_size());
}

void order_DO_instantiate_order_with_functor_returning_non_tuple_EXPECT_input_and_output_sizes_correct() {
  using namespace k2o;

  order f([](int) -> int { return 0; });

  TEST_ASSERT_EQUAL_INT(sizeof(int), f.input_size());
  TEST_ASSERT_EQUAL_INT(sizeof(int), f.output_size());
}

void order_DO_instantiate_order_with_functor_non_returning_EXPECT_input_and_output_sizes_correct() {
  using namespace k2o;

  order f([](int) {});

  TEST_ASSERT_EQUAL_INT(sizeof(int), f.input_size());
  TEST_ASSERT_EQUAL_INT(0, f.output_size());
}