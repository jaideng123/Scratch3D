#include "string_converter.h"

#include <string>
#include <sstream>
#include <ios>
#include <vector>
#include <cstring>

#include <glm/gtc/type_ptr.hpp>
#include <utilities/assert.h>

#ifdef __MINGW32__
#define _strtoull_l _strtoul_l
#define _strtoll_l _strtol_l
#endif

#ifdef _MSC_VER
#define LC_NUMERIC_MASK LC_NUMERIC
#define newlocale(cat, loc, base) _create_locale(cat, loc)
#define locale_t _locale_t
#define strtod_l _strtod_l
#define strtol_l _strtol_l
#define strnicmp _strnicmp
#endif

#ifdef __GNUC__
#define strnicmp strncasecmp
#endif


#define OGRE_DEFAULT_LOCALE "C"


locale_t scratch::StringConverter::numLocale = newlocale(LC_NUMERIC_MASK, OGRE_DEFAULT_LOCALE, NULL);

//-----------------------------------------------------------------------
std::string scratch::StringConverter::toString(float val, unsigned short precision,
                                               unsigned short width, char fill, std::ios::fmtflags flags) {
    std::stringstream stream;
    stream.precision(precision);
    stream.width(width);
    stream.fill(fill);
    if (flags)
        stream.setf(flags);
    stream << val;
    return stream.str();
}

//-----------------------------------------------------------------------
std::string scratch::StringConverter::toString(const glm::vec2 &val) {
    std::stringstream stream;
    stream << val.x << " " << val.y;
    return stream.str();
}

//-----------------------------------------------------------------------
std::string scratch::StringConverter::toString(const glm::vec3 &val) {
    std::stringstream stream;
    stream << val.x << " " << val.y << " " << val.z;
    return stream.str();
}

//-----------------------------------------------------------------------
std::string scratch::StringConverter::toString(const glm::vec4 &val) {
    std::stringstream stream;
    stream << val.x << " " << val.y << " " << val.z << " " << val.w;
    return stream.str();
}

//-----------------------------------------------------------------------
std::string scratch::StringConverter::toString(const glm::mat3 &val) {
    std::stringstream stream;
    stream << val[0][0] << " "
           << val[0][1] << " "
           << val[0][2] << " "
           << val[1][0] << " "
           << val[1][1] << " "
           << val[1][2] << " "
           << val[2][0] << " "
           << val[2][1] << " "
           << val[2][2];
    return stream.str();
}

//-----------------------------------------------------------------------
std::string scratch::StringConverter::toString(bool val, bool yesNo) {
    if (val) {
        if (yesNo) {
            return "yes";
        } else {
            return "true";
        }
    } else if (yesNo) {
        return "no";
    } else {
        return "false";
    }
}

//-----------------------------------------------------------------------
std::string scratch::StringConverter::toString(const glm::mat4 &val) {
    std::stringstream stream;
    stream << val[0][0] << " "
           << val[0][1] << " "
           << val[0][2] << " "
           << val[0][3] << " "
           << val[1][0] << " "
           << val[1][1] << " "
           << val[1][2] << " "
           << val[1][3] << " "
           << val[2][0] << " "
           << val[2][1] << " "
           << val[2][2] << " "
           << val[2][3] << " "
           << val[3][0] << " "
           << val[3][1] << " "
           << val[3][2] << " "
           << val[3][3];
    std::string returnValue = stream.str();
    return returnValue;
}

//-----------------------------------------------------------------------
float scratch::StringConverter::parsefloat(const std::string &val) {
    char *end;
    return (float) strtod_l(val.c_str(), &end, numLocale);
}

//-----------------------------------------------------------------------
int scratch::StringConverter::parseint(const std::string &val) {
    char *end;
    return (int) strtol_l(val.c_str(), &end, 0, numLocale);
}

//-----------------------------------------------------------------------
bool scratch::StringConverter::parsebool(const std::string &val) {
    //FIXME Returns both parsed value and error in same value - ambiguous.
    // Suggested alternatives: implement exception handling or make either
    // error or parsed value a parameter.
    if ((scratch::StringConverter::startsWith(val, "true") || scratch::StringConverter::startsWith(val, "yes") ||
         scratch::StringConverter::startsWith(val, "1") || scratch::StringConverter::startsWith(val, "on")))
        return true;
    else if ((scratch::StringConverter::startsWith(val, "false") || scratch::StringConverter::startsWith(val, "no") ||
              scratch::StringConverter::startsWith(val, "0") || scratch::StringConverter::startsWith(val, "off")))
        return false;
    else
        SCRATCH_ASSERT_NEVER("Invalid Bool value");
}

//-----------------------------------------------------------------------
glm::vec2 scratch::StringConverter::parsevec2(const std::string &val) {
    // Split on space
    std::vector<std::string> vec = scratch::StringConverter::split(val);
    std::vector<float> floatVec;
    for (size_t i = 0; i < vec.size(); i++) {
        floatVec.push_back(scratch::StringConverter::parsefloat(vec[i]));
    }
     if (vec.size() != 2) {
         SCRATCH_ASSERT_NEVER("Invalid Vector Size");
     }
    return glm::vec2(floatVec[0], floatVec[1]);
}

//-----------------------------------------------------------------------
glm::vec3 scratch::StringConverter::parsevec3(const std::string &val) {
    // Split on space
    std::vector<std::string> vec = scratch::StringConverter::split(val);
    std::vector<float> floatVec;
    for (size_t i = 0; i < vec.size(); i++) {
        floatVec.push_back(scratch::StringConverter::parsefloat(vec[i]));
    }

    if (vec.size() != 3) {
        SCRATCH_ASSERT_NEVER("Invalid Vector Size, should be 3");
    }
    return glm::vec3(floatVec[0], floatVec[1], floatVec[2]);
}

//-----------------------------------------------------------------------
glm::vec4 scratch::StringConverter::parsevec4(const std::string &val) {
    // Split on space
    std::vector<std::string> vec = scratch::StringConverter::split(val);
    std::vector<float> floatVec;
    for (size_t i = 0; i < vec.size(); i++) {
        floatVec.push_back(scratch::StringConverter::parsefloat(vec[i]));
    }
    if (vec.size() != 4) {
        SCRATCH_ASSERT_NEVER("Invalid Vector Size");
    }
    return glm::vec4(floatVec[0], floatVec[1], floatVec[2], floatVec[3]);
}

//-----------------------------------------------------------------------
glm::mat3 scratch::StringConverter::parsemat3(const std::string &val) {
    // Split on space
    std::vector<std::string> vec = scratch::StringConverter::split(val);
    std::vector<float> floatVec;
    for (size_t i = 0; i < vec.size(); i++) {
        floatVec.push_back(scratch::StringConverter::parsefloat(vec[i]));
    }
    if (vec.size() != 9) {
        SCRATCH_ASSERT_NEVER("Invalid Vector Size");
    }
    return glm::mat3(floatVec[0], floatVec[1], floatVec[2],
                     floatVec[3], floatVec[4], floatVec[5],
                     floatVec[6], floatVec[7], floatVec[8]);
}

//-----------------------------------------------------------------------
glm::mat4 scratch::StringConverter::parsemat4(const std::string &val) {
    // Split on space
    std::vector<std::string> vec = scratch::StringConverter::split(val);
    std::vector<float> floatVec;
    for (size_t i = 0; i < vec.size(); i++) {
        floatVec.push_back(scratch::StringConverter::parsefloat(vec[i]));
    }
    if (vec.size() != 16) {
        SCRATCH_ASSERT_NEVER("Invalid Vector Size");
    }
    return glm::make_mat4(floatVec.data());
}

bool scratch::StringConverter::startsWith(const std::string &str, const std::string &pattern, bool lowerCase) {
    if (pattern.empty())
        return false;

    if (lowerCase) {
        return strnicmp(str.c_str(), pattern.c_str(), pattern.size()) == 0;
    }

    return strncmp(str.c_str(), pattern.c_str(), pattern.size()) == 0;
}

std::vector<std::string>
scratch::StringConverter::split(const std::string &str, const std::string &delims, unsigned int maxSplits,
                                bool preserveDelims) {
    std::vector<std::string> ret;
    // Pre-allocate some space for performance
    ret.reserve(maxSplits ? maxSplits + 1 : 10); // 10 is guessed capacity for most case

    unsigned int numSplits = 0;

    // Use STL methods
    size_t start, pos;
    start = 0;
    do {
        pos = str.find_first_of(delims, start);
        if (pos == start) {
            // Do nothing
            start = pos + 1;
        } else if (pos == std::string::npos || (maxSplits && numSplits == maxSplits)) {
            // Copy the rest of the string
            ret.push_back(str.substr(start));
            break;
        } else {
            // Copy up to delimiter
            ret.push_back(str.substr(start, pos - start));

            if (preserveDelims) {
                // Sometimes there could be more than one delimiter in a row.
                // Loop until we don't find any more delims
                size_t delimStart = pos, delimPos;
                delimPos = str.find_first_not_of(delims, delimStart);
                if (delimPos == std::string::npos) {
                    // Copy the rest of the string
                    ret.push_back(str.substr(delimStart));
                } else {
                    ret.push_back(str.substr(delimStart, delimPos - delimStart));
                }
            }

            start = pos + 1;
        }
        // parse up to next real data
        start = str.find_first_not_of(delims, start);
        ++numSplits;

    } while (pos != std::string::npos);

    return ret;
}