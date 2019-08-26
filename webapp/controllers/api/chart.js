
  // Dependencies
  var DataManager = require('../../core/DataManager');
  var {Connection} = require('../../core/utility/connection');

  // Facade
  var ViewFacade = require("../../core/facade/View");

  /**
   * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
   *
   * @param {Object}
   * @returns {Object}
   */
  module.exports = function(app) {
    return {
      get: async (request, response) => {
        let {
          viewId, attributeName, functionGrouping, groupBy, label, legendFromMap, dateFrom, dateTo, chartType, interval
        } = request.query

        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
        const tableName = dataSeries.dataSets[0].format.table_name
        const timestampProperty = dataSeries.dataSets[0].format.timestamp_property
        const conn = new Connection(dataProvider.uri)
        await conn.connect()
        let dateSQL = "";
        if(timestampProperty) {
          if(dateTo != "undefined" && dateFrom != "undefined"){
            dateSQL = `where ${timestampProperty}::date >= '${dateFrom}' AND ${timestampProperty}::date <= '${dateTo}'`;
          }
        }

        let sql = "";

        if(chartType === "line" || chartType === "area" || chartType == "line-compare") {
          if(!interval || interval == "undefined"){
            interval = "day";
          }
          if(chartType == "line-compare") {
            sql = `
              SELECT date_trunc('${interval}', data_hora_gmt) AS date, ${groupBy} as groupby, count(*) AS value
              FROM ${tableName}
              ${dateSQL}
              GROUP BY date, ${groupBy}
              ORDER BY date;
            `
          }else{
            sql = `
              SELECT date_trunc('${interval}', data_hora_gmt) AS date, count(*) AS value
              FROM ${tableName}
              ${dateSQL}
              GROUP BY date
              ORDER BY date;
              `;
          }
        } else {
          let select = `SELECT coalesce(${groupBy}::text, 'Undefined') ${groupBy}`;
          let groupBySql = groupBy;
          let join = "";
          if(legendFromMap === "true"){
            select += `, MAX(SUBSTRING(color, 1, 7)) AS color`;
            join+=`JOIN terrama2.view_style_colors AS vsc ON ${groupBy} = vsc.value `;
            // groupBySql+=`, color`;
          }

          if(functionGrouping === "count"){
            sql = `${select}, COUNT(*) AS value FROM ${tableName} ${join} ${dateSQL} GROUP BY ${groupBySql}`;
          } else if(functionGrouping === "sum"){
            sql = `${select}, SUM(${attributeName}) AS value FROM ${tableName} ${join} ${dateSQL} GROUP BY ${groupBySql}`;
          }
        }

        const result = await conn.execute(sql)
        let rows = result.rows
        if(chartType == "line-compare") {
          var count = 0;
          rows = rows.map((element, index, arr) => {
            let date = element.date;
            count=0
            arr.forEach((e, i, a) => {
              if(e.date.getTime() === date.getTime()){
                element[e.groupby] = e.value;
                delete a[i]
                count++;
                // a.splice(i, 1)
              }
            });
            delete element.groupby;
            delete element.value;
            return element;
          });
          let newRows = [];
          for (let i = 0; i < rows.length; i++) {
            const element = rows[i];
            if(element){
              newRows.push(element)
            }
          }
          newRows.push({"lineCount":count})
          rows = newRows
        }

        await conn.disconnect()
        response.json(rows)
      },
    }
} ();