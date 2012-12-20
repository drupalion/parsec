<?php

/**
 * Formatter for Jalali Dates. Contains helper function to work with dates.
 */
class DatexFormatter {

  /**
   * Similar to php date_format
   *
   * @param mixed $date @see DatexFormatter::get_date_object().
   * @param string $format date format
   * @param boolean $use_intl wheter to use intl or not.
   * @return string formatted date.
   */
  public static function date_format($date, $format, $use_intl = FALSE) {
    if (self::has_intl() && $use_intl) {
      return self::format_intl($date, $format);
    }
    else {
      return self::format_php($date, $format);
    }
  }

  /**
   * Returns array containing names of days and monthes in persian.
   *
   * @return array
   */
  public static function persian_date_names() {
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
  public static function to_jalali($gregorianYear = NULL, $gregorianMonth = NULL, $gregorianDay = NULL) {
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
  public static function to_gregorian($jalaliYear = NULL, $jalaliMonth = NULL, $jalaliDay = NULL) {
    $now = datex_api_date_to_jalali();
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
   * Converts php date format string (like 'Y-m-d') to it's Intl equivilant.
   *
   * @param string $format format accepted by php date_format
   * @return string format accepted by Intl.
   */
  public static function php_to_intl($format) {
    static $format_map = array(
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
    );
    static $whitespace_characters = array(
  ' ',
  '-',
  '/',
  '.'
    );

    $final = '';
    for ($i = 0; $i < strlen($format); $i++) {
      if ($format[$i] == '\\') {
        //skip next charachter
        $i++;
        continue;
      }
      elseif (array_key_exists($format[$i], $format_map)) {
        $final .= $format_map[$format[$i]];
      }
      elseif (array_key_exists($format[$i], $whitespace_characters)) {
        $final .= $format[$i];
      }
      else {
        continue;
      }
    }

    return $final;
  }

  /**
   * Formats a date according to format given. This function uses internal
   * methods for converting, format_intl is suggested instead.
   *
   * @param mixed $date @see get_date_object
   * @param string $format formats accepted by php format_date
   * @return string formatted date
   */
  public static function format_php($date, $format) {
    static $persian_date_names = NULL;
    if ($persian_date_names == NULL) {
      $persian_date_names = self::persian_date_names();
    }

    $date = self::get_date_object($date);

    $gregorian_date = array(
      'd' => intval($date->format('j')),
      'm' => intval($date->format('n')),
      'Y' => intval($date->format('Y')),
    );

    $jalali_date = self::to_jalali($gregorian_date['Y'], $gregorian_date['m'], $gregorian_date['d']);
    $jalali_date['week_day'] = $date->format('w');

    $final = '';
    for ($i = 0; $i < strlen($format); $i++) {
      switch ($format[$i]) {
        case 'd':
          $final .= sprintf('%02d', $jalali_date['day']);
          break;

        case 'D':
          $final .= $persian_date_names['day_abbr'][$jalali_date['week_day']];
          break;

        case 'j':
          $final .= $jalali_date['day'];
          break;

        case 'l':
          $final .= $persian_date_names['day'][$jalali_date['week_day']];
          break;

        case 'N':
          $final .= $jalali_date['week_day'];
          break;

        case 'S':
          $final .= $persian_date_names['day_abbr'][$jalali_date['week_day']];
          break;

        case 'w':
          $final .= $jalali_date['week_day'];
          break;

        case 'z':
          if ($jalali_date['month'] <= 6) {
            $final .= ($jalali_date['month'] * 31 + $jalali_date['day']);
          }
          else {
            $final .= 186 + (($jalali_date['month'] - 6) * 30 + $jalali_date['day']);
          }
          break;

        case 'W':
          if ($jalali_date['month'] <= 6) {
            $final .= ceil((($jalali_date['month'] * 31 + $jalali_date['day'])) / 7);
          }
          else {
            $final .= ceil((186 + (($jalali_date['month'] - 6) * 30 + $jalali_date['day'])) / 7);
          }
          break;

        case 'm':
          $final .= sprintf('%02d', $jalali_date['month']);
          break;

        case 'M': // No month abbreviation in persian! use full name.
        case 'F':
          $final .= $persian_date_names['months'][$jalali_date['month']];
          break;

        case 'n':
          $final .= $jalali_date['month'];
          break;

        case 't':
          //is it leap-year? last month is 28days'
          if ($date->format('L')) {
            $number_of_days = array(0, 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 30);
            $final .= $number_of_days[$jalali_date['month']];
          }
          //is it not a leap-year? then last month is 29 days
          else {
            $number_of_days = array(0, 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29);
            $final .= $number_of_days[$jalali_date['month']];
          }
          break;

        case 'L':
          $final .= $date->format('L');
          break;

        case 'o': // kind of not supported.
        case 'y': // same as Y in persian
        case 'Y':
          $final .= $jalali_date['year'];
          break;

        case 'a':
        case 'A':
          $final .= $persian_date_names['ampm'][$date->format('a')];
          break;

        case 'B':
          $final .= $date->format('B');
          break;

        case 'g':
          $final .= $date->format('g');
          break;

        case 'G':
          $final .= $date->format('G');
          break;

        case 'h':
          $final .= sprintf('%02d', $date->format('h'));
          break;

        case 'H':
          $final .= sprintf('%02d', $date->format('H'));
          break;

        case 'i':
          $final .= $date->format('i');
          break;

        case 's':
          $final .= $date->format('s');
          break;

        case 'u';
          $final .= $date->format('u');
          break;

        case 'e':
          $tz = $date->format('e');
          if ($tz == 'Asia/Tehran') {
            $final .= $persian_date_names['tz'];
          }
          else {
            $final .= $tz;
          }
          break;

        case 'I':
          $date->format('I');
          break;

        case 'O':
          $date->format('O');
          break;

        case 'P':
          $date->format('P');
          break;

        case 'T':
          $date->format('T');
          break;

        case 'Z':
          $date->format('Z');
          break;

        case 'c':
          $final .= $jalali_date['year'] . '-' . $jalali_date['month'] . '-' . $jalali_date['day'] . 'T' . $date->format('H:i:sP');
          break;

        case 'r':
          // Thu, 21 Dec 2000 16:01:07 +0200
          $day_abbr = $persian_date_names['day_abbr'][$jalali_date['week_day']];
          $month_abbr = $persian_date_names['months'][$jalali_date['month']];
          $final .= $day_abbr . ', ' . $jalali_date['day'] . ' ' . $month_abbr . ' ' . $jalali_date['year'] . $date->format('H:i:s P');
          break;

        case 'U':
          $final .= $date->format('U');
          break;

        //if $format has charchter \ which is \\ in php, the one passed
        //$format to us meant the next charachter is not formatting charachter
        //and should be treated as default case below, just add the character
        //something like \Y which wont change to 2012, but to Y
        case '\\':
          $final .= $format[$i++];
          break;

        //it's not a formatting character, probably something like : or /
        default :
          $final .= $format[$i];
      }
    }

    return $final;
  }

  /**
   * Formats a date according to format given. This function uses php Intl
   * methods for converting, thus php-intl needs to be enabled.
   *
   * @staticvar null $intl_formatter
   * @param type $date
   * @param type $format
   * @return type
   */
  public static function format_intl($date, $format) {
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

    $intl_formatter->setPattern(self::php_to_intl($format));
    return $intl_formatter->format(self::get_date_timestamp($date));
  }

  /**
   * Created a DateTime object containing converted date from Jalali
   *
   * Accepted date formats are an integer (as timestamp), a DatexObject object,
   * a DateTime or DateObject object, an array containing Jalali date parts.
   * If none of above is given, DateTime form current date is created.
   *
   * @param mixed $date
   * @return DateTime
   */
  public static function get_date_object_from_jalali($date = NULL) {
    if (is_int($date)) {
      return self::get_date_object($date);
    }
    elseif (is_object($date) && get_class($date) == 'DatexObject') {
      return get_date_object($date);
    }
    elseif (is_object($date) && get_class($date) == 'DateTime' || get_class($date) == 'DateObject') {
      return get_date_object($date->format('U'));
    }
    elseif (is_array($date)) {
      $jalali_date = self::to_gregorian(@$date['year'], @$date['month'], @$date['day']);
      list($date['year'], $date['month'], $date['day']) = array($jalali_date['year'], $jalali_date['month'], $jalali_date['day']);

      return get_date_object($date);
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
  public static function get_date_object($date = NULL) {
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
        return $date->get_dateobj_clone();
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
    elseif ($date === NULL) {
      return new DateTime();
    }
    return NULL;
  }

  /**
   * Generated timestamp from given date.
   *
   * @param mixed $date @see get_date_object
   * @return int timestamp
   */
  public static function get_date_timestamp($date) {
    $date = get_date_object($date);
    return $date->format('U');
  }

  /**
   * Determines wether php-intl library is available or not.
   *
   * @return bool
   */
  public static function has_intl() {
    return class_exists('IntlDateFormatter');
  }

}

class DatexObject {//extends DateTime {

  private $dateobj;
  public $error;
  public $has_error;

  //#########################################################
  public function __construct($date_is_gregorian = TRUE, $datetime = NULL, $format = NULL, $tz = NULL) {
    $this->has_error = FALSE;
    $this->error = '';

    if (!class_exists('DatexFormatter')) {
      throw new Exception('Class DatexFormatter does not exist in this scope');
    }

    $this->set_datex($date_is_gregorian, $datetime, $format, $tz);
  }

  public function format($format = NULL, $use_intl = FALSE) {
    if ($format == NULL) {
      $format = 'c';
    }
    return DatexFormatter::date_format($this->dateobj, $format, $use_intl);
  }

  public function get_dateobj_clone() {
    return clone($this->dateobj);
  }

  public function set_datex($date_is_gregorian = TRUE, $datetime = NULL, $format = NULL, $tz = NULL) {
    if ($date_is_gregorian) {
      $this->dateobj = DatexFormatter::get_date_object($datetime);
    }
    else {
      $this->dateobj = DatexFormatter::get_date_object_from_jalali($datetime);
    }
  }

  //############################DATE_OBJCT##
  //  public function __wakeup() {
  public function __toString() {
    return $this->format(); //. ' ' . $this->getTimeZone()->getName();
  }

  public function merge(FeedsDateTime $other) {

  }

  public function setTimezone($tz, $force = FALSE) {

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
    if (get_class($date2_in) != 'DatexObject' || $date2_in->has_error || $this->has_error) {
      return NULL;
    }
    $date1 = $this->get_dateobj_clone();
    $date2 = $date2_in->get_dateobj_clone();

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
      // The easy cases first.
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
