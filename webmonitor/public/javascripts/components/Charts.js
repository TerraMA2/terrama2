'use strict';


define(
  ['components/Layers', 'components/Utils', 'TerraMA2WebComponents'],
  function(Layers, Utils, TerraMA2WebComponents) {
    var chartList = [];
    var layer = null;
    var windowContent = null;
    var loadEvents = function() {
      $(document).on('click', '.fa-line-chart', function() {
        layer = Layers.getLayerById($(this).closest("li").data("layerid"));
        if($(".chart-panel").is(':hidden')){
          $(".chart-panel").css("left", "0");
          $(".chart-panel").toggle("slide", { direction: "left" }, 250);
        }
        setCharts(layer)
      });

      $(document).on("click", ".closeChart", function() {
        $(".chart-panel").css("left", "-100%");
        $(".chart-panel").toggle("slide", { direction: "left" }, 250);
      });
      $(document).on("click", "#newWindowBtn", function(event){
        var newWindow = window.open("", "", "toolbar=yes,scrollbars=yes,resizable=yes,top=500,left=500,width=400,height=400");
        newWindow.document.write(windowContent);
      });
    };

    var setCharts = function(layer) {
      let {charts, viewId} = layer;
      let chartsHtml="";
      // `<a id="newWindowBtn" href="#">New window</a><br />`
      let chartsLen=0;
      chartsLen = charts?charts.length:0;
      for (let i = 0; i < chartsLen; i++) {
        const chart = charts[i];
        chartsHtml += `
            <div class="panel panel-default">
              <div class="panel-heading" data-toggle="collapse" data-target="#chartPanel${i+1}">
                <h4 class="panel-title">${chart.title}</h4>
              </div>
              <div id="chartPanel${i+1}" class="panel-collapse collapse ${i==0?'in':''}">
                <div class="panel-body">
                  <div class="form-inline">
                    <div class="input-group date">
                      <input type="text" id="chartFilterDate${i+1}" class="form-control chart-date-picker"/>
                      <span class="input-group-addon">
                        <span class="glyphicon glyphicon-calendar"></span>
                      </span>
                    </div>
                  </div>
                  <br />
                  <div id="chart${i+1}" class="chart"></div>
                </div>
                <div class="chart-description">
                  ${chart.description}
                </div>
              </div>
            </div>
        `;
      }
      
      let html = `
        <div class="col-sm-12">
          <div class="pull-right closeChart"><i class="fa fa-close"></i></div>
          <div style="margin-bottom:25px"></div>
          <div class="panel-group" id="chartAccordion">
            ${chartsHtml}
          </div>
        </div>
      `;

      // windowContent = html;
      $(".chart-panel").html(html);
      Utils.translate('.chart-panel');

      for (let [i, chartConfig] of charts.entries()) {
        const chartType = chartConfig.type;
        const chartSeries = chartConfig.series;
        const chartFunctionGrouping = chartConfig.functionGrouping;
        const chartGroupBy = chartConfig.groupBy;
        const chartLabel = chartConfig.label;
        
        let url = `http://localhost:36000/api/charts?viewId=${viewId}&attributeName=${chartSeries}`;
        if(chartFunctionGrouping){
          url += `&functionGrouping=${chartFunctionGrouping}`;
        }
        if(chartGroupBy){
          url += `&groupBy=${chartGroupBy}`;
        }
        if(chartLabel){
          url += `&label=${chartLabel}`;
        }
        
        const dateFrom = moment().startOf('day').format("Y-M-D HH:mm:ss")
        const dateTo = moment().format("Y-M-D HH:mm:ss");
        url += `&dateFrom=${dateFrom}`;
        url += `&dateTo=${dateTo}`;
        am4core.ready(function() {
          am4core.useTheme(am4themes_material);
          am4core.useTheme(am4themes_animated);
          if(chartType==="pie") {
            var chart = am4core.create(`chart${i+1}`, am4charts.PieChart3D);
            chart.id = `chart${i+1}`
            chart.dataSource.url = url;
            chart.legend = new am4charts.Legend();
            
            chart.responsive.enabled = true;
            chart.tapToActivate = true;
            
            var pieSeries = chart.series.push(new am4charts.PieSeries3D());
            pieSeries.dataFields.category = chartGroupBy;
            pieSeries.dataFields.value = "value";
            pieSeries.slices.template.strokeWidth = 2;
            pieSeries.slices.template.strokeOpacity = 1;
            pieSeries.hiddenState.properties.endAngle = -90;
            pieSeries.hiddenState.properties.opacity = 1;
            pieSeries.hiddenState.properties.startAngle = -90;

            chart.exporting.menu = new am4core.ExportMenu();

            chart.exporting.menu.items = [{
                "label": "<i class='fa fa-download'></i>",
                "menu": [
                  {
                    "label": "Image",
                    "menu": [
                      { "type": "png", "label": "PNG" },
                      { "type": "jpg", "label": "JPG" },
                      { "type": "gif", "label": "GIF" },
                      { "type": "svg", "label": "SVG" },
                      { "type": "pdf", "label": "PDF" }
                    ]
                  }, {
                    "label": "Data",
                    "menu": [
                      { "type": "json", "label": "JSON" },
                      { "type": "csv", "label": "CSV" },
                      { "type": "xlsx", "label": "XLSX" }
                    ]
                  }, {
                    "label": "Print", "type": "print"
                  }
                ]
              }
            ];

            pieSeries.colors.list = [
              // am4core.color("#0000FF"),
              // am4core.color("#FF0000"),
            ];
          } else if(chartType==="bar") {
            var chart = am4core.create(`chart${i+1}`, am4charts.XYChart3D);
            chart.id = `chart${i+1}`
            chart.dataSource.url = url;

            let categoryAxis = chart.xAxes.push(new am4charts.CategoryAxis());
            categoryAxis.dataFields.category = chartGroupBy;
            categoryAxis.renderer.labels.template.rotation = 270;
            categoryAxis.renderer.labels.template.hideOversized = false;
            categoryAxis.renderer.minGridDistance = 20;
            categoryAxis.renderer.labels.template.horizontalCenter = "right";
            categoryAxis.renderer.labels.template.verticalCenter = "middle";
            categoryAxis.tooltip.label.rotation = 270;
            categoryAxis.tooltip.label.horizontalCenter = "right";
            categoryAxis.tooltip.label.verticalCenter = "middle";

            let valueAxis = chart.yAxes.push(new am4charts.ValueAxis());
            valueAxis.title.text = chartGroupBy;
            valueAxis.title.fontWeight = "bold";

            var series = chart.series.push(new am4charts.ColumnSeries3D());
            series.dataFields.valueY = "value";
            series.dataFields.categoryX = chartGroupBy;
            series.name = chartGroupBy;
            series.tooltipText = "{categoryX}: [bold]{valueY}[/]";
            series.columns.template.fillOpacity = .8;

            var columnTemplate = series.columns.template;
            columnTemplate.strokeWidth = 2;
            columnTemplate.strokeOpacity = 1;
            columnTemplate.stroke = am4core.color("#FFFFFF");

            columnTemplate.adapter.add("fill", (fill, target) => {
              return chart.colors.getIndex(target.dataItem.index);
            })

            columnTemplate.adapter.add("stroke", (stroke, target) => {
              return chart.colors.getIndex(target.dataItem.index);
            })

            chart.cursor = new am4charts.XYCursor();
            chart.cursor.lineX.strokeOpacity = 0;
            chart.cursor.lineY.strokeOpacity = 0;

            chart.exporting.menu = new am4core.ExportMenu();
          } else if(chartType==="horizontal-bar") {
            var chart = am4core.create(`chart${i+1}`, am4charts.XYChart3D);
            chart.id = `chart${i+1}`
            chart.dataSource.url = url;

            var categoryAxis = chart.yAxes.push(new am4charts.CategoryAxis());
            categoryAxis.dataFields.category = chartGroupBy;
            categoryAxis.numberFormatter.numberFormat = "#";
            categoryAxis.renderer.inversed = true;

            var  valueAxis = chart.xAxes.push(new am4charts.ValueAxis()); 
            valueAxis.title.text = chartGroupBy;
            valueAxis.title.fontWeight = "bold";
            
            var series = chart.series.push(new am4charts.ColumnSeries3D());
            series.dataFields.valueX = "value";
            series.dataFields.categoryY = chartGroupBy;
            series.name = chartGroupBy;

            var columnTemplate = series.columns.template;
            columnTemplate.strokeWidth = 2;
            columnTemplate.strokeOpacity = 1;
            columnTemplate.stroke = am4core.color("#FFFFFF");

            columnTemplate.adapter.add("fill", (fill, target) => {
              return chart.colors.getIndex(target.dataItem.index);
            })

            columnTemplate.adapter.add("stroke", (stroke, target) => {
              return chart.colors.getIndex(target.dataItem.index);
            })

            chart.cursor = new am4charts.XYCursor();
            chart.cursor.lineX.strokeOpacity = 0;
            chart.cursor.lineY.strokeOpacity = 0;
          }
          else if (chartType === "line"){
            var chart = am4core.create(`chart${i+1}`, am4charts.XYChart);
            chart.id = `chart${i+1}`
            chart.dataSource.url = url;
            
            var dateAxis = chart.xAxes.push(new am4charts.DateAxis());
            dateAxis.renderer.minGridDistance = 50;
            
            var valueAxis = chart.yAxes.push(new am4charts.ValueAxis());
            
            var series = chart.series.push(new am4charts.LineSeries());
            series.dataFields.valueY = "value";
            series.dataFields.dateX = chartGroupBy;
            series.strokeWidth = 2;
            series.minBulletDistance = 10;
            series.tooltipText = "{valueY}";
            series.tooltip.pointerOrientation = "vertical";
            series.tooltip.background.cornerRadius = 20;
            series.tooltip.background.fillOpacity = 0.5;
            series.tooltip.label.padding(12,12,12,12)
            
            chart.scrollbarX = new am4charts.XYChartScrollbar();
            chart.scrollbarX.series.push(series);
            
            chart.cursor = new am4charts.XYCursor();
            chart.cursor.xAxis = dateAxis;
            chart.cursor.snapToSeries = series;
          }
          chartList.push(chart)
        });
      }
      
      $('.chart-date-picker').daterangepicker({
        "timePicker": true,
        // "minDate": moment(),
        "startDate": moment().startOf('day'),
        "endDate": moment(),
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
      
      $('.chart-date-picker').on('apply.daterangepicker', function(ev, picker) {
        const dateFrom = picker.startDate.format('YYYY-MM-DD')
        const dateTo = picker.endDate.format('YYYY-MM-DD')
        const chartContainer = $(ev.currentTarget).closest('.panel-body').find('.chart:first')[0]
        
        const chart = getChart(chartContainer.id)
        let oldUrl = chart.dataSource.url
        var index = oldUrl.indexOf("&dateFrom");
        if(index!=-1){
          oldUrl = oldUrl.substring(0, index);
        }
        const newUrl = oldUrl+`&dateFrom=${dateFrom}&dateTo=${dateTo}`
        chart.dataSource.url = newUrl
        chart.dataSource.load()
        chart.dataSource.url = oldUrl
      });
    }
    function getChart(id) {
      return chartList.find(chart => chart.id==id)
    }

    var init = function() {
      loadEvents();
    };

    return {
      init: init
    };
  }
);