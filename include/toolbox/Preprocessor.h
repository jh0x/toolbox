#pragma once

#define XYZ_STRINGIFY2(x) #x
#define XYZ_STRINGIFY(x) XYZ_STRINGIFY2(x)

#define XYZ_CONCAT2(x, y) x ## y
#define XYZ_CONCAT(x, y) XYZ_CONCAT2(x, y)

