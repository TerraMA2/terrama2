define([],()=> {
  class ChartComponent {
    constructor(i18n, dataProviderService, $timeout, formTranslator) {
      this.i18n = i18n
      this.dataProviderService=dataProviderService
      this.$timeout=$timeout
      this.getAttributes()
      this.model = []
      this.selectedChart = null;
      // this.form = [
      //   {
      //     "type": "help",
      //     "helpvalue": "<h4>Tabbed Array Example</h4><p>Tab arrays can have tabs to the left, top or right.</p>"
      //   },
      //   {
      //     "key": "comments",
      //     "type": "tabarray",
      //     "add": "New",
      //     "remove": "Delete",
      //     "style": {
      //       "remove": "btn-danger"
      //     },
      //     "title": "{{ value.name || 'Tab '+$index }}",
      //     "items": [
      //       "comments[].name",
      //       "comments[].email",
      //       {
      //         "key": "comments[].comment",
      //         "type": "textarea"
      //       }
      //     ]
      //   },
      //   {
      //     "type": "submit",
      //     "style": "btn-default",
      //     "title": "OK"
      //   }
      // ];
      // this.schema = {
      //   "type": "object",
      //   "title": "Comment",
      //   "properties": {
      //     "comments": {
      //       "type": "array",
      //       "items": {
      //         "type": "object",
      //         "properties": {
      //           "name": {
      //             "title": "Name",
      //             "type": "string"
      //           },
      //           "email": {
      //             "title": "Email",
      //             "type": "string",
      //             "pattern": "^\\S+@\\S+$",
      //             "description": "Email will be used for evil."
      //           },
      //           "comment": {
      //             "title": "Comment",
      //             "type": "string",
      //             "maxLength": 20,
      //             "validationMessage": "Don't be greedy!"
      //           }
      //         },
      //         "required": [
      //           "name",
      //           "email",
      //           "comment"
      //         ]
      //       }
      //     }
      //   }
      // };
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
                "type": {
                  "title": "Type",
                  "type": "string"
                },
                "title": {
                  "title": "Title",
                  "type": "string"
                },
                "series": {
                  "title": "Series",
                  "type": "string"
                },
                "description": {
                  "title": "Description",
                  "type": "string"
                },
                "grouping": {
                  "title": "Grouping",
                  "type": "string"
                },
                "label": {
                  "title": "Label",
                  "type": "string"
                },
                "model": {
                  "title": "Model",
                  "type": "string",
                  "enum": [
                    "map",
                    "new",
                  ]
                },
              },
              "required": [
                "name"
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
          "items": 
          [
            {
              "key": "charts[].name",
              "htmlClass": "col-xs-6"
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
                  "value": "bar",
                  "name": "Bar"
                },
                {
                  "value": "line",
                  "name": "Line"
                }
              ]
            },
            {
              "key": "charts[].title",
              "htmlClass": "col-xs-6"
            },
            {
              "key": "charts[].series",
              "type": "select",
              "htmlClass": "col-xs-6",
              "titleMap": [
                {
                  "value": "pie",
                  "name": "Pie"
                },
                {
                  "value": "bar",
                  "name": "Bar"
                },
                {
                  "value": "line",
                  "name": "Line"
                }
              ]
            },
            {
              "key": "charts[].description",
              "type": "textarea",
              "htmlClass": "col-xs-6"
            },
            {
              "key": "charts[].grouping",
              "type": "select",
              "htmlClass": "col-xs-6",
              "titleMap": [
                {
                  "value": "pie",
                  "name": "Pie"
                },
                {
                  "value": "bar",
                  "name": "Bar"
                },
                {
                  "value": "line",
                  "name": "Line"
                }
              ]
            },
            {
              "key": "charts[].label",
              "type": "select",
              "htmlClass": "col-xs-6",
              "titleMap": [
                {
                  "value": "pie",
                  "name": "Pie"
                },
                {
                  "value": "bar",
                  "name": "Bar"
                },
                {
                  "value": "line",
                  "name": "Line"
                }
              ]
            },
            {
              "key": "charts[].model",
              "type": "radios",
              "htmlClass": "col-xs-12",
            },
          ]
        },
        {
          "type": "submit",
          "style": "btn-default",
          "title": "OK"
        },
      ]
      
      // var propertiesLocale = FormTranslator(this.schema);
    }
    
    addChart(chartForm) {
      if(this.selectedChart && chartForm.$invalid){
        return;
      }
      this.selectedChart = {};
      this.selectedChart.attributes = [];
      this.selectedChart.legends = [];
      this.model.push(this.selectedChart);
    }

    addAttribute() {
      this.selectedChart.attributes.push({});
    }

    addLegend() {
      this.selectedChart.legends.push({});
    }

    deleteChart(index) {
      this.model.splice(index, 1);
      this.selectedChart = this.model[0];
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
            this.attrs.push(element.column_name);
          });
        });
      })
    }
  }

  ChartComponent.$inject = ["i18n", "DataProviderService", "$timeout", "FormTranslator"];

  const component = {
    bindings : {
      model: "=",
      provider: "<",
      tableName: "<",
      chartForm: "="
    },
    controller: ChartComponent,
    template: `
        <form name="$ctrl.chartForm" sf-form="$ctrl.form" sf-schema="$ctrl.schema" sf-model="$ctrl.model">
        </form>
      `
  };
  return component;
})
