define([],()=> {
  class ChartComponent {
    constructor(i18n, dataProviderService, $timeout, formTranslator, $scope) {
      this.i18n = i18n
      this.$scope = $scope
      this.dataProviderService=dataProviderService
      this.$timeout=$timeout
      this.getAttributes()
      this.model = []
      this.schema = {
        "type": "object",
        "title": "Chart",
        "properties": {
          "charts": {
            "type": "array",
            "items": {
              "type": "object", 
              "properties": {
                "name": {
                  "title": "Name",
                  "type": "string"
                },
                "title": {
                  "title": "Title",
                  "type": "string"
                },
                "description": {
                  "title": "Description",
                  "type": "string"
                },
                "type": {
                  "title": "Type",
                  "type": "string"
                },
                "functionGrouping": {
                  "title": "Function grouping",
                  "type": "string"
                },
                "series": {
                  "title": "Grouping series",
                  "type": "string",
                },
                "groupBy": {
                  "title": "Group by",
                  "type": "string"
                },
                "fromMap": {
                  "title": "From map?",
                  "type": "boolean"
                }
              },
              "required": [
                // "name",
                // "groupBy",
                // "type",
                // "title",
                // "series",
                // "functionGrouping"
              ]
            }
          }
        }
      };
      this.form = [
        {
          "key": "charts",
          "type": "tabarray",
          "add": "New",
          "remove": "Delete",
          "style": {
            "remove": "btn-danger"
          },
          "title": "{{ value.name || 'Tab '+($index+1) }}",
          "items": [
            {
              "key": "charts[].name",
              "htmlClass": "col-xs-6"
            },
            {
              "key": "charts[].title",
              "htmlClass": "col-xs-6"
            },
            {
              "key": "charts[].description",
              "type": "textarea",
              "htmlClass": "col-xs-12"
            },
            {
              "key": "charts[].type",
              "type": "select",
              "htmlClass": "col-xs-6",
              "titleMap": [
                {
                  "value": "pie",
                  "name": "Pie"
                },
                {
                  "value": "donut",
                  "name": "Donut"
                },
                {
                  "value": "bar",
                  "name": "Bar"
                },
                {
                  "value": "horizontal-bar",
                  "name": "Horizontal bar"
                },
                {
                  "value": "line",
                  "name": "Line"
                },
                // {
                //   "value": "line-compare",
                //   "name": "Line compare"
                // },
                {
                  "value": "area",
                  "name": "Area"
                },
                // {
                //   "value": "scatter-plot",
                //   "name": "Scatter plot"
                // },
                // {
                //   "value": "map",
                //   "name": "Map"
                // }
              ]
            },
            {
              "key": "charts[].functionGrouping",
              "type": "select",
              "htmlClass": "col-xs-6",
              "titleMap": [
                {
                  "value": "sum",
                  "name": "Sum"
                },
                {
                  "value": "count",
                  "name": "Count"
                }
              ]
            },
            {
              "identificator":"1",
              "key": "charts[].series",
              "type": "select",
              "htmlClass": "col-xs-6",
              "titleMap": [],
              "condition": "model.charts[arrayIndex].functionGrouping != 'count' && model.charts[arrayIndex].type != 'line' && model.charts[arrayIndex].type != 'area'"
            },
            {
              "identificator":"2",
              "key": "charts[].groupBy",
              "type": "select",
              "htmlClass": "col-xs-6",
              "titleMap": [],
              "condition": "model.charts[arrayIndex].type != 'line' && model.charts[arrayIndex].type != 'area'"
            },
            {
              "key": "charts[].fromMap",
              "htmlClass": "col-xs-12",
              "condition": "model.charts[arrayIndex].type != 'line' && model.charts[arrayIndex].type != 'area'"
            },
          ]
        }
      ]
      
      // var propertiesLocale = FormTranslator(this.schema);
    }

    getAttributes(){
      this.$timeout(()=>{
        this.attrs = [];
        const {tableName, provider, dataProviderService} = this
        dataProviderService.listPostgisObjects({providerId: provider, objectToGet: "column", tableName})
        .then(response=>{
          if (response.data.status == 400){
            return result.reject(response.data);
          }
          response.data.data.map(element => {
            this.attrs.push({'name': element.column_name, 'value': element.column_name});
          });
          let series = this.form[0].items.find(formItem => formItem.identificator === "1");
          series.titleMap = this.attrs
          let groupBy = this.form[0].items.find(formItem => formItem.identificator === "2");
          groupBy.titleMap = this.attrs
          // let label = this.form[0].items.find(formItem => formItem.identificator === "3");
          // label.titleMap = this.attrs
          this.$scope.$broadcast('schemaFormRedraw')
        });
      })
    }
  }

  ChartComponent.$inject = ["i18n", "DataProviderService", "$timeout", "FormTranslator", "$scope"];

  const component = {
    bindings : {
      model: "=",
      provider: "<",
      tableName: "<",
      chartForm: "=",
      styleForm: "<"
    },
    controller: ChartComponent,
    template: `
        <form name="$ctrl.chartForm" sf-form="$ctrl.form" sf-schema="$ctrl.schema" sf-model="$ctrl.model"></form>
      `
  };
  return component;
})
