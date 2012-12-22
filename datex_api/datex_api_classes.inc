<?php

define('DEFAULT_DATEXOBJECT_POLICY', TRUE);
define('DATEX_USE_INTL', FALSE);

/**
 * Date tools for Jalali Dates.
 */
class DatexFormatter {

  /**
   * Similar to php date_format
   *
   * @param mixed $date @see DatexFormatter::get_date_object().
   * @param string $format date format
   * @param boolean $use_intl wheter to use php-intl or not, recomended.
   * @param array &$errors array to put errors in.
   * @return string formatted date.
   */
  public static function format($date, $format, $use_intl = DATEX_USE_INTL, &$error_code, &$error_message) {
    if (self::hasINTL() && $use_intl) {
      return self::formatINTL($date, $format, $error_code, $error_message);
    }
    else {
      return self::formatPHP($date, $format);
    }
  } 

  /**
   * Returns array containing names of days and monthes in persian.
   *
   * @return array
   */
  public static function persianDateNames() {
    static $names = NULL;
    if (!$names) {
      $names = array(
        'months' => array(
          1 => 'فروردین', //Frvrdin
          'اردیبهشت',
          'خرداد', //Khrdad
          'تیر', //Tir
          'مرداد', //Mrdad
          'شهریور', //Shahrivr
          'مهر', //Mhr
          'آبان', //Aban
          'آذر', //Azr
          'دی', //Dey
          'بهمن', //Bahmn
          'اسفند' // Esfnd
        ),
        'ampm' => array(
          'am' => 'ق.ظ',
          'pm' => 'ب.ظ',
        ),
        'day_abbr' => array(
          1 => 'ش.', // shanbe as sh.
          'ی.', // ykshnbe as y.
          'د.', // dshnbe as d.
          'س.', // seshanbe as s.
          'چ.', // chehar as ch.
          'پ.', // panj as p.
          'ج.', // jom'e as j.
        ),
        'day' => array(
          1 => 'شنبه', //shanbe
          'یک‌شنبه', //yeksh
          'دوشنبه', //doshnbe
          'سه‌شنبه', //seshnbe
          'چهارشنبه', //chehar
          'پنج‌شنبه', //pnj
          'جمعه', // jome
        ),
        'tz' => 'تهران',
      );
    }

    return $names;
  }

  /**
   * Converts a Gregorian date to Jalali.
   *
   * @param int $gregorianYear
   * @param int $gregorianMonth
   * @param int $gregorianDay
   * @return array
   */
  public static function toJalali($gregorianYear = NULL, $gregorianMonth = NULL, $gregorianDay = NULL) {
    $now = getdate();
    if (!$gregorianMonth) {
      $gregorianMonth = $now['mon'];
    }
    if (!$gregorianYear) {
      $gregorianYear = $now['year'];
    }
    if (!$gregorianDay) {
      $gregorianDay = $now['mday'];
    }

    $numDays_in_gregorianMonth = array(31, 28, 31, 30, 31, 30,
      31, 31, 30, 31, 30, 31);
    $numDays_in_jalaliMonth = array(31, 31, 31, 31, 31, 31,
      30, 30, 30, 30, 30, 29);

    $gYear = $gregorianYear - 1600;
    $gMonth = $gregorianMonth - 1;
    $gDay = $gregorianDay - 1;

    $gregorianDay_no = 365 * $gYear
        + intval(($gYear + 3) / 4)
        - intval(($gYear + 99) / 100)
        + intval(($gYear + 399) / 400);
    for ($i = 0; $i < $gMonth; ++$i) {
      $gregorianDay_no += $numDays_in_gregorianMonth[$i];
    }
    if ($gMonth > 1 && ( ($gYear % 4 == 0 && $gYear % 100 != 0)
        || ($gYear % 400 == 0))
    ) {
      // leap and after Feb
      $gregorianDay_no++;
    }
    $gregorianDay_no += $gDay;

    $jalaliDay_no = $gregorianDay_no - 79;
    $j_np = intval($jalaliDay_no / 12053);
    $jalaliDay_no = $jalaliDay_no % 12053;

    $jYear = 979 + 33 * $j_np + 4 * intval($jalaliDay_no / 1461);
    $jalaliDay_no %= 1461;

    if ($jalaliDay_no >= 366) {
      $jYear += intval(($jalaliDay_no - 1) / 365);
      $jalaliDay_no = ( $jalaliDay_no - 1 ) % 365;
    }

    for ($i = 0; $i < 11 && $jalaliDay_no >= $numDays_in_jalaliMonth[$i]; ++$i) {
      $jalaliDay_no -= $numDays_in_jalaliMonth[$i];
    }

    $jMonth = $i + 1;
    $jDay = $jalaliDay_no + 1;
    return array('day' => $jDay, 'month' => $jMonth, 'year' => $jYear);
  }

  /**
   * Converts a Gregorian date to Jalali.
   *
   * @param int $jalaliYear
   * @param int $jalaliMonth
   * @param int $jalaliDay
   * @return array
   */
  public static function toGregorian($jalaliYear = NULL, $jalaliMonth = NULL, $jalaliDay = NULL) {
    $now = self::toJalali();
    if (!$jalaliDay) {
      $jalaliDay = $now['day'];
    }
    if (!$jalaliMonth) {
      $jalaliMonth = $now['month'];
    }
    if (!$jalaliYear) {
      $jalaliYear = $now['year'];
    }

    $gregorianDays_inMonth = array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
    $jalaliDays_inMonth = array(31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29);

    $jYear = $jalaliYear - 979;
    $jMonth = $jalaliMonth - 1;
    $jDay = $jalaliDay - 1;

    $jalaliDayNo = 365 * $jYear + intval($jYear / 33) * 8 + intval((($jYear % 33) + 3) / 4);
    for ($i = 0; $i < $jMonth; ++$i) {
      $jalaliDayNo += $jalaliDays_inMonth[$i];
    }
    $jalaliDayNo += $jDay;
    $gregorianDayNo = $jalaliDayNo + 79;

    $gYear = 1600 + 400 * intval($gregorianDayNo / 146097);
    $gregorianDayNo = $gregorianDayNo % 146097;

    $leap = true;
    if ($gregorianDayNo >= 36525) {
      $gregorianDayNo--;
      $gYear += 100 * intval($gregorianDayNo / 36524);
      $gregorianDayNo = $gregorianDayNo % 36524;

      if ($gregorianDayNo >= 365) {
        $gregorianDayNo++;
      }
      else {
        $leap = false;
      }
    }

    $gYear += 4 * intval($gregorianDayNo / 1461);
    $gregorianDayNo %= 1461;

    if ($gregorianDayNo >= 366) {
      $leap = false;

      $gregorianDayNo--;
      $gYear += intval($gregorianDayNo / 365);
      $gregorianDayNo = $gregorianDayNo % 365;
    }

    for ($i = 0; $gregorianDayNo >= $gregorianDays_inMonth[$i] + ($i == 1 && $leap); $i++) {
      $gregorianDayNo -= $gregorianDays_inMonth[$i] + ($i == 1 && $leap);
    }
    $gMonth = $i + 1;
    $gDay = $gregorianDayNo + 1;

    return array('year' => $gYear, 'month' => $gMonth, 'day' => $gDay);
  }

  /**
   * Converts php date format string (like 'Y-m-d') to it's php-intl equivilant.
   *
   * @param string $format format accepted by php date_format
   * @return string format accepted by php-intl date formatter (ICU).
   */
  public static function phpToIntl($format) {
    static $format_map = NULL;
    if ($format_map == NULL) {
      $format_map = array(
        'd' => 'dd',
        'D' => 'LLL',
        'j' => 'd',
        'l' => 'LLLL',
        'N' => 'e',
        'S' => 'LLLL',
        'w' => '',
        'z' => 'D',
        'W' => 'w',
        'm' => 'LL',
        'M' => 'LLL',
        'F' => 'MMMM',
        'n' => 'L',
        't' => '',
        'L' => '',
        'o' => 'Y',
        'y' => 'yy',
        'Y' => 'YYYY',
        'a' => 'a',
        'A' => 'a',
        'B' => '',
        'g' => 'h',
        'G' => 'H',
        'h' => 'hh',
        'H' => 'HH',
        'i' => 'mm',
        's' => 'ss',
        'u' => 'SSSSSS',
        'e' => 'z',
        'I' => '',
        'O' => 'Z',
        'P' => 'ZZZZZ',
        'T' => 'v',
        'Z' => '',
        'c' => '',
        'r' => '',
        'U' => '',
        ' ' => ' ',
        '-' => '-',
        '.' => '.',
        '-' => '-',
      );
    }

    $replace_pattern = '/[^ \-\/\.\\\\dDjlNSwzWmMFntLoyYaABgGhHisueIOPTZcrU]/';
    return strtr(preg_replace($replace_pattern, '', $format), $format_map);
  }

  /**
   * Formats a date according to format given. This function uses internal
   * methods for converting, @see DatexFormatter::formatINTL is suggested instead.
   *
   * @param mixed $date @see get_date_object
   * @param string $format formats accepted by php format_date
   * @return string formatted date
   */
  public static function formatPHP($date, $format) {
    $persian_date_names = self::persianDateNames();
    $number_of_days = array(0, 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29);

    $date = self::ObjectFromDate($date);
    $gregorian_date = array(
      'd' => intval($date->format('j')),
      'm' => intval($date->format('n')),
      'Y' => intval($date->format('Y')),
    );
    $jalali_date = self::toJalali($gregorian_date['Y'], $gregorian_date['m'], $gregorian_date['d']);

    $z = $jalali_date['month'] <= 6 ?
        ($jalali_date['month'] * 31 + $jalali_date['day']) :
        186 + (($jalali_date['month'] - 6) * 30 + $jalali_date['day']);
    $W = ceil($z / 7);
    $t = $number_of_days[$jalali_date['month']];
    $r = $persian_date_names['day_abbr'][$jalali_date['week_day']] .
        ', ' . $jalali_date['day'] . ' ' .
        $persian_date_names['months'][$jalali_date['month']] . ' ' .
        $jalali_date['year'] . $date->format('H:i:s P');

    if (($date->format('Y') % 4 == 0 && $date->format('Y') % 100 != 0) || $date->format('Y') % 400 == 0) {
      $L = 1;
      if ($jalali_date['month'] == 12) {
        $t = 30;
      }
    }
    else {
      $L = 0;
    }

    $format = preg_replace('/[\\\\][a-z]/', '', $format);

    $replacements = array(
      'd' => sprintf('%02d', $jalali_date['day']),
      'D' => $persian_date_names['day_abbr'][$date->format('N')],
      'j' => $jalali_date['day'],
      'l' => $persian_date_names['day'][$date->format('N')],
      'S' => $persian_date_names['day_abbr'][$date->format('N')],
      'F' => $persian_date_names['months'][$jalali_date['month']],
      'm' => sprintf('%02d', $jalali_date['month']),
      'n' => $jalali_date['month'],
      'L' => str_replace('L', $L, $format),
      'Y' => $jalali_date['year'],
      'y' => $jalali_date['year'],
      'o' => $jalali_date['year'],
      'a' => $persian_date_names['ampm'][$date->format('a')],
      'A' => $persian_date_names['ampm'][$date->format('a')],
      'B' => $persian_date_names['ampm'][$date->format('a')],
      'c' => $final .= $jalali_date['year'] . '-' . $jalali_date['month'] . '-' . $jalali_date['day'] . 'T' . $date->format('H:i:sP'),
      'g' => $date->format('g'),
      'G' => $date->format('G'),
      'h' => $date->format('h'),
      'H' => $date->format('H'),
      'i' => $date->format('i'),
      's' => $date->format('s'),
      'u' => $date->format('u'),
      'I' => $date->format('I'),
      'O' => $date->format('O'),
      'P' => $date->format('P'),
      'T' => $date->format('T'),
      'Z' => $date->format('Z'),
      'U' => $date->format('U'),
      'w' => $date->format('w'),
      'N' => $date->format('N'),
      'e' => $date->format('e'),
      'z' => $z,
      'W' => $W,
      'r' => $r,
      't' => $t,
    );

    return strtr($format, $replacements);
  }

  /**
   * Formats a date according to format given. This function uses php-intl
   * methods for converting.
   * PECL package php-intl needs to be enabled.
   *
   * @param mixed $date @see ObjectFromDate
   * @param string $format
   * @return string
   */
  public static function formatINTL($date, $format, &$error_code = NULL, &$error_message) {
    echo "in INTL";
    static $intl_formatter = NULL;
    if ($intl_formatter == NULL) {
      $intl_formatter = new IntlDateFormatter(
              "fa_IR@calendar=persian",
              IntlDateFormatter::FULL,
              IntlDateFormatter::FULL,
              'Asia/Tehran',
              IntlDateFormatter::TRADITIONAL
      );
    }
    $intl_formatter->setPattern(self::phpToIntl($format));
    $date = $intl_formatter->format(self::getTimestamp($date));

    if($error_code) {
      $error_code = $intl_formatter->getErrorCode();
    }
    if($error_message) {
      $error_message = $intl_formatter->getErrorMessage();
    }

    return $date;
  }

  /**
   * Created a DateTime object according to a jalali date given.
   *
   * Accepted date formats are an integer (as timestamp), a DatexObject object,
   * a DateTime or DateObject object, an array containing Jalali date parts.
   * If none of above is given, DateTime form current date is created.
   *
   * @param mixed $date
   * @return DateTime
   */
  public static function ObjectFromJalali($date = NULL) {
    if (is_int($date) || is_object($date)) {
      return self::ObjectFromDate($date);
    }
    elseif (is_array($date)) {
      $jalali_date = self::toGregorian(@$date['year'], @$date['month'], @$date['day']);
      list($date['year'], $date['month'], $date['day']) = array($jalali_date['year'], $jalali_date['month'], $jalali_date['day']);
      return self::ObjectFromDate($date);
    }
  }

  /**
   * Created a DateTime object containing date from $date
   *
   * Accepted date formats are an integer (as timestamp), a DatexObject object,
   * a DateTime or DateObject object, an array containing Gregorian date parts.
   * If none of above is given, DateTime form current date is created.
   *
   * @param mixed $date
   * @return DateTime
   */
  public static function ObjectFromDate($date = NULL) {
    if (is_int($date)) {
      return new DateTime('@' . $date);
    }
    elseif (is_string($date)) {
      try {
        $date = new DateTime($date);
      }
      catch (Exception $e) {
        return NULL;
      }
    }
    elseif (is_object($date)) {
      $c = get_class($date);
      if ($c == 'DatexObject') {
        return $date->getDateobjClone();
      }
      elseif ($c == 'DateObject' || $c == 'DateTime') {
        return $date;
      }
    }
    elseif (is_array($date)) {
      list($year, $month, $day, $hour, $minute, $second) = array(
        isset($date['year']) ? $date['year'] : date('Y'),
        isset($date['month']) ? $date['year'] : date('n'),
        isset($date['day']) ? $date['year'] : date('j'),
        isset($date['hour']) ? $date['year'] : date('G'),
        isset($date['minute']) ? $date['year'] : date('i'),
        isset($date['second']) ? $date['year'] : date('s'),
      );
      return new DateTime('@' . mktime($hour, $minute, $second, $month, $day, $year));
    }
    elseif ($date == NULL) {
      return new DateTime();
    }
    return NULL;
  }

  /**
   * Generated timestamp from given date.
   *
   * @param mixed $date @see getDateObject
   * @return int timestamp
   */
  public static function getTimestamp($date) {
    $date = self::ObjectFromDate($date);
    return intval($date->format('U'));
  }

  /**
   * Determines wether PECL package php-intl is available or not.
   *
   * @return bool
   */
  public static function hasINTL() {
    return class_exists('IntlDateFormatter');
  }

  /**
   * Returns current Jalali Year.
   *
   * @return int year
   */
  public static function getYear() {
    $date = self::toJalali();
    return $date['year'];
  }

  /**
   * Returns current Jalali month.
   *
   * @return int month
   */
  public static function getMonth() {
    $date = self::toJalali();
    return $date['month'];
  }

  /**
   * Returns current Jalali day.
   *
   * @return int day
   */
  public static function getDay() {
    $date = self::toJalali();
    return $date['day'];
  }

  //php utils

  /**
   * @see php date().
   */
  public static function date($format, $timestamp = NULL) {
    return self::format($timestamp === NULL ? time() : $timestamp, $format);
  }

  /**
   * @see php mktime().
   */
  public static function mktime($hour = NULL, $minute = NULL, $second = NULL, $month = NULL, $day = NULL, $year = NULL) {
    $date = self::toGregorian($year, $month, $day);
    list($year, $month, $date) = array(
      $year === NULL ? NULL : $date['year'],
      $month === NULL ? NULL : $date['month'],
      $day === NULL ? NULL : $date['day']);
    return mktime($hour, $minute, $second, $month, $day, $year);
  }

  /**
   * @see php getdate().
   */
  public static function getdate($timestamp = NULL) {
    $ret = array();

    $timestamp = $timestamp === NULL ? time() : $timestamp;
    $date = new DateTime($timestamp);
    $ret['seconds'] = intval($date->format('s'));
    $ret['minutes'] = intval($date->format('i'));
    $ret['hours'] = intval($date->format('G'));

    $jalali = self::toJalali();
    $ret['mday'] = intval($jalali['day']);
    $ret['mon'] = intval($jalali['month']);
    $ret['year'] = intval($jalali['year']);

    $ret['wday'] = self::formatPHP($date, 'w');
    $ret['yday'] = self::formatPHP($date, 'z');
    $ret['weekday'] = self::formatPHP($date, 'l');
    $ret['month'] = self::formatPHP($date, 'F');
    $ret[0] = $timestamp;
    return $ret;
  }

}

/**
 * This class is Jalali equivilant of php DateTime. It also has some
 * functionallity from object defiend in Drupal's date module DateObject.
 */
class DatexObject {

  private $dateobj;
  public $error;
  public $error_message;
  public $hasError;
  private $format_string;

  /**
   * Constructor for DatexObject
   *
   * @param mixed $datetime @see DatexFormatter::getDateObject
   * @param boolean $date_is_gregorian determines wheter given date to
   * constructor is Gregorian or not, default is set by a constant in module file
   * @param DateTimezone $tz timezone
   * @param string $format format used for formatting date.
   * @return DatexObject
   */
  public function __construct($datetime = NULL, $date_is_gregorian = DEFAULT_DATEXOBJECT_POLICY, $tz = NULL, $format = NULL) {
    $this->hasError = FALSE;
    $this->error = '';
    $this->error_message = '';
    $this->setDatetime($datetime, $date_is_gregorian, $tz);
    $format = $format ? $format : 'Y-m-d';
    $this->setFormat($format);
    return $this;
  }

  public function __toString() {
    return $this->format(); //. ' ' . $this->getTimeZone()->getName();
  }

  /**
   * Similar to DateTime::format
   *
   * @param string $format
   * @param bool $use_intl whether to use php-intl or not
   * @return string formatted date
   */
  public function format($format = NULL, $use_intl = DATEX_USE_INTL) {
    $format = $format ? $format : $this->format_string;
    return DatexFormatter::format($this->dateobj, $format, $use_intl,$this->error,$this->error_message);
  }

  /**
   * Returns a clone of internal DateTime object, this object always contains
   * Gregorian date converted from jalali date given to DatexObject.
   *
   * @return DateTime
   */
  public function getDateobjClone() {
    return clone($this->dateobj);
  }

  /**
   * Set's date
   *
   * @param mixed $datetime @see ObjectFromDate and @see ObjectFromJalali
   * @param boolean $date_is_gregorian if is true, the given date is gregorian
   * otherwise it's jalali.
   * @param DateTimeZone  $tz
   */
  public function setDatetime($datetime = NULL, $date_is_gregorian = DEFAULT_DATEXOBJECT_POLICY, $tz = NULL) {
    if ($date_is_gregorian) {
      $this->dateobj = DatexFormatter::ObjectFromDate($datetime);
    }
    else {
      $this->dateobj = DatexFormatter::ObjectFromJalali($datetime);
    }

    if ($tz) {
      $this->dateobj->setTimezone($tz);
    }
  }

  public function setDate($year, $month, $day) {
    $year = $year === NULL ? $this->format('Y') : $year;
    $month = $month === NULL ? $this->format('n') : $month;
    $day = $day === NULL ? $this->format('j') : $day;

    $date = DatexFormatter::toGregorian($year, $month, $day);

    return $this->xsetDate($date['year'], $date['month'], $date['day']);
  }

  /**
   * Set format string used for formating date.
   *
   * @param string $format
   */
  public function setFormat($format) {
    $this->format_string = $format;
  }

  /**
   * Returns format string set by setFormat.
   *
   * @return string
   */
  public function getFormat() {
    return $this->format_string;
  }

  /**
   * @see DateTime::getTimezone
   *
   */
  public function getTimezone() {
    return $this->dateobj->getTimezone();
  }

  /**
   * @see DateTime::setTimezone
   *
   */
  public function setTimezone(DateTimeZone $timezone) {
    return $this->dateobj->setTimezone($timezone);
  }

  /**
   * @see DateTime::setTimestamp
   *
   */
  public function setTimestamp($timestamp) {
    return $this->dateobj->setTimestamp($timestamp);
  }

  /**
   * @see DateTime::setTime
   *
   */
  public function setTime($hour, $minute, $second = 0) {
    return $this->dateobj->setTime($hour, $minute, $second);
  }

  /**
   * @see DateTime::getOffset
   *
   */
  public function getOffset() {
    return $this->dateobj->getOffset();
  }

  /**
   * @see DateTime::diff
   *
   */
  public function xdiff(DateTime $datetime2, $absolute = false) {
    return $this->dateobj->diff($datetime2, $absolute);
  }

  /**
   * @see DateTime::format
   *
   */
  public function xformat($format) {
    return $this->dateobj->format($format);
  }

  /**
   * @see DateTime::getLastErrors
   *
   */
  public function xgetLastErrors() {
    return $this->getLastErrors();
  }

  /**
   * @see DateTime::setDate
   *
   */
  public function xsetDate($year, $month, $day) {
    return $this->dateobj->setDate($year, $month, $day);
  }

  /**
   * @see DateTime::getTimestamp
   *
   */
  public function getTimestamp() {
    return $this->dateobj->format('U');
  }

  /**
   * @see DateTime::modify
   *
   */
  public function modify($modify) {
    return $this->dateobj->modify($modify);
  }

  /**
   * Returns a Jalali Object from a given date.
   *
   * @param mixed $date @see DatexFormatter::getDateObject
   * @return DatexObject
   */
  public static function getJalaliObject($date = NULL) {
    return new DatexObject($date, FALSE);
  }

  /**
   * Returns an object containing first day of Jalali month stored in this object.
   *
   * @return DateObject
   */
  public function monthFirstDay() {
    return DatexObjectUtils::monthFirstDay(clone($this));
  }

  /**
   * Returns an object containing last day of Jalali month stored in this object.
   *
   * @return DateObject
   */
  public function monthLastDay() {
    return DatexObjectUtils::monthLastDay(clone($this));
  }

  //############################DATE_OBJCT##


  public function merge(FeedsDateTime $other) {

  }

  public function addGranularity($g) {

  }

  public function removeGranularity($g) {

  }

  public function hasGranularity($g = NULL) {

  }

  public function validGranularity($granularity = NULL, $flexible = FALSE) {

  }

  public function hasTime() {

  }

  public function completeDate() {

  }

  public function limitGranularity($granularity) {

  }

  public function toArray() {
    return array(
      'year' => $this->format('Y'),
      'month' => $this->format('n'),
      'day' => $this->format('j'),
      'hour' => intval($this->format('H')),
      'minute' => intval($this->format('i')),
      'second' => intval($this->format('s')),
      'timezone' => $this->format('e'),
    );
  }

  public function toISO($arr, $full = FALSE) {

  }

  public function setFuzzyDate($date, $format = NULL, $default = 'first') {

  }

  public function arrayErrors($arr) {

  }

  public function difference($date2_in, $measure = 'seconds', $absolute = TRUE) {
    // Create cloned objects or original dates will be impacted by the
    // date_modify() operations done in this code.
    if (get_class($date2_in) != 'DatexObject') {
      return NULL;
    }
    $date1 = $this->getDateobjClone();
    $date2 = $date2_in->getDateobjClone();

    $diff = date_format($date2, 'U') - date_format($date1, 'U');
    if ($diff == 0) {
      return 0;
    }
    elseif ($diff < 0 && $absolute) {
      // Make sure $date1 is the smaller date.
      $temp = $date2;
      $date2 = $date1;
      $date1 = $temp;
      $diff = date_format($date2, 'U') - date_format($date1, 'U');
    }

    $year_diff = intval(date_format($date2, 'Y') - date_format($date1, 'Y'));

    switch ($measure) {
      case 'seconds':
        return $diff;

      case 'minutes':
        return $diff / 60;

      case 'hours':
        return $diff / 3600;

      case 'years':
        return $year_diff;

      case 'months':
        $format = 'n';
        $item1 = $this->format_php($format, $date1);
        $item2 = $this->format_php($format, $date2);
        if ($year_diff == 0) {
          return intval($item2 - $item1);
        }
        else {
          $item_diff = 12 - $item1;
          $item_diff += intval(($year_diff - 1) * 12);
          return $item_diff + $item2;
        }
        break;

      case 'days':
        $format = 'z';
        $item1 = $this->format_php($format, $date1);
        $item2 = $this->format_php($format, $date2);
        if ($year_diff == 0) {
          return intval($item2 - $item1);
        }
        else {
          $item_diff = date_days_in_year($date1) - $item1;
          for ($i = 1; $i < $year_diff; $i++) {
            date_modify($date1, '+1 year');
            $item_diff += date_days_in_year($date1);
          }
          return $item_diff + $item2;
        }
        break;

      case 'weeks':
        throw new Exception('week not implemented!');
        $week_diff = date_format($date2, 'W') - date_format($date1, 'W');
        $year_diff = date_format($date2, 'o') - date_format($date1, 'o');
        for ($i = 1; $i <= $year_diff; $i++) {
          date_modify($date1, '+1 year');
          $week_diff += date_iso_weeks_in_year($date1);
        }
        return $week_diff;
    }

    return NULL;
  }

}

/**
 * methods of this class need DatexObject to exists, So they are not added to
 * DatexFormatter.
 */
class DatexObjectUtils {

  /**
   * Returns minumun day of a month
   *
   * @param mixed $date @see DatexFormatter::getDateObject
   */
  public static function monthFirstDay ($date = NULL) {
    $date = new DatexObject($date, FALSE);
    $date->setDate(NULL, NULL, 1);
    return $date;
  }

  /**
   * Returns minumun day of a month
   *
   * @param mixed $date @see DatexFormatter::getDateObject
   */
  public static function monthLastDay ($date = NULL) {
    $date = new DatexObject($date, FALSE);
    $date->setDate(NULL, NULL, DatexFormatter::format($date, 't'));
    return $date;
  }
}