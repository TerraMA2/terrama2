'use strict';


define(
  ['components/Layers', 'components/Utils', 'TerraMA2WebComponents'],
  function(Layers, Utils, TerraMA2WebComponents) {
    var loadEvents = function() {
      $('.fa-line-chart').on('click', function() {
        $(this).closest("li")
        if($(".chart-panel").is(':hidden')){
          $(".chart-panel").css("left", "0");
          $(".chart-panel").toggle("slide", { direction: "left" }, 250);
        }else{
          $(".chart-panel").css("left", "-100%");
          $(".chart-panel").toggle("slide", { direction: "right" }, 250);
        }
      });

      $(".chart-panel").on("setCharts", function(event) {
        setCharts();
      });

      $(".closeChart").on("click", function() {
        $(".chart-panel").css("left", "-100%");
        $(".chart-panel").toggle("slide", { direction: "left" }, 250);
      });
    };

    var setCharts = function() {
      let html = `
      <div class="col-sm-12">
      <div class="pull-right closeChart"><i class="fa fa-close"></i></div>
      <div style="margin-bottom:25px"></div>
      <div class="panel-group" id="chartAccordion">
        <div class="panel panel-default">
          <div class="panel-heading" data-toggle="collapse" data-target="#filterPanel">
            <h4 class="panel-title">
                Filters
            </h4>
          </div>
          <div id="filterPanel" class="panel-collapse collapse in">
            <div class="panel-body">
              <div class="input-group date">
                <input type="text" id="chartFilterDate" class="form-control"/>
                <span class="input-group-addon">
                  <span class="glyphicon glyphicon-calendar"></span>
                </span>
              </div>
              <br />
              <div class="form-group">
                <a class="btn btn-primary">Filter</a>
              </div>
            </div>
          </div>
        </div>
        <div class="panel panel-default">
          <div class="panel-heading" data-toggle="collapse" data-target="#chartPanel">
            <h4 class="panel-title">
                Chart
            </h4>
          </div>
          <div id="chartPanel" class="panel-collapse collapse in">
            <div class="panel-body">
              <div id="chart" class="chart"></div>
            </div>
          </div>
        </div>

        <div class="panel panel-default">
          <div class="panel-heading" data-toggle="collapse" data-target="#chartPanel2">
            <h4 class="panel-title">
                Chart
            </h4>
          </div>
          <div id="chartPanel2" class="panel-collapse collapse">
            <div class="panel-body">
              <div id="chart2" class="chart"></div>
            </div>
          </div>
        </div>

        <div class="panel panel-default">
          <div class="panel-heading" data-toggle="collapse" data-target="#chartPanel3">
            <h4 class="panel-title">
                Chart
            </h4>
          </div>
          <div id="chartPanel3" class="panel-collapse collapse">
            <div class="panel-body">
              <div id="chart3" class="chart"></div>
            </div>
          </div>
        </div>

        <div class="panel panel-default">
          <div class="panel-heading" data-toggle="collapse" data-target="#chartPanel4">
            <h4 class="panel-title">
                Chart
            </h4>
          </div>
          <div id="chartPanel4" class="panel-collapse collapse">
            <div class="panel-body">
              <div id="chart4" class="chart"></div>
            </div>
          </div>
        </div>
      </div>
      </div>
      `;
      $(".chart-panel").html(html);
      Utils.translate('.chart-panel');
      var chart = c3.generate({
        data: {
            columns: [
                ['data1', 30, 200, 100, 400, 150, 250],
                ['data2', 130, 100, 140, 200, 150, 50]
            ],
            type: 'line'
        },
      });
      chart.resize();
      var chart = c3.generate({
        bindto: '#chart2',
        data: {
            columns: [
                ['data1', 60, 180, 80, 120, 500, 250],
                ['data2', 250, 100, 500, 50, 120, 80]
            ],
            type: 'bar'
        },
        bar: {
          width: {
              ratio: 0.5
          }
        }
      });
      chart.resize();
      var chart = c3.generate({
        bindto: '#chart3',
        data: {
            columns: [
              ["data1", 0.2, 0.2, 0.2, 0.2, 0.2, 0.4, 0.3, 0.2, 0.2, 0.1, 0.2, 0.2, 0.1, 0.1, 0.2, 0.4, 0.4, 0.3, 0.3, 0.3, 0.2, 0.4, 0.2, 0.5, 0.2, 0.2, 0.4, 0.2, 0.2, 0.2, 0.2, 0.4, 0.1, 0.2, 0.2, 0.2, 0.2, 0.1, 0.2, 0.2, 0.3, 0.3, 0.2, 0.6, 0.4, 0.3, 0.2, 0.2, 0.2, 0.2],
              ["data2", 1.4, 1.5, 1.5, 1.3, 1.5, 1.3, 1.6, 1.0, 1.3, 1.4, 1.0, 1.5, 1.0, 1.4, 1.3, 1.4, 1.5, 1.0, 1.5, 1.1, 1.8, 1.3, 1.5, 1.2, 1.3, 1.4, 1.4, 1.7, 1.5, 1.0, 1.1, 1.0, 1.2, 1.6, 1.5, 1.6, 1.5, 1.3, 1.3, 1.3, 1.2, 1.4, 1.2, 1.0, 1.3, 1.2, 1.3, 1.3, 1.1, 1.3],
              ["data3", 2.5, 1.9, 2.1, 1.8, 2.2, 2.1, 1.7, 1.8, 1.8, 2.5, 2.0, 1.9, 2.1, 2.0, 2.4, 2.3, 1.8, 2.2, 2.3, 1.5, 2.3, 2.0, 2.0, 1.8, 2.1, 1.8, 1.8, 1.8, 2.1, 1.6, 1.9, 2.0, 2.2, 1.5, 1.4, 2.3, 2.4, 1.8, 1.8, 2.1, 2.4, 2.3, 1.9, 2.3, 2.5, 2.3, 1.9, 2.0, 2.3, 1.8],
            ],
            type: 'pie',
            onclick: function (d, i) { console.log("onclick", d, i); },
            onmouseover: function (d, i) { console.log("onmouseover", d, i); },
            onmouseout: function (d, i) { console.log("onmouseout", d, i); }
        },
      });
      chart.resize();
      var chart = c3.generate({
        bindto: '#chart4',
        data: {
            columns: [
                ['data1', 300, 350, 300, 0, 0, 0],
                ['data2', 130, 100, 140, 200, 150, 50]
            ],
            types: {
                data1: 'area',
                data2: 'area-spline'
            }
        }
      });

    // var find_query = function () {
    //   var _map = window.location.search.substr(1).split('&').map(function (a) {
    //       return a.split('=');
    //   }).reduce(function (p, v) {
    //     p[v[0]] = true;
    //       if (v.length > 1)
    //           p[v[0]] = decodeURIComponent(v[1].replace(/\+/g, " "));
    //       return p;
    //   }, {});
    //   return function (field) {
    //       return _map[field] || null;
    //   };
    // }();
    // var resizeMode = find_query('resize') || 'widhei';
    
    // function apply_resizing(chart, adjustX, adjustY, onresize) {
    //     if (resizeMode.toLowerCase() === 'viewbox') {
    //         chart
    //             .width(600)
    //             .height(400)
    //             .useViewBoxResizing(true);
    //         d3.select(chart.anchor()).classed('fullsize', true);
    //     } else {
    //         adjustX = adjustX || 0;
    //         adjustY = adjustY || adjustX || 0;
    //         chart
    //             .width(window.innerWidth - adjustX)
    //             .height(window.innerHeight - adjustY);
    //         window.onresize = function () {
    //             if (onresize) {
    //                 onresize(chart);
    //             }
    //             chart
    //                 .width(window.innerWidth - adjustX)
    //                 .height(window.innerHeight - adjustY);
    
    //             if (chart.rescale) {
    //                 chart.rescale();
    //             }
    //             chart.redraw();
    //         };
    //     }
    // }

    //    d3.csv("https://raw.githubusercontent.com/dc-js/dc.js/master/web/examples/morley.csv").then(data=>{
    //     let chart = dc.rowChart("#chart");
    //     var ndx                 = crossfilter(data),
    //     runDimension        = ndx.dimension(function(d) {return +d.Run;}),
    //     speedSumGroup       = runDimension.group().reduceSum(function(d) {return d.Speed * d.Run / 1000;});
    //     chart
    //       .x(d3.scaleLinear().domain([6,20]))
    //       .elasticX(true)
    //       .dimension(runDimension)
    //       .group(speedSumGroup);
    //       apply_resizing(chart, 20);
    //       chart.render();
    //    });

    //    d3.csv("https://raw.githubusercontent.com/dc-js/dc.js/master/web/examples/morley.csv").then(data=>{
    //     let chart = dc.barChart("#chart2");
    //     var ndx                 = crossfilter(data),
    //     runDimension        = ndx.dimension(function(d) {return +d.Run;}),
    //     speedSumGroup       = runDimension.group().reduceSum(function(d) {return d.Speed * d.Run / 1000;});
    //     console.log(runDimension);
    //     console.log("");
    //     console.log(speedSumGroup);
    //     chart
    //       .x(d3.scaleLinear().domain([6,30]))
    //       .elasticX(true)
    //       .brushOn(false)
    //       .dimension(runDimension)
    //       .group(speedSumGroup);
    //       apply_resizing(chart, 30);
    //       chart.render();
    //    });

      var calendar = $('#chartFilterDate');
      calendar.daterangepicker({
        "timePicker": true,
        // "minDate": moment(),
        // "startDate": moment(),
        // "endDate": moment(),
        // "maxDate": moment(),
        "timePicker24Hour": true,
        "opens": "center",
        "locale": {
          "format": Utils.getTranslatedString("DATE-FORMAT"),
          "separator": " - ",
          "applyLabel": Utils.getTranslatedString("Apply"),
          "cancelLabel": Utils.getTranslatedString("Cancel"),
          "fromLabel": Utils.getTranslatedString("From"),
          "toLabel": Utils.getTranslatedString("To"),
          "customRangeLabel": Utils.getTranslatedString("Custom"),
          "weekLabel": Utils.getTranslatedString("WEEK-LABEL"),
          "daysOfWeek": [
            Utils.getTranslatedString("DAY-SU"),
            Utils.getTranslatedString("DAY-MO"),
            Utils.getTranslatedString("DAY-TU"),
            Utils.getTranslatedString("DAY-WE"),
            Utils.getTranslatedString("DAY-TH"),
            Utils.getTranslatedString("DAY-FR"),
            Utils.getTranslatedString("DAY-SA"),
          ],
          "monthNames": [
            Utils.getTranslatedString("January"),
            Utils.getTranslatedString("February"),
            Utils.getTranslatedString("March"),
            Utils.getTranslatedString("April"),
            Utils.getTranslatedString("May"),
            Utils.getTranslatedString("June"),
            Utils.getTranslatedString("July"),
            Utils.getTranslatedString("August"),
            Utils.getTranslatedString("September"),
            Utils.getTranslatedString("October"),
            Utils.getTranslatedString("November"),
            Utils.getTranslatedString("December")
          ],
          "firstDay": 1
        }
      });
    }

    var init = function() {
      setCharts();
      loadEvents();
    };

    return {
      init: init
    };
  }
);