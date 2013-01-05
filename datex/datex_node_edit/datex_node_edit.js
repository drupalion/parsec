
Drupal.behaviors.datexNodeEdit = {
    attach: function(context) {
        jQuery("#edit-date").datetimeEntry({
            datetimeFormat: "Y-O-D H:Ma",
            spinnerImage: "spinnerOrange.png",
            useMouseWheel: true,
            minDatetime: new Date(1900, 1 - 1, 26, 11, 30, 0)
        });
    }
};
