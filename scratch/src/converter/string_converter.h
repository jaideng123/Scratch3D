#pragma once

#include <string>
#include <sstream>
#include <ios>
#include <vector>

#include <glm/glm.hpp>

#ifdef __MINGW32__
#define _strtoull_l _strtoul_l
#define _strtoll_l _strtol_l
#define newlocale(cat, loc, base) 0
#define locale_t _locale_t
#endif

#ifdef _MSC_VER
#define LC_NUMERIC_MASK LC_NUMERIC
#define newlocale(cat, loc, base) _create_locale(cat, loc)
#define locale_t _locale_t
#endif

namespace scratch {
    class StringConverter {
    public:
        static std::string toString(int val) { return std::to_string(val); };

        /** Converts a float to a String. */
        static std::string toString(float val, unsigned short precision = 6,
                                    unsigned short width = 0, char fill = ' ',
                                    std::ios::fmtflags flags = std::ios::fmtflags(0));

        /** Converts a boolean to a String.
        @param val
        @param yesNo If set to true, result is 'yes' or 'no' instead of 'true' or 'false'
        */
        static std::string toString(bool val, bool yesNo = false);

        /** Converts a Vector2 to a String.
        @remarks
            Format is "x y" (i.e. 2x Real values, space delimited)
        */
        static std::string toString(const glm::vec2 &val);

        /** Converts a Vector3 to a String.
        @remarks
            Format is "x y z" (i.e. 3x Real values, space delimited)
        */
        static std::string toString(const glm::vec3 &val);

        /** Converts a Vector4 to a String.
        @remarks
            Format is "x y z w" (i.e. 4x Real values, space delimited)
        */
        static std::string toString(const glm::vec4 &val);

        /** Converts a Matrix3 to a String.
        @remarks
            Format is "00 01 02 10 11 12 20 21 22" where '01' means row 0 column 1 etc.
        */
        static std::string toString(const glm::mat3 &val);

        /** Converts a Matrix4 to a String.
        @remarks
            Format is "00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33" where 
            '01' means row 0 column 1 etc.
        */
        static std::string toString(const glm::mat4 &val);

        static glm::mat4 parsemat4(const std::string &str);

        static glm::mat3 parsemat3(const std::string &str);

        static glm::vec4 parsevec4(const std::string &str);

        static glm::vec3 parsevec3(const std::string &str);

        static glm::vec2 parsevec2(const std::string &str);

        static int parseint(const std::string &str);

        static bool parsebool(const std::string &str);

        static float parsefloat(const std::string &str);

        static locale_t numLocale;

        static bool startsWith(const std::string &str, const std::string &pattern, bool lowerCase = true);

        static std::vector<std::string>
        split(const std::string &str, const std::string &delims = "\t\n ", unsigned int maxSplits = 0,
              bool preserveDelims = false);
    };
} // namespace scratch