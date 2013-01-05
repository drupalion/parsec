/**
 * @file Adds a Jalali Datepicker to all text inputs with class pdate
 *
 */

//keep track of elements we added a datepicker to, saves their IDs.
var persianDateWidgetProcessed = new Array();

Drupal.behaviors.persianDateWidget = {
  attach: function(context, settings) {
      jQuery('.pdate').each(function(dummy) {
          var id = jQuery(this).attr('id');
          if(persianDateWidgetProcessed.indexOf(id) < 0) {

              //save ID of processed element to prevent re-processing it.
              persianDateWidgetProcessed.push(id);

              //create actual datepicker and put it in window to live there
              //while page is alive.
              //name of created object is determined by pattern 'datex' + number
              //of processed items, which will increment by one each time and
              //make uniqe names.note that created objects name should be same
              //as first argument passed to AMIB constructor.
              window['persianDateWidget'+persianDateWidgetProcessed.length] = new AMIB.persianCalendar( 'persianDateWidget'+persianDateWidgetProcessed.length, id );
          }
      });
  }
};
