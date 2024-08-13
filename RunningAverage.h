#pragma once
//
//    FILE: RunningAverage.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.4.5
//    DATE: 2011-01-30
// PURPOSE: Arduino library to calculate the running average by means of a circular buffer
//     URL: https://github.com/RobTillaart/RunningAverage
//
//  The library stores N individual values in a circular buffer,
//  to calculate the running average.


#include "Arduino.h"


#define RUNNINGAVERAGE_LIB_VERSION    (F("0.4.5"))


class RunningAverage
{
public:
  explicit RunningAverage(const uint16_t size);
  ~RunningAverage();

  void     clear();
  void     add(const uint16_t value)    { addValue(value); };
  void     addValue(const uint16_t value);
  void     fillValue(const uint16_t value, const uint16_t number);
  uint16_t    getValue(const uint16_t position);

  uint16_t    getAverage();            //  iterates over all elements.
  uint16_t    getFastAverage() const;  //  reuses previous calculated values.

  //  return statistical characteristics of the running average
  uint16_t    getStandardDeviation() const;
  uint16_t    getStandardError() const;

  //  returns min/max added to the data-set since last clear
  uint16_t    getMin() const { return _min; };
  uint16_t    getMax() const { return _max; };

  //  returns min/max from the values in the internal buffer
  uint16_t    getMinInBuffer() const;
  uint16_t    getMaxInBuffer() const;

  //  return true if buffer is full
  bool     bufferIsFull() const { return _count == _size; };

  uint16_t    getElement(uint16_t index) const;

  uint16_t getSize() const { return _size; }
  uint16_t getCount() const { return _count; }

  //  use not all elements just a part from 0..partial-1
  //  (re)setting partial will clear the internal buffer.
  void     setPartial(const uint16_t partial = 0);  // 0 ==> use all
  uint16_t getPartial()   { return _partial; };


  //  get some stats from the last count additions.
  uint16_t    getAverageLast(uint16_t count);
  uint16_t    getMinInBufferLast(uint16_t count);
  uint16_t    getMaxInBufferLast(uint16_t count);

  //       Experimental 0.4.3
  float    getAverageSubset(uint16_t start, uint16_t count);


protected:
  uint16_t _size;
  uint16_t _count;
  uint16_t _index;
  uint16_t _partial;
  uint16_t    _sum;
  uint16_t*   _array;
  uint16_t    _min;
  uint16_t    _max;
};


//  -- END OF FILE --

