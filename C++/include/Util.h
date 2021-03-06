/*
 * Util.h
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov), Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Utility functions.
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include <limits>
#include <vector>

#if defined(_MSC_VER) /*********** MSC++ ****************/

#include <float.h>
#include "stdint.h" // you may need to download this header file to use with MS Visual Studio

typedef long long INT64FM;
static const double Pi  = 3.141592653589793;
#define M_PI Pi

const unsigned long lnan[2]={0xffffffff, 0x7fffffff};
#define NaN *( double* )lnan
#define PINFINITY std::numeric_limits<double>::infinity()
#define NINFINITY -std::numeric_limits<double>::infinity()
//#define MAXDOUBLE numeric_limits<double>::max()
#define MAXDOUBLE DBL_MAX
#define ISNAN _isnan
#define ISINF !_finite

// this is a "signaling NaN" that may throw a fp exception, but this is implementation dependent (and the flag can be set by other libraries).  In general, don't use this.
#define SIGNaN std::numeric_limits<double>::signaling_NaN()


#else /*********** GCC ****************/

#include <inttypes.h>
typedef int64_t INT64FM;

#define NaN std::numeric_limits<double>::quiet_NaN()
#define PINFINITY std::numeric_limits<double>::infinity()
#define NINFINITY -std::numeric_limits<double>::infinity()
#define MAXDOUBLE std::numeric_limits<double>::max()
#define ISNAN std::isnan
#define ISINF std::isinf

// this is a "signaling NaN" that may throw a fp exception, but this is implementation dependent (and the flag can be set by other libraries).  In general, don't use this.
#define SIGNaN std::numeric_limits<double>::signaling_NaN()

#endif /***************************/

#define ISFINITE(d) (!ISNAN(d) && !ISINF(d))
#define ISPINF(d) (ISINF(d) && d > 0)
#define ISNINF(d) (ISINF(d) && d < 0)

namespace larcfm {

#if defined(_MSC_VER)
static const INT64FM PRECISION13 = 16348;
static const INT64FM PRECISION5 = 1ULL << 40;
static const INT64FM PRECISION7 = 1ULL << 34;
static const INT64FM PRECISION9 = 1ULL << 27;
static const INT64FM PRECISION_DEFAULT = PRECISION13;
static const INT64FM HIGH_BIT = 0x8000000000000000ULL;
# else
static const double Pi  = M_PI;
static const INT64FM PRECISION13 = 16348;
static const INT64FM PRECISION5 = 1ULL << 40; // UINT64_C(1) << 40; //1LLU << 40;
static const INT64FM PRECISION7 = 1LLU << 34;
static const INT64FM PRECISION9 = 1LLU << 27;
static const INT64FM PRECISION_DEFAULT = PRECISION13;
static const INT64FM HIGH_BIT = 0x8000000000000000LLU;
#endif

// M_PI = 3.1415926535897931
// Java = 3.141592653589793

/** A broad collection of utilities */
class Util {
public:

	/**
	 * Determines if a &lt; b, without being almost equal, according to the
	 * definition of the almostEquals() method..
	 *
	 */
	static bool almost_less(double a, double b);

	/**
	 * Determines if a &lt; b, without being almost equal, according to the
	 * definition of the almostEquals() method..
	 *
	 */
	static bool almost_less(double a, double b, INT64FM maxUlps);

	/**
	 * Determines if a &gt; b, without being almost equal, according to the
	 * definition of the almostEquals() method.
	 *
	 */
	static bool almost_greater(double a, double b);

	/**
	 * Determines if a &gt; b, without being almost equal, according to the
	 * definition of the almostEquals() method.
	 *
	 */
	static bool almost_greater(double a, double b, INT64FM maxUlps);

	/**
	 * Determines if a is greater than or almost equal to b, according to the
	 * definition of the almostEquals() method.
	 *
	 */
	static bool almost_geq(double a, double b, INT64FM maxUlps);

	/**
	 * Determines if a is greater than or almost equal to b, according to the
	 * definition of the almostEquals() method.
	 *
	 */
	static bool almost_geq(double a, double b);

	/**
	 * Determines if a is less than or almost equal to b, according to the
	 * definition of the almostEquals() method.
	 *
	 */
	static bool almost_leq(double a, double b, INT64FM maxUlps);

	/**
	 * Determines if a is less than or almost equal to b, according to the
	 * definition of the almostEquals() method.
	 *
	 */
	static bool almost_leq(double a, double b);

	/**
	 * Determines if these two doubles, relative to each other, are almost
	 * equal. The "nearness" metric is captured in maxUlps.
	 * Mathematically, a == b is the same as a - b == 0.  Due to quirks
	 * in floating point, generally almostEquals(a, b) is not the same
	 * as almostEquals(a - b, 0).  The form without the subtraction is
	 * preferred.  <p>
	 *
	 * Consistent with the IEEE-754 floating point specification, "not a number"
	 * (NaN) won't compare as equal to anything (including itself or another
	 * NaN).
	 * <p>
	 *
	 * If two doubles are almost_equals() with a maxUlps parameter of 16348, then
	 * this means there can be at most 16347 floating point
	 * numbers between them. A value of 16348 for "units of least
	 * precision" (ulps) corresponds to a and b agreeing to about 13
	 * decimal places.  Said another way, the two numbers have an
	 * absolute difference of (approximately) 1e-13 if the two floating
	 * point numbers are near 1.  <p>
	 *
	 * The maxUlps parameter must be positive and smaller than 2^50
	 * <p>
	 *
	 * The implementation is based on the discussion (but not the code) in
	 * (google: comparing floats cygnus)
	 */
	static bool almost_equals(double a, double b, INT64FM maxUlps);

	/** Are these two numbers almost equal, given the PRECISION_DEFAULT */
	static bool almost_equals(double a, double b);


	/**
	 * Comparison of two values to determine if their absolute difference is within a value
	 * epsilon.  If the epsilon value is too small relative to the
	 * a and b values in question, then this is essentially the same as ==.  Epsilon must be positive.
	 */
	static bool within_epsilon(double a, double b, double epsilon);

	/**
	 * Returns true if the magnitude of a is less than epsilon. Epsilon must be positive.
	 */
	static bool within_epsilon(double a, double epsilon);


	/**
	 * Discretize the value of nvoz in the direction from voz in units of discreteUnits
	 * @param voz    The value nvoz was derived from
	 * @param nvoz   The value to be discretized
	 * @param discreteUnits     the size of discretization, e.g. 0.1, 1.0, 10.0, 100.0  etc
	 * @return       nvoz discretized to units if discreteUnits
	 */
	static double discretizeDir(double voz, double nvoz, double discreteUnits);

	/** Square */
	static double sq(const double x);
	/** return the absolute value */
	static INT64FM llabs(const INT64FM x);
	/** a safe (won't return NaN or throw exceptions) version of square root */
	static double sqrt_safe(const double x);
	/** a safe (won't return NaN or throw exceptions) version of arc-tangent */
	static double atan2_safe(const double y, const double x);
	/** a safe (won't return NaN or throw exceptions) version of arc-sine */
	static double asin_safe(double x);
	/** a safe (won't return NaN or throw exceptions) version of arc-cosine */
	static double acos_safe(double x);
	/** Discriminant of a quadratic */
	static double discr(const double a, const double b, const double c);
	/** Quadratic equation, eps = -1 or +1 */
	static double root(const double a, const double b, const double c, int eps);
	/** root2b(a,b,c,eps) = root(a,2*b,c,eps) , eps = -1 or +1*/
	static double root2b(const double a, const double b, const double c, const int eps);
	/** Returns +1 if the argument is positive or 0, -1 otherwise */
	static int    sign(const double x);

	/**
	 * Computes the modulo of val and mod. The returned value is in the range [0,mod)
	 */
	static double modulo(double val, double mod);

	/**
	 * Converts <code>rad</code> radians to the range
	 * (-<code>pi</code>, <code>pi</code>].
	 *
	 * @param rad Radians
	 *
	 * @return <code>rad</code> in the range
	 * [-<code>pi</code>, <code>pi</code>].
	 */
	static double to_pi(double rad);

	/**
	 * Converts <code>deg</code> degrees to the range
	 * [<code>0</code>, <code>360</code>).
	 *
	 * @param deg Degrees
	 *
	 * @return <code>deg</code> in the range [<code>0</code>, <code>360</code>).
	 */
	static double to_360(double deg);

	/**
	 * Converts <code>rad</code> radians to the range
	 * [<code>0</code>, <code>2*pi</code>].
	 *
	 * @param rad Radians
	 *
	 * @return <code>rad</code> in the range
	 * [-<code>0</code>, <code>2*pi</code>).
	 */
	static double to_2pi(double rad);

	/**
	 * Converts <code>rad</code> radians to the range
	 * [<code>-Math.PI/2</code>, <code>Math.PI/2</code>).
	 * This function is continuous, so to_pi2_cont(PI/2+eps) equals PI/2-eps.
	 *
	 * @param rad Radians
	 *
	 * @return <code>rad</code> in the range [<code>-Math.PI/2</code>, <code>Math.PI/2</code>).
	 */
	static double to_pi2_cont(double rad);

	/**
	 * Converts <code>deg</code> degrees to the range
	 * (<code>-180</code>, <code>180</code>].
	 *
	 * @param deg Degrees
	 *
	 * @return <code>deg</code> in the range (<code>-180</code>, <code>180</code>].
	 */
	static double to_180(double deg);

	/** Returns true if string s1 is less than or equal to string s2. */
	static bool less_or_equal(std::string s1, std::string s2);

	/**
	 * Returns true if a turn from track angle alpha to track angle beta is
	 * clockwise (by the shortest path).  If the two angles are equal, then
	 * this function returns true.
	 */
	static bool clockwise(double alpha, double beta);

	/**
	 * Returns 1 if the minimal turn to goalTrack (i.e. less than pi) is to the right, else -1
	 * @param initTrack   initial track [rad]
	 * @param goalTrack   target track [rad]
	 **/
	static int turnDir(double initTrack, double goalTrack);


	/**
	 * Returns the smallest angle between two track angles [0,PI].
	 */
	static double turnDelta(double alpha, double beta);


	/**
	 * Returns the smallest angle between two track angles [-PI,PI]. The sign indicates the direction of
	 * the turn, positive is clockwise, negative counterclockwise.
	 */
	static double signedTurnDelta(double alpha, double beta);


	/**
	 * Returns the angle between two tracks when turning in direction indicated by turnRight flag [0,2PI]
	 * Note: this function can return an angle larger than PI!
	 */
	static double turnDelta(double alpha, double beta, bool turnRight);

	static bool is_double(const std::string& str);

	static double parse_double(const std::string& str);

	/** @param degMinSec  Lat/Lon String of the form "46:55:00"  or "-111:57:00"
      @return   numbers of degrees in decimal form
	 */
	static double decimalDegrees(const std::string& degMinSec);

	/* Reads in a clock string and converts it to seconds.  If clock is true, then format is hh:mm:ss
	 * Otherwise, string must be a single decimal number
	 *
	 */
	static double parse_time(const std::string& s);

	/**
	 *
	 * @param t time in seconds
	 * @return String of hours:mins:secs
	 */
	static std::string hoursMinutesSeconds(double t);

	/**
	 * The behavior of the x%y operator is different between Java and C++ if either x or y is negative.  Use this to always return a value between 0 and y.
	 * @param x value
	 * @param y range
	 * @return x mod y, having the same sign as y (Java behavior)
	 */
	int mod(int x, int y);


};//---------------------------------------------



/** Convert arbitrary parameter to a string */
template <class T>
inline std::string to_string (const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

/** Remove element i from the array */
template <typename T>
void remove(T a[], int i, int size) {
	while (i < size - 1) {
		a[i] = a[i+1];
		i++;
	}
}


// deprecated functions:

/**
 * \deprecated {Use Util:: version.}
 * Square */
double sq(const double x);
/**
 * \deprecated {Use Util:: version.}
 * return the absolute value */
INT64FM llabs(const INT64FM x);
//  /** return the minimum of these two numbers */
//  double min(const double a, const double b);
//  /** return the maximum of these two numbers */
//  double max(const double a, const double b);
/**
 * \deprecated {Use Util:: version.}
 * a safe (won't return NaN or throw exceptions) of square root */
double sqrt_safe(const double x);
/**
 * \deprecated {Use Util:: version.}
 * a safe (won't return NaN or throw exceptions) of arc-tangent */
double atan2_safe(const double y, const double x);
/**
 * \deprecated {Use Util:: version.}
 * a safe (won't return NaN or throw exceptions) of arc-sine */
double asin_safe(double x);
/**
 * \deprecated {Use Util:: version.}
 * a safe (won't return NaN or throw exceptions) of arc-cosine */
double acos_safe(double x);
/**
 * \deprecated {Use Util:: version.}
 * Discriminant of a quadratic */
double discr(const double a, const double b, const double c);
/**
 * \deprecated {Use Util:: version.}
 * Quadratic equation */
double root(const double a, const double b, const double c, int eps);
/**
 * \deprecated {Use Util:: version.}
 * root2b(a,b,c,eps) = root(a,2*b,c,eps) */
double root2b(const double a, const double b, const double c, const int eps);
/**
 * \deprecated {Use Util:: version.}
 * Returns +1 if the arguement is positive or 0, -1 otherwise */
int    sign(const double x);

/**
 * \deprecated {Use Util:: version.}
 * Converts <code>rad</code> radians to the range
 * (-<code>pi</code>, <code>pi</code>].
 *
 * @param rad Radians
 *
 * @return <code>rad</code> in the range
 * [-<code>pi</code>, <code>pi</code>].
 */
double to_pi(double rad);

/**
 * \deprecated {Use Util:: version.}
 * Converts <code>deg</code> degrees to the range
 * [<code>0</code>, <code>360</code>).
 *
 * @param deg Degrees
 *
 * @return <code>deg</code> in the range [<code>0</code>, <code>360</code>).
 */
double to_360(double deg);

/**
 * \deprecated {Use Util:: version.}
 * Converts <code>rad</code> radians to the range
 * [<code>0</code>, <code>2*pi</code>].
 *
 * @param rad Radians
 *
 * @return <code>rad</code> in the range
 * [-<code>0</code>, <code>2*pi</code>).
 */
double to_2pi(double rad);

/**
 * \deprecated {Use Util:: version.}
 * Converts <code>rad</code> radians to the range
 * [<code>-Math.PI/2</code>, <code>Math.PI/2</code>).
 * This function is continuous, so to_pi2_cont(PI/2+eps) equals PI/2-eps.
 *
 * @param rad Radians
 *
 * @return <code>rad</code> in the range [<code>-Math.PI/2</code>, <code>Math.PI/2</code>).
 */
double to_pi2_cont(double rad);

/**
 * \deprecated {Use Util:: version.}
 * Converts <code>deg</code> degrees to the range
 * (<code>-180</code>, <code>180</code>].
 *
 * @param deg Degrees
 *
 * @return <code>deg</code> in the range (<code>-180</code>, <code>180</code>].
 */
double to_180(double deg);

}

#endif /* UTIL_H_ */
