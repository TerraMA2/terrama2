  const {Connection} = require('../../core/utility/connection');
  const DataManager = require('../../core/DataManager');
  const ViewFacade = require("../../core/facade/View");


  /**
   * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
   *
   * @param {Object}
   * @returns {Object}
   */

  function generate_color(){
    const hexadecimal = '0123456789ABCDEF';
    let color = '#';
    const date = new Date()
    const milliseconds = date.getMilliseconds();

    for (var i = 0; i < 6; i++ ) {
      color += hexadecimal[Math.floor(Math.random(milliseconds) * 16)];
    }
    return color;
  };
  async function setGraphic(resultAux, value1, subtitle){
    let labels = [];
    let data = [];
    let backgroundColor = [];
    let hoverBackgroundColor = [];

    resultAux.rows.forEach( value => {
      labels.push(value[`${subtitle}`]);
      data.push(value[`${value1}`]);
      backgroundColor.push(generate_color());
      hoverBackgroundColor.push(generate_color());
    });

    return {
      labels: labels,
      datasets: [{
        data: data,
        backgroundColor: backgroundColor,
        hoverBackgroundColor: hoverBackgroundColor
      }]
    };
  };
  function setAlertGraphic(alert, graphic1, graphic2) {
    return {
      cod: alert.cod,
      codGroup: alert.codgroup,
      label: alert.label,
      active: alert.isPrimary,
      isEmpty: graphic1.labels.length === 0 || graphic2.labels.length === 0,
      graphics: [{
        data: graphic1,
        options: {
          title: {
            display: true,
            text: alert.codgroup,
            fontSize: 16
          },
          legend: {
            position: 'bottom'
          }
        }
      },
      {
        data: graphic2,
        options: {
          title: {
            display: true,
            text: alert.codgroup,
            fontSize: 16
          },
          legend: {
            position: 'bottom'
          }
        }
      }]
    }
  };
  async function getTableOwner(conn, alerts ){
    if (alerts.length > 0) {
      for (let alert of alerts) {
        if (alert.idview && alert.idview > 0 && alert.idview !== 'null' && alert.isAnalysis && alert.isPrimary) {
          return await getTable(conn, alert.idview);
        }
      }
    }
    return '';
  };
  async function getTable(conn, idView){
    const view = await ViewFacade.retrieve(idView);

    const dataSeries = await DataManager.getDataSeries({id: view.data_series_id});
    const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id});
    const uri = dataProvider.uri;

    const dataSet = dataSeries.dataSets[0];
    const tableName = dataSet.format.table_name;

    const sqlTableName = ` SELECT DISTINCT table_name FROM ${tableName}`;
    resultTableName = await conn.execute(sqlTableName);

    return resultTableName.rows[0]['table_name'];
  };
  function getColumns(alert, tableOwner){
    let column1 = '';
    let column2 = '';
    let column3 = '';
    let column4 = '';

    if (alert.codgroup && alert.codgroup === 'FOCOS') {
      if (alert.isAnalysis && alert.isPrimary) {
        column1 = ` de_car_validado_sema_numero_do1 `;
        column2 = ` dd_focos_inpe_bioma `;
        column3 = '1';
        column4 = ` dd_focos_inpe_bioma `;
      } else {
        column1 = ` ${tableOwner}_de_car_validado_sema_numero_do1 `;
        column2 = ` ${tableOwner}_dd_focos_inpe_bioma `;
        column3 = '1';
        column4 = ` ${tableOwner}_dd_focos_inpe_bioma `;
      }
    } else if (alert.codgroup && alert.codgroup === 'DETER') {
      if (alert.isAnalysis && alert.isPrimary) {
        column1 = ` de_car_validado_sema_numero_do1 `;
        column2 = ` dd_deter_inpe_classname `;
      } else {
        column1 = ` ${tableOwner}_de_car_validado_sema_numero_do1 `;
        column2 = ` ${tableOwner}_dd_deter_inpe_classname `;
      }
      column3 = alert.activearea ? ' calculated_area_ha ' : '1';
    } else if (alert.codgroup && alert.codgroup === 'PRODES') {
      if (alert.isAnalysis && alert.isPrimary) {
        column1 = ` de_car_validado_sema_numero_do1 `;
        column2 = ` dd_prodes_inpe_mainclass `;
      } else {
        column1 = ` ${tableOwner}_de_car_validado_sema_numero_do1 `;
        column2 = ` ${tableOwner}_dd_prodes_inpe_mainclass `;
      }
      column3 = alert.activearea ? ' calculated_area_ha ' : '1';
    }

    return { column1, column2, column3, column4 };
  };
  function getSqlWhere(params, alert, columns){
    let sqlWhere = '';

    if (params.date && params.date !== "null") {
      const dateFrom = params.date[0];
      const dateTo = params.date[1];
      sqlWhere += ` WHERE execution_date BETWEEN '${dateFrom}' AND '${dateTo}' `
    }

    if (params.area && params.area !== "null") {
      sqlWhere += ` AND ${columns.column3} > ${params.area} `
    }

    return sqlWhere;
  };
  async function getSql(conn, alert, tableOwner, params){
    let sql1 = '';
    let sql2 = '';

    const value1 = 'value';
    const subtitle = 'subtitle'

    if (alert.idview && alert.idview > 0 && alert.idview !== 'null') {

      const table = await getTable(conn, alert.idview);

      const columns = getColumns(alert, tableOwner);

      const limit = params.limit && params.limit !== 'null' && params.limit > 0 ?
          params.limit :
          10;


      const columnsFor1 =
          `   ${columns.column1} AS ${subtitle},
                      COALESCE(SUM(${columns.column3})) AS ${value1} `;

      const columnsFor2 =
          `   ${columns.column2} AS ${subtitle},
                      COALESCE(SUM(${columns.column3})) AS ${value1} `;

      const sqlFrom = ` FROM public.${table} `;

      const sqlWhere = getSqlWhere(params, alert, columns);

      const sqlGroupBy1 = ` GROUP BY ${columns.column1}  `;
      const sqlGroupBy2 = ` GROUP BY ${columns.column2}  `;
      const sqlOrderBy = ` ORDER BY ${value1} DESC `;
      const sqlLimit = ` LIMIT ${limit} `;

      sql1 += ` SELECT ${columnsFor1} ${sqlFrom} ${sqlWhere} ${sqlGroupBy1} ${sqlOrderBy} ${sqlLimit} `;
      sql2 += ` SELECT ${columnsFor2} ${sqlFrom} ${sqlWhere} ${sqlGroupBy2} ${sqlOrderBy} ${sqlLimit} `;
    } else {
      sql1 += ` SELECT 
                        ' --- ' AS ${subtitle},
                        0.00 AS ${value1} `;
      sql2 += ` SELECT 
                        ' --- ' AS ${subtitle},
                        0.00 AS ${value1} `;
    }
    return { sql1, sql2, value1, subtitle };
  }

  module.exports = function(app) {
    return {
      getStaticData: async (request, response) => {
        const {
          projectName,
          limit,
          offset,
          count,
          viewId
        } = request.query
        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id: view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id})
        const uri = dataProvider.uri
        // const conn = new Connection(uri)
        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        const dataSet = dataSeries.dataSets[0]
        let tableName = dataSet.format.table_name

        let sqlSelect = `SELECT *`
        let sqlFrom = ''
        let sqlWhere = ''

        sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(geom), 4326)) AS "lat",
                      ST_X(ST_Transform (ST_Centroid(geom), 4326)) AS "long"
        `

        sqlFrom += ` FROM public.${tableName}`

        if (limit) {
          sqlWhere += ` LIMIT ${limit}`
        }

        if (offset) {
          sqlWhere += ` OFFSET ${offset}`
        }

        const sql = sqlSelect + sqlFrom + sqlWhere

        let result
        let resultCount
        try {
          result = await conn.execute(sql)
          let dataJson = result.rows

          let sqlCount
          if (count) {
            sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName}`
            resultCount = await conn.execute(sqlCount)
            dataJson.push(resultCount.rows[0]['count'])
          }

          await conn.disconnect()
          response.json(dataJson)
        } catch (error) {
          console.log(error)
        }
      },
      getDynamicData: async (request, response) => {
        const {
          limit,
          offset,
          date,
          localization,
          area,
          count,
          viewId
        } = request.query
        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id: view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id})
        const uri = dataProvider.uri
        // const conn = new Connection(uri)
        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        const dataSet = dataSeries.dataSets[0]
        const tableName = dataSet.format.table_name
        const geomColumn = dataSet.format.geometry_property
        const timeStampColumn = dataSet.format.timestamp_property

        let sqlSelect = `SELECT *`
        let sqlFrom = ''
        let sqlWhere = ''

        if (geomColumn) {
          sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(${geomColumn}), 4326)) AS "lat",
                        ST_X(ST_Transform (ST_Centroid(${geomColumn}), 4326)) AS "long"
          `
        }

        sqlFrom += ` FROM public.${tableName}`

        if (date) {
          const {dateFrom, dateTo} = date

          sqlWhere += `
              WHERE ${timeStampColumn}::date >= '${dateFrom}' AND ${timeStampColumn}::date <= '${dateTo}'
          `
        }

        if (limit) {
          sqlWhere += ` LIMIT ${limit}`
        }

        if (offset) {
          sqlWhere += ` OFFSET ${offset}`
        }

        const sql = sqlSelect + sqlFrom + sqlWhere

        let result
        let resultCount
        try {
          result = await conn.execute(sql)
          let dataJson = result.rows

          let sqlCount
          if (count) {
            sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName}`
            resultCount = await conn.execute(sqlCount)
            dataJson.push(resultCount.rows[0]['count'])
          }

          await conn.disconnect()
          response.json(dataJson)
        } catch (error) {
          console.log(error)
        }
      },
      getAnalysisData: async (request, response) => {
        const {
          projectName,
          limit,
          offset,
          group,
          date,
          localization,
          area,
          count,
          viewId
        } = request.query
        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id: view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id})
        const uri = dataProvider.uri
        // const conn = new Connection(uri)
        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        const dataSet = dataSeries.dataSets[0]
        let tableName = dataSet.format.table_name

        let sqlTableName = `SELECT DISTINCT table_name FROM ${tableName}`
        resultTableName = await conn.execute(sqlTableName)

        tableName = resultTableName.rows[0]['table_name']

        let sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName} `
        let sqlSelect = `SELECT *`
        let sqlFrom = ''
        let sqlWhere = ''

        sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(intersection_geom), 4326)) AS "lat",
                      ST_X(ST_Transform (ST_Centroid(intersection_geom), 4326)) AS "long"
        `

        sqlFrom += ` FROM public.${tableName}`

        if (date) {
          const dateFrom = date[0];
          const dateTo = date[1];

          sqlWhere += `
              WHERE execution_date::date >= '${dateFrom}' AND execution_date::date <= '${dateTo}'
          `
        }
        sqlCount += sqlWhere

        if (area) {
          sqlWhere += ` AND calculated_area_ha > ${area}`
        }

        if (limit) {
          sqlWhere += ` LIMIT ${limit}`
        }

        if (offset) {
          sqlWhere += ` OFFSET ${offset}`
        }

        const sql = sqlSelect + sqlFrom + sqlWhere

        let result
        let resultCount
        try {
          result = await conn.execute(sql)
          let dataJson = result.rows

          if (count) {
            resultCount = await conn.execute(sqlCount)
            dataJson.push(resultCount.rows[0]['count'])
          }

          await conn.disconnect()
          response.json(dataJson)
        } catch (error) {
          console.log(error)
        }
      },
      getCarData: async (request, response) => {
        const {
          carRegister,
          date,
          viewId
        } = request.query

        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id: view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id})
        // const uri = dataProvider.uri
        const tableName = dataSeries.dataSets[0].format.table_name
        // const conn = new Connection(uri)
        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()
        let dateFrom = null;
        let dateTo = null;
        if (date) {
          dateFrom = date[0];
          dateTo = date[1];
        }

        // let sqlDeter = `
        //   SELECT orbitpoint, date, sensor, satellite FROM public.dd_deter_inpe di where di.gid = ${intersectId}
        // `
        let sql = `SELECT
            car.numero_do2 AS register,
            car.area_ha_ AS area,
            car.nome_da_p1 AS name,
            car.municipio1 AS city,
            substring(ST_EXTENT(munic.geom)::TEXT, 5, length(ST_EXTENT(munic.geom)::TEXT) - 5) as citybbox,
            substring(ST_EXTENT(car.geom)::TEXT, 5, length(ST_EXTENT(car.geom)::TEXT) - 5) as bbox,
            ST_Y(ST_Transform (ST_Centroid(car.geom), 4326)) AS "lat",
            ST_X(ST_Transform (ST_Centroid(car.geom), 4326)) AS "long"
            FROM public.${tableName} AS car
            INNER JOIN de_municipios_sema munic ON
            car.numero_do2 = '${carRegister}'
            AND munic.municipio = car.municipio1
            GROUP BY car.numero_do2, car.area_ha_, car.nome_da_p1, car.municipio1, car.geom`;

        const result = await conn.execute(sql)
        let propertyData = result.rows[0]

        const sqlBurningSpotlights = `
                    SELECT
                    count(*) as focuscount,
                    extract('YEAR' FROM focus.execution_date) as year
                    FROM public.apv_car_focos_48 focus
                    INNER JOIN public.${tableName} AS car on
                    focus.de_car_validado_sema_numero_do2 = car.numero_do2 AND
                    car.numero_do2 = '${carRegister}'
                    group by year
                  `

        const resultBurningSpotlights = await conn.execute(sqlBurningSpotlights)
        const burningSpotlights = resultBurningSpotlights.rows

        const sqlBurnedAreas = `
                    SELECT
                    count(*) as focuscount,
                    extract('YEAR' FROM focus.execution_date) as year
                    FROM public.apv_car_focos_48 focus
                    INNER JOIN public.${tableName} AS car on
                    focus.de_car_validado_sema_numero_do2 = car.numero_do2 AND
                    car.numero_do2 = '${carRegister}'
                    group by year
                  `

        // const resultDeter = await conn.execute(sqlDeter)
        // const deter = resultDeter.rows

        const resultBurnedAreas = await conn.execute(sqlBurnedAreas)
        const burnedAreas = resultBurnedAreas.rows

        const sqlProdesYear = `SELECT
                              extract(year from date_trunc('year', cp.execution_date)) AS date,
                              SUM(cp.calculated_area_ha) as area
                              FROM public.apv_car_prodes_40 cp
                              WHERE cp.de_car_validado_sema_numero_do2 = '${carRegister}'
                              GROUP BY date
                              ORDER BY date;`

        const sqlSpotlightsYear = `SELECT
                              extract(year from date_trunc('year', cf.execution_date)) AS date,
                              COUNT(cf.*) as spotlights
                              FROM public.apv_car_focos_48 cf
                              WHERE cf.de_car_validado_sema_numero_do2 = '${carRegister}'
                              GROUP BY date
                              ORDER BY date;`

        let dateSql = ` and execution_date::date >= '${dateFrom}' AND execution_date::date <= '${dateTo}'`

        const sqlIndigenousLand = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_ti_cardeter_68 where apv_car_deter_28_de_car_validado_sema_numero_do2 = '${carRegister}' ${dateSql}`
        const sqlConservationUnit = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_uc_carprodes_65 where apv_car_prodes_40_de_car_validado_sema_numero_do2 = '${carRegister}' ${dateSql}`
        const sqlLegalReserve = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_reserva_cardeter_37 where apv_car_deter_28_de_car_validado_sema_numero_do2 = '${carRegister}' ${dateSql}`
        const sqlAPP = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_app_cardeter_35 where apv_car_deter_28_de_car_validado_sema_numero_do2 = '${carRegister}' ${dateSql}`
        const sqlConsolidatedUse = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_usocon_cardeter_39 where apv_car_deter_28_de_car_validado_sema_numero_do2 = '${carRegister}' ${dateSql}`
        const sqlAnthropizedUse = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_usoant_cardeter_36 where apv_car_deter_28_de_car_validado_sema_numero_do2 = '${carRegister}' ${dateSql}`
        const sqlNativeVegetation = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_veg_cardeter_38 where apv_car_deter_28_de_car_validado_sema_numero_do2 = '${carRegister}' ${dateSql}`

        const resultIndigenousLand = await conn.execute(sqlIndigenousLand)
        const indigenousLand = resultIndigenousLand.rows

        const resultConservationUnit = await conn.execute(sqlConservationUnit)
        const conservationUnit = resultConservationUnit.rows

        const resultLegalReserve = await conn.execute(sqlLegalReserve)
        const legalReserve = resultLegalReserve.rows

        const resultAPP = await conn.execute(sqlAPP)
        const app = resultAPP.rows

        const resultConsolidatedUse = await conn.execute(sqlConsolidatedUse)
        const consolidatedArea = resultConsolidatedUse.rows

        const resultAnthropizedUse = await conn.execute(sqlAnthropizedUse)
        const anthropizedUse = resultAnthropizedUse.rows

        const resultNativeVegetation = await conn.execute(sqlNativeVegetation)
        const nativeVegetation = resultNativeVegetation.rows

        const resultProdesYear = await conn.execute(sqlProdesYear)
        const prodesYear = resultProdesYear.rows

        const resultSpotlightsYear = await conn.execute(sqlSpotlightsYear)
        const spotlightsYear = resultSpotlightsYear.rows

        if (propertyData) {
          propertyData.burningSpotlights = burningSpotlights
          propertyData.burnedAreas = burnedAreas
          // propertyData.deter = deter[0]
          propertyData.prodesYear = prodesYear
          propertyData.spotlightsYear = spotlightsYear
          propertyData.indigenousLand = indigenousLand[0]
          propertyData.conservationUnit = conservationUnit[0]
          propertyData.legalReserve = legalReserve[0]
          propertyData.app = app[0]
          propertyData.consolidatedArea = consolidatedArea[0]
          propertyData.anthropizedUse = anthropizedUse[0]
          propertyData.nativeVegetation = nativeVegetation[0]
        }

        await conn.disconnect()
        response.json(propertyData)
      },
      getAnalysisTotals: async (request, response) => {
        const params = {
          listAlert,
          codgroup,
          date,
          projectName,
          group,
          localization,
          area,
          count
        } = request.query;

        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt");
        await conn.connect();

        const alerts = params.listAlert && params.listAlert !== 'null' ?
            JSON.parse(params.listAlert) :
            [];
        let sql = '';

        if (alerts.length > 0) {
          for (let alert of alerts) {
            sql += sql.trim() === '' ? '' : ' UNION ALL ';
            if (alert.idview && alert.idview > 0 && alert.idview !== 'null') {
              const view = await ViewFacade.retrieve(alert.idview);

              const dataSeries = await DataManager.getDataSeries({id: view.data_series_id});
              const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id});
              const uri = dataProvider.uri;

              const dataSet = dataSeries.dataSets[0];
              let tableName = dataSet.format.table_name;

              let sqlTableName = ` SELECT DISTINCT table_name FROM ${tableName}`;
              resultTableName = await conn.execute(sqlTableName);

              tableName = resultTableName.rows[0]['table_name'];

              let sqlWhere = '';

              if (params.date && params.date !== "null") {
                const dateFrom = params.date[0];
                const dateTo = params.date[1];
                sqlWhere += ` WHERE execution_date BETWEEN '${dateFrom}' AND '${dateTo}' `
              }
              if (params.area && params.area !== "null") {
                sqlWhere += ` AND calculated_area_ha > ${params.area} `
              }

              const value1 = alert.codgroup === 'FOCOS' ?
                  ` (  SELECT ROW_NUMBER() OVER(ORDER BY de_car_validado_sema_numero_do1 ASC) AS Row
                         FROM public.${tableName}
                         ${sqlWhere}
                         GROUP BY de_car_validado_sema_numero_do1
                         ORDER BY Row DESC
                         LIMIT 1
                      ) AS value1 ` :
                  `  COALESCE(COUNT(1), 00.00) AS value1 `;

              const value2 = alert.codgroup === 'FOCOS' ?
                  ` (  SELECT coalesce(sum(1), 0.00) as num_focos
                         FROM public.${tableName}
                         ${sqlWhere} 
                      ) AS value2 ` :
                  ` COALESCE(SUM(calculated_area_ha), 0.00) AS value2 `;

              const sqlFrom = alert.codgroup === 'FOCOS' ?
                  ` ` :
                  ` FROM public.${tableName} ${sqlWhere} `;

              sql += `SELECT 
                          '${alert.idview}' AS idview,
                          '${alert.cod}' AS cod,
                          '${alert.codgroup}' AS codgroup,
                          '${alert.label}' AS label,
                          ${value1},
                          ${value2},
                          ${alert.selected} AS selected,
                          ${alert.activearea} AS activearea,
                          false AS immobilitactive,
                          null AS alertsgraphics 
                    ${sqlFrom}`;
            } else {
              sql += ` SELECT 
                        '${alert.idview}' AS idview,
                        '${alert.cod}' AS cod,
                        '${alert.codgroup}' AS codgroup,
                        '${alert.label}' AS label,
                        0.00 AS value1,
                        00.00 AS value2 ,
                        ${alert.selected} AS selected,
                        ${alert.activearea} AS activearea,
                        false AS immobilitactive,
                        null AS alertsgraphics `;
            }
          }
        }
        try {
          const result = await conn.execute(sql);

          await conn.disconnect();
          response.json(result.rows);
        } catch (error) {
          console.log(error);
        }
      },
      getDetailsAnalysisTotals: async (request, response) => {
        const params = {
          listAlert,
          codgroup,
          date,
          projectName,
          group,
          localization,
          area,
          count,
          limit
        } = request.query;

        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt");
        await conn.connect();

        const alerts = params.listAlert && params.listAlert !== 'null' ?
            JSON.parse(params.listAlert) :
            [];

        const result = [];
        const tableOwner = await getTableOwner(conn, alerts);

        if (alerts.length > 0) {
          for (let alert of alerts) {
            const sql = await getSql(conn, alert, tableOwner, params);

            let resultAux = await conn.execute(sql.sql1);
            const graphic1 = await setGraphic(resultAux, sql.value1, sql.subtitle);

            resultAux = await conn.execute(sql.sql2);
            const graphic2 = await setGraphic(resultAux, sql.value1, sql.subtitle);

            result.push(setAlertGraphic(alert, graphic1, graphic2));
          }
        }

        try {
          await conn.disconnect();
          response.json(result);
        } catch (error) {
          console.log(error);
        }
      }
    }
} ();
