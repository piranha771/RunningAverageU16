//
//    FILE: RunningAverage.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.4.5
//    DATE: 2011-01-30
// PURPOSE: Arduino library to calculate the running average by means of a circular buffer
//     URL: https://github.com/RobTillaart/RunningAverage
//
//  The library stores N individual values in a circular buffer,
//  to calculate the running average.


#include "RunningAverage.h"


RunningAverage::RunningAverage(const uint16_t size)
{
  _size = size;
  _partial = _size;
  _array = (float*) malloc(_size * sizeof(float));
  if (_array == NULL) _size = 0;
  clear();
}


RunningAverage::~RunningAverage()
{
  if (_array != NULL) free(_array);
}


//  resets all counters
void RunningAverage::clear()
{
  _count = 0;
  _index = 0;
  _sum = 0;
  _min = 0;
  _max = 0;
  for (uint16_t i = _size; i > 0; )
  {
    _array[--i] = 0;  //  keeps addValue simpler
  }
}


//  adds a new value to the data-set
void RunningAverage::addValue(const uint16_t value)
{
  if (_array == NULL)
  {
    return;
  }

  _sum -= _array[_index];
  _array[_index] = value;
  _sum += _array[_index];
  _index++;

  if (_index == _partial) _index = 0;  //  faster than %

  //  handle min max
  if (_count == 0) _min = _max = value;
  else if (value < _min) _min = value;
  else if (value > _max) _max = value;

  //  update count as last otherwise if ( _count == 0) above will fail
  if (_count < _partial) _count++;
}


//  returns the average of the data-set added so far, NAN if no elements.
uint16_t RunningAverage::getAverage()
{
  if (_count == 0)
  {
    return NAN;
  }
  //  OPTIMIZE local variable for sum.
  _sum = 0;
  for (uint16_t i = 0; i < _count; i++)
  {
    _sum += _array[i];
  }
  return _sum / _count;   //  multiplication is faster ==> extra admin
}


//  the larger the size of the internal buffer 
//  the greater the gain wrt getAverage()
uint16_t RunningAverage::getFastAverage() const
{
  if (_count == 0)
  {
    return 0;
  }

  return _sum / _count;   //  multiplication is faster ==> extra admin
}


//  returns the minimum value in the buffer
uint16_t RunningAverage::getMinInBuffer() const
{
  if (_count == 0)
  {
    return 0;
  }

  uint16_t _min = _array[0];
  for (uint16_t i = 1; i < _count; i++)
  {
    if (_array[i] < _min) _min = _array[i];
  }
  return _min;
}


//  returns the maximum value in the buffer
uint16_t RunningAverage::getMaxInBuffer() const
{
  if (_count == 0)
  {
    return 0;
  }

  uint16_t _max = _array[0];
  for (uint16_t i = 1; i < _count; i++)
  {
    if (_array[i] > _max) _max = _array[i];
  }
  return _max;
}


//  returns the value of an element if exist, NAN otherwise
uint16_t RunningAverage::getElement(uint16_t index) const
{
  if (_count == 0)
  {
    return 0;
  }

  return _array[index];
}


//  Return standard deviation of running average.
//  If buffer is empty or has only one element, return NAN.
float RunningAverage::getStandardDeviation() const
{
  //  see issue #13
  //  need float _stddev = -1;
  //     + patch add() and clear() to reset _stddev to -1;
  //  if (_stddev != -1) return _stddev;
  if (_count <= 1) return NAN;

  float temp = 0;
  float average = getFastAverage();
  for (uint16_t i = 0; i < _count; i++)
  {
    temp += pow((_array[i] - average), 2);
  }
  temp = sqrt(temp/(_count - 1));
  return temp;
  //  see issue #13
  //  _stddev = temp;  // cache the calculate value
  //  return _stddev;
}


//  Return standard error of running average.
//  If buffer is empty or has only one element, return NAN.
float RunningAverage::getStandardError() const
{
  float temp = getStandardDeviation();
  if (temp == NAN) return NAN;

  float n;
  if (_count >= 30) n = _count;
  else n = _count - 1;
  temp = temp/sqrt(n);

  return temp;
}


//  fill the average with the same value number times. (weight)
//  This is maximized to size times.
//  no need to fill the internal buffer over 100%
void RunningAverage::fillValue(const float value, const uint16_t number)
{
  clear();
  uint16_t s = number;
  if (s > _partial) s = _partial;

  for (uint16_t i = s; i > 0; i--)
  {
    addValue(value);
  }
}


// https://github.com/RobTillaart/RunningAverage/issues/13
// - substantially faster version off fillValue()
// - adds to program size
// void RunningAverage::fillValue(const float value, const uint16_t number)
// {
  // uint16_t s = number;
  // if (s > _partial) s = _partial;
  // for (uint16_t i = 0; i < s; i++)
  // {
    // _array[i] = value;
  // }
  // _min = value;
  // _max = value;
  // _sum = value * s;
  // _count = s;
  // _index = s;
  // if (_index == _partial) _index = 0;
// }


float RunningAverage::getValue(const uint16_t position)
{
  if (_count == 0)
  {
    return 0;
  }
  if (position >= _count)
  {
    return 0;  // cannot ask more than is added
  }

  uint16_t _pos = position + _index;
  if (_pos >= _count) _pos -= _count;
  return _array[_pos];
}


void RunningAverage::setPartial(const uint16_t partial)
{
  _partial = partial;
  if ((_partial == 0) || (_partial > _size)) _partial = _size;
  clear();
}


float RunningAverage::getAverageLast(uint16_t count)
{
  uint16_t cnt = count;
  if (cnt > _count) cnt = _count;
  if (cnt == 0) return NAN;

  uint16_t idx = _index;
  float _sum = 0;
  for (uint16_t i = 0; i < cnt; i++)
  {
    if (idx == 0) idx = _size;
    idx--;
    _sum +=_array[idx];
  }
  return _sum / cnt;
}


float RunningAverage::getMinInBufferLast(uint16_t count)
{
  uint16_t cnt = count;
  if (cnt > _count) cnt = _count;
  if (cnt == 0) return NAN;

  uint16_t idx = _index;
  if (idx == 0) idx = _size;
  idx--;
  float _min = _array[idx];
  for (uint16_t i = 0; i < cnt; i++)
  {
    if (_array[idx] < _min) _min = _array[idx];
    if (idx == 0) idx = _size;
    idx--;
  }
  return _min;
}


float RunningAverage::getMaxInBufferLast(uint16_t count)
{
  uint16_t cnt = count;
  if (cnt > _count) cnt = _count;
  if (cnt == 0) return NAN;

  uint16_t idx = _index;
  if (idx == 0) idx = _size;
  idx--;
  float _max = _array[idx];
  for (uint16_t i = 0; i < cnt; i++)
  {
    if (_array[idx] > _max) _max = _array[idx];
    if (idx == 0) idx = _size;
    idx--;
  }
  return _max;
}


float RunningAverage::getAverageSubset(uint16_t start, uint16_t count)
{
  if (_count == 0)
  {
    return NAN;
  }

  uint16_t cnt = _count;
  if (cnt > count) cnt = count;

  float sum = 0;   //  do not disrupt global _sum
  for (uint16_t i = 0; i < cnt; i++)
  {
    uint16_t idx = _index + start + i;
    while (idx >= _partial) idx -= _partial;
    sum += _array[idx];
  }
  return sum / cnt;
}

