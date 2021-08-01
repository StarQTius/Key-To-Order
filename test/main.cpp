#include "main.hpp"

void setup() {
  UNITY_BEGIN();

  RUN_TEST(order_DO_serialize_argument_into_stream_EXPECT_order_getting_unaltered_argument);
  RUN_TEST(order_DO_give_then_return_argument_from_order_EXPECT_unaltered_value);
  RUN_TEST(order_DO_instantiate_order_with_functor_taking_arguments_EXPECT_input_and_output_sizes_correct);
  RUN_TEST(order_DO_instantiate_order_with_functor_taking_no_arguments_EXPECT_input_and_output_sizes_correct);
  RUN_TEST(order_DO_instantiate_order_with_functor_returning_non_tuple_EXPECT_input_and_output_sizes_correct);
  RUN_TEST(order_DO_instantiate_order_with_functor_non_returning_EXPECT_input_and_output_sizes_correct);

  RUN_TEST(key_DO_serialize_arguments_EXPECT_correct_byte_sequence);
  RUN_TEST(key_DO_unserialize_data_sequence_EXPECT_correct_value);

  UNITY_END();
}

void loop() {}
