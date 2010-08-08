// $Id$
(function ($) {

var transformers_endpoints = new Array();
var transformers_variables = new Array();


Drupal.behaviors.transformers_pipelines = {
  attach: function(context) {
    jsPlumb.Defaults.DragOptions = { cursor: 'pointer', zIndex:2000, containment: '#transformers_panel'};

    jsPlumb.draggable($('.transformers_action'));
    
    var exampleColor = '#0074bd';
    
    
    var endPointIn = {
      endpoint:new jsPlumb.Endpoints.Rectangle(),
      style:{ width:25, height:12, fillStyle:exampleColor },
      connectorStyle : { strokeStyle:"#666" },
      drop: transformers_pipelines_drop(),
      isTarget:true,
      isSource:false
    };
    
    var endPointOut = {
        endpoint:new jsPlumb.Endpoints.Rectangle(),
        style:{ width:25, height:12, fillStyle:exampleColor },
        connectorStyle : { strokeStyle:"#666" },
        isTarget:false,
        isSource:true,
        dropOptions: {
          tolerance:'touch',
          hoverClass:'dropHover',
          activeClass:'dragActive'
        }
    };
    
    elementlist = Drupal.settings.transformers_pipelines;
    
    for (var key in elementlist) {
      if (elementlist.hasOwnProperty(key)) {
        element = elementlist[key];
        transformers_pipelines_generate_endpoints(element.element_id, element.provides, endPointOut, false);
        transformers_pipelines_generate_endpoints(element.element_id, element.parameter, endPointIn, true);

        /*for (var key in element.parameter) {
          if (element.parameter.hasOwnProperty(key) && element.parameter[key].source != null) {
            jsPlumb.connect({
              sourceEndpoint:transformers_endpoints[element.parameter[key].source],
              targetEndpoint:transformers_endpoints[element.parameter[key].target]
            });
          }
        }*/
      }
    }
  }
};

transformers_pipelines_generate_endpoints = function(elementId, endPoints, endPointOptions, isInput) {
  element_height = $("#" + elementId).height();
  element_width = $("#" + elementId).width();
  for (var key in endPoints) {
    if (endPoints.hasOwnProperty(key)) {
      endPointOptions.scope = endPoints[key].scope;
      if (isInput) {
        top = $("#" + endPoints[key].target).position().top;
        transformers_endpoints[endPoints[key].target] = jsPlumb.addEndpoint(elementId, jsPlumb.extend({ anchor:jsPlumb.makeAnchor(0, top/element_height + 0.15, 0, 0) }, endPointOptions));
        //transformers_variables = endPoints[key].target;
      }
      else {
        top = $("#" + endPoints[key].source).position().top;
        transformers_endpoints[endPoints[key].source] = jsPlumb.addEndpoint(elementId, jsPlumb.extend({ anchor:jsPlumb.makeAnchor(1, top/element_height + 0.15, 1, 0) }, endPointOptions));
        //transformers_variables] = endPoints[key].source;
      }
    };
  }
}

transformers_pipelines_drop = function(e, ui) {
  console.log('dropped.');
}

})(jQuery);