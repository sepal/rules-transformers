// $Id$

(function ($) {

var transformers_endpointsIn = new Array();
var transformers_endpointsOut = new Array();
var transformers_canvasIn = new Array();
var transformers_canvasOut = new Array();
var transformers_currently_dragging = null;
var transformers_current_rule = null;


Drupal.behaviors.transformers_pipelines = {
  attach: function(context) {
    jsPlumb.Defaults.DragOptions = { cursor: 'pointer', zIndex:2000, containment: '#transformers_panel', stop: transformers_pipelines_action_drag};

    jsPlumb.draggable($('.transformers_action'));
    $('#transformers_panel').resizable({
      handles: 'n, s',
      alsoResize: '#transformers_variables',
      minHeight: $('#transformers_panel').css('height').replace(/px/g, '')
    });
    
    var fillColor = '#0074bd';
    
    var endPointIn = {
      endpoint:new jsPlumb.Endpoints.Rectangle(),
      style:{ width:25, height:12, fillStyle:fillColor },
      connectorStyle : { strokeStyle:"#666" },
      dropOptions: {
        tolerance:'touch'
      },
      isTarget:true,
      isSource:false
    };
    
    var endPointOut = {
      endpoint:new jsPlumb.Endpoints.Rectangle(),
      style:{ width:25, height:12, fillStyle:fillColor },
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
    
    endPointVars = jQuery.extend(true, {}, endPointOut);
    var variablelist = Drupal.settings.transformers_pipelines.variables;
    for (var key in variablelist) {
      variable_id = variablelist[key].source;
      //endPointVars.scope = variablelist[key].scope;
      transformers_endpointsOut[variablelist[key].source] = jsPlumb.addEndpoint('transformers_variables_' + variable_id, jsPlumb.extend({ anchor:jsPlumb.makeAnchor(1, 0.5, 1, 0) }, endPointVars));
      transformers_canvasOut[$(transformers_endpointsOut[variablelist[key].source].canvas).attr('id')] = {
        source: variablelist[key].source,
        connected: false,
        source_element_id: variable_id
      };
    }


    elementlist = Drupal.settings.transformers_pipelines.elements;
    endPointIn.dropOptions.drop = transformers_pipelines_drop;
    for (var key in elementlist) {
      element = elementlist[key];
      transformers_pipelines_generate_endpoints(element.element_id, element.provides, endPointOut);
      transformers_pipelines_generate_endpoints(element.element_id, element.parameter, endPointIn);
    }
    
    endPointSplitterIn = jQuery.extend(true, {}, endPointIn);
    endPointSplitterOut = jQuery.extend(true, {}, endPointOut);
    endPointSplitterIn.dropOptions.drop = transformers_pipelines_drop_splitter;
    var splitterlist = Drupal.settings.transformers_pipelines.splitter;
    for (var key in splitterlist) {
      element = splitterlist[key];
      transformers_pipelines_generate_endpoints(element.element_id, element.parameter, endPointSplitterIn);
      transformers_pipelines_generate_endpoints(element.element_id, element.provides, endPointSplitterOut);
    }
    
    jQuery.extend(elementlist, splitterlist);
    for (var elementid in elementlist) {
      element = elementlist[elementid];
      for (var key in element.parameter) {
        if (element.parameter[key].source != null) {
          inputid = elementid + element.parameter[key].target;
          jsPlumb.connect({
            sourceEndpoint:transformers_endpointsOut[element.parameter[key].source],
            targetEndpoint:transformers_endpointsIn[inputid]
          });
          transformers_canvasIn[$(transformers_endpointsIn[inputid].canvas).attr('id')].connected = true;
          transformers_canvasIn[$(transformers_endpointsIn[inputid].canvas).attr('id')].source = element.parameter[key].source;
          transformers_canvasOut[$(transformers_endpointsOut[element.parameter[key].source].canvas).attr('id')].connected = true;
          transformers_canvasOut[$(transformers_endpointsOut[element.parameter[key].source].canvas).attr('id')].target = element.parameter[key].target;
        }
      }
    }
  }
};

transformers_pipelines_generate_endpoints = function(elementId, endPointsConfig, endPointOptions) {
  $element_html_id = "transformers_element_" + elementId;
  element_height = $("#" + $element_html_id).height();
  element_width = $("#" + $element_html_id).width();
  for (var key in endPointsConfig) {
    //endPointOptions.scope = endPointsConfig[key].scope;
    if (endPointOptions.isTarget && !endPointOptions.isSource) {
      var inputid = elementId + endPointsConfig[key].target;
      var top = $("#" + $element_html_id + '_' + endPointsConfig[key].target).position().top;
      transformers_endpointsIn[inputid] = jsPlumb.addEndpoint($element_html_id, jsPlumb.extend({ anchor:jsPlumb.makeAnchor(0, top/element_height + 0.15, -1, 0) }, endPointOptions));
      transformers_canvasIn[$(transformers_endpointsIn[inputid].canvas).attr('id')] = {
        target: endPointsConfig[key].target,
        connected: false,
        target_element_id: elementId
      };
      $(transformers_endpointsIn[inputid].canvas).click(transformers_pipelines_in_click);
    }
    else {
      // For selector we need a :, but since jquery dont likes colons for  
      // objects(or as html ids), I had to replaced it with a -.
      var source = endPointsConfig[key].source.replace(/:/g, "-");
      var top = $("#" + $element_html_id + '_' + source).position().top;
      transformers_endpointsOut[endPointsConfig[key].source] = jsPlumb.addEndpoint($element_html_id, jsPlumb.extend({ anchor:jsPlumb.makeAnchor(1, top/element_height + 0.15, 1, 0) }, endPointOptions));
      transformers_canvasOut[$(transformers_endpointsOut[endPointsConfig[key].source].canvas).attr('id')] = {
        source: endPointsConfig[key].source,
        connected: false,
        source_element_id: elementId
      };
    }
  }
}

transformers_pipelines_in_click = function() {
  canvas = $(this).attr('id');
  target = transformers_canvasIn[canvas];
  if (target.connected) {
    
  }
}

transformers_pipelines_action_drag = function(e, ui) {
  var info = {
    'id': $(this).attr('id'),
    'position' : $(this).position()
  }
  var url = Drupal.settings.basePath + 'admin/config/workflow/transformers/config/' + transformers_current_rule + '/save_position';
  $.ajax({
    url: location.protocol + '//' + location.host + url,
    type: 'POST',
    dataType: 'json',
    data: {
      'position_info': info
    },
    success: function(data) {
      if (data.result == false) {
        alert(data.error.message);
      }
    }
  });
}

transformers_pipelines_drop = function(e, ui) {
  target = transformers_canvasIn[$(this).attr('id')];
  source = transformers_currently_dragging;
  target.source_element_id = source.source_element_id;

  if (!transformers_canvasIn[$(this).attr('id')].connected) {
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
  else {
    // Since there is no apropriate detach function in currently in jsplumb,
    // users have to drag a connection onto a connected endpoint to disconnect.
    transformers_canvasIn[$(this).attr('id')].connected = false;
    transformers_canvasIn[$(this).attr('id')].source = '';
    var url = Drupal.settings.basePath + 'admin/config/workflow/transformers/config/' + transformers_current_rule + '/disconnect';
    $.ajax({
      url: location.protocol + '//' + location.host + url,
      type: 'POST',
      dataType: 'json',
      data:{
        'transformers_target_connection': target
      },
      success: function(data) {
        //location.reload();
      }
    });
  }
}

transformers_pipelines_drop_splitter = function(e, ui) {
  target = transformers_canvasIn[$(this).attr('id')];
  source = transformers_currently_dragging;
  target.source_element_id = source.source_element_id;

  if (!transformers_canvasIn[$(this).attr('id')].connected) {
    transformers_canvasIn[$(this).attr('id')].connected = true;
    transformers_canvasIn[$(this).attr('id')].source = source.source;
    var url = Drupal.settings.basePath + 'admin/config/workflow/transformers/config/' + transformers_current_rule + '/splitter/connect';
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
          location.reload(); // Temporary, while now detach function available.
        }
        else {
          location.reload();
        }
      }
    });
  }
  else {
    var url = Drupal.settings.basePath + 'admin/config/workflow/transformers/config/' + transformers_current_rule + '/splitter/disconnect';
    $.ajax({
      url: location.protocol + '//' + location.host + url,
      type: 'POST',
      dataType: 'json',
      data:{
        'transformers_target_connection': target
      },
      success: function(data) {
        location.reload();
      }
    });
  }
}

})(jQuery);