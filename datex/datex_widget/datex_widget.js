/**
 * @file Adds a Jalali Datepicker to all text inputs with class pdate
 *
 */

//keep track of elements we added a datepicker to, saves their IDs.
var datex_processed = new Array();

Drupal.behaviors.datex_popup = {
  attach: function(context, settings) {
      jQuery('.pdate').each(function(dummy) {
          var id = jQuery(this).attr('id');
          if(datex_processed.indexOf(id) < 0) {

              //save ID of processed element to prevent re-processing it.
              datex_processed.push(id);

              //create actual datepicker and put it in window to live there
              //while page is alive.
              //name of created object is determined by pattern 'datex' + number
              //of processed items, which will increment by one each time and
              //make uniqe names.note that created objects name should be same
              //as first argument passed to AMIB constructor.
              window['datex'+datex_processed.length] = new AMIB.persianCalendar( 'datex'+datex_processed.length, id );
          }
      });
  }
};
