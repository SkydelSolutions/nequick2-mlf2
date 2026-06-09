#pragma once

#include <vector>

namespace OnnxUtils
{
using DataType = float;

class InputParameters
{
public:
  virtual ~InputParameters() = default;

  virtual DataType* data() = 0;
  virtual size_t size() const = 0;
};

} // namespace OnnxUtils

class OnnxWrapperInterface
{
public:
  virtual ~OnnxWrapperInterface() = default;

  virtual std::vector<OnnxUtils::DataType> run(OnnxUtils::InputParameters& inputParameters) = 0;
  virtual std::vector<OnnxUtils::DataType> run(OnnxUtils::InputParameters&& inputParameters) = 0;
};
