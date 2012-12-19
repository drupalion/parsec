<?php

/*
 * Field handler for date fields in views to convert it's format to Jalali.
 * 
 * @ingroup views_field_handlers
 */

class views_handler_field_datex extends views_handler_field_date {
    function render($values) {
        global $language;
        if ($language->language == 'fa') {
            $format = $this->options['date_format'];
            $value = $values->{$this->field_alias};
            return datex_format(intval($value), $format);
        } else {
            return parent::render($values);
        }
    }

}
