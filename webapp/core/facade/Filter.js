(function() {
  'use strict';

  const DataManager = require('../../core/DataManager');
  const ViewFacade = require("../../core/facade/View");

  function addAND(sqlWhere) {
    return sqlWhere.trim() ? `AND ` : '';
  };
  function isDeter(analyze, view) {
    return ((analyze.type === 'deter') && (view.codgroup === 'DETER'));
  };
  function isDeforestation(analyze, view) {
    return ((analyze.type && analyze.type === 'deforestation') && (view.codgroup === 'PRODES'));
  };
  function isBurned(analyze, view) {
    return ((analyze.type && analyze.type === 'burned') && (view.codgroup === 'FOCOS'));
  };
  function isBurnedArea(analyze, view) {
    return ((analyze.type && analyze.type === 'burned_area') && (view.codgroup === 'AREA_QUEIMADA'));
  };
  function isCarArea(type){
    return (type === 'carArea');
  };
  const specificSearch = {
    car: async function(conn, sql, filter, columns, cod, aliasTablePrimary) {
      sql.secondaryTables = '';
      sql.sqlWhere += ` ${addAND(sql.sqlWhere)} ${columns.column1} like '${filter.specificSearch.inputValue}' `;
    },
    cpf: async function(conn, sql, filter, columns, cod, aliasTablePrimary) {
      sql.secondaryTables = '';
      sql.sqlWhere += ` ${addAND(sql.sqlWhere)} ${columns.columnCpfCnpj} like '%${filter.specificSearch.inputValue}%' `;
    }
  };
  const themeSelected = {
    biome: async function(conn, sql, filter, columns, cod, aliasTablePrimary, srid) {
      const codGoup = {
        focos: async function() {
          sql.sqlWhere += ` ${addAND(sql.sqlWhere)} ${columns.filterColumns.columnsTheme.biomes} like '${filter.themeSelected.value.name}' `;
        },
        others: async function() {
          sql.secondaryTables += ' , public.de_biomas_mt biome ';

          const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_biomas_mt LIMIT 1`);
          const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'biome.geom' : ` st_transform(biome.geom, ${srid.rows[0].srid}) ` ;

          sql.sqlWhere += ` AND ST_Intersects(ST_CollectionExtract(${aliasTablePrimary}.intersection_geom, 3), ${fieldIntersects}) `;
          sql.sqlWhere += ` AND biome.gid = ${filter.themeSelected.value.gid} `;
        }
      };

      await codGoup[cod]();
    },
    region: async function(conn, sql, filter, columns, cod, aliasTablePrimary, srid){
      sql.secondaryTables += ' , public.de_municipios_sema county ';
      const codGoup = {
        focos: async function() {
          sql.sqlWhere += ` AND county.comarca like '${filter.themeSelected.value.name}' `;
          sql.sqlWhere += ` AND ${columns.filterColumns.columnsTheme.geocod} = cast(county.geocodigo AS integer) `;
        },
        others: async function() {
          const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_municipios_sema LIMIT 1`);
          const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? ' county.geom ' : ` st_transform(county.geom, ${srid.rows[0].srid}) ` ;

          sql.sqlWhere += ` AND ST_Intersects(ST_CollectionExtract(${aliasTablePrimary}.intersection_geom, 3), ${fieldIntersects}) `;
          sql.sqlWhere += ` AND county.comarca = '${filter.themeSelected.value.name}'  `;
        }
      };

      await codGoup[cod]();
    },
    mesoregion: async function(conn, sql, filter, columns, cod, aliasTablePrimary, srid){
      sql.secondaryTables += ' , public.de_municipios_sema county ';
      const codGoup = {
        focos: async function() {
          sql.sqlWhere += ` AND county.nm_meso like '${filter.themeSelected.value.name}' `;
          sql.sqlWhere += ` AND ${columns.filterColumns.columnsTheme.geocod} = cast(county.geocodigo AS integer) `;
        },
        others: async function() {
          const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_municipios_sema LIMIT 1`);
          const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'county.geom' : ` st_transform(county.geom, ${srid.rows[0].srid}) ` ;

          sql.sqlWhere += ` AND ST_Intersects(ST_CollectionExtract(${aliasTablePrimary}.intersection_geom, 3), ${fieldIntersects}) `;
          sql.sqlWhere += ` AND county.nm_meso = '${filter.themeSelected.value.name}' `;
        }
      };

      await codGoup[cod]();
    },
    microregion: async function(conn, sql, filter, columns, cod, aliasTablePrimary, srid){
      sql.secondaryTables += ' , public.de_municipios_sema county ';
      const codGoup = {
        focos: async function() {
          sql.sqlWhere += ` AND county.nm_micro like '${filter.themeSelected.value.name}' `;
          sql.sqlWhere += ` AND ${columns.filterColumns.columnsTheme.geocod} = cast(county.geocodigo AS integer) `;
        },
        others: async function() {
          const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_municipios_sema LIMIT 1`);
          const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'county.geom' : ` st_transform(county.geom, ${srid.rows[0].srid}) ` ;

          sql.sqlWhere += ` AND ST_Intersects(ST_CollectionExtract(${aliasTablePrimary}.intersection_geom, 3), ${fieldIntersects}) `;
          sql.sqlWhere += ` AND county.nm_micro = '${filter.themeSelected.value.name}'  `;
        }
      };
      await codGoup[cod]();
    },
    city: async function(conn, sql, filter, columns, cod, aliasTablePrimary, srid){
      const codGoup = {
        focos: async function() {
          sql.sqlWhere += ` AND ${columns.filterColumns.columnsTheme.geocod} = ${filter.themeSelected.value.geocodigo} `;
        },
        others: async function() {
          sql.secondaryTables += ' , public.de_municipios_sema county ';

          const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_municipios_sema LIMIT 1`);
          const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'county.geom' : ` st_transform(county.geom, ${srid.rows[0].srid}) ` ;

          sql.sqlWhere += ` AND ST_Intersects(ST_CollectionExtract(${aliasTablePrimary}.intersection_geom, 3), ${fieldIntersects}) `;
          sql.sqlWhere += ` AND county.gid = ${filter.themeSelected.value.gid} `;
        }
      };
      await codGoup[cod]();
    },
    uc: async function(conn, sql, filter, columns, cod, aliasTablePrimary, srid){
      sql.secondaryTables += ' , public.de_unidade_cons_sema uc ';

      const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_unidade_cons_sema LIMIT 1`);
      const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'uc.geom' : ` st_transform(uc.geom, ${srid.rows[0].srid}) ` ;

      sql.sqlWhere += ` AND ST_Intersects(ST_CollectionExtract(${aliasTablePrimary}.intersection_geom, 3), ${fieldIntersects}) `;

      if (filter.themeSelected.value.gid > 0) {
        sql.sqlWhere += ` AND uc.gid = ${filter.themeSelected.value.gid} `;
      }
    },
    ti: async function(conn, sql, filter, columns, cod, aliasTablePrimary, srid){
      sql.secondaryTables += ' , public.de_terra_indigena_sema ti ';

      const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_terra_indigena_sema LIMIT 1`);
      const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'ti.geom' : ` st_transform(ti.geom, ${srid.rows[0].srid}) ` ;

      sql.sqlWhere += ` AND ST_Intersects(ST_CollectionExtract(${aliasTablePrimary}.intersection_geom, 3), ${fieldIntersects}) `;

      if (filter.themeSelected.value.gid > 0) {
        sql.sqlWhere += ` AND ti.gid = ${filter.themeSelected.value.gid} `;
      }
    },
    projus: async function(conn, sql, filter, columns, cod, aliasTablePrimary, srid){
      sql.secondaryTables += ' , public.de_projus_bacias_sema projus ';

      const sridSec = await conn.execute(`SELECT ST_SRID(geom) AS srid FROM public.de_projus_bacias_sema LIMIT 1`);
      const fieldIntersects =(srid.rows[0].srid === sridSec.rows[0].srid) ? 'projus.geom' : ` st_transform(projus.geom, ${srid.rows[0].srid}) ` ;

      sql.sqlWhere += ` ${addAND(sql.sqlWhere)} ST_Intersects(ST_CollectionExtract(${aliasTablePrimary}.intersection_geom, 3), ${fieldIntersects}) `;

      if (filter.themeSelected.value.gid > 0) {
        sql.sqlWhere += ` ${addAND(sql.sqlWhere)} projus.gid = ${filter.themeSelected.value.gid} `;
      }
    }
  };
  function alertType(analyzes, sql, columns, aliasTablePrimary, view) {
    analyzes.forEach(analyze => {
      if (analyze.valueOption && analyze.valueOption.value && analyze.type) {
        const values = getValues(analyze);
        const alertType = {
          burned() {
            sql.sqlHaving += ` HAVING count(1) ${values.columnValueFocos} `;
          },
          carArea() {
            sql.secondaryTables += ' , public.de_car_validado_sema car ';
            sql.sqlWhere += ` ${addAND(sql.sqlWhere)} car.area_ha_ ${values.columnValue} `;
            sql.sqlWhere += ` ${addAND(sql.sqlWhere)} car.numero_do1 = ${columns.column1} `;
          },
          others() {
            sql.sqlWhere += ` ${addAND(sql.sqlWhere)} ${aliasTablePrimary}.calculated_area_ha ${values.columnValue} `;
          }
        };

        if (isDeter(analyze, view) || isBurnedArea(analyze, view) || isDeforestation(analyze, view)) {
          alertType['others']();
        }

        if (isBurned(analyze, view)) {
          alertType[analyze.type]();
        }

        if (isCarArea(analyze.type)) {
          alertType[analyse.type]();
        }
      }
    });
  };
  const setFilter = {
    specificSearch: async function(conn, sql, filter, columns, cod, table, view) {
      await specificSearch[filter.specificSearch.CarCPF.toLowerCase()](conn, sql, filter, columns, table.alias) ;
    },
    others: async function(conn, sql, filter, columns, cod, table, view) {
      if (filter.themeSelected && filter.themeSelected.type) {
        const srid = await conn.execute(
            ` SELECT ST_SRID(${table.alias}.intersection_geom) AS srid 
              FROM public.${table.name} AS ${table.alias} LIMIT 1`);

        await themeSelected[filter.themeSelected.type](conn, sql, filter, columns, cod, table.alias, srid);
      }

      if (filter.alertType && (filter.alertType.radioValue !== 'ALL') && (filter.alertType.analyzes.length > 0)) {
        alertType(filter.alertType.analyzes, sql, columns, table.alias, view);
      }
    }
  };
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
  function getColumns(view, tableOwner, aliasTablePrimary) {
    let column1 = '';
    let column2 = '';
    let column3 = '';
    let column4 = '';

    const columnCpfCnpj =
      (view.isAnalysis && view.isPrimary) ?
        ` ${aliasTablePrimary}.de_car_validado_sema_cpfcnpj ` :
        ` ${aliasTablePrimary}.${tableOwner}_de_car_validado_sema_cpfcnpj `;

    const columnArea = `${aliasTablePrimary}.calculated_area_ha`;

    const filterColumns = {
      columnDate: `${aliasTablePrimary}.execution_date`,
      columnsTheme: {
        biomes: (view.isAnalysis && view.isPrimary) ? ` ${aliasTablePrimary}.dd_focos_inpe_bioma ` : ` ${aliasTablePrimary}.${tableOwner}_dd_focos_inpe_bioma `,
        geocod: (view.isAnalysis && view.isPrimary) ? ` ${aliasTablePrimary}.dd_focos_inpe_id_2 ` : ` ${aliasTablePrimary}.${tableOwner}_dd_focos_inpe_id_2 `,
        mesoregion: 'de_mesoregiao_ibge_gid',
        microregion: 'de_microregiao_ibge_gid',
        region: 'de_comarca_ibge_gid'
      }
    };

    if (view.codgroup && view.codgroup === 'FOCOS') {
      if (view.isAnalysis && view.isPrimary) {
        column1 = ` ${aliasTablePrimary}.de_car_validado_sema_numero_do1 `;
        column2 = ` ${aliasTablePrimary}.dd_focos_inpe_bioma `;
        column3 = '1';
        column4 = ` ${aliasTablePrimary}.dd_focos_inpe_bioma `;
      } else {
        column1 = ` ${aliasTablePrimary}.${tableOwner}_de_car_validado_sema_numero_do1 `;
        column2 = ` ${aliasTablePrimary}.${tableOwner}_dd_focos_inpe_bioma `;
        column3 = '1';
        column4 = ` ${aliasTablePrimary}.${tableOwner}_dd_focos_inpe_bioma `;
      }
    } else if (view.codgroup && view.codgroup === 'DETER') {
      if (view.isAnalysis && view.isPrimary) {
        column1 = ` ${aliasTablePrimary}.de_car_validado_sema_numero_do1 `;
        column2 = ` ${aliasTablePrimary}.dd_deter_inpe_classname `;
      } else {
        column1 = ` ${aliasTablePrimary}.${tableOwner}_de_car_validado_sema_numero_do1 `;
        column2 = ` ${aliasTablePrimary}.${tableOwner}_dd_deter_inpe_classname `;
      }

      column3 = view.activearea ? ` ${aliasTablePrimary}.calculated_area_ha ` : '1';
    } else if (view.codgroup && view.codgroup === 'PRODES') {
      if (view.isAnalysis && view.isPrimary) {
        column1 = ` ${aliasTablePrimary}.de_car_validado_sema_numero_do1 `;
        column2 = ` ${aliasTablePrimary}.dd_prodes_inpe_mainclass `;
      } else {
        column1 = ` ${aliasTablePrimary}.${tableOwner}_de_car_validado_sema_numero_do1 `;
        column2 = ` ${aliasTablePrimary}.${tableOwner}_dd_prodes_inpe_mainclass `;
      }

      column3 = view.activearea ? ` ${aliasTablePrimary}.calculated_area_ha ` : '1';
    }

    return {column1, column2, column3, column4, filterColumns, columnArea, columnCpfCnpj};
  };
  function getValues(analyze) {
    const values = {columnValue: '', columnValueFocos: ''};
    const setValue = {
      1() {
        values.columnValue = ` <= 5 `;
        values.columnValueFocos = ` BETWEEN 0 AND 10 `;
      },
      2() {
        values.columnValue = ` BETWEEN 5 AND 25 `;
        values.columnValueFocos = ` BETWEEN 10 AND 20 `;
      },
      3() {
        values.columnValue = ` BETWEEN 25 AND 50 `;
        values.columnValueFocos = ` BETWEEN 20 AND 50 `;
      },
      4() {
        values.columnValue = ` BETWEEN 50 AND 100 `;
        values.columnValueFocos = ` BETWEEN 50 AND 100 `;
      },
      5() {
        values.columnValue = ` >= 100 `;
        values.columnValueFocos = ` > 100 `;
      },
      6() {
        values.columnValue = ` > ${analyze.valueOptionBiggerThen} `;
        values.columnValueFocos = ` > ${analyze.valueOptionBiggerThen} `;
      }
    };

    setValue[analyze.valueOption.value.toString()]();
    return values;
  };
  async function getFilter(conn, table, params, view, columns) {
    const filter = params.filter && params.filter !== 'null' ? JSON.parse(params.filter) : {};

    const sql = {
      sqlWhere: '',
      secondaryTables: '',
      sqlHaving: '',
      order: '',
      limit: '',
      offset: ''
    };

    if (params.date && params.date !== "null") {
      const dateFrom = params.date[0];
      const dateTo = params.date[1];
      sql.sqlWhere += ` WHERE ${table.alias}.execution_date BETWEEN '${dateFrom}' AND '${dateTo}' `
    }

    if (filter) {
      const filtered = filter.specificSearch && filter.specificSearch.isChecked ? 'specificSearch' : 'others';

      const cod = (view.codgroup === 'FOCOS') ? 'focos' : 'others';
      await setFilter[filtered](conn, sql, filter, columns, cod, table, view);
    }

    sql.order = (params.sortColumn && params.sortOrder) ? ` ORDER BY ${params.sortColumn} ${params.sortOrder === '1'?'ASC':'DESC'} ` : ``;
    sql.limit = (params.limit) ? ` LIMIT ${params.limit} ` : ``;
    sql.offset = (params.offset) ? ` OFFSET ${params.offset} ` : ``;

    return sql;
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
    let resultTableName = { rows: [] };

    try {
      resultTableName = await conn.execute(sqlTableName);
    } catch(e) {
      resultTableName.rows.push({table_name: tableName});
    }

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

      const table = {
        name: await getTable(conn, alert.idview),
        alias: 'main_table',
        owner: tableOwner
      };

      const columns = getColumns(alert, tableOwner, table.alias);

      const limit = params.limit && params.limit !== 'null' && params.limit > 0 ?
          params.limit :
          10;


      const columnsFor1 =
          `   ${columns.column1} AS ${subtitle},
                                      COALESCE(SUM(${columns.column3})) AS ${value1} `;

      const columnsFor2 =
          `   ${columns.column2} AS ${subtitle},
                                      COALESCE(SUM(${columns.column3})) AS ${value1} `;

      const sqlFrom = ` FROM public.${table.name} AS ${table.alias}`;

      const filter = await getFilter(conn, table, params, alert, columns);

      const sqlGroupBy1 = ` GROUP BY ${columns.column1}  `;
      const sqlGroupBy2 = ` GROUP BY ${columns.column2}  `;
      const sqlOrderBy = ` ORDER BY ${value1} DESC `;
      const sqlLimit = ` LIMIT ${limit} `;

      sql1 +=
        ` SELECT ${columnsFor1} ${sqlFrom} ${filter.secondaryTables}
          ${filter.sqlWhere}
          ${sqlGroupBy1}
          ${filter.sqlHaving}
          ${sqlOrderBy}
          ${sqlLimit}
        `;
      sql2 +=
        ` SELECT ${columnsFor2} ${sqlFrom} ${filter.secondaryTables} 
          ${filter.sqlWhere} 
          ${sqlGroupBy2} 
          ${filter.sqlHaving} 
          ${sqlOrderBy} 
          ${sqlLimit} 
        `;
    } else {
      sql1 +=
        ` SELECT 
          ' --- ' AS ${subtitle},
          0.00 AS ${value1} 
        `;
      sql2 +=
        ` SELECT 
          ' --- ' AS ${subtitle},
          0.00 AS ${value1}
        `;
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

          const table = {
            name: await getTable(conn, alert.idview),
            alias: 'main_table'
          };

          const collumns = getColumns(alert, '', table.alias);

          const filter = await getFilter(conn, table, params, alert, collumns);

          const value1 = alert.codgroup === 'FOCOS' ?
              ` COALESCE( ( SELECT ROW_NUMBER() OVER(ORDER BY ${collumns.column1} ASC) AS Row
                                         FROM public.${table.name} AS ${table.alias}
                                         ${filter.secondaryTables}
                                         ${filter.sqlWhere}
                                         GROUP BY ${collumns.column1}
                                         ${filter.sqlHaving}
                                         ORDER BY Row DESC
                                         LIMIT 1
                                      ), 00.00) AS value1 ` :
              `  COALESCE(COUNT(1), 00.00) AS value1 `;

          const sqlWhere =
            filter.sqlHaving ?
              ` ${filter.sqlWhere} 
                AND ${table.alias}.${collumns.column1} IN
                    ( SELECT tableWhere.${collumns.column1} AS subtitle
                      FROM public.${table.name} AS tableWhere
                      GROUP BY tableWhere.${collumns.column1}
                      ${filter.sqlHaving}) ` :
              filter.sqlWhere;

          const value2 = alert.codgroup === 'FOCOS' ?
              ` ( SELECT coalesce(sum(1), 0.00) as num_focos FROM public.${table.name} AS ${table.alias} ${filter.secondaryTables} ${sqlWhere} ) AS value2 ` :
              ` COALESCE(SUM(${collumns.columnArea}), 0.00) AS value2 `;

          const sqlFrom = alert.codgroup === 'FOCOS' ?
              ` ` :
              ` FROM public.${table.name} AS ${table.alias} ${filter.secondaryTables} ${filter.sqlWhere} `;

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
  };
  Filter.getTableName = async function (conn, idView) {
    return await getTable(conn, idView);
  };
  Filter.setFilter = async function (conn, params, table, view) {

    const columns = getColumns(view, table.owner, table.alias);

    return await getFilter(conn, table, params, view, columns);
  }
} ());
