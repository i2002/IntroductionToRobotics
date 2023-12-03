#ifndef DATA_STORE_H
#define DATA_STORE_H
#include <EEPROM.h>

template<typename T, int N = 1>
class DataStore {
  // T buf;
  int storeAddress;

public:
  DataStore(int _storeAddress) : storeAddress{_storeAddress} {}
  T readValue(int index = 0);
  void updateValue(T val, int index = 0);
};


template <typename T, int N>
T DataStore<T, N>::readValue(int index) {
  index = min(index, N - 1);
  T val;
  EEPROM.get(storeAddress + index * sizeof(T), val);
  return val;
}

template <typename T, int N>
void DataStore<T, N>::updateValue(T val, int index) {
  index = min(index, N - 1);
  EEPROM.put(storeAddress + index * sizeof(T), val);
}

#endif // DATA_STORE_H
