# TerraMA² Webapp

**NOTE:** **Check the website for the most recent stable version: http://www.terrama2.dpi.inpe.br/**
* **If you have any question, please, send us an e-mail at: terrama2-team@dpi.inpe.br.**

TerraMA² Webapp is a free and open source component based web API for use in web geographic information systems (WebGIS).

## Installation

Before you start the installation, make sure you have installed Node.js version 8.12.0 LTS. You can download it from https://nodejs.org.

Bellow we show the steps to run TerraMA² Webapp:

- Clone the TerraMA² repository:

```
git clone https://github.com/TerraMA2/terrama2.git
```

Note: This repository contain the entire TerraMA², if you want just the web API, the important is the webapp directory.

- Access the webapp folder and run the following nodejs command:

```bash
cd webapp
sudo npm install -g grunt-cli # It install grunt in system path
npm install # It installs backend web dependencies
grunt # To compile and prepares TerraMA2 FrontEnd app
```

## Execution Instructions

TerraMA2 web application uses NodeJS Sequelize ORM for handling database manipulation. Make sure you have PostgreSQL database and PostGIS spatial extension installed before.

Make sure to edit `config/settings.json` the option `defaultFilePathList` which represents the default data provider path handling. You can also set `basePath` when you are running TerraMA² behind proxy server.

## Configuration File
The TerraMA² Web settings can be found in [config/settings.json](https://raw.githubusercontent.com/TerraMA2/terrama2/master/webapp/config/settings.json).

Set database configuration into `config/db.json`. By default, TerraMA² has two database config: `development` and `production` which indicates
application state. You can customize and add own database credentials. Make sure to pass through `NODE_ENV` variable.

Key               | Description
----------------- | -----------------------------------------------------------------
                  | Database credentials
  username        | username
  password        | password
  host            | host of database
  port            | database port number
  dialect         | dialect database (postgres/mysql). **Only postgres supported.**
  define          | It represents a extra properties along database
  define.schema   | A database schema


## Web Application components dependencies


If you want to build yourself TerraMA² then you need to install some third-party libraries. Below we show the list of third-party libraries dependencies and its versions:

- **Node.js (Mandatory):** Node.js is a JavaScript runtime for server side scripts. Make sure you have version 8.12.0 LTS installed. You can download it from: https://nodejs.org.

- **AdminLTE (Mandatory):** TerraMA² web front-end is built on top of AdminLTE web template. You can download it from: https://github.com/almasaeed2010/AdminLTE.

- **Bootstrap (Mandatory):** TerraMA² web front-end is built on top of Bootstrap. You can download it from: http://getbootstrap.com.

- **Chart.js (Mandatory):** Chart.js is an HTML5 based framework for drawing charts. You can download it from: http://www.chartjs.org.

- **DataTables (Mandatory):** DataTables is a table plugin for jQuery. It is available at: http://www.datatables.net.

- **jQuery (Mandatory):** jQuery is a JavaScript library for handling HTML document elements. Make sure to have at least version 2.1.4. It is available at: https://jquery.com.

- **AngularJS (Mandatory):** AngularJS is a SuperHeroic javascript MTW framework for handling client side application. Make sure to have at least version 1.6.0. It is available at: https://angularjs.org/.
