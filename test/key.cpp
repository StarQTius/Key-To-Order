#include <k2o/cpp11.hpp>
#include <k2o/flist.hpp>
#include <k2o/key.hpp>
#include <k2o/keyring.hpp>
#include <k2o/ticket.hpp>

#include "utility.hpp"

struct object_t {
  uint8_t a;
  uint16_t b, c;
};

int function(int);
int big_function(int, const int (&)[8], char);
int integer_function(int, short, char);
int procedure();
auto functor = [](int x) { return x; };
const object_t &function_object(const object_t &x) { return x; }

constexpr auto list = k2o::make_flist(K2O_CTREF(function),
                                      K2O_CTREF(big_function),
                                      K2O_CTREF(integer_function),
                                      K2O_CTREF(procedure),
                                      K2O_CTREF(functor),
                                      K2O_CTREF(function_object));
constexpr auto kring = k2o::make_keyring(list);

struct object_extension_t {
  template<typename View_T>
  static void serialize(const object_t &o, View_T &view) {
    view = upd::make_tuple(o.a, o.b, o.c);
  }

  static object_t unserialize(uint8_t a, uint16_t b, uint16_t c) { return {a, b, c}; }
};
object_extension_t upd_extension(object_t *) { return {}; }

static void key_base_DO_serialize_arguments_EXPECT_correct_byte_sequence() {
  using namespace k2o;

  constexpr auto k = kring.get(K2O_CTREF(big_function));
  auto index = k.index;
  int array[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
  auto t = upd::make_tuple(index, int{64}, array, char{16});
  upd::byte_t dest_buf[t.size];

  int i = 0;
  t.view<1, 3>().invoke(k) >> [&](upd::byte_t byte) { dest_buf[i++] = byte; };

  TEST_ASSERT_EQUAL_UINT8_ARRAY(t.begin(), dest_buf, t.size);
}

static void key_base_DO_serialize_arguments_with_parameter_EXPECT_correct_byte_sequence() {
  using namespace k2o;

  constexpr auto kring = make_keyring(list, upd::little_endian, upd::one_complement);
  constexpr auto k = kring.get(K2O_CTREF(integer_function));
  auto dest_tuple = upd::make_tuple<decltype(k)::index_t, int, short, char>();

  int i = 0;
  k(64, 32, 16) >> [&](upd::byte_t byte) { *(dest_tuple.begin() + i++) = byte; };

  TEST_ASSERT_EQUAL_INT(2, dest_tuple.get<0>());
  TEST_ASSERT_EQUAL_INT(64, dest_tuple.get<1>());
  TEST_ASSERT_EQUAL_INT(32, dest_tuple.get<2>());
  TEST_ASSERT_EQUAL_INT(16, dest_tuple.get<3>());
}

static void key_base_DO_unserialize_data_sequence_EXPECT_correct_value() {
  using namespace k2o;

  constexpr auto k = kring.get(K2O_CTREF(procedure));
  auto t = upd::make_tuple(int{64});

  int i = 0;
  auto value = k << [&]() { return t[i++]; };

  TEST_ASSERT_EQUAL_INT(t.get<0>(), value);
}

static void key_base_DO_unserialize_data_sequence_with_parameter_EXPECT_correct_value() {
  using namespace k2o;

  constexpr auto kring = make_keyring(list, upd::little_endian, upd::one_complement);
  constexpr auto k = kring.get(K2O_CTREF(integer_function));
  auto src_tuple = upd::make_tuple(upd::little_endian, upd::two_complement, int{64});

  int i = 0;
  auto result = k << [&]() { return *(src_tuple.begin() + i++); };

  TEST_ASSERT_EQUAL_INT(src_tuple.get<0>(), result);
}

static void key_base_DO_create_key_from_ftor_signature_EXPECT_key_holding_ftor_signature() {
  using namespace k2o;

  auto k = kring.get(K2O_CTREF(functor));

  int i = 0, j = sizeof k.index;
  auto buf = upd::make_tuple<decltype(k)::index_t, int>();
  k(64) >> [&](upd::byte_t byte) { buf[i++] = byte; };
  auto result = k << [&]() { return buf[j++]; };

  TEST_ASSERT_EQUAL_INT(64, result);
}

static void key_base_DO_create_key_from_function_using_user_extended_type_EXPECT_correct_behaviour() {
  using namespace k2o;

  auto k = kring.get(K2O_CTREF(function_object));
  auto buf = upd::make_tuple<decltype(k)::index_t, object_t>();

  int i = 0, j = sizeof k.index;
  k({0xa, 0xb, 0xc}) >> [&](upd::byte_t byte) { buf[i++] = byte; };
  auto result = k << [&]() { return buf[j++]; };

  TEST_ASSERT_EQUAL_UINT8(result.a, 0xa);
  TEST_ASSERT_EQUAL_UINT16(result.b, 0xb);
  TEST_ASSERT_EQUAL_UINT16(result.c, 0xc);
}

static void key_base_DO_hook_a_callback_EXPECT_callback_receiving_correct_argument() {
  using namespace k2o;

  auto k = kring.get(K2O_CTREF(procedure));
  auto t = upd::make_tuple(int{64});

  int i = 0;
  k.with_hook([](int value) { TEST_ASSERT_EQUAL_INT(64, value); })([&]() { return t[i++]; });
}

#define KEY DECLVAL(key<uint8_t, 0, int(int), upd::endianess::BUILTIN, upd::signed_mode::BUILTIN>)
#define TICKET DECLVAL(ticket)

int main() {
  using namespace k2o;

  DETECT(INTEGER = KEY.read_all(BYTE_PTR),
         INTEGER = KEY.read_all(READABLE),
         INTEGER = KEY << BYTE_PTR,
         INTEGER = KEY << READABLE,
         KEY(0).write_all(BYTE_PTR),
         KEY(0).write_all(WRITABLE),
         KEY(0) >> BYTE_PTR,
         KEY(0) >> WRITABLE,
         TICKET(BYTE_PTR),
         TICKET(READABLE));

  UNITY_BEGIN();
  RUN_TEST(key_base_DO_serialize_arguments_EXPECT_correct_byte_sequence);
  RUN_TEST(key_base_DO_serialize_arguments_with_parameter_EXPECT_correct_byte_sequence);
  RUN_TEST(key_base_DO_unserialize_data_sequence_EXPECT_correct_value);
  RUN_TEST(key_base_DO_unserialize_data_sequence_with_parameter_EXPECT_correct_value);
  RUN_TEST(key_base_DO_create_key_from_ftor_signature_EXPECT_key_holding_ftor_signature);
  RUN_TEST(key_base_DO_create_key_from_function_using_user_extended_type_EXPECT_correct_behaviour);
  RUN_TEST(key_base_DO_hook_a_callback_EXPECT_callback_receiving_correct_argument);
  return UNITY_END();
}
