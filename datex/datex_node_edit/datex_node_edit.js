(function($) {
    Drupal.behaviors.datexNodeEdit = {
        attach: function(context) {
            $('.node-form-author,#edit-date').datetimeEntry({datetimeFormat: 'Y-O-D H:Ma'});
        }
    };
})(jQuery);
