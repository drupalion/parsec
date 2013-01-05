/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

Drupal.behaviors.datexNodeEdit={
    attach:function(e){
        jQuery("#edit-date").datetimeEntry({datetimeFormat:"Y-O-D h:M:S",spinnerImage:"spinnerOrange.png",useMouseWheel:true});
        jQuery("#edit-date").attr('autocomplete', 'Off');
    }
};
