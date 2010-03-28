// Copyright (C) 2006-2008 David Sugar, Tycho Softworks.
//
// This file is part of GNU ucommon.
//
// GNU ucommon is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GNU ucommon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GNU ucommon.  If not, see <http://www.gnu.org/licenses/>.

/**
 * Basic classes for manipulating time and date based data, particularly
 * that may be in strings.
 * @file ucommon/datetime.h
 */

#ifndef _UCOMMON_DATETIME_H_
#define	_UCOMMON_DATETIME_H_

#ifndef _UCOMMON_CONFIG_H_
#include <ucommon/platform.h>
#endif

#ifndef _UCOMMON_NUMBERS_H_
#include <ucommon/numbers.h>
#endif

#ifndef	_UCOMMON_STRING_H_
#include <ucommon/string.h>
#endif

NAMESPACE_UCOMMON

#ifdef __BORLANDC__
	using std::tm;
	using std::time_t;
#endif

/**
 * The Date class uses a julian date representation of the current
 * year, month, and day.  This is then manipulated in several forms
 * and may be exported as needed.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short julian number based date class.
 */
class __EXPORT Date
{
protected:
	long julian;

	void toJulian(long year, long month, long day);
	void fromJulian(char *buf) const;

	/**
	 * A method to use to "post" any changed values when shadowing
	 * a mixed object class.  This is used by DateNumber.
	 */
	virtual void update(void);

public:
	/**
	 * Create a julian date from a time_t type.
	 * @param value from time()
	 */
	Date(time_t value);

	/**
	 * Create a julian date from a local or gmt date and time.
	 * @param object from DateTime::glt() or gmt().
	 */
	Date(struct tm *object);

	/**
	 * Create a julian date from a ISO date string of a specified size.
	 * @param pointer to ISO date string.
	 * @param size of date field if not null terminated.
	 */
	Date(const char *pointer, size_t size = 0);

	/**
	 * Create a julian date from an arbitrary year, month, and day.
	 * @param year of date.
	 * @param month of date (1-12).
	 * @param day of month (1-31).
	 */
	Date(int year, unsigned month = 1, unsigned day = 1);

	/**
	 * Create a julian date object from another object.
	 * @param object to copy.
	 */
	Date(const Date& object);

	/**
	 * Construct a new julian date with today's date.
	 */
	Date();

	/**
	 * Destroy julian date object.
	 */
	virtual ~Date();

	/**
	 * Get the year of the date.
	 * @return year of the date
	 */
	int getYear(void) const;

	/**
	 * Get the month of the date (1-12).
	 * @return month of year
	 */
	unsigned getMonth(void) const;

	/**
	 * Get the day of the month of the date.
	 * @return day of month
	 */
	unsigned getDay(void) const;

	/**
	 * Get the day of the week (0-7).
	 * @return day of week
	 */
	unsigned getDayOfWeek(void) const;

	/**
	 * Get a ISO string representation of the date (yyyy-mm-dd).
	 * @param buffer to store string.
	 * @return string representation.
	 */
	char *get(char *buffer) const;

	/**
	 * Get a time_t for the julian date if in time_t epoch.
	 * @return time_t or -1 if out of range.
	 */
	time_t getTime(void) const;
	
	/**
	 * Get the julian number for the object.
	 * @return julian number.
	 */
	long get(void) const;

	/**
	 * Set the julian date based on an ISO date string of specified size.
	 * @param pointer to date string field.
	 * @param size of field if not null terminated.
	 */
	void set(const char *pointer, size_t size = 0);

	/**
	 * Check if date is valid.
	 * @return true if julian date is valid.
	 */
	bool isValid(void) const;

	/**
	 * Casting operator to return julian number.
	 * @return julian number.
	 */
	inline operator long() const
		{return get();};

	/**
	 * Expression operator to return an ISO date string for the current
	 * julian date.
	 * @return ISO date string.
	 */
	String operator()() const;

	/**
	 * Increment date by one day.
	 * @return instance of current date object.
	 */
	Date& operator++();

	/**
	 * Decrement date by one day.
	 * @return instance of current date object.
	 */
	Date& operator--();

	/**
	 * Increment date by offset.
	 * @param offset to add to julian date.
	 * @return instance of current date object.
	 */
	Date& operator+=(long offset);

	/**
	 * Decrement date by offset.
	 * @param offset to subtract from julian date.
	 * @return instance of current date object.
	 */
	Date& operator-=(long offset);
	
	/**
	 * Add days to julian date in an expression.
	 * @param days to add.
	 * @return new date object with modified days.
	 */
	Date operator+(long days);

	/**
	 * Subtract days from a julian date in an expression.
	 * @param days to subtract.
	 * @return new date object with modified days.
	 */
	Date operator-(long days);

	/**
	 * Operator to compute number of days between two dates.
	 * @param date offset for computation.
	 * @return number of days difference.
	 */
	inline long operator-(const Date &date)
		{return (julian - date.julian);};

	/**
	 * Assign date from another date object.
	 * @param date object to assign from.
	 * @return current modified date object.
	 */
	Date& operator=(const Date& date);

	/**
	 * Compare julian dates if same date.
	 * @param date to compare with.
	 * @return true if same.
	 */
	bool operator==(const Date& date);

	/**
	 * Compare julian dates if not same date.
	 * @param date to compare with.
	 * @return true if not same.
	 */
	bool operator!=(const Date& date);

	/**
	 * Compare julian date if earlier than another date.
	 * @param date to compare with.
	 * @return true if earlier.
	 */
	bool operator<(const Date& date);

	/**
	 * Compare julian date if earlier than or equal to another date.
	 * @param date to compare with.
	 * @return true if earlier or same.
	 */
	bool operator<=(const Date& date);

	/**
	 * Compare julian date if later than another date.
	 * @param date to compare with.
	 * @return true if later.
	 */
	bool operator>(const Date& date);

	/**
	 * Compare julian date if later than or equal to another date.
	 * @param date to compare with.
	 * @return true if later or same.
	 */
	bool operator>=(const Date& date);

	/**
	 * Check if julian date is not valid.
	 * @return true if date is invalid.
	 */
	inline bool operator!() const
		{return !isValid();};

	/**
	 * Check if julian date is valid for is() expression.
	 * @return true if date is valid.
	 */
	inline operator bool() const
		{return isValid();};
};

/**
 * The Time class uses a integer representation of the current
 * time.  This is then manipulated in several forms and may be
 * exported as needed.  The time object can represent an instance in
 * time (hours, minutes, and seconds) in a 24 hour period or can
 * represent a duration.  Millisecond accuracy can be offered.
 *
 * @author Marcelo Dalmas <mad@brasmap.com.br> and David Sugar <dyfet@gnutelephony.org>
 * @short Integer based time class.
 */

class __EXPORT Time
{
protected:
	long seconds;

protected:
	void toSeconds(int hour, int minute = 0, int second = 0);
	void fromSeconds(char *buf) const;
	virtual void update(void);

public:
	/**
	 * Create a time from the time portion of a time_t.
	 * @param value of time_t to use.
	 */
	Time(time_t value);

	/**
	 * Create a time from the time portion of a date and time object.
	 * @param object from DateTime::glt() or gmt().
	 */
	Time(struct tm *object);

	/**
	 * Create a time from a hh:mm:ss formatted time string.
	 * @param pointer to formatted time field.
	 * @param size of field if not null terminated.
	 */
	Time(char *pointer, size_t size = 0);

	/**
	 * Create a time from hours (0-23), minutes (0-59), and seconds (0-59).
	 * @param hour of time.	
	 * @param minute of time.
	 * @param second of time.
	 */
	Time(int hour, int minute, int second);

	/**
	 * Create a time object from another object.
	 * @param object to copy.
	 */
	Time(const Time& object);

	/**
	 * Create a time from current time.
	 */
	Time();

	/**
	 * Destroy time object.
	 */
	virtual ~Time();

	/**
	 * Get current time in seconds from midnight.
	 * @return seconds from midnight.
	 */
	long get(void) const;
	
	/**
	 * Get hours from midnight.
	 * @return hours from midnight.
	 */
	int getHour(void) const;

	/**
	 * Get minutes from current hour.
	 * @return minutes from current hour.
	 */
	int getMinute(void) const;

	/**
	 * Get seconds from current minute.
	 * @return seconds from current minute.
	 */
	int getSecond(void) const;

	/**
	 * Get a hh:mm:ss formatted string for current time.
	 * @param buffer to store time string in.
	 * @return time string buffer or NULL if invalid.
	 */
	char *get(char *buffer) const;

	/**
	 * Set time from a hh:mm:ss formatted string.
	 * @param pointer to time field.
	 * @param size of field if not null terminated.
	 */
	void set(char *pointer, size_t size = 0);

	/**
	 * Check if time object had valid value.
	 * @return true if object is valid.
	 */
	bool isValid(void) const;

	/**
	 * Check if time object has valid value for is() operator.
	 * @return true if object is valid.
	 */
	inline operator bool() const
		{return isValid();};

	/**
	 * Check if time object has valid value for ! operator.
	 * @return true if object is not valid.
	 */
	inline bool operator!() const
		{return !isValid();};

	/**
	 * Get difference (in seconds) between two times.
	 * @param reference time to get difference from.
	 * @return difference in seconds.
	 */
	long operator-(const Time &reference);

	/**
	 * Add seconds to the current time, wrap if 24 hours.
     * @param seconds to add.
	 * @return new time object with modified value.
	 */
	Time operator+(long seconds);

	/**
	 * Subtract seconds to the current time, wrap if 24 hours.
	 * @param seconds to subtract.
	 * @return new time object with modified value.
	 */
	Time operator-(long seconds);

	/**
	 * Get time in seconds.
	 * @return seconds.
	 */
	inline operator long()
		{return get();};
	
	/**
	 * Convert to standard 24 hour time string.
	 * @return time string.
	 */
	String operator()() const;

	/**
	 * Incrememnt time by 1 second, wrap on 24 hour period.
	 * @return modified instance of current time object.
	 */
	Time& operator++();

	/**
	 * Decrement time by 1 second, wrap on 24 hour period.
	 * @return modified instance of current time object.
	 */
	Time& operator--();

	/**
	 * Assign a time as a copy of another time.
	 * @param time to assign from.
	 * @return time object that was assigned.
	 */
	Time& operator=(const Time& time);

	/**
	 * Increment time by specified seconds.  Wraps on 24 hour period.
	 * @param seconds to add to current time.
	 * @return modified instance of current time object.
	 */
	Time& operator+=(long seconds);

	/**
	 * Decrement time by specified seconds.  Wraps on 24 hour period.
	 * @param seconds to subtract from current time.
	 * @return modified instance of current time object.
	 */
	Time& operator-=(long seconds);

	/**
	 * Compare time with another time to see if same time.
	 * @param time to compare with.
	 * @return true if same time.
	 */
	bool operator==(const Time &time);

	/**
	 * Compare time with another time to see if not same time.
	 * @param time to compare with.
	 * @return true if not same time.
	 */
	bool operator!=(const Time &time);

	/**
	 * Compare time if earlier than another time.
	 * @param time object to compare with.
	 * @return true if earlier than object. 
	 */
	bool operator<(const Time &time);

	/**
	 * Compare time if earlier than or equal to another time.
	 * @param time object to compare with.
	 * @return true if earlier or same as object.
	 */
	bool operator<=(const Time &time);

	/**
	 * Compare time if later than another time.
	 * @param time object to compare with.
	 * @return true if later than object.
	 */
	bool operator>(const Time &time);

	/**
	 * Compare time if later than or equal to another time.
	 * @param time object to compare with.
	 * @return true if later than or same as object.
	 */
	bool operator>=(const Time &time);
};

/**
 * The Datetime class uses a julian date representation of the current
 * year, month, and day and a integer representation of the current
 * time.  This is then manipulated in several forms
 * and may be exported as needed.
 *
 * @author Marcelo Dalmas <mad@brasmap.com.br>
 * @short Integer based time class.
 */

class __EXPORT DateTime : public Date, public Time
{
protected:
	void update(void);

public:
	/**
	 * Constant for number of seconds in a day.
	 */
	static const long c_day;

	/**
	 * Constant for number of seconds in a hour.
	 */
	static const long c_hour;

	/**
	 * Constant for number of seconds in a week.
	 */
	static const long c_week;

	/**
	 * Construct a date and time from C libraray time_t type.		
	 * @param time type to make date and time from.
	 */
  	DateTime(time_t time);

	/**
	 * Construct a date and time from C library time structure.
	 * @param tm structure from C library (from glt or gmt).
	 */
	DateTime(struct tm *tm);

	/**
	 * Construct a date and time from ISO string buffer.
	 * @param pointer to string field holding date and time.
	 * @param size of field if not null terminated string.
	 */
	DateTime(const char *pointer, size_t size = 0);

	/**
	 * Construct a date and time object from explicit date and time values.
	 * @param year of object.
	 * @param month of object (1-12).
	 * @param day of month of object (1-31).
	 * @param hour of object (0-23).
	 * @param minute of object (0-59).
	 * @param second of object (0-59).
	 */
	DateTime(int year, unsigned month = 1, unsigned day = 1,
		 int hour = 0, int minute = 0, int second = 0);

	/**
	 * Create a datetime object from another object.
	 * @param object to copy.
	 */
	DateTime(const DateTime& object);

	/**
	 * Construct a new date and time object with current date and time.
	 */
	DateTime();

	/**
	 * Destroy date and time object.
	 */
	virtual ~DateTime();

	/**
	 * Get a ISO formatted date and time string for current object.
	 * @param buffer to store date and time in (yyyy-mm-dd hh:mm:ss).
	 * @return string buffer if object is valid.
	 */
	char *get(char *buffer) const;

	/**
	 * Get C library time_t type if object in C library epoch range.
	 * @return time in seconds from epoch or ~0l if out of range.
	 */
	time_t get(void) const;

	/**
	 * Test if object is valid.
	 * @return true if object is valid.
	 */
	bool isValid(void) const;

	/**
	 * Operator to compute number of days between two dates.
	 * @param datetime to offset from for computation.
	 * @return number of days difference.
	 */
	long operator-(const DateTime &datetime);

	/**
	 * Assign date and time from another datetime object.
	 * @param datetime object to assign from.
	 * @return assigned datetime object.
	 */
	DateTime& operator=(const DateTime& datetime);

	/**
	 * Add seconds to the current datetime object.  Day overflows update
	 * julian date.
	 * @param seconds to add to object.
	 * @return modified datetime object.
	 */
	DateTime& operator+=(long seconds);

	/**
	 * Subtract seconds from current datetime object.  Day underflows
	 * update julian date.
	 * @param seconds to subtract from object.
	 * @return modified datetime object.
	 */
	DateTime& operator-=(long seconds);

	/**
	 * Add seconds to datetime in an expression.  Day overflows update
	 * julian date.
	 * @param seconds to add to datetime.
	 * @return new modified datetime object.
	 */
	DateTime operator+(long seconds);

	/**
	 * Subtract seconds from datetime in an expression.  Day underflows
	 * update julian date.
	 * @param seconds to subtract from datetime.
	 * @return new modified datetime object.
	 */
	DateTime operator-(long seconds);

	/**
	 * Add a day from the current date and time.
	 * @return datetime object reference that was modified.
	 */
	DateTime& operator++();
	
	/**
	 * Subtract a day from the current date and time.
	 * @return datetime object reference that was modified.
	 */
	DateTime& operator--();

	/**
	 * Compare date and time with another date and time to see if the same.
	 * @param datetime to compare with.
	 * @return true if equal.
	 */
	bool operator==(const DateTime& datetime);

	/**
	 * Compare date and time with another date and time to see if not same.
	 * @param datetime to compare with.
	 * @return true if not equal.
	 */
	bool operator!=(const DateTime& datetime);

	/**
	 * Compare date and time with another date and time to see if earlier.
	 * @param datetime to compare with.
	 * @return true if earlier.
	 */
	bool operator<(const DateTime& datetime);

	/**
	 * Compare date and time with another date and time to see if earlier or
	 * the same.
	 * @param datetime to compare with.
	 * @return true if earlier or equal.
	 */
	bool operator<=(const DateTime& datetime);

	/**
	 * Compare date and time with another date and time to see if later.
	 * @param datetime to compare with.
	 * @return true if later.
	 */
	bool operator>(const DateTime& datetime);

	/**
	 * Compare date and time with another date and time to see if later or
	 * the same.
	 * @param datetime to compare with.
	 * @return true if later or equal.
	 */
	bool operator>=(const DateTime& datetime);

	/**
	 * Check if date and time is not valid.
	 * @return true if not valid.
	 */
	bool operator!() const;

	/**
	 * Test is date and time is valid for is() operator.
	 * @return true if object is valid.
	 */
	operator bool() const;

	/**
	 * Return date and time formatted using strftime format values.
	 * @param strftime format to use.
	 * @return String object with formatted time.
	 */
	String format(const char *strftime) const;

	/**
	 * Fetch an instance of time converted to local time.  If the localtime
	 * abi is not re-entrant, than a lock is held, otherwise a unique
	 * object is returned.  In either case, when you are done, you must
	 * release the object.
	 * @param time object or NULL if using current time.
	 * @return locked instance of struct tm object.
	 */
	static struct tm *glt(time_t *time = NULL);

	/**
	 * Fetch an instance of time converted to gmt.  If the gmtime abi
	 * is not re-entrant, than a lock is held, otherwise a unique
	 * object is returned.  In either case, when you are done, you must
	 * release the object.
	 * @param time object or NULL if using current time.
	 * @return locked instance of struct tm object.
	 */
	static struct tm *gmt(time_t *time = NULL);

	/**
	 * Release a struct tm object from glt or gmt.
	 * @param object to release.
	 */
	static void release(struct tm *object);
};

/**
 * A number class that manipulates a string buffer that is also a date.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short a number that is also a date string.
 */
class __EXPORT DateNumber : public Number, public Date
{
protected:
	void update(void)
		{fromJulian(buffer);};

public:
	/**
	 * Create a date number tied to a refreshed string buffer.
	 * @param pointer to string buffer to rewrite.
	 */
	DateNumber(char *pointer);

	/**
	 * Release a datenumber object.
	 */
	virtual ~DateNumber();
};

/**
 * Convenience type for using DateTime object.
 */
typedef	DateTime	datetime_t;

/**
 * Convenience type for using Date object.
 */
typedef	Date		date_t;

/**
 * Convenience type for struct tm.
 */
typedef	struct tm	tm_t;

END_NAMESPACE

#endif