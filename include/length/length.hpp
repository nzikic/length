/*********************************************************************************
 *
 *  MIT License
 *
 *  Copyright (c) 2017 Nenad Zikic
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 *********************************************************************************/

#pragma once

#include <type_traits>
#include <ratio>


namespace length
{

    template <typename Ratio>
    struct length_unit
    {
            using ratio = Ratio;
    };

    struct metre      : length_unit<std::ratio<1>> {};
    struct centimetre : length_unit<std::centi> {};
    struct millimetre : length_unit<std::milli> {};
    struct inch       : length_unit<std::ratio_multiply<std::ratio<254, 100>, centimetre::ratio>> {};
    struct foot       : length_unit<std::ratio_multiply<std::ratio<12, 1>, inch::ratio>> {};

    template <typename Unit>
    class Length
    {
            static_assert (std::disjunction_v<
                           std::is_same<metre, Unit>,
                           std::is_same<centimetre, Unit>,
                           std::is_same<millimetre, Unit>,
                           std::is_same<inch, Unit>,
                           std::is_same<foot, Unit>>
                           , "Lenght `Unit` template parameter must be of type length_unit");

            double m_value;

        public:
            using unit = Unit;
            Unit base_unit = Unit{};

            constexpr explicit Length(double val = 0.0) : m_value{val} {}

            /** Value of length measured in current units **/
            [[nodiscard]] constexpr double value() const { return m_value; }

    };

    /** Conversts `Length` of `FromUnit` to `Length` of `ToUnit`
     *
     * @param Length<FromUnit> from - lenght of unit `FromUnit` being converted
     * @return - Length<ToUnit> - result of cenversion in units `ToUnit`
     *
     * @example usage
     *          1. Length<meter> lenM = convert<centimetre, metre>(25_cm); // returns 0.25_m
     *          2. Length<foot> lenFt = convert<inch, foot>(24_in); // results in 2_ft
     */
    template <typename FromUnit, typename ToUnit, template<typename> typename Length>
    [[nodiscard]] constexpr Length<ToUnit> convert(const Length<FromUnit>& from)
    {
        using r = std::ratio_divide<typename FromUnit::ratio, typename ToUnit::ratio>;
        return Length<ToUnit>{from.value() * static_cast<double>(r::ratio::num) /
                                             static_cast<double>(r::ratio::den)};
    }

    /** Compares lhs `Length` in units `Unit1` to rsh `Length` in units `Unit2`
     *  by converting rhs to Unit1 first and then comparing values.
     *
     * @param lhs - lenght of unit `Unit1` being converted
     * @param rhs - lenght of unit `Unit2` being converted
     * @return true if
     *
     * @example usage
     *          1. Length<meter> lenM = convert<centimetre, metre>(25_cm); // returns 0.25_m
     *          2. Length<foot> lenFt = convert<inch, foot>(24_in); // results in 2_ft
     */
    template <typename Unit1, typename Unit2, template<typename> typename Length>
    [[nodiscard]] constexpr bool operator==(const Length<Unit1>& lhs, const Length<Unit2>& rhs)
    {
        if constexpr (std::is_same_v<Unit1, Unit2>)
        {
            return lhs.value() == rhs.value();
        }
        return lhs.value() == convert<Unit2, Unit1>(rhs).value();
    }

    // addition

    template<typename Unit1, typename Unit2, template<typename> typename Length>
    [[nodiscard]] constexpr Length<Unit1> operator+ (Length<Unit1>&& lhs, Length<Unit2>&& rhs) { return Length<Unit1> { lhs.value() + convert<Unit2, Unit1>(rhs).value() }; }

    template<typename Unit> // spacialization for same units
    [[nodiscard]] constexpr Length<Unit> operator+ (Length<Unit>&& lhs, Length<Unit>&& rhs) { return Length<Unit> { lhs.value() + rhs.value() }; }

    // substraction

    template<typename Units1, typename Units2, template<typename> typename Length>
    [[nodiscard]] constexpr Length<Units1> operator- (Length<Units1>&& lhs, Length<Units2>&& rhs) { return Length<Units1> { lhs.value() - convert<Units2, Units1>(rhs).value() }; }

    template<typename Units> // spacialization for same units
    [[nodiscard]] constexpr Length<Units> operator- (Length<Units>&& lhs, Length<Units>&& rhs) { return Length<Units> { lhs.value() - rhs.value() }; }

    // multiplication

    template <typename Units, template<typename> typename Length>
    [[nodiscard]] constexpr Length<Units> operator* (double k, Length<Units>&& length) { return Length<Units> { k * length.value()}; }

    template <typename Units, template<typename> typename Length>
    [[nodiscard]] constexpr Length<Units> operator* (Length<Units>&& length, double k) { return Length<Units> { k * length.value()}; }

    // division

    template <typename Units, template<typename> typename Length>
    [[nodiscard]] constexpr Length<Units> operator/ (Length<Units> length, double k) { return Length<Units> { length.value() / k}; }

    template<typename Units1, typename Units2, template<typename> typename Length>
    [[nodiscard]] constexpr double operator/ (Length<Units1>&& lhs, Length<Units2>&& rhs) { return lhs.value() / convert<Units2, Units1>(rhs).value(); }

    //////////////////////////////////

    inline
    namespace literals {
        constexpr auto operator"" _m  (long double m)  { return Length<metre>{static_cast<double>(m)}; }
        constexpr auto operator"" _m  (unsigned long long m)  { return Length<metre>{static_cast<double>(m)}; }

        constexpr auto operator"" _cm (unsigned long long cm) { return Length<centimetre>{static_cast<double>(cm)}; }
        constexpr auto operator"" _cm (long double cm) { return Length<centimetre>{static_cast<double>(cm)}; }

        constexpr auto operator"" _mm (long double mm) { return Length<millimetre>{static_cast<double>(mm)}; }
        constexpr auto operator"" _mm (unsigned long long mm) { return Length<millimetre>{static_cast<double>(mm)}; }

        constexpr auto operator"" _in (unsigned long long in) { return Length<inch>{static_cast<double>(in)}; }
        constexpr auto operator"" _in (long double in) { return Length<inch>{static_cast<double>(in)}; }

        constexpr auto operator"" _ft (long double ft) { return Length<foot>{static_cast<double>(ft)}; }
        constexpr auto operator"" _ft (unsigned long long ft) { return Length<foot>{static_cast<double>(ft)}; }
    }

    //////////////////
    // test
    //////////////////

    // literals
    static_assert (1_m == 100_cm);
    static_assert (1_m == 100_cm);
    // static_assert (1_m == 200_cm);
    static_assert (12_in == 1_ft);
    static_assert (1_ft == 12_in);
    static_assert (250_cm == 2.5_m);

    // direct object instantiation
    static_assert (Length<metre>{2.5} == Length<centimetre>{250});
    static_assert (Length<foot>{3} == Length<inch>{36_in});
    static_assert (Length<millimetre>{2} == Length<centimetre>{0.2});
    static_assert (2_m == convert<centimetre, millimetre>(200_cm));
    static_assert (Length<millimetre>{2} == convert<centimetre, metre>(Length<centimetre>{0.2}));
    static_assert (1_m == Length<inch>{1 / 0.0254});

    // operations
    static_assert ( 4 * 1_m == 50_cm * 8.0);
    static_assert ( 2_mm * 10 == 0.5 * 4_cm);
    static_assert ( 2_mm * 10 == 4_cm / 2);
    static_assert ( 200_mm + 3_m / 6 == 2 * 10_cm + 50_cm / 1.0);
    static_assert ( 2_m / 2 == 1_in / 0.0254);
    static_assert ( ((2* 5_m + 10_m) / 200_cm * 1_m).value() == 10.0);
}


