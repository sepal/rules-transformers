// $Id$
(function ($) {

Drupal.behaviors.transformers_pipelines = {
  attach: function(context) {
    jsPlumb.Defaults.DragOptions = { cursor: 'pointer', zIndex:2000 };
  }
};
})(jQuery);