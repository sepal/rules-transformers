// $Id$
(function ($) {

var transformers_endpoints = new Array();
var transformers_canvasIn = new Array();
var transformers_canvasOut = new Array();
var transformers_currently_dragging = null;
var transformers_current_rule = null;


Drupal.behaviors.transformers_pipelines = {
  attach: function(context) {
    jsPlumb.Defaults.DragOptions = { cursor: 'pointer', zIndex:2000, containment: '#transformers_panel'};

    jsPlumb.draggable($('.transformers_action'));
    
    var exampleColor = '#0074bd';
    
    
    var endPointIn = {
      endpoint:new jsPlumb.Endpoints.Rectangle(),
      style:{ width:25, height:12, fillStyle:exampleColor },
      connectorStyle : { strokeStyle:"#666" },
      dropOptions: {
        tolerance:'touch',
        drop: transformers_pipelines_drop
      },
      isTarget:true,
      isSource:false
    };
    
    var endPointOut = {
        endpoint:new jsPlumb.Endpoints.Rectangle(),
        style:{ width:25, height:12, fillStyle:exampleColor },
        connectorStyle : { strokeStyle:"#666" },
        connector: new jsPlumb.Connectors.Bezier(125),
        maxConnections: 5, // maxConnections: -1 would set it to infinity, but is currently broken in jsPlumb
        dragOptions: {
          start: function(e, ui) {
            transformers_currently_dragging = transformers_canvasOut[$(this).attr('id')];
          }
        },
        isTarget:false,
        isSource:true
    };
    
    transformers_current_rule = Drupal.settings.transformers_pipelines.rule;
    elementlist = Drupal.settings.transformers_pipelines.elements;
    
    for (var key in elementlist) {
      if (elementlist.hasOwnProperty(key)) {
        element = elementlist[key];
        transformers_pipelines_generate_endpoints(element.element_id, element.provides, endPointOut);
        transformers_pipelines_generate_endpoints(element.element_id, element.parameter, endPointIn);

        for (var key in element.parameter) {
          if (element.parameter.hasOwnProperty(key) && element.parameter[key].source != null) {
            jsPlumb.connect({
              sourceEndpoint:transformers_endpoints[element.parameter[key].source],
              targetEndpoint:transformers_endpoints[element.parameter[key].target]
            });
            transformers_canvasIn[$(transformers_endpoints[element.parameter[key].target].canvas).attr('id')].connected = true;
            transformers_canvasIn[$(transformers_endpoints[element.parameter[key].target].canvas).attr('id')].source = element.parameter[key].source;
            transformers_canvasOut[$(transformers_endpoints[element.parameter[key].source].canvas).attr('id')].connected = true;
            transformers_canvasOut[$(transformers_endpoints[element.parameter[key].source].canvas).attr('id')].target = element.parameter[key].target;
          }
        }
      }
    }
  }
};

transformers_pipelines_generate_endpoints = function(elementId, endPoints, endPointOptions) {
  element_height = $("#transformers_element_" + elementId).height();
  element_width = $("#transformers_element_" + elementId).width();
  for (var key in endPoints) {
    if (endPoints.hasOwnProperty(key)) {
      endPointOptions.scope = endPoints[key].scope;
      if (endPointOptions.isTarget && !endPointOptions.isSource) {
        var top = $("#transformers_" + endPoints[key].target).position().top;
        transformers_endpoints[endPoints[key].target] = jsPlumb.addEndpoint("transformers_element_" + elementId, jsPlumb.extend({ anchor:jsPlumb.makeAnchor(0, top/element_height + 0.15, -1, 0) }, endPointOptions));
        transformers_canvasIn[$(transformers_endpoints[endPoints[key].target].canvas).attr('id')] = {
          target: endPoints[key].target,
          connected: false,
          element_id: elementId
        };
        $(transformers_endpoints[endPoints[key].target].canvas).click(transformers_pipelines_in_click);
      }
      else {
        var top = $("#transformers_" + endPoints[key].source).position().top;
        transformers_endpoints[endPoints[key].source] = jsPlumb.addEndpoint("transformers_element_" + elementId, jsPlumb.extend({ anchor:jsPlumb.makeAnchor(1, top/element_height + 0.15, 1, 0) }, endPointOptions));
        transformers_canvasOut[$(transformers_endpoints[endPoints[key].source].canvas).attr('id')] = {
          source: endPoints[key].source,
          connected: false,
          element_id: elementId
        };
      }
    };
  }
}

transformers_pipelines_in_click = function() {
  canvas = $(this).attr('id');
  target = transformers_canvasIn[canvas];
  if (target.connected) {
    
  }
}

transformers_pipelines_drop = function(e, ui) {
  target = transformers_canvasIn[$(this).attr('id')];
  source = transformers_currently_dragging;

  transformers_canvasIn[$(this).attr('id')].connected = true;
  transformers_canvasIn[$(this).attr('id')].source = source.source;

  var url = Drupal.settings.basePath + 'admin/config/workflow/transformers/config/' + transformers_current_rule + '/connect';
  $.ajax({
    url: location.protocol + '//' + location.host + url,
    type: 'POST',
    dataType: 'json',
    data:{
      'transformers_target_connection': target
    },
    success: function(data) {
      if (data.result == false) {
        alert(data.error.message);
      }
    }
  });

}

})(jQuery);