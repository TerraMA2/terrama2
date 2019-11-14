(function() {
  'use strict';

  const DataManager = require('../../core/DataManager');
  const ViewFacade = require("../../core/facade/View");

  function generate_color() {
    const hexadecimal = '0123456789ABCDEF';
    let color = '#';
    const date = new Date();
    const milliseconds = date.getMilliseconds();

    for (var i = 0; i < 6; i++) {
      color += hexadecimal[Math.floor(Math.random(milliseconds) * 16)];
    }
    return color;
  };
  function getColumns(alert, tableOwner) {
    let column1 = '';
    let column2 = '';
    let column3 = '';
    let column4 = '';
    const columnArea = 'main_table.calculated_area_ha';

    const filterColumns = {
      columnDate: 'main_table.execution_date',
      columnsTheme: {
        biomes: (alert.isAnalysis && alert.isPrimary) ? ` main_table.dd_focos_inpe_bioma ` : ` main_table.${tableOwner}_dd_focos_inpe_bioma `,
        geocod: (alert.isAnalysis && alert.isPrimary) ? ` main_table.dd_focos_inpe_id_2 ` : ` main_table.${tableOwner}_dd_focos_inpe_id_2 `,
        mesoregion: 'de_mesoregiao_ibge_gid',
        microregion: 'de_microregiao_ibge_gid',
        region: 'de_comarca_ibge_gid',
        uc: '',
        ti: ''
      },
      columnsAlertType: {
        area: '',
        burning: ''
      },
      columnsauthorization: {},
      comumnsSpecificSearch: {
        car: '',
        cpfCnpj: ''
      }
    }

    if (alert.codgroup && alert.codgroup === 'FOCOS') {
      if (alert.isAnalysis && alert.isPrimary) {
        column1 = ` main_table.de_car_validado_sema_numero_do1 `;
        column2 = ` main_table.dd_focos_inpe_bioma `;
        column3 = '1';
        column4 = ` main_table.dd_focos_inpe_bioma `;
      } else {
        column1 = ` main_table.${tableOwner}_de_car_validado_sema_numero_do1 `;
        column2 = ` main_table.${tableOwner}_dd_focos_inpe_bioma `;
        column3 = '1';
        column4 = ` main_table.${tableOwner}_dd_focos_inpe_bioma `;
      }
    } else if (alert.codgroup && alert.codgroup === 'DETER') {
      if (alert.isAnalysis && alert.isPrimary) {
        column1 = ` main_table.de_car_validado_sema_numero_do1 `;
        column2 = ` main_table.dd_deter_inpe_classname `;
      } else {
        column1 = ` main_table.${tableOwner}_de_car_validado_sema_numero_do1 `;
        column2 = ` main_table.${tableOwner}_dd_deter_inpe_classname `;
      }

      column3 = alert.activearea ? ' main_table.calculated_area_ha ' : '1';
    } else if (alert.codgroup && alert.codgroup === 'PRODES') {
      if (alert.isAnalysis && alert.isPrimary) {
        column1 = ` main_table.de_car_validado_sema_numero_do1 `;
        column2 = ` main_table.dd_prodes_inpe_mainclass `;
      } else {
        column1 = ` main_table.${tableOwner}_de_car_validado_sema_numero_do1 `;
        column2 = ` main_table.${tableOwner}_dd_prodes_inpe_mainclass `;
      }

      column3 = alert.activearea ? ' main_table.calculated_area_ha ' : '1';
    }

    return {column1, column2, column3, column4, filterColumns, columnArea};
  };
  function getValues(analyze) {
    const values = {columnValue: '', columnValueFocos: ''};
    if (analyze.valueOption && analyze.valueOption.value) {
      switch (analyze.valueOption.value) {
        case 1 :
          values.columnValue = ` <= 5 `;
          values.columnValueFocos = ` BETWEEN 0 AND 10 `;
          break;
        case 2:
          values.columnValue = ` BETWEEN 5 AND 25 `;
          values.columnValueFocos = ` BETWEEN 10 AND 20 `;
          break;
        case 3:
          values.columnValue = ` BETWEEN 25 AND 50 `;
          values.columnValueFocos = ` BETWEEN 20 AND 50 `;
          break;
        case 4:
          values.columnValue = ` BETWEEN 50 AND 100 `;
          values.columnValueFocos = ` BETWEEN 50 AND 100 `;
          break;
        case 5:
          values.columnValue = ` >= 100 `;
          values.columnValueFocos = ` > 100 `;
          break;
        case 6:
          values.columnValue = ` > ${analyze.valueOptionBiggerThen} `;
          values.columnValueFocos = ` > ${analyze.valueOptionBiggerThen} `;
          break;
      }
    }
    return values;
  };
  async function getFilter(conn, table, params, alert, columns) {
    const filter = params.filter && params.filter !== 'null' ? JSON.parse(params.filter) : {};
    const srid = await conn.execute(`SELECT ST_SRID(intersection_geom) AS srid FROM ${table} LIMIT 1`);

    let sqlWhere = '';
    let secondaryTables = '';
    let sqlHaving = '';

    if (params.date && params.date !== "null") {
      const dateFrom = params.date[0];
      const dateTo = params.date[1];
      sqlWhere += ` WHERE main_table.execution_date BETWEEN '${dateFrom}' AND '${dateTo}' `
    }

    if (filter) {
      if (filter.specificSearch && filter.specificSearch.isChecked) {
        if (filter.specificSearch.CarCPF === 'CAR') {
          sqlWhere += ` AND ${columns.column1} like '${filter.specificSearch.inputValue}' `;
        } else if (filter.specificSearch.CarCPF === 'CPF') {
          // Missing table associating CARs with CPFCNPJ
          sqlWhere += ` `;
        }
      } else {
        if (filter.themeSelected && filter.themeSelected.type) {
          if (filter.themeSelected.type === 'biome') {
            if (alert.codgroup === 'FOCOS') {
              sqlWhere += ` AND ${columns.filterColumns.columnsTheme.biomes} like '${filter.themeSelected.value.name}' `;
            } else {
              secondaryTables += ' , public.de_biomas_mt biome ';

              const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_biomas_mt LIMIT 1`);
              const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'biome.geom' : ` st_transform(biome.geom, ${srid.rows[0].srid}) ` ;

              sqlWhere += ` AND st_intersects(main_table.intersection_geom, ${fieldIntersects}) `;
              sqlWhere += ` AND biome.gid = ${filter.themeSelected.value.gid} `;
            }
          } else if (filter.themeSelected.type === 'region') {
            secondaryTables += ' , public.de_municipios_sema county ';

            if (alert.codgroup === 'FOCOS') {
              sqlWhere += ` AND county.comarca like '${filter.themeSelected.value.name}' `;
              sqlWhere += ` AND ${columns.filterColumns.columnsTheme.geocod} = cast(county.geocodigo AS integer) `;
            } else {
              const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_municipios_sema LIMIT 1`);
              const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? ' county.geom ' : ` st_transform(county.geom, ${srid.rows[0].srid}) ` ;

              sqlWhere += ` AND st_intersects(main_table.intersection_geom, ${fieldIntersects}) `;
              sqlWhere += ` AND county.comarca = '${filter.themeSelected.value.name}'  `;
            }
          } else if (filter.themeSelected.type === 'mesoregion') {
            secondaryTables += ' , public.de_municipios_sema county ';

            if (alert.codgroup === 'FOCOS') {
              sqlWhere += ` AND county.nm_meso like '${filter.themeSelected.value.name}' `;
              sqlWhere += ` AND ${columns.filterColumns.columnsTheme.geocod} = cast(county.geocodigo AS integer) `;
            } else {
              const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_municipios_sema LIMIT 1`);
              const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'county.geom' : ` st_transform(county.geom, ${srid.rows[0].srid}) ` ;

              sqlWhere += ` AND st_intersects(main_table.intersection_geom, ${fieldIntersects}) `;
              sqlWhere += ` AND county.nm_meso = '${filter.themeSelected.value.name}' `;
            }
          } else if (filter.themeSelected.type === 'microregion') {
            secondaryTables += ' , public.de_municipios_sema county ';
            if (alert.codgroup === 'FOCOS') {
              sqlWhere += ` AND county.nm_micro like '${filter.themeSelected.value.name}' `;
              sqlWhere += ` AND ${columns.filterColumns.columnsTheme.geocod} = cast(county.geocodigo AS integer) `;
            } else {
              const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_municipios_sema LIMIT 1`);
              const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'county.geom' : ` st_transform(county.geom, ${srid.rows[0].srid}) ` ;

              sqlWhere += ` AND st_intersects(main_table.intersection_geom, ${fieldIntersects}) `;
              sqlWhere += ` AND county.nm_micro = '${filter.themeSelected.value.name}'  `;
            }
          } else if (filter.themeSelected.type === 'city') {
            if (alert.codgroup === 'FOCOS') {
              sqlWhere += ` AND ${columns.filterColumns.columnsTheme.geocod} = ${filter.themeSelected.value.geocodigo} `;
            } else {
              secondaryTables += ' , public.de_municipios_sema county ';

              const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_municipios_sema LIMIT 1`);
              const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'county.geom' : ` st_transform(county.geom, ${srid.rows[0].srid}) ` ;

              sqlWhere += ` AND st_intersects(main_table.intersection_geom, ${fieldIntersects}) `;
              sqlWhere += ` AND county.gid = ${filter.themeSelected.value.gid} `;
            }
          } else if (filter.themeSelected.type === 'uc') {
            secondaryTables += ' , public.de_unidade_cons_sema uc ';

            const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_unidade_cons_sema LIMIT 1`);
            const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'uc.geom' : ` st_transform(uc.geom, ${srid.rows[0].srid}) ` ;

            sqlWhere += ` AND st_intersects(main_table.intersection_geom,${fieldIntersects}) `;

            if (filter.themeSelected.value.gid > 0) {
              sqlWhere += ` AND uc.gid = ${filter.themeSelected.value.gid} `;
            }
          } else if (filter.themeSelected.type === 'ti') {
            secondaryTables += ' , public.de_terra_indigena_sema ti ';

            const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_terra_indigena_sema LIMIT 1`);
            const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'ti.geom' : ` st_transform(ti.geom, ${srid.rows[0].srid}) ` ;

            sqlWhere += ` AND st_intersects(main_table.intersection_geom, ${fieldIntersects}) `;

            if (filter.themeSelected.value.gid > 0) {
              sqlWhere += ` AND ti.gid = ${filter.themeSelected.value.gid} `;
            }

          } else if (filter.themeSelected.type === 'projus') {
            secondaryTables += ' , public.de_projus_bacias_sema projus ';

            const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_projus_bacias_sema LIMIT 1`);
            const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'projus.geom' : ` st_transform(projus.geom, ${srid.rows[0].srid}) ` ;

            sqlWhere += ` AND st_intersects(main_table.intersection_geom, ${fieldIntersects}) `;

            if (filter.themeSelected.value.gid > 0) {
              sqlWhere += ` AND projus.gid = ${filter.themeSelected.value.gid} `;
            }
          }
        };

        if (filter.alertType && (filter.alertType.radioValue !== 'ALL') && (filter.alertType.analyzes.length > 0)) {
          filter.alertType.analyzes.forEach(analyze => {
            const values = getValues(analyze);

            if (analyze.valueOption && analyze.valueOption.value) {
              if ((analyze.type && analyze.type === 'deter') && (alert.codgroup === 'DETER')) {
                sqlWhere += ` AND main_table.calculated_area_ha ${values.columnValue} `;
              };

              if ((analyze.type && analyze.type === 'deforestation') && (alert.codgroup === 'PRODES')) {
                sqlWhere += ` AND main_table.calculated_area_ha ${values.columnValue} `;
              };

              if ((analyze.type && analyze.type === 'burned') && (alert.codgroup === 'FOCOS')) {
                sqlHaving += ` HAVING count(1) ${values.columnValueFocos} `;
              };

              if ((analyze.type && analyze.type === 'burned_area') && (alert.codgroup === 'AREA_QUEIMADA')) {
                sqlWhere += ` AND main_table.calculated_area_ha ${values.columnValue} `;
              };

              if ((analyze.type && analyze.type === 'car_area')) {
                secondaryTables += ' , public.de_car_validado_sema car ';
                sqlWhere += ` AND car.area_ha_ ${values.columnValue} `;
                sqlWhere += ` AND car.numero_do1 = ${columns.column1} `;
              };
            };
          });
        }
      }
    }

    return {sqlWhere, secondaryTables, sqlHaving};
  };
  async function getTableOwner(conn, alerts) {
    if (alerts.length > 0) {
      for (let alert of alerts) {
        if (alert.idview && alert.idview > 0 && alert.idview !== 'null' && alert.isAnalysis && alert.isPrimary) {
          return await getTable(conn, alert.idview);
        }
      }
    }
    return '';
  };
  async function getTable(conn, idView) {
    const view = await ViewFacade.retrieve(idView);

    const dataSeries = await DataManager.getDataSeries({id: view.data_series_id});
    const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id});
    const uri = dataProvider.uri;

    const dataSet = dataSeries.dataSets[0];
    const tableName = dataSet.format.table_name;

    const sqlTableName = ` SELECT DISTINCT table_name FROM ${tableName}`;
    const resultTableName = await conn.execute(sqlTableName);

    return resultTableName.rows[0]['table_name'];
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
  async function setGraphic(resultAux, value1, subtitle) {
    let labels = [];
    let data = [];
    let backgroundColor = [];
    let hoverBackgroundColor = [];

    resultAux.rows.forEach(value => {
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
  async function getSqlDetailsAnalysisTotals(conn, alert, tableOwner, params) {
    let sql1 = '';
    let sql2 = '';

    const value1 = 'value';
    const subtitle = 'subtitle';

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

      const sqlFrom = ` FROM public.${table} AS main_table`;

      const filter = await getFilter(conn, table, params, alert, columns);

      const sqlGroupBy1 = ` GROUP BY ${columns.column1}  `;
      const sqlGroupBy2 = ` GROUP BY ${columns.column2}  `;
      const sqlOrderBy = ` ORDER BY ${value1} DESC `;
      const sqlLimit = ` LIMIT ${limit} `;

      sql1 += ` SELECT ${columnsFor1} ${sqlFrom} ${filter.secondaryTables} ${filter.sqlWhere} ${sqlGroupBy1} ${filter.sqlHaving} ${sqlOrderBy} ${sqlLimit} `;
      sql2 += ` SELECT ${columnsFor2} ${sqlFrom} ${filter.secondaryTables} ${filter.sqlWhere} ${sqlGroupBy2} ${filter.sqlHaving} ${sqlOrderBy} ${sqlLimit} `;
    } else {
      sql1 += ` SELECT 
                                        ' --- ' AS ${subtitle},
                                        0.00 AS ${value1} `;
      sql2 += ` SELECT 
                                        ' --- ' AS ${subtitle},
                                        0.00 AS ${value1} `;
    }
    return {sql1, sql2, value1, subtitle};
  };

  const Filter = module.exports = {};

  Filter.getSqlAnalysisTotals = async function(conn, params) {
    const alerts = params.specificParameters && params.specificParameters !== 'null' ?
        JSON.parse(params.specificParameters) : [];

    let sql = '';
    if (alerts.length > 0) {
      for (let alert of alerts) {
        sql += sql.trim() === '' ? '' : ' UNION ALL ';

        if (alert.idview && alert.idview > 0 && alert.idview !== 'null') {

          const tableName = await getTable(conn, alert.idview);

          const collumns = getColumns(alert, '');

          const filter = await getFilter(conn, tableName, params, alert, collumns);

          const value1 = alert.codgroup === 'FOCOS' ?
              ` COALESCE( ( SELECT ROW_NUMBER() OVER(ORDER BY ${collumns.column1} ASC) AS Row
                                         FROM public.${tableName} AS main_table
                                         ${filter.secondaryTables}
                                         ${filter.sqlWhere}
                                         GROUP BY ${collumns.column1}
                                         ${filter.sqlHaving}
                                         ORDER BY Row DESC
                                         LIMIT 1
                                      ), 00.00) AS value1 ` :
              `  COALESCE(COUNT(1), 00.00) AS value1 `;

          const value2 = alert.codgroup === 'FOCOS' ?
              ` ( SELECT coalesce(sum(1), 0.00) as num_focos
                                   FROM public.${tableName} AS main_table
                                   ${filter.secondaryTables}
                                   ${filter.sqlWhere}
                                      ) AS value2 ` :
              ` COALESCE(SUM(${collumns.columnArea}), 0.00) AS value2 `;

          const sqlFrom = alert.codgroup === 'FOCOS' ?
              ` ` :
              ` FROM public.${tableName} AS main_table ${filter.secondaryTables} ${filter.sqlWhere} `;

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

    return sql;
  };
  Filter.getAlertsGraphics = async function (conn, params) {
    const alerts = params.specificParameters && params.specificParameters !== 'null' ?
        JSON.parse(params.specificParameters) : [];

    const result = [];
    const tableOwner = await getTableOwner(conn, alerts);

    if (alerts.length > 0) {
      for (let alert of alerts) {
        const sql = await getSqlDetailsAnalysisTotals(conn, alert, tableOwner, params);

        let resultAux = await conn.execute(sql.sql1);
        const graphic1 = await setGraphic(resultAux, sql.value1, sql.subtitle);

        resultAux = await conn.execute(sql.sql2);
        const graphic2 = await setGraphic(resultAux, sql.value1, sql.subtitle);

        result.push(setAlertGraphic(alert, graphic1, graphic2));
      }
    }
    return result;
  }
} ());
